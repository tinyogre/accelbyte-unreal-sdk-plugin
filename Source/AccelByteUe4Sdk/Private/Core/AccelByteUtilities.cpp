// Copyright (c) 2020 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "Core/AccelByteUtilities.h"
#include "Core/AccelByteRegistry.h"
#include "Core/AccelByteReport.h"
#include "Core/AccelByteHttpRetryScheduler.h"
#include "Models/AccelByteUserModels.h"

#include <memory>

// enclosing with namespace because of collision with Unreal types
namespace openssl
{
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
}

using namespace openssl;


int32 constexpr RS256_SIGNATURE_LENGTH = 342;
int32 constexpr RSA_MODULUS_BASE64_LENGTH = 342;
int32 constexpr RSA_EXPONENT_BASE64_LENGTH = 4;


void UnescapeB64Url(FString& B64UrlString)
{
	B64UrlString.ReplaceCharInline('-', '+');
	B64UrlString.ReplaceCharInline('_', '/');	
}


FRsaPublicKey::FRsaPublicKey(FString ModulusB64Url, FString ExponentB64Url) :
	ModulusB64Url{MoveTemp(ModulusB64Url)},
	ExponentB64Url{MoveTemp(ExponentB64Url)}
{}

bool FRsaPublicKey::IsValid() const
{
	return ModulusB64Url.Len() == RSA_MODULUS_BASE64_LENGTH && ExponentB64Url.Len() == RSA_EXPONENT_BASE64_LENGTH;
}

FString FRsaPublicKey::ToPem() const
{
	FString UnarmoredPem = FString::Printf(
		TEXT("MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA%sID%s"),
		*ModulusB64Url,
		*ExponentB64Url);

	UnescapeB64Url(UnarmoredPem);

	return FString::Printf(TEXT("-----BEGIN PUBLIC KEY-----\n%s\n-----END PUBLIC KEY-----"), *UnarmoredPem);
}

int32 GetHeaderEnd(FString const& JwtString)
{
	int32 HeaderEnd;
	JwtString.FindChar('.', HeaderEnd);

	return HeaderEnd;
}


int32 GetPayloadEnd(FString const& JwtString)
{
	int32 PayloadEnd;
	JwtString.FindLastChar('.', PayloadEnd);

	return PayloadEnd;
}

TSharedPtr<FJsonObject> ExtractJson(FString const& JsonB64Url, int32 Start, int32 Count)
{
	if (JsonB64Url.IsEmpty())
	{
		return nullptr;
	}
	
	FString JsonStr = JsonB64Url.Mid(Start, Count);
	UnescapeB64Url(JsonStr);
	FBase64::Decode(JsonStr, JsonStr);

	TSharedPtr<FJsonObject> JsonObjectPtr{ MakeShared<FJsonObject>() };

	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonStr), JsonObjectPtr))
	{
		return nullptr;
	}	

	return JsonObjectPtr;
}

FJwt::FJwt(FString JwtString)
	:
	JwtString{MoveTemp(JwtString)},
	HeaderEnd{GetHeaderEnd(this->JwtString)},
	PayloadEnd{GetPayloadEnd(this->JwtString)},
	HeaderJsonPtr{ExtractJson(this->JwtString, 0, HeaderEnd)},
	PayloadJsonPtr{ExtractJson(this->JwtString, HeaderEnd + 1, PayloadEnd - HeaderEnd - 1)}
{
}

EJwtResult FJwt::VerifyWith(FRsaPublicKey Key) const
{
	if (!IsValid())
	{
		return EJwtResult::MalformedJwt;
	}

	if (!Key.IsValid())
	{
		return EJwtResult::MalformedPublicKey;
	}

	if (!HeaderJsonPtr->HasField("alg") || HeaderJsonPtr->GetStringField("alg") != TEXT("RS256"))
	{
		return EJwtResult::AlgorithmMismatch;
	}

	std::unique_ptr<BIO, decltype(&BIO_free_all)> const BIOPublicKeyPtr{BIO_new(BIO_s_mem()), BIO_free_all};

	if (!BIOPublicKeyPtr)
	{
		return EJwtResult::SignatureMismatch;
	}

	FString const PemPublicKey = Key.ToPem();
	int const KeyLength = PemPublicKey.Len();

	if (BIO_write(BIOPublicKeyPtr.get(), StringCast<ANSICHAR>(*PemPublicKey).Get(), KeyLength) != KeyLength)
	{
		return EJwtResult::SignatureMismatch;
	}

	FString const Password{};
	std::shared_ptr<EVP_PKEY> const PublicKeyPtr{
		PEM_read_bio_PUBKEY(BIOPublicKeyPtr.get(), nullptr, nullptr, (void*)StringCast<ANSICHAR>(*Password).Get()),
		EVP_PKEY_free};

	if (!PublicKeyPtr)
	{
		return EJwtResult::SignatureMismatch;
	}

	std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> const MDContextPtr(EVP_MD_CTX_create(), EVP_MD_CTX_free);

	if (!MDContextPtr)
	{
		return EJwtResult::SignatureMismatch;
	}

	if (!EVP_VerifyInit(MDContextPtr.get(), EVP_sha256()))
	{
		return EJwtResult::SignatureMismatch;
	}

	if (!EVP_VerifyUpdate(MDContextPtr.get(), TCHAR_TO_ANSI(*JwtString), PayloadEnd))
	{
		return EJwtResult::SignatureMismatch;
	}

	FString SignatureB64 = JwtString.Right(JwtString.Len() - PayloadEnd - 1);
	UnescapeB64Url(SignatureB64);
	TArray<uint8> SignatureBytes{};
	FBase64::Decode(SignatureB64, SignatureBytes);

	auto const VerifyResult = EVP_VerifyFinal(
		MDContextPtr.get(),
		SignatureBytes.GetData(),
		SignatureBytes.Num(),
		PublicKeyPtr.get());

	if (VerifyResult != 1)
	{
		return EJwtResult::SignatureMismatch;
	}
	
	return EJwtResult::Ok;
}

