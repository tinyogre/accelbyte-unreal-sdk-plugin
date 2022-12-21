// Copyright (c) 2018 - 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.


#include "Api/AccelByteUserApi.h"
#include "Api/AccelByteEntitlementApi.h"
#include "Api/AccelByteItemApi.h"
#include "Core/AccelByteRegistry.h"
#include "Core/AccelByteReport.h"
#include "Models/AccelByteEcommerceModels.h"
#include "Core/AccelByteHttpListenerExtension.h"
#include "Core/AccelByteHttpRetryScheduler.h"
#include "Core/AccelByteEnvironment.h"
#include "Core/AccelByteOauth2Api.h"
#include "Api/AccelByteQos.h"
#include "Core/AccelByteUtilities.h"
#include "Core/IAccelByteDataStorage.h"
#include "Core/AccelByteDataStorageBinaryFile.h"
#include "AccelByteUe4SdkModule.h"


DECLARE_LOG_CATEGORY_EXTERN(LogAccelByteUser, Log, All);
DEFINE_LOG_CATEGORY(LogAccelByteUser);

using AccelByte::Api::Oauth2;

namespace AccelByte
{
namespace Api
{

User::User(Credentials& InCredentialsRef
	, Settings& InSettingsRef
	, FHttpRetryScheduler& InHttpRef)
	: FApiBase(InCredentialsRef, InSettingsRef, InHttpRef)
	, HttpRef{InHttpRef}
	, CredentialsRef{InCredentialsRef}
	, SettingsRef{InSettingsRef}
{
}

User::~User()
{}

static FString SearchStrings[] =
{
	TEXT(""),
	TEXT("displayName"),
	TEXT("username"),
};

void User::FinalPreLoginEvents()
{
	if (CredentialsRef.GetSessionState() == Credentials::ESessionState::Valid)
		CredentialsRef.ForgetAll();
}
	

#pragma region Login Methods
void User::LoginWithOtherPlatform(
	EAccelBytePlatformType PlatformType,
	const FString& PlatformToken,
	const FVoidHandler& OnSuccess,
	const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));
	FReport::LogDeprecated(
		FString(__FUNCTION__),
		TEXT("When 2FA is enabled then this method should be changed to the method using FCustomErrorHandler"));

	FinalPreLoginEvents(); // Clears CredentialsRef post-auth info, inits schedulers
	
	Oauth2::GetTokenWithOtherPlatformToken(SettingsRef.IamServerUrl
		, CredentialsRef.GetOAuthClientId()
		, CredentialsRef.GetOAuthClientSecret()
		, FAccelByteUtilities::GetPlatformString(PlatformType)
		, PlatformToken
		, THandler<FOauth2Token>::CreateLambda(
			[this, PlatformType, OnSuccess](const FOauth2Token& Result)
			{
				OnLoginSuccess(OnSuccess, Result);
			})
		, FErrorHandler::CreateLambda(
			[OnError](const int32 ErrorCode, const FString& ErrorMessage)
			{
				OnError.ExecuteIfBound(ErrorCode, ErrorMessage);
			}));

	CredentialsRef.SetBearerAuthRejectedHandler(HttpRef);
}

void User::LoginWithOtherPlatform(
	EAccelBytePlatformType PlatformType,
	const FString& PlatformToken,
	const FVoidHandler& OnSuccess,
	const FCustomErrorHandler& OnError,
	bool bCreateHeadless)
{
	FReport::Log(FString(__FUNCTION__));

	FinalPreLoginEvents(); // Clears CredentialsRef post-auth info, inits schedulers
	
	Oauth2::GetTokenWithOtherPlatformToken(SettingsRef.IamServerUrl
		, CredentialsRef.GetOAuthClientId()
		, CredentialsRef.GetOAuthClientSecret()
		, FAccelByteUtilities::GetPlatformString(PlatformType)
		, PlatformToken
		, THandler<FOauth2Token>::CreateLambda(
			[this, PlatformType, OnSuccess](const FOauth2Token& Result)
			{
				OnLoginSuccess(OnSuccess, Result);
			})
		, FCustomErrorHandler::CreateLambda(
			[this, OnError](const int32 ErrorCode, const FString& ErrorMessage, const FJsonObject& ErrorJson)
			{
				FErrorOauthInfo ErrorOauthInfo;
				TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>(ErrorJson);
				if (FJsonObjectConverter::JsonObjectToUStruct<FErrorOauthInfo>(JsonObject.ToSharedRef(), &ErrorOauthInfo, 0, 0) == false)
				{
					FReport::Log(TEXT("Cannot deserialize the whole ErrorJson to the struct "));
				}
				CredentialsRef.SetErrorOAuth(ErrorOauthInfo);
				OnError.ExecuteIfBound(ErrorCode, ErrorMessage, ErrorJson);
			})
		, bCreateHeadless);

	CredentialsRef.SetBearerAuthRejectedHandler(HttpRef);
}

void User::LoginWithOtherPlatformId(
	const FString& PlatformId,
	const FString& PlatformToken,
	const FVoidHandler& OnSuccess,
	const FCustomErrorHandler& OnError,
	bool bCreateHeadless)
{
	FReport::Log(FString(__FUNCTION__));

	FinalPreLoginEvents(); // Clears CredentialsRef post-auth info, inits schedulers
	
	Oauth2::GetTokenWithOtherPlatformToken(SettingsRef.IamServerUrl
		, CredentialsRef.GetOAuthClientId()
		, CredentialsRef.GetOAuthClientSecret()
		, PlatformId
		, PlatformToken
		, THandler<FOauth2Token>::CreateLambda(
			[this, OnSuccess, OnError](const FOauth2Token& Result)
			{
				OnLoginSuccess(OnSuccess, Result);
			})
		, FCustomErrorHandler::CreateLambda(
			[this, OnError](const int32 ErrorCode, const FString& ErrorMessage, const FJsonObject& ErrorJson)
			{
				FErrorOauthInfo ErrorOauthInfo;
				TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>(ErrorJson);
				if (FJsonObjectConverter::JsonObjectToUStruct<FErrorOauthInfo>(JsonObject.ToSharedRef(), &ErrorOauthInfo, 0, 0) == false)
				{
					FReport::Log(TEXT("Cannot deserialize the whole ErrorJson to the struct "));
				}
				CredentialsRef.SetErrorOAuth(ErrorOauthInfo);
				OnError.ExecuteIfBound(ErrorCode, ErrorMessage, ErrorJson);
			})
		, bCreateHeadless);

	CredentialsRef.SetBearerAuthRejectedHandler(HttpRef);
}
	
