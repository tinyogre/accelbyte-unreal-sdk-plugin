// Copyright (c) 2018 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "TestUtilities.h"
#include "AutomationTest.h"
#include "HttpModule.h"
#include "HttpManager.h"
#include "AccelByteSettings.h"
#include "AccelByteCredentials.h"
#include "FileManager.h"

using AccelByte::FErrorHandler;
using AccelByte::Settings;
using AccelByte::Credentials;
using AccelByte::HandleHttpError;

//void SendNotificationResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Successful, const UAccelByteBlueprintsTest::FSendNotificationSuccess OnSuccess, const UAccelByteBlueprintsTest::FBlueprintErrorHandler OnError)
//{
//	int32 Code = 0;
//	FString Message;
//	if (!Response.IsValid())
//	{
//		UE_LOG(LogTemp, Log, TEXT("Response invalid"));
//		return;
//	} else
//	if (EHttpResponseCodes::IsOk(
//		Response->GetResponseCode()
//	))
//	{
//		OnSuccess.ExecuteIfBound();
//		return;
//	}
//}
//
//void SendNotificationRequest(const UAccelByteBlueprintsTest::FSendNotificationSuccess& OnSuccess, const UAccelByteBlueprintsTest::FBlueprintErrorHandler& OnError)
//{
//	FString Authorization = FString::Printf(TEXT("Bearer %s"), *Credentials::Get().GetClientAccessToken());
//	FString Url = FString::Printf(TEXT("%snotification/namespaces/%s/users/%s/freeform"), *Settings::LobbyServerUrl, *Credentials::Get().GetUserNamespace(), *Credentials::Get().GetUserId());
//	FString Verb = TEXT("POST");
//	FString ContentType = TEXT("application/json");
//	FString Accept = TEXT("application/json");
//	FString Content = TEXT("{\"message\":\"notification\"}");
//	Url.Append(TEXT("?async=true"));
//
//	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
//	Request->SetURL(Url);
//	Request->SetHeader(TEXT("Authorization"), Authorization);
//	Request->SetVerb(Verb);
//	Request->SetHeader(TEXT("Content-Type"), ContentType);
//	Request->SetHeader(TEXT("Accept"), Accept);
//	Request->SetContentAsString(Content);
//	//Request->OnProcessRequestComplete().BindStatic(SendNotificationResponse, OnSuccess, OnError);
//	Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnError](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful)
//	{
//		int32 Code = 0;
//		FString Message;
//		if (!Response.IsValid())
//		{
//			UE_LOG(LogTemp, Log, TEXT("Response invalid"));
//			HandleHttpError(Request, Response, Code, Message);
//			OnError.ExecuteIfBound(Code, Message);
//			return;
//		}
//		else
//		if (EHttpResponseCodes::IsOk(
//			Response->GetResponseCode()
//		))
//		{
//			OnSuccess.ExecuteIfBound();
//			return;
//		}
//	});
//	Request->ProcessRequest();
//}

void UAccelByteBlueprintsTest::SendNotification(FString Message, bool bAsync, const UAccelByteBlueprintsTest::FSendNotificationSuccess& OnSuccess, const UAccelByteBlueprintsTest::FBlueprintErrorHandler& OnError)
{
	FString Authorization = FString::Printf(TEXT("Bearer %s"), *Credentials::Get().GetClientAccessToken());
	FString Url = FString::Printf(TEXT("%snotification/namespaces/%s/users/%s/freeform"), *Settings::LobbyServerUrl, *Credentials::Get().GetUserNamespace(), *Credentials::Get().GetUserId());
	FString Verb = TEXT("POST");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");
	FString Content = FString::Printf(TEXT("{\"message\":\"%s\"}"), *Message);
	Url = Url.Replace(TEXT("wss"), TEXT("https")); //change protocol
	Url = Url.Replace(TEXT("lobby/"), TEXT("")); //no /lobby
	if (bAsync) { Url.Append(TEXT("?async=true")); } else { Url.Append(TEXT("?async=false")); }

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);
	Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnError](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful)
	{
		int32 Code;
		FString Message;
		if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
		{
			OnSuccess.ExecuteIfBound();
			return;
		} 
		else
		{
			HandleHttpError(Request, Response, Code, Message);
			OnError.ExecuteIfBound(Code, Message);
			return;
		}
	});
	Request->ProcessRequest();
}
