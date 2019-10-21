// Copyright (c) 2019 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "Api/AccelByteStatisticApi.h"
#include "Modules/ModuleManager.h"
#include "Core/AccelByteRegistry.h"
#include "Core/AccelByteHttpRetryScheduler.h"
#include "Core/AccelByteSettings.h"

namespace AccelByte
{
namespace Api
{

Statistic::Statistic(const AccelByte::Credentials& Credentials, const AccelByte::Settings& Setting) : Credentials(Credentials), Settings(Setting){}

Statistic::~Statistic() {}

void Statistic::GetAllProfileStatItems(const FString& ProfileId, const THandler<FAccelByteModelsUserStatItemPagingSlicedResult>& OnSuccess, const FErrorHandler& OnError)
{
    Statistic::GetProfileStatItems(*this, ProfileId, {}, OnSuccess, OnError);
}

void Statistic::GetProfileStatItemsByStatCodes(const FString& ProfileId, const TArray<FString>& StatCodes, const THandler<FAccelByteModelsUserStatItemPagingSlicedResult>& OnSuccess, const FErrorHandler & OnError)
{
    Statistic::GetProfileStatItems(*this, ProfileId, StatCodes, OnSuccess, OnError);
}

void Statistic::GetProfileStatItems(const Statistic& Object, const FString& ProfileId, const TArray<FString>& StatCodes, const THandler<FAccelByteModelsUserStatItemPagingSlicedResult>& OnSuccess, const FErrorHandler & OnError)
{
	FString Authorization = FString::Printf(TEXT("Bearer %s"), *Object.Credentials.GetUserSessionId());
	FString Url = FString::Printf(TEXT("%s/v1/public/namespaces/%s/users/%s/profiles/%s/statitems"), *Object.Settings.StatisticServerUrl, *Object.Credentials.GetUserNamespace(), *Object.Credentials.GetUserId(), *ProfileId);
	FString Verb = TEXT("GET");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");
	FString Content;

    for (int i = 0; i < StatCodes.Num(); i++)
    {
        Url.Append((i == 0) ? TEXT("?statCodes=") : TEXT(",")).Append(StatCodes[i]);
    }

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);
	FRegistry::HttpRetryScheduler.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void Statistic::GetAllUserStatItems(const THandler<FAccelByteModelsUserStatItemPagingSlicedResult>& OnSuccess, const FErrorHandler& OnError)
{
    Statistic::GetUserStatItems(*this, {}, OnSuccess, OnError);
}

void Statistic::GetUserStatItemsByStatCodes(const TArray<FString>& StatCodes, const THandler<FAccelByteModelsUserStatItemPagingSlicedResult>& OnSuccess, const FErrorHandler & OnError)
{
    Statistic::GetUserStatItems(*this, StatCodes, OnSuccess, OnError);
}

void Statistic::GetUserStatItems(const Statistic& Object, const TArray<FString>& StatCodes, const THandler<FAccelByteModelsUserStatItemPagingSlicedResult>& OnSuccess, const FErrorHandler & OnError)
{
    FString Authorization = FString::Printf(TEXT("Bearer %s"), *Object.Credentials.GetUserSessionId());
    FString Url = FString::Printf(TEXT("%s/v1/public/namespaces/%s/users/%s/statitems"), *Object.Settings.StatisticServerUrl, *Object.Credentials.GetUserNamespace(), *Object.Credentials.GetUserId());
    FString Verb = TEXT("GET");
    FString ContentType = TEXT("application/json");
    FString Accept = TEXT("application/json");
    FString Content;

    for (int i = 0; i < StatCodes.Num(); i++)
    {
        Url.Append((i == 0) ? TEXT("?statCodes=") : TEXT(",")).Append(StatCodes[i]);
    }

    FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetHeader(TEXT("Authorization"), Authorization);
    Request->SetVerb(Verb);
    Request->SetHeader(TEXT("Content-Type"), ContentType);
    Request->SetHeader(TEXT("Accept"), Accept);
    Request->SetContentAsString(Content);
    FRegistry::HttpRetryScheduler.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void Statistic::BulkAddStatItemValue(const TArray<FAccelByteModelsBulkUserStatItemInc>& data, const THandler<TArray<FAccelByteModelsBulkStatItemIncResult>>& OnSuccess, const FErrorHandler& OnError)
{
	FString Authorization = FString::Printf(TEXT("Bearer %s"), *Credentials.GetUserSessionId());
	FString Url = FString::Printf(TEXT("%s/v1/public/namespaces/%s/statitems/value/bulk"), *Settings.StatisticServerUrl, *Credentials.GetUserNamespace());
	FString Verb = TEXT("PUT");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");
	FString Contents = "[";
	FString Content;
	for (int i = 0; i < data.Num(); i++)
	{
		FJsonObjectConverter::UStructToJsonObjectString(data[i], Content);
		Contents += Content;
		if (i < data.Num() - 1)
		{
			Contents += ",";
		}
		else
		{
			Contents += "]";
		}
	}
	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Contents);
	FRegistry::HttpRetryScheduler.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void Statistic::BulkAddProfileStatItemValue(const FString& ProfileId, const TArray<FAccelByteModelsBulkStatItemInc>& data, const THandler<TArray<FAccelByteModelsBulkStatItemIncResult>>& OnSuccess, const FErrorHandler& OnError)
{
	FString Authorization = FString::Printf(TEXT("Bearer %s"), *Credentials.GetUserSessionId());
	FString Url = FString::Printf(TEXT("%s/v1/public/namespaces/%s/users/%s/profiles/%s/statitems/value/bulk"), *Settings.StatisticServerUrl, *Credentials.GetUserNamespace(), *Credentials.GetUserId(), *ProfileId);
	FString Verb = TEXT("PUT");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");
	FString Contents = "[";
	FString Content;
	for (int i = 0; i < data.Num(); i++)
	{
		FJsonObjectConverter::UStructToJsonObjectString(data[i], Content);
		Contents += Content;
		if (i < data.Num() - 1)
		{
			Contents += ",";
		}
		else
		{
			Contents += "]";
		}
	}
	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Contents);
	FRegistry::HttpRetryScheduler.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void Statistic::AddProfileStatItemValue(const FString& ProfileId, const FString& statCode, const float& inc, const THandler<FAccelByteModelsStatItemIncResult>& OnSuccess, const FErrorHandler& OnError)
{
	FString Authorization = FString::Printf(TEXT("Bearer %s"), *Credentials.GetUserSessionId());
	FString Url = FString::Printf(TEXT("%s/v1/public/namespaces/%s/users/%s/profiles/%s/stats/%s/statitems/value"), *Settings.StatisticServerUrl, *Credentials.GetUserNamespace(), *Credentials.GetUserId(), *ProfileId, *statCode);
	FString Verb = TEXT("PUT");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");
	FString Content = FString::Printf(TEXT("{\"inc\":%f}"), inc);

	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetHeader(TEXT("Authorization"), Authorization);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), ContentType);
	Request->SetHeader(TEXT("Accept"), Accept);
	Request->SetContentAsString(Content);
	FRegistry::HttpRetryScheduler.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void Statistic::BulkAddUserStatItemValue(const TArray<FAccelByteModelsBulkStatItemInc>& data, const THandler<TArray<FAccelByteModelsBulkStatItemIncResult>>& OnSuccess, const FErrorHandler& OnError)
{
    FString Authorization = FString::Printf(TEXT("Bearer %s"), *Credentials.GetUserSessionId());
    FString Url = FString::Printf(TEXT("%s/v1/public/namespaces/%s/users/%s/statitems/value/bulk"), *Settings.StatisticServerUrl, *Credentials.GetUserNamespace(), *Credentials.GetUserId());
    FString Verb = TEXT("PUT");
    FString ContentType = TEXT("application/json");
    FString Accept = TEXT("application/json");
    FString Contents = "[";
    FString Content;
    for (int i = 0; i < data.Num(); i++)
    {
        FJsonObjectConverter::UStructToJsonObjectString(data[i], Content);
        Contents += Content;
        if (i < data.Num() - 1)
        {
            Contents += ",";
        }
        else
        {
            Contents += "]";
        }
    }
    FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetHeader(TEXT("Authorization"), Authorization);
    Request->SetVerb(Verb);
    Request->SetHeader(TEXT("Content-Type"), ContentType);
    Request->SetHeader(TEXT("Accept"), Accept);
    Request->SetContentAsString(Contents);
    FRegistry::HttpRetryScheduler.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

void Statistic::AddUserStatItemValue(const FString& statCode, const float& inc, const THandler<FAccelByteModelsStatItemIncResult>& OnSuccess, const FErrorHandler& OnError)
{
    FString Authorization = FString::Printf(TEXT("Bearer %s"), *Credentials.GetUserSessionId());
    FString Url = FString::Printf(TEXT("%s/v1/public/namespaces/%s/users/%s/stats/%s/statitems/value"), *Settings.StatisticServerUrl, *Credentials.GetUserNamespace(), *Credentials.GetUserId(), *statCode);
    FString Verb = TEXT("PUT");
    FString ContentType = TEXT("application/json");
    FString Accept = TEXT("application/json");
    FString Content = FString::Printf(TEXT("{\"inc\":%f}"), inc);

    FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetHeader(TEXT("Authorization"), Authorization);
    Request->SetVerb(Verb);
    Request->SetHeader(TEXT("Content-Type"), ContentType);
    Request->SetHeader(TEXT("Accept"), Accept);
    Request->SetContentAsString(Content);
    FRegistry::HttpRetryScheduler.ProcessRequest(Request, CreateHttpResultHandler(OnSuccess, OnError), FPlatformTime::Seconds());
}

} // Namespace Api
} // Namespace AccelByte