void User::LoginWithUsername(
	const FString& Username,
	const FString& Password,
	const FVoidHandler& OnSuccess,
	const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));
	FReport::LogDeprecated(
		FString(__FUNCTION__),
		TEXT("When 2FA is enabled then this method should be changed to the method using FCustomErrorHandler"));

	FinalPreLoginEvents(); // Clears CredentialsRef post-auth info, inits schedulers
	
	Oauth2::GetTokenWithPasswordCredentials(SettingsRef.IamServerUrl
		, CredentialsRef.GetOAuthClientId()
		, CredentialsRef.GetOAuthClientSecret()
		, Username
		, Password
		, THandler<FOauth2Token>::CreateLambda(
			[this, OnSuccess](const FOauth2Token& Result)
			{
				OnLoginSuccess(OnSuccess, Result); // Curry to general handler					
			})
		, FErrorHandler::CreateLambda(
			[OnError](const int32 ErrorCode, const FString& ErrorMessage)
			{
				OnError.ExecuteIfBound(ErrorCode, ErrorMessage);
			}));
	
	CredentialsRef.SetBearerAuthRejectedHandler(HttpRef);
}

void User::LoginWithUsername(
	const FString& Username,
	const FString& Password,
	const FVoidHandler& OnSuccess,
	const FCustomErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FinalPreLoginEvents(); // Clears CredentialsRef post-auth info, inits schedulers
    	
	Oauth2::GetTokenWithPasswordCredentials(SettingsRef.IamServerUrl
		, CredentialsRef.GetOAuthClientId()
		, CredentialsRef.GetOAuthClientSecret()
		, Username
		, Password
		, THandler<FOauth2Token>::CreateLambda(
			[this, OnSuccess](const FOauth2Token& Result)
			{
				OnLoginSuccess(OnSuccess, Result); // Curry to general handler

			})
		, FCustomErrorHandler::CreateLambda(
			[OnError](const int32 ErrorCode, const FString& ErrorMessage, const FJsonObject& ErrorJson)
			{
				OnError.ExecuteIfBound(ErrorCode, ErrorMessage, ErrorJson);
			}));
	
	CredentialsRef.SetBearerAuthRejectedHandler(HttpRef);
}

void User::LoginWithUsernameV3(
	const FString& Username,
	const FString& Password,
	const FVoidHandler& OnSuccess,
	const FErrorHandler& OnError,
	const bool bRememberMe)
{
	FReport::Log(FString(__FUNCTION__));
	FReport::LogDeprecated(
		FString(__FUNCTION__),
		TEXT("When 2FA enabled This method should change to User::LoginWithUsernameV3(const FString& Username, const FString& Password, const FVoidHandler& OnSuccess, const FCustomErrorHandler& OnError, const bool RememberMe)"));

	FinalPreLoginEvents(); // Clears CredentialsRef post-auth info, inits schedulers
	
	Oauth2::GetTokenWithPasswordCredentialsV3(SettingsRef.IamServerUrl
		, CredentialsRef.GetOAuthClientId()
		, CredentialsRef.GetOAuthClientSecret()
		, Username
		, Password
		, THandler<FOauth2Token>::CreateLambda(
			[this, OnSuccess](const FOauth2Token& Result)
			{
				OnLoginSuccess(OnSuccess, Result); // Curry to general handler	
			})
		, FErrorHandler::CreateLambda(
			[OnError](const int32 ErrorCode, const FString& ErrorMessage)
			{
				OnError.ExecuteIfBound(ErrorCode, ErrorMessage);
			})
		, bRememberMe);

	CredentialsRef.SetBearerAuthRejectedHandler(HttpRef);
}

void User::LoginWithUsernameV3(
	const FString& Username,
	const FString& Password,
	const FVoidHandler& OnSuccess,
	const FCustomErrorHandler& OnError,
	const bool bRememberMe)
{
	FReport::Log(FString(__FUNCTION__));

	FinalPreLoginEvents(); // Clears CredentialsRef post-auth info, inits schedulers
	
	Oauth2::GetTokenWithPasswordCredentialsV3(SettingsRef.IamServerUrl
		, CredentialsRef.GetOAuthClientId()
		, CredentialsRef.GetOAuthClientSecret()
		, Username
		, Password
		, THandler<FOauth2Token>::CreateLambda(
			[this, OnSuccess](const FOauth2Token& Result)
			{
				OnLoginSuccess(OnSuccess, Result); // Curry to general handler	
			})
		, FCustomErrorHandler::CreateLambda(
			[OnError](const int32 ErrorCode, const FString& ErrorMessage, const FJsonObject& ErrorJson)
			{
				OnError.ExecuteIfBound(ErrorCode, ErrorMessage, ErrorJson);
			})
		, bRememberMe);

	CredentialsRef.SetBearerAuthRejectedHandler(HttpRef);
}