TSharedPtr<FJsonObject> const& FJwt::Header() const
{
	return HeaderJsonPtr;
}

TSharedPtr<FJsonObject> const& FJwt::Payload() const
{
	return PayloadJsonPtr;
}

bool FJwt::IsValid() const
{
	return
		HeaderEnd != INDEX_NONE
		&& PayloadEnd != INDEX_NONE
		&& HeaderEnd != PayloadEnd
		&& HeaderJsonPtr != nullptr
		&& PayloadJsonPtr != nullptr
		&& JwtString.Len() - PayloadEnd - 1 == RS256_SIGNATURE_LENGTH;
}


void FAccelByteUtilities::RemoveEmptyStrings(TSharedPtr<FJsonObject> JsonPtr)
{
	TArray<FString> KeysToRemove;
	for (auto& Item : JsonPtr->Values)
	{
		if (Item.Value->Type == EJson::String && Item.Value->AsString() == "")
		{
			KeysToRemove.Add(Item.Key);
		}
		else if (Item.Value->Type == EJson::Object)
		{
			TSharedPtr<FJsonObject> const Child = Item.Value->AsObject();
			RemoveEmptyStrings(Child);
		}
	}

	for (FString const& Key : KeysToRemove)
	{
		JsonPtr->RemoveField(Key);
	}
}

FString FAccelByteUtilities::GetPlatformString(EAccelBytePlatformType Platform)
{
	switch (Platform)
	{
	case EAccelBytePlatformType::Steam:
		return "steam";
	case EAccelBytePlatformType::PS4:
		return "ps4";
	case EAccelBytePlatformType::PS4CrossGen:
	case EAccelBytePlatformType::PS5:
		return "ps5";
	case EAccelBytePlatformType::Live:
		return "live";
	case EAccelBytePlatformType::Google:
		return "google";
	case EAccelBytePlatformType::Apple:
		return "apple";	
	case EAccelBytePlatformType::Facebook:
		return "facebook";
	case EAccelBytePlatformType::Android:
		return "android";
	case EAccelBytePlatformType::iOS:
		return "ios";
	case EAccelBytePlatformType::Device:
		return "device";
	case EAccelBytePlatformType::Twitch:
		return "twitch";
	case EAccelBytePlatformType::Oculus:
		return "oculus";
	case EAccelBytePlatformType::Twitter:
		return "twitter";
	case EAccelBytePlatformType::EpicGames:
		return "epicgames";
	case EAccelBytePlatformType::Stadia:
		return "stadia";
	case EAccelBytePlatformType::AwsCognito:
		return "awscognito";
	case EAccelBytePlatformType::Nintendo:
		return "nintendo";	
	default:
		return "unknown";
	}
}

void FAccelByteNetUtilities::GetPublicIP(const THandler<FAccelByteModelsPubIp>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));
	FString Url = FString::Printf(TEXT("https://api.ipify.org?format=json"));
	FString Verb = TEXT("GET");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	FRegistry::HttpRetryScheduler.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void FAccelByteNetUtilities::DownloadFrom(const FString& Url, const FHttpRequestProgressDelegate& OnProgress, const THandler<TArray<uint8>>& OnDownloaded, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Verb = TEXT("GET");
	FString Accept = TEXT("application/octet-stream");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(Verb);
	Request->OnRequestProgress() = OnProgress;
	Request->SetHeader(TEXT("Accept"), Accept);

	FRegistry::HttpRetryScheduler.ProcessRequest(Request, CreateHttpResultHandler(OnDownloaded, OnError), FPlatformTime::Seconds());
}

void FAccelByteNetUtilities::UploadTo(const FString& Url, const TArray<uint8>& DataUpload, const FHttpRequestProgressDelegate& OnProgress,
	const AccelByte::FVoidHandler& OnSuccess, const FErrorHandler& OnError, FString ContentType)
{
	FReport::Log(FString(__FUNCTION__));

	FString Verb = TEXT("PUT");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(Verb);
	Request->OnRequestProgress() = OnProgress;
	Request->SetHeader(TEXT("Content-Type"), *ContentType);
	Request->SetContent(DataUpload);

	FRegistry::HttpRetryScheduler.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
	
}