void User::LoginWithDeviceId(const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FinalPreLoginEvents(); // Clears CredentialsRef post-auth info, inits schedulers
	
	Oauth2::GetTokenWithDeviceId(SettingsRef.IamServerUrl
		, CredentialsRef.GetOAuthClientId()
		, CredentialsRef.GetOAuthClientSecret()
		, THandler<FOauth2Token>::CreateLambda(
			[this, OnSuccess](const FOauth2Token& Result)
			{
				OnLoginSuccess(OnSuccess, Result); // Curry to general handler	
			})
		, FErrorHandler::CreateLambda(
			[OnError](const int32 ErrorCode, const FString& ErrorMessage)
			{
				OnError.ExecuteIfBound(ErrorCode, ErrorMessage);
			}));

	CredentialsRef.SetBearerAuthRejectedHandler(HttpRef);
}

void User::VerifyLoginWithNewDevice2FAEnabled(const FString& MfaToken, EAccelByteLoginAuthFactorType AuthFactorType, const FString& Code,
	const FVoidHandler& OnSuccess, const FCustomErrorHandler& OnError, bool bRememberDevice)
{
	FReport::Log(FString(__FUNCTION__));

	Oauth2::VerifyAndRememberNewDevice(SettingsRef.IamServerUrl
		, CredentialsRef.GetOAuthClientId()
		, CredentialsRef.GetOAuthClientSecret()
		, MfaToken
		, AuthFactorType
		, Code
		, THandler<FOauth2Token>::CreateLambda(
			[this, OnSuccess](const FOauth2Token& Result)
			{
				OnLoginSuccess(OnSuccess, Result); // Curry to general handler	
			})
		, FCustomErrorHandler::CreateLambda(
			[OnError](const int32 ErrorCode, const FString& ErrorMessage, const FJsonObject& ErrorJson)
			{
				OnError.ExecuteIfBound(ErrorCode, ErrorMessage, ErrorJson);
			})
		, bRememberDevice);

	CredentialsRef.SetBearerAuthRejectedHandler(HttpRef);
}

void User::LoginWithLauncher(const FVoidHandler& OnSuccess, const FErrorHandler & OnError)
{
	FReport::Log(FString(__FUNCTION__));

	const FString AuthorizationCode = Environment::GetEnvironmentVariable(TEXT("JUSTICE_AUTHORIZATION_CODE"), 1000);

	FinalPreLoginEvents(); // Clears CredentialsRef post-auth info, inits schedulers

	Oauth2::GetTokenWithAuthorizationCode(SettingsRef.IamServerUrl
		, CredentialsRef.GetOAuthClientId()
		, CredentialsRef.GetOAuthClientSecret()
		, AuthorizationCode
		, SettingsRef.RedirectURI
		, THandler<FOauth2Token>::CreateLambda(
			[this, OnSuccess](const FOauth2Token& Result)
			{
				OnLoginSuccess(OnSuccess, Result); // Curry to general handler			
			})
		, FErrorHandler::CreateLambda([OnError](const int32 ErrorCode, const FString& ErrorMessage)
			{
				OnError.ExecuteIfBound(ErrorCode, ErrorMessage);
			}));

	CredentialsRef.SetBearerAuthRejectedHandler(HttpRef);
}

void User::LoginWithRefreshToken(const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	LoginWithRefreshToken(CredentialsRef.GetRefreshToken(), OnSuccess, OnError);
}

void User::LoginWithRefreshToken(const FString& RefreshToken, const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	Oauth2::GetTokenWithRefreshToken(SettingsRef.IamServerUrl
		, CredentialsRef.GetOAuthClientId()
		, CredentialsRef.GetOAuthClientSecret()
		, RefreshToken
		, THandler<FOauth2Token>::CreateLambda(
			[this, OnSuccess](const FOauth2Token& Result)
			{
				OnLoginSuccess(OnSuccess, Result); // Curry to general handler
			})
		, FErrorHandler::CreateLambda(
			[OnError](const int32 ErrorCode, const FString& ErrorMessage)
			{
				OnError.ExecuteIfBound(ErrorCode, ErrorMessage);
			}));
}

bool IsTokenExpired(FRefreshInfo RefreshInfo)
{
	return RefreshInfo.Expiration <= FDateTime::UtcNow();
}

void User::TryRelogin(const FString& PlatformUserID, const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
#if PLATFORM_WINDOWS
	FReport::Log(FString(__FUNCTION__));

	IAccelByteUe4SdkModuleInterface::Get().GetLocalDataStorage()->GetItem(PlatformUserID, THandler<TPair<FString, FString>>::CreateLambda(
		[this, OnSuccess, OnError](TPair<FString, FString> Pair)
		{
			if (Pair.Key.IsEmpty() || Pair.Value.IsEmpty())
			{
				OnError.ExecuteIfBound((int32)AccelByte::ErrorCodes::CachedTokenNotFound, TEXT("The cached token is not found. Cannot continue the previous login session. Please login again."));
				return;
			}

			auto Decoded = FAccelByteUtilities::XOR(Pair.Value, FAccelByteUtilities::GetDeviceId());
			FRefreshInfo RefreshInfo;
			if (!FJsonObjectConverter::JsonObjectStringToUStruct<FRefreshInfo>(Decoded, &RefreshInfo, 0, 0))
			{
				OnError.ExecuteIfBound((int32)AccelByte::ErrorCodes::UnableToSerializeCachedToken, TEXT("The cached token can't be parsed. Cannot continue the previous login session. Please login again."));
				return;
			}

			if (IsTokenExpired(RefreshInfo))
			{
				OnError.ExecuteIfBound((int32)AccelByte::ErrorCodes::CachedTokenExpired, TEXT("Your previous login session is expired. Please login again."));
				return;
			}

			this->LoginWithRefreshToken(RefreshInfo.RefreshToken, OnSuccess, OnError);
		}));
#else
	OnError.ExecuteIfBound((int32)AccelByte::ErrorCodes::CachedTokenNotFound, TEXT("Cannot relogin using cached token on other platforms."));
#endif
}

void User::CreateHeadlessAccountAndLogin(const FVoidHandler& OnSuccess, const FCustomErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__)); 
	
	Oauth2::CreateHeadlessAccountAndResponseToken(SettingsRef.IamServerUrl
		, SettingsRef.ClientId
		, SettingsRef.ClientSecret 
		, CredentialsRef.GetLinkingToken()
		, THandler<FOauth2Token>::CreateLambda(
			[this, OnSuccess](const FOauth2Token& Result)
			{
				OnLoginSuccess(OnSuccess, Result); // Curry to general handler
			})	
		, FCustomErrorHandler::CreateLambda([OnError](const int32 ErrorCode, const FString& ErrorMessage, const FJsonObject& ErrorJson)
			{
				OnError.ExecuteIfBound(ErrorCode, ErrorMessage, ErrorJson);
			})); 
}

void User::AuthenticationWithPlatformLinkAndLogin(const FString& Username, const FString& Password, const FVoidHandler& OnSuccess, const FCustomErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	Oauth2::AuthenticationWithPlatformLink(SettingsRef.IamServerUrl
		, SettingsRef.ClientId
		, SettingsRef.ClientSecret
		, Username
		, Password
		, CredentialsRef.GetLinkingToken()
		, THandler<FOauth2Token>::CreateLambda(
			[this, OnSuccess](const FOauth2Token& Result)
			{
				OnLoginSuccess(OnSuccess, Result); // Curry to general handler
			})		
		, FCustomErrorHandler::CreateLambda(
			[OnError](const int32 ErrorCode, const FString& ErrorMessage, const FJsonObject& ErrorJson)
			{
				OnError.ExecuteIfBound(ErrorCode, ErrorMessage, ErrorJson);
			}));
}
#pragma endregion /Login Methods
	

void User::OnLoginSuccess(const FVoidHandler& OnSuccess, const FOauth2Token& Response)
{
	// Set auth token before anything: Set before anything to prevent race condition issues.
	CredentialsRef.SetAuthToken(Response, FPlatformTime::Seconds());

	if (Response.Auth_Trust_Id != TEXT(""))
	{
		FAccelByteUtilities::SetAuthTrustId(Response.Auth_Trust_Id);
	}
	
	FVoidHandler CallbackFunction = FVoidHandler::CreateLambda([this, OnSuccess, Response]() {
		CredentialsRef.OnLoginSuccess().Broadcast(Response); // Sets auth tokens, inits qosScheduler
		OnSuccess.ExecuteIfBound();
	});

	GetData(THandler<FAccountUserData>::CreateLambda(
			[CallbackFunction](const FAccountUserData& AccountUserData)
			{
				CallbackFunction.Execute();
			})
		, FErrorHandler::CreateLambda(
			[CallbackFunction](int ErrorCode, const FString& ErrorMessage)
			{
				FReport::Log(FString::Printf(TEXT("[AccelByte] Error GetData after Login Success, Error Code: %d Message: %s"), ErrorCode, *ErrorMessage));
				CallbackFunction.Execute();
			})
		);

	FHttpRetryScheduler::SetHeaderNamespace(Response.Namespace);

#ifndef PLATFORM_WINDOWS // the following code is working on Windows only at the moment
	return;
#endif

	if (Response.Platform_user_id.IsEmpty() || Response.Refresh_token.IsEmpty())
	{
		return;
	}

	// Store the refresh token
	FRefreshInfo Info;
	Info.RefreshToken = Response.Refresh_token;
	Info.Expiration = FDateTime::UtcNow() + FTimespan::FromSeconds(Response.Refresh_expires_in);
	FString SerializedInfo;
	FJsonObjectConverter::UStructToJsonObjectString(Info, SerializedInfo);

	auto XorInfo = FAccelByteUtilities::XOR(SerializedInfo, FAccelByteUtilities::GetDeviceId());
	IAccelByteUe4SdkModuleInterface::Get().GetLocalDataStorage()->SaveItem(Response.Platform_user_id, XorInfo,
		THandler<bool>::CreateLambda([this, OnSuccess, Response](bool IsSuccess)
		{
			// On Save Refresh Token Success
		}));
}

void User::Logout(const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	Oauth2::RevokeUserToken(SettingsRef.IamServerUrl
		, CredentialsRef.GetOAuthClientId()
		, CredentialsRef.GetOAuthClientSecret()
		, CredentialsRef.GetAccessToken()
		, FVoidHandler::CreateLambda(
			[this, OnSuccess]() 
			{
				ForgetAllCredentials();
				OnSuccess.ExecuteIfBound();
			})
		, OnError);
}

void User::ForgetAllCredentials()
{
	FReport::Log(FString(__FUNCTION__));

	CredentialsRef.ForgetAll();
}

void User::Register(const FString& Username, const FString& Password, const FString& DisplayName, const FString& Country, const FString& DateOfBirth, const THandler<FRegisterResponse>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FRegisterRequest NewUserRequest;
	NewUserRequest.DisplayName  = DisplayName;
	NewUserRequest.Password     = Password;
	NewUserRequest.EmailAddress = Username;
	NewUserRequest.AuthType     = TEXT("EMAILPASSWD");
	NewUserRequest.Country      = Country;
	NewUserRequest.DateOfBirth  = DateOfBirth;

	FString Url             = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb            = TEXT("POST");
	FString ContentType     = TEXT("application/json");
	FString Accept          = TEXT("application/json");
	FString Content;
	FJsonObjectConverter::UStructToJsonObjectString(NewUserRequest, Content);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::Registerv2(const FString& EmailAddress, const FString& Username, const FString& Password, const FString& DisplayName, const FString& Country, const FString& DateOfBirth, const THandler<FRegisterResponse>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FRegisterRequestv2 NewUserRequest;
	NewUserRequest.DisplayName = DisplayName;
	NewUserRequest.Password = Password;
	NewUserRequest.EmailAddress = EmailAddress;
	NewUserRequest.Username = Username;
	NewUserRequest.AuthType = TEXT("EMAILPASSWD");
	NewUserRequest.Country = Country;
	NewUserRequest.DateOfBirth = DateOfBirth;

	FString Url = FString::Printf(TEXT("%s/v4/public/namespaces/%s/users"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb = TEXT("POST");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");
	FString Content;
	FJsonObjectConverter::UStructToJsonObjectString(NewUserRequest, Content);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::Registerv3(const FRegisterRequestv3& RegisterRequest, const THandler<FRegisterResponse>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FRegisterRequestv3 NewUserRequest;
	NewUserRequest.AcceptedPolicies = RegisterRequest.AcceptedPolicies;
	NewUserRequest.DisplayName = RegisterRequest.DisplayName;
	NewUserRequest.Password = RegisterRequest.Password;
	NewUserRequest.EmailAddress = RegisterRequest.EmailAddress;
	NewUserRequest.Username = RegisterRequest.Username;
	NewUserRequest.AuthType = TEXT("EMAILPASSWD");
	NewUserRequest.Country = RegisterRequest.Country;
	NewUserRequest.DateOfBirth = RegisterRequest.DateOfBirth;

	FString Url = FString::Printf(TEXT("%s/v4/public/namespaces/%s/users"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb = TEXT("POST");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");
	FString Content;
	FJsonObjectConverter::UStructToJsonObjectString(NewUserRequest, Content);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::GetData(const THandler<FAccountUserData>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v3/public/users/me"), *SettingsRef.IamServerUrl);
	FString Verb            = TEXT("GET");
	FString ContentType     = TEXT("application/json");
	FString Accept          = TEXT("application/json");
	FString Content;

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(THandler<FAccountUserData>::CreateLambda(
		[this, OnSuccess](const FAccountUserData& AccountUserData)
		{
			CredentialsRef.SetAccountUserData(AccountUserData);
			OnSuccess.ExecuteIfBound(AccountUserData);
		}),
		OnError), FPlatformTime::Seconds());
}

void User::UpdateUser(FUserUpdateRequest UpdateRequest, const THandler<FAccountUserData>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	if (!UpdateRequest.EmailAddress.IsEmpty())
	{
		OnError.ExecuteIfBound(400, TEXT("Cannot update user email using this function. Use UpdateEmail instead."));
		return;
	}

	FString Authorization = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url = FString::Printf(TEXT("%s/v4/public/namespaces/%s/users/me"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb = TEXT("PATCH");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");
	FString Content;
	FJsonObjectConverter::UStructToJsonObjectString(UpdateRequest, Content);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::UpdateEmail(FUpdateEmailRequest UpdateEmailRequest, const FVoidHandler & OnSuccess, const FErrorHandler & OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url = FString::Printf(TEXT("%s/v4/public/namespaces/%s/users/me/email"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb = TEXT("PUT");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");
	FString Content;
	FJsonObjectConverter::UStructToJsonObjectString(UpdateEmailRequest, Content);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::BulkGetUserByOtherPlatformUserIds(EAccelBytePlatformType PlatformType, const TArray<FString>& OtherPlatformUserId, const THandler<FBulkPlatformUserIdResponse>& OnSuccess, const FErrorHandler & OnError)
{
	FReport::Log(FString(__FUNCTION__));

	const FString PlatformString = FAccelByteUtilities::GetPlatformString(PlatformType);
	const FBulkPlatformUserIdRequest UserIdRequests{ OtherPlatformUserId };

	FString Authorization = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url = FString::Printf(TEXT("%s/v3/public/namespaces/%s/platforms/%s/users"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace, *PlatformString);
	FString Verb = TEXT("POST");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");
	FString Content;
	FJsonObjectConverter::UStructToJsonObjectString(UserIdRequests, Content);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::SendVerificationCode(const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	if (CredentialsRef.GetUserEmailAddress().IsEmpty())
	{
		OnError.ExecuteIfBound(400, TEXT("User email address cannot be empty"));
	}

	FVerificationCodeRequest SendVerificationCodeRequest
	{
		EVerificationContext::UserAccountRegistration,
		TEXT(""),
		CredentialsRef.GetUserEmailAddress()
	};

	SendVerificationCode(SendVerificationCodeRequest, OnSuccess, OnError);
}

void User::SendUpdateEmailVerificationCode(const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	if (CredentialsRef.GetUserEmailAddress().IsEmpty())
	{
		OnError.ExecuteIfBound(400, TEXT("User email address cannot be empty"));
	}

	FVerificationCodeRequest SendVerificationCodeRequest
	{
		EVerificationContext::UpdateEmailAddress,
		TEXT(""),
		CredentialsRef.GetUserEmailAddress()
	};

	SendVerificationCode(SendVerificationCodeRequest, OnSuccess, OnError);
}

void User::SendUpgradeVerificationCode(const FString& EmailAddress, const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FVerificationCodeRequest SendUpgradeVerificationCodeRequest
	{
		EVerificationContext::upgradeHeadlessAccount,
		TEXT(""),
		EmailAddress
	};

	SendVerificationCode(SendUpgradeVerificationCodeRequest, OnSuccess, OnError);
}

void User::UpgradeAndVerify(const FString& Username, const FString& Password, const FString& VerificationCode, const THandler<FAccountUserData>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users/me/headless/code/verify"), *SettingsRef.IamServerUrl, *CredentialsRef.GetNamespace(), *CredentialsRef.GetUserId());
	FString Verb            = TEXT("POST");
	FString ContentType     = TEXT("application/json");
	FString Accept          = TEXT("application/json");
	FString Content         = FString::Printf(TEXT("{ \"code\": \"%s\", \"emailAddress\": \"%s\", \"password\": \"%s\"}"), *VerificationCode, *Username, *Password);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(
		Request,
		CreateHttpResultHandler(
			THandler<FAccountUserData>::CreateLambda(
				[OnSuccess](const FAccountUserData& UserData)
				{
					OnSuccess.ExecuteIfBound(UserData);
				}),
			OnError),
		FPlatformTime::Seconds());
}

void User::UpgradeAndVerify2(const FUpgradeAndVerifyRequest& UpgradeAndVerifyRequest,  const THandler<FAccountUserData>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));
	
	const FString Url             = FString::Printf(TEXT("%s/v4/public/namespaces/%s/users/me/headless/code/verify"), *SettingsRef.IamServerUrl, *CredentialsRef.GetNamespace(), *CredentialsRef.GetUserId());
	
	HttpClient.ApiRequest("POST", Url, {}, UpgradeAndVerifyRequest, OnSuccess, OnError);
}

void User::Upgrade(const FString& Username, const FString& Password, const THandler<FAccountUserData>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users/me/headless/verify"), *SettingsRef.IamServerUrl, *CredentialsRef.GetNamespace());
	FString Verb            = TEXT("POST");
	FString ContentType     = TEXT("application/json");
	FString Accept          = TEXT("application/json");
	FString Content         = FString::Printf(TEXT("{ \"EmailAddress\": \"%s\", \"Password\": \"%s\"}"), *Username, *Password);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(
		Request,
		CreateHttpResultHandler(
			THandler<FAccountUserData>::CreateLambda(
				[OnSuccess](const FAccountUserData& UserData)
				{
					OnSuccess.ExecuteIfBound(UserData);
				}),
			OnError),
		FPlatformTime::Seconds());
}

void User::Upgradev2(const FString& EmailAddress, const FString& Username, const FString& Password, const THandler<FAccountUserData>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url = FString::Printf(TEXT("%s/v4/public/namespaces/%s/users/me/headless/verify"), *SettingsRef.IamServerUrl, *CredentialsRef.GetNamespace());
	FString Verb = TEXT("POST");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");
	FString Content = FString::Printf(TEXT("{ \"emailAddress\": \"%s\", \"password\": \"%s\", \"username\": \"%s\"}"), *EmailAddress, *Password, *Username);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(
		Request,
		CreateHttpResultHandler(
			THandler<FAccountUserData>::CreateLambda(
				[OnSuccess](const FAccountUserData& UserData)
				{
					OnSuccess.ExecuteIfBound(UserData);
				}),
			OnError),
		FPlatformTime::Seconds());
}

void User::Verify(const FString& VerificationCode, const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString ContactType     = TEXT("email");
	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users/me/code/verify"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb            = TEXT("POST");
	FString ContentType     = TEXT("application/json");
	FString Accept          = TEXT("application/json");
	FString Content         = FString::Printf(TEXT("{ \"Code\": \"%s\",\"ContactType\":\"%s\"}"), *VerificationCode, *ContactType);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::SendResetPasswordCode(const FString& EmailAddress, const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Url             = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users/forgot"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb            = TEXT("POST");
	FString ContentType     = TEXT("application/json");
	FString Accept          = TEXT("application/json");
	FString Content         = FString::Printf(TEXT("{\"emailAddress\": \"%s\"}"), *EmailAddress);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::ResetPassword(const FString& VerificationCode, const FString& EmailAddress, const FString& NewPassword, const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FResetPasswordRequest ResetPasswordRequest;
	ResetPasswordRequest.Code           = VerificationCode;
	ResetPasswordRequest.EmailAddress   = EmailAddress;
	ResetPasswordRequest.NewPassword    = NewPassword;
	FString Url             = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users/reset"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb            = TEXT("POST");
	FString ContentType     = TEXT("application/json");
	FString Accept          = TEXT("application/json");
	FString Content;
	FJsonObjectConverter::UStructToJsonObjectString(ResetPasswordRequest, Content);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::GetPlatformLinks(const THandler<FPagedPlatformLinks>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users/%s/platforms"), *SettingsRef.IamServerUrl, *CredentialsRef.GetNamespace(), *CredentialsRef.GetUserId());
	FString Verb            = TEXT("GET");
	FString ContentType     = TEXT("application/json");
	FString Accept          = TEXT("application/json");
	FString Content;

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::LinkOtherPlatform(EAccelBytePlatformType PlatformType, const FString& Ticket, const FVoidHandler& OnSuccess, const FCustomErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	auto PlatformId = FAccelByteUtilities::GetPlatformString(PlatformType);

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users/me/platforms/%s"), *SettingsRef.IamServerUrl, *CredentialsRef.GetNamespace(), *PlatformId);
	FString Verb            = TEXT("POST");
	FString ContentType     = TEXT("application/x-www-form-urlencoded");
	FString Accept          = TEXT("application/json");
	FString Content         = FString::Printf(TEXT("ticket=%s"), *FGenericPlatformHttp::UrlEncode(*Ticket));

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::ForcedLinkOtherPlatform(EAccelBytePlatformType PlatformType, const FString& PlatformUserId, const FVoidHandler& OnSuccess, const FCustomErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	auto PlatformId = FAccelByteUtilities::GetPlatformString(PlatformType);

	FLinkPlatformAccountRequest linkRequest;
	linkRequest.PlatformId = PlatformId;
	linkRequest.PlatformUserId = PlatformUserId;

	FString Authorization = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users/%s/platforms/link"), *SettingsRef.IamServerUrl, *CredentialsRef.GetNamespace(), *CredentialsRef.GetUserId());
	FString Verb = TEXT("POST");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");
	FString Content = TEXT("");
	FJsonObjectConverter::UStructToJsonObjectString(linkRequest, Content);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::UnlinkOtherPlatform(EAccelBytePlatformType PlatformType, const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	auto PlatformId = FAccelByteUtilities::GetPlatformString(PlatformType);

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users/me/platforms/%s"), *SettingsRef.IamServerUrl, *CredentialsRef.GetNamespace(), *PlatformId);
	FString Verb            = TEXT("DELETE");
	FString ContentType     = TEXT("application/json");
	FString Accept          = TEXT("application/json");
	FString Content;

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::SendVerificationCode(const FVerificationCodeRequest& VerificationCodeRequest, const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = TEXT("");
	FString Namespace       = TEXT("");
	
	Authorization = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	Namespace = CredentialsRef.GetNamespace();
	
	FString Url             = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users/me/code/request"), *SettingsRef.IamServerUrl, *Namespace);
	FString Verb            = TEXT("POST");
	FString ContentType     = TEXT("application/json");
	FString Accept          = TEXT("application/json");
	FString Content         = TEXT("");
	FJsonObjectConverter::UStructToJsonObjectString(VerificationCodeRequest, Content);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::SearchUsers(const FString& Query, EAccelByteSearchType By, const THandler<FPagedPublicUsersInfo>& OnSuccess, const FErrorHandler& OnError,
	const int32& Offset, const int32& Limit)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users"), *SettingsRef.IamServerUrl, *CredentialsRef.GetNamespace());
	FString Verb            = TEXT("GET");
	FString ContentType     = TEXT("application/json");
	FString Accept          = TEXT("application/json");
	FString Content;

	TMap<FString, FString> QueryParams = {};
	QueryParams.Add("query", Query);
	if (By != EAccelByteSearchType::ALL)
	{
		FString SearchId = SearchStrings[static_cast<std::underlying_type<EAccelByteSearchType>::type>(By)];
		QueryParams.Add("by", *SearchId);
	}
	if (Limit >= 0)
	{
		QueryParams.Add("limit", FString::FromInt(Limit));
	}
	if (Offset >= 0)
	{
		QueryParams.Add("offset", FString::FromInt(Offset));
	}
	
	// Converting TMap QueryParams as one line QueryString 
	FString QueryString;
	int i = 0;
	for (const auto& Kvp : QueryParams)
	{
		QueryString.Append(FString::Printf(TEXT("%s%s=%s"), (i++ == 0 ? TEXT("?") : TEXT("&")),
				*FGenericPlatformHttp::UrlEncode(Kvp.Key), *FGenericPlatformHttp::UrlEncode(Kvp.Value)));
	}
	Url.Append(QueryString);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::SearchUsers(const FString& Query, const THandler<FPagedPublicUsersInfo>& OnSuccess, const FErrorHandler& OnError)
{
	SearchUsers(Query, EAccelByteSearchType::ALL, OnSuccess, OnError);
}

void User::SearchUsers(const FString& Query, int32 Offset, int32 Limit, const THandler<FPagedPublicUsersInfo>& OnSuccess, const FErrorHandler& OnError)
{
	SearchUsers(Query, EAccelByteSearchType::ALL, OnSuccess, OnError, Offset, Limit);
}
	
void User::GetUserByUserId(const FString& UserID, const THandler<FSimpleUserData>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users/%s"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace, *UserID);
	FString Verb            = TEXT("GET");
	FString ContentType     = TEXT("application/json");
	FString Accept          = TEXT("application/json");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::GetUserByOtherPlatformUserId(EAccelBytePlatformType PlatformType, const FString& OtherPlatformUserId, const THandler<FAccountUserData>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));
	FString PlatformId      = FAccelByteUtilities::GetPlatformString(PlatformType);

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v3/public/namespaces/%s/platforms/%s/users/%s"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace, *PlatformId, *OtherPlatformUserId);
	FString Verb            = TEXT("GET");
	FString Accept          = TEXT("application/json");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Accept"), Accept);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::GetCountryFromIP(const THandler<FCountryInfo>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v3/location/country"), *SettingsRef.IamServerUrl);
	FString Verb            = TEXT("GET");
	FString Accept          = TEXT("application/json");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Accept"), Accept);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::GetUserEligibleToPlay(const THandler<bool>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	auto onItemInfoGot = THandler<FAccelByteModelsItemInfo>::CreateLambda([this, OnSuccess, OnError](const FAccelByteModelsItemInfo& itemInfoResult)
	{

		TArray<FString> itemIds;
		TArray<FString> skus = itemInfoResult.Features;
		TArray<FString> appIds;
		appIds.Init(*SettingsRef.AppId, 1);

		FRegistry::Entitlement.GetUserEntitlementOwnershipAny(itemIds, appIds, skus, THandler<FAccelByteModelsEntitlementOwnership>::CreateLambda([OnSuccess, OnError](FAccelByteModelsEntitlementOwnership ownership)
		{
			OnSuccess.ExecuteIfBound(ownership.Owned);
		}), FErrorHandler::CreateLambda([OnError](int32 ErrorCode, const FString& ErrorMsg)
		{
			OnError.ExecuteIfBound(ErrorCode, ErrorMsg);
		}));
	});

	FRegistry::Item.GetItemByAppId(*SettingsRef.AppId, "", "", onItemInfoGot, FErrorHandler::CreateLambda([OnError](int32 ErrorCode, const FString& ErrorMsg)
	{
		OnError.ExecuteIfBound(ErrorCode, ErrorMsg);
	}));
}

void User::BulkGetUserInfo(const TArray<FString>& UserIds, const THandler<FListBulkUserInfo>& OnSuccess, const FErrorHandler& OnError) 
{
	FReport::Log(FString(__FUNCTION__));

	if (UserIds.Num() <= 0)
	{
		OnError.ExecuteIfBound((int32)ErrorCodes::InvalidRequest, TEXT("UserIds cannot be empty!"));
		return;
	}

	const FListBulkUserInfoRequest UserList{ UserIds };

	FString Url = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users/bulk/basic"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb = TEXT("POST");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");
	FString Content = TEXT("");
	FJsonObjectConverter::UStructToJsonObjectString(UserList, Content);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::GetInputValidations(const FString& LanguageCode, THandler<FInputValidation> const& OnSuccess, FErrorHandler const& OnError,
	bool bDefaultOnEmpty)
{
	FReport::Log(FString(__FUNCTION__));
	
	FString Url = FString::Printf(TEXT("%s/v3/public/inputValidations"), *SettingsRef.IamServerUrl);	 
	const TMap<FString, FString> Params ({{"languageCode", *LanguageCode}, {"defaultOnEmpty", bDefaultOnEmpty ? TEXT("true") : TEXT("false")}});
	FString Content = TEXT("");  

	// Api Request 
	HttpClient.ApiRequest("GET", Url, Params, Content, OnSuccess, OnError); 
}
	
void User::Enable2FaBackupCode(const THandler<FUser2FaBackupCode>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v4/public/namespaces/%s/users/me/mfa/backupCode/enable"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb            = TEXT("POST");
	FString Accept          = TEXT("application/json");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Accept"), Accept);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::Disable2FaBackupCode(const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v4/public/namespaces/%s/users/me/mfa/backupCode/disable"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb            = TEXT("DELETE");
	FString Accept          = TEXT("application/json");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Accept"), Accept);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::GenerateBackupCode(const THandler<FUser2FaBackupCode>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v4/public/namespaces/%s/users/me/mfa/backupCode"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb            = TEXT("POST");
	FString Accept          = TEXT("application/json");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Accept"), Accept);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::GetBackupCode(const THandler<FUser2FaBackupCode>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v4/public/namespaces/%s/users/me/mfa/backupCode"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb            = TEXT("GET");
	FString Accept          = TEXT("application/json");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Accept"), Accept);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::Enable2FaAuthenticator(const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v4/public/namespaces/%s/users/me/mfa/authenticator/enable"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb            = TEXT("POST");
	FString Accept          = TEXT("application/json");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Accept"), Accept);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::Disable2FaAuthenticator(const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v4/public/namespaces/%s/users/me/mfa/authenticator/disable"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb            = TEXT("DELETE");
	FString Accept          = TEXT("application/json");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Accept"), Accept);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::GenerateSecretKeyFor2FaAuthenticator(const THandler<FUser2FaSecretKey>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v4/public/namespaces/%s/users/me/mfa/authenticator/key"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb            = TEXT("POST");
	FString Accept          = TEXT("application/json");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Accept"), Accept);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::GetEnabled2FaFactors(const THandler<FUser2FaMethod>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v4/public/namespaces/%s/users/me/mfa/factor"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb            = TEXT("GET");
	FString Accept          = TEXT("application/json");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Accept"), Accept);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::MakeDefault2FaFactors(EAccelByteLoginAuthFactorType AuthFactorType ,const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	const FString Factor = FAccelByteUtilities::GetAuthenticatorString(AuthFactorType);

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v4/public/namespaces/%s/users/me/mfa/factor"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb            = TEXT("POST");
	FString Accept          = TEXT("application/json");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	Request->SetVerb(Verb);
	Request->SetContentAsString(FString::Printf(TEXT("factor=%s"), *Factor));

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::UpdateUserV3(FUserUpdateRequest UpdateRequest, const THandler<FAccountUserData>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	if (!UpdateRequest.EmailAddress.IsEmpty())
	{
		OnError.ExecuteIfBound(400, TEXT("Cannot update user email using this function. Use UpdateEmail instead."));
		return;
	}

	FString Authorization = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users/me"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace);
	FString Verb = TEXT("PATCH");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");
	FString Content;
	FJsonObjectConverter::UStructToJsonObjectString(UpdateRequest, Content);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::GetPublisherUser(const FString& UserId, const THandler<FGetPublisherUserResponse>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users/%s/publisher"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace, *UserId);
	FString Verb            = TEXT("GET");
	FString Accept          = TEXT("application/json");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Accept"), Accept);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::VerifyToken(const FVoidHandler& OnSuccess, const FErrorHandler & OnError)
{
	FReport::Log(FString(__FUNCTION__));

	Oauth2::VerifyToken(SettingsRef.IamServerUrl
		, CredentialsRef.GetOAuthClientId()
		, CredentialsRef.GetOAuthClientSecret()
		, CredentialsRef.GetAccessToken()
		, FVoidHandler::CreateLambda(
			[this, OnSuccess]() 
			{
				OnSuccess.ExecuteIfBound();
			})
		, OnError);
}

void User::GetUserInformation(const FString& UserId, const THandler<FGetUserInformationResponse>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	FString Authorization   = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url             = FString::Printf(TEXT("%s/v3/public/namespaces/%s/users/%s/information"), *SettingsRef.IamServerUrl, *SettingsRef.Namespace, *UserId);
	FString Verb            = TEXT("GET");
	FString Accept          = TEXT("application/json");

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Accept"), Accept);

	HttpRef.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void User::GenerateOneTimeCode(EAccelBytePlatformType PlatformType, const THandler<FGeneratedOneTimeCode>& OnSuccess, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__)); 

	const FString PlatformString = FAccelByteUtilities::GetPlatformString(PlatformType); 	
	Oauth2::GenerateOneTimeCode(
		CredentialsRef.GetAccessToken(),
		PlatformString,
		THandler<FGeneratedOneTimeCode>::CreateLambda([this, OnSuccess](const FGeneratedOneTimeCode& Result) 
		{
			OnSuccess.ExecuteIfBound(Result);
		}),
		OnError);
}
 
} // Namespace Api
} // Namespace AccelByte
