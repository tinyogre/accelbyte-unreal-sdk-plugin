// Copyright (c) 2023 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "GameServerApi/AccelByteServerUGCApi.h"
#include "Core/AccelByteRegistry.h"
#include "Core/AccelByteHttpRetryScheduler.h"
#include "Core/AccelByteReport.h"
#include "Core/AccelByteUtilities.h"
#include "Core/AccelByteServerSettings.h"

namespace AccelByte
{
namespace GameServerApi
{

ServerUGC::ServerUGC(ServerCredentials const& InCredentialsRef
	, ServerSettings const& InSettingsRef
	, FHttpRetryScheduler& InHttpRef)
	: FServerApiBase(InCredentialsRef, InSettingsRef, InHttpRef)
{}

ServerUGC::~ServerUGC()
{}

void ServerUGC::SearchContents(FAccelByteModelsUGCSearchContentsRequest const& Request
	, THandler<FAccelByteModelsUGCSearchContentsPagingResponse> const& OnSuccess
	, FErrorHandler const& OnError
	, int32 Limit
	, int32 Offset)
{
	FReport::Log(FString(__FUNCTION__));

	FString Url = FString::Printf(TEXT("%s/v1/admin/namespaces/%s/contents/search")
		, *ServerSettingsRef.UGCServerUrl
		, *ServerCredentialsRef->GetNamespace());

	TMultiMap<FString, FString> QueryParams {
		{ TEXT("sortby"), FAccelByteUGCUtilities::ConvertUGCSortByToString(Request.SortBy) },
		{ TEXT("orderby"), FAccelByteUGCUtilities::ConvertUGCOrderByToString(Request.OrderBy) },
		{ TEXT("name"), Request.Name },
		{ TEXT("creator"), Request.Creator },
		{ TEXT("type"), Request.Type },
		{ TEXT("subtype"), Request.Subtype },
		{ TEXT("isofficial"), Request.bIsOfficial ? TEXT("true") : TEXT("false") },
		{ TEXT("ishidden"), Request.bIsHidden ? TEXT("true") : TEXT("false") },
		{ TEXT("limit"), Limit >= 0 ? FString::FromInt(Limit) : TEXT("") },
		{ TEXT("offset"), Offset >= 0 ? FString::FromInt(Offset) : TEXT("") },
		{ TEXT("userId"), Request.UserId }
	};

	for (const auto& Tag : Request.Tags)
	{
		QueryParams.AddUnique(TEXT("tags"), Tag);
	}

	HttpClient.ApiRequest(TEXT("GET"), Url, QueryParams, FString(), OnSuccess, OnError);
}

void ServerUGC::SearchContentsSpecificToChannel(FString const& ChannelId
	, FAccelByteModelsUGCSearchContentsRequest const& Request
	, THandler<FAccelByteModelsUGCSearchContentsPagingResponse> const& OnSuccess
	, FErrorHandler const& OnError
	, int32 Limit
	, int32 Offset)
{
	FReport::Log(FString(__FUNCTION__));

	if (ChannelId.IsEmpty())
	{
		OnError.ExecuteIfBound(static_cast<int32>(ErrorCodes::InvalidRequest), TEXT("Invalid request, ContentId is empty."));
		return;
	}
	if (!ValidateAccelByteId(ChannelId, EAccelByteIdHypensRule::NO_RULE
		, FAccelByteIdValidator::GetChannelIdInvalidMessage(ChannelId)
		, OnError))
	{
		return;
	}

	FString Url = FString::Printf(TEXT("%s/v1/admin/namespaces/%s/channels/%s/contents/search")
		, *ServerSettingsRef.UGCServerUrl
		, *ServerCredentialsRef->GetNamespace()
		, *ChannelId);

	TMultiMap<FString, FString> QueryParams {
		{ TEXT("sortby"), FAccelByteUGCUtilities::ConvertUGCSortByToString(Request.SortBy) },
		{ TEXT("orderby"), FAccelByteUGCUtilities::ConvertUGCOrderByToString(Request.OrderBy) },
		{ TEXT("name"), Request.Name },
		{ TEXT("creator"), Request.Creator },
		{ TEXT("type"), Request.Type },
		{ TEXT("subtype"), Request.Subtype },
		{ TEXT("isofficial"), Request.bIsOfficial ? TEXT("true") : TEXT("false") },
		{ TEXT("ishidden"), Request.bIsHidden ? TEXT("true") : TEXT("false") },
		{ TEXT("limit"), Limit >= 0 ? FString::FromInt(Limit) : TEXT("") },
		{ TEXT("offset"), Offset >= 0 ? FString::FromInt(Offset) : TEXT("") },
		{ TEXT("userId"), Request.UserId }
	};

	for (const auto& Tag : Request.Tags)
	{
		QueryParams.AddUnique(TEXT("tags"), Tag);
	}

	HttpClient.ApiRequest(TEXT("GET"), Url, QueryParams, FString(), OnSuccess, OnError);
}

void ServerUGC::ModifyContentByShareCode(FString const& UserId
	, FString const& ChannelId
	, FString const& ShareCode
	, FAccelByteModelsUGCUpdateRequest const& ModifyRequest
	, THandler<FAccelByteModelsUGCResponse> const& OnSuccess
	, FErrorHandler const& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	if (ChannelId.IsEmpty())
	{
		OnError.ExecuteIfBound(static_cast<int32>(ErrorCodes::InvalidRequest), TEXT("Invalid request, ChannelId is empty."));
		return;
	}

	if (ShareCode.IsEmpty())
	{
		OnError.ExecuteIfBound(static_cast<int32>(ErrorCodes::InvalidRequest), TEXT("Invalid request, ShareCode is empty."));
		return;
	}
	
	if (!ValidateAccelByteId(ChannelId, EAccelByteIdHypensRule::NO_RULE
		, FAccelByteIdValidator::GetChannelIdInvalidMessage(ChannelId)
		, OnError))
	{
		return;
	}

	FString Url = FString::Printf(TEXT("%s/v1/admin/namespaces/%s/users/%s/channels/%s/contents/s3/sharecodes/%s")
		, *ServerSettingsRef.UGCServerUrl
		, *ServerCredentialsRef->GetNamespace()
		, *UserId
		, *ChannelId
		, *ShareCode);

	FAccelByteModelsUGCUpdateRequest Request = ModifyRequest;
	if (Request.ContentType.IsEmpty())
	{
		Request.ContentType = TEXT("application/octet-stream");
	}

	HttpClient.ApiRequest(TEXT("PUT"), Url, {}, Request, OnSuccess, OnError);
}

void ServerUGC::DeleteContentByShareCode(FString const& UserId
	, FString const& ChannelId
	, FString const& ShareCode
	, FVoidHandler const& OnSuccess
	, FErrorHandler const& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	if (ChannelId.IsEmpty())
	{
		OnError.ExecuteIfBound(static_cast<int32>(ErrorCodes::InvalidRequest), TEXT("Invalid request, ChannelId is empty."));
		return;
	}

	if (ShareCode.IsEmpty())
	{
		OnError.ExecuteIfBound(static_cast<int32>(ErrorCodes::InvalidRequest), TEXT("Invalid request, ShareCode is empty."));
		return;
	}
	
	if (!ValidateAccelByteId(ChannelId, EAccelByteIdHypensRule::NO_RULE
		, FAccelByteIdValidator::GetChannelIdInvalidMessage(ChannelId)
		, OnError))
	{
		return;
	}

	FString Url = FString::Printf(TEXT("%s/v1/admin/namespaces/%s/users/%s/channels/%s/contents/sharecodes/%s")
		, *ServerSettingsRef.UGCServerUrl
		, *ServerCredentialsRef->GetNamespace()
		, *UserId
		, *ChannelId
		, *ShareCode);

	HttpClient.ApiRequest(TEXT("DELETE"), Url, {}, FString(), OnSuccess, OnError);
}

void ServerUGC::PublicGetContentByContentId(FString const& ContentId, THandler<FAccelByteModelsUGCContentResponse> const& OnSuccess, FErrorHandler const& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	if (ContentId.IsEmpty())
	{
		OnError.ExecuteIfBound(static_cast<int32>(ErrorCodes::InvalidRequest), TEXT("Url is invalid. ContentId is empty."));
		return;
	}

	if (!ValidateAccelByteId(ContentId, EAccelByteIdHypensRule::NO_RULE
		, FAccelByteIdValidator::GetContentIdInvalidMessage(ContentId)
		, OnError))
	{
		OnError.ExecuteIfBound(static_cast<int32>(ErrorCodes::InvalidRequest), TEXT("Invalid request, ContentId is not valid."));
		return;
	}

	const FString Url = FString::Printf(TEXT("%s/v1/public/namespaces/%s/contents/%s")
		, *ServerSettingsRef.UGCServerUrl
		, *GetNamespace()
		, *ContentId);

	HttpClient.Request(TEXT("GET"), Url, {}, FString(), GetDefaultHeaders(), OnSuccess, OnError);
}

void ServerUGC::PublicGetContentByShareCode(FString const& ShareCode, THandler<FAccelByteModelsUGCContentResponse> const& OnSuccess, FErrorHandler const& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	const FString Url = FString::Printf(TEXT("%s/v1/public/namespaces/%s/contents/sharecodes/%s")
		, *ServerSettingsRef.UGCServerUrl
		, *GetNamespace()
		, *ShareCode);

	HttpClient.Request(TEXT("GET"), Url, {}, FString(), GetDefaultHeaders(), OnSuccess, OnError);
}

void ServerUGC::InternalSearchContents(const FString& Name, const FString& Creator, const FString& Type, const FString& Subtype, const TArray<FString>& Tags, bool IsOfficial, const FString& UserId, EAccelByteUgcSortBy SortBy, EAccelByteUgcOrderBy OrderBy, int32 Limit, int32 Offset, FString& OutUrl, TMultiMap<FString, FString>& OutQueryParams)
{
	OutUrl = FString::Printf(TEXT("%s/v1/public/namespaces/%s/contents")
		, *ServerSettingsRef.UGCServerUrl
		, *GetNamespace());

	OutQueryParams = TMultiMap<FString, FString>{
		{ TEXT("sortby"), FAccelByteUGCUtilities::ConvertUGCSortByToString(SortBy) },
		{ TEXT("orderby"), FAccelByteUGCUtilities::ConvertUGCOrderByToString(OrderBy) },
		{ TEXT("name"), Name },
		{ TEXT("creator"), Creator },
		{ TEXT("type"), Type },
		{ TEXT("subtype"), Subtype },
		{ TEXT("isofficial"), IsOfficial ? TEXT("true") : TEXT("false") },
		{ TEXT("limit"), Limit >= 0 ? FString::FromInt(Limit) : TEXT("") },
		{ TEXT("offset"), Offset >= 0 ? FString::FromInt(Offset) : TEXT("") },
		{ TEXT("userId"), UserId }
	};

	for (const auto& Tag : Tags)
	{
		OutQueryParams.AddUnique(TEXT("tags"), Tag);
	}
}

void ServerUGC::PublicSearchContents(const FString & Name, const FString & Creator, const FString & Type, const FString & Subtype, const TArray<FString>&Tags, bool IsOfficial, const FString & UserId, THandler<FAccelByteModelsUGCSearchContentsPagingResponse> const& OnSuccess, FErrorHandler const& OnError, EAccelByteUgcSortBy SortBy, EAccelByteUgcOrderBy OrderBy, int32 Limit, int32 Offset)
{
	FReport::Log(FString(__FUNCTION__));

	FString Url;
	TMultiMap<FString, FString> QueryParams;
	InternalSearchContents(Name, Creator, Type, Subtype, Tags, IsOfficial, UserId, SortBy, OrderBy, Limit, Offset, Url, QueryParams);

	HttpClient.Request(TEXT("GET"), Url, QueryParams, GetDefaultHeaders(), OnSuccess, OnError);
}

void ServerUGC::PublicSearchContents(FAccelByteModelsUGCSearchContentsRequest const& Request, THandler<FAccelByteModelsUGCSearchContentsPagingResponse> const& OnSuccess, FErrorHandler const& OnError, int32 Limit, int32 Offset)
{
	FReport::Log(FString(__FUNCTION__));

	FString Url;
	TMultiMap<FString, FString> QueryParams;
	InternalSearchContents(Request.Name, Request.Creator, Request.Type, Request.Subtype, Request.Tags, Request.bIsOfficial, Request.UserId, Request.SortBy, Request.OrderBy, Limit, Offset, Url, QueryParams);

	HttpClient.Request(TEXT("GET"), Url, QueryParams, GetDefaultHeaders(), OnSuccess, OnError);
}

void ServerUGC::PublicGetContentBulk(const TArray<FString>&ContentIds, THandler<TArray<FAccelByteModelsUGCContentResponse>> const& OnSuccess, FErrorHandler const& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	const FString Url = FString::Printf(TEXT("%s/v1/public/namespaces/%s/contents/bulk")
		, *ServerSettingsRef.UGCServerUrl
		, *GetNamespace());

	FAccelByteModelsUGCGetContentBulkRequest ContentRequest;
	ContentRequest.ContentIds = ContentIds;

	FString ContentString;
	FJsonObjectConverter::UStructToJsonObjectString(ContentRequest, ContentString);

	HttpClient.Request(TEXT("POST"), Url, ContentString, GetDefaultHeaders(), OnSuccess, OnError);
}

void ServerUGC::PublicGetUserContent(const FString & UserId, THandler<FAccelByteModelsUGCContentPageResponse> const& OnSuccess, FErrorHandler const& OnError, int32 Limit, int32 Offset)
{
	FReport::Log(FString(__FUNCTION__));

	if (!ValidateAccelByteId(UserId, EAccelByteIdHypensRule::NO_HYPENS
		, FAccelByteIdValidator::GetUserIdInvalidMessage(UserId)
		, OnError))
	{
		return;
	}

	const FString Url = FString::Printf(TEXT("%s/v1/public/namespaces/%s/users/%s/contents")
		, *ServerSettingsRef.UGCServerUrl
		, *GetNamespace()
		, *UserId);

	TMultiMap<FString, FString> QueryParams;
	if (Offset > 0)
	{
		QueryParams.Add(TEXT("offset"), FString::FromInt(Offset));
	}
	if (Limit > 0)
	{
		QueryParams.Add(TEXT("limit"), FString::FromInt(Limit));
	}

	HttpClient.Request(TEXT("GET"), Url, QueryParams, GetDefaultHeaders(), OnSuccess, OnError);
}

void ServerUGC::PublicSearchContentsSpecificToChannelV2(FString const& ChannelId, THandler<FAccelByteModelsUGCSearchContentsPagingResponseV2> const& OnSuccess, FErrorHandler const& OnError, int32 Limit, int32 Offset, EAccelByteUGCContentUtilitiesSortByV2 SortBy)
{
	FReport::Log(FString(__FUNCTION__));

	if (ChannelId.IsEmpty())
	{
		OnError.ExecuteIfBound(static_cast<int32>(ErrorCodes::InvalidRequest), TEXT("Invalid request, ChannelId is empty."));
		return;
	}
	if (!ValidateAccelByteId(ChannelId, EAccelByteIdHypensRule::NO_RULE
		, FAccelByteIdValidator::GetChannelIdInvalidMessage(ChannelId)
		, OnError))
	{
		return;
	}

	const FString Url = FString::Printf(TEXT("%s/v2/public/namespaces/%s/channels/%s/contents")
		, *ServerSettingsRef.UGCServerUrl
		, *GetNamespace()
		, *ChannelId);

	const TMap<FString, FString> QueryParams = {
		{TEXT("offset"), Offset >= 0 ? FString::FromInt(Offset) : TEXT("")},
		{TEXT("limit"), Limit >= 0 ? FString::FromInt(Limit) : TEXT("")},
		{TEXT("sortBy"), SortBy == EAccelByteUGCContentUtilitiesSortByV2::NONE ? TEXT("") : FAccelByteUGCUtilities::ConvertUGCUtilitiesSortByToString(SortBy)}
	};

	HttpClient.Request(TEXT("GET"), Url, QueryParams, GetDefaultHeaders(), OnSuccess, OnError);
}

void ServerUGC::InternalSearchContentsV2(FAccelByteModelsUGCFilterRequestV2 const& Filter, int32 Limit, int32 Offset, EAccelByteUGCContentSortByV2 SortBy, FString & OutUrl, TMap<FString, FString>&OutQueryParams)
{
	OutUrl = FString::Printf(TEXT("%s/v2/public/namespaces/%s/contents")
		, *ServerSettingsRef.UGCServerUrl
		, *GetNamespace());

	FString TagValue = FString::Join(Filter.Tags, TEXT(","));

	OutQueryParams = TMap<FString, FString>{
		{TEXT("name"), Filter.Name.IsEmpty() ? TEXT("") : Filter.Name},
		{TEXT("type"), Filter.Type.IsEmpty() ? TEXT("") : Filter.Type},
		{TEXT("subType"), Filter.SubType.IsEmpty() ? TEXT("") : Filter.SubType},
		{TEXT("offset"), Offset >= 0 ? FString::FromInt(Offset) : TEXT("")},
		{TEXT("limit"), Limit >= 0 ? FString::FromInt(Limit) : TEXT("")},
		{TEXT("sortBy"), FAccelByteUGCUtilities::ConvertGetUGContentsSortByToString(SortBy)},
		{TEXT("tags"), TagValue.IsEmpty() ? TEXT("") : TagValue}
	};
}

const FString ServerUGC::GetNamespace()
{
	if (ServerCredentialsRef->GetSessionState() == BaseCredentials::ESessionState::Valid)
	{
		return ServerCredentialsRef->GetNamespace();
	}
	return ServerSettingsRef.Namespace;
}

const TMap<FString, FString> ServerUGC::GetDefaultHeaders()
{
	auto Headers = ServerCredentialsRef->GetAuthHeader();
	Headers.Add(TEXT("Content-Type"), TEXT("application/json"));

	return Headers;
}

void ServerUGC::PublicSearchContentsV2(FAccelByteModelsUGCFilterRequestV2 const& Filter, THandler<FAccelByteModelsUGCSearchContentsPagingResponseV2> const& OnSuccess, FErrorHandler const& OnError, int32 Limit, int32 Offset, EAccelByteUGCContentSortByV2 SortBy)
{
	FReport::Log(FString(__FUNCTION__));

	FString Url;
	TMap<FString, FString> QueryParams;
	InternalSearchContentsV2(Filter, Limit, Offset, SortBy, Url, QueryParams);

	HttpClient.Request(TEXT("GET"), Url, QueryParams, GetDefaultHeaders(), OnSuccess, OnError);
}

void ServerUGC::PublicGetContentBulkByIdsV2(TArray<FString> const& ContentIds, THandler<TArray<FAccelByteModelsUGCContentResponseV2>> const& OnSuccess, FErrorHandler const& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	if (ContentIds.Num() == 0)
	{
		OnError.ExecuteIfBound(static_cast<int32>(ErrorCodes::InvalidRequest), TEXT("Invalid request, ContentIds is empty."));
		return;
	}
	if (ContentIds.Num() > MAX_BULK_CONTENT_IDS_COUNT)
	{
		OnError.ExecuteIfBound(static_cast<int32>(ErrorCodes::InvalidRequest), FString::Printf(TEXT("Keys cannot exceed %d!"), MAX_BULK_CONTENT_IDS_COUNT));
		return;
	}

	const FString Url = FString::Printf(TEXT("%s/v2/public/namespaces/%s/contents/bulk")
		, *ServerSettingsRef.UGCServerUrl
		, *GetNamespace());

	FAccelByteModelsUGCGetContentBulkRequest ContentRequest;
	ContentRequest.ContentIds = ContentIds;

	FString ContentString;
	FJsonObjectConverter::UStructToJsonObjectString(ContentRequest, ContentString);

	HttpClient.Request(TEXT("POST"), Url, ContentString, GetDefaultHeaders(), OnSuccess, OnError);
}

void ServerUGC::PublicGetContentByShareCodeV2(FString const& ShareCode, THandler<FAccelByteModelsUGCContentResponseV2> const& OnSuccess, FErrorHandler const& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	if (ShareCode.IsEmpty())
	{
		OnError.ExecuteIfBound(static_cast<int32>(ErrorCodes::InvalidRequest), TEXT("Invalid request, ShareCode is empty."));
		return;
	}

	const FString Url = FString::Printf(TEXT("%s/v2/public/namespaces/%s/contents/sharecodes/%s")
		, *ServerSettingsRef.UGCServerUrl
		, *GetNamespace()
		, *ShareCode);

	HttpClient.Request(TEXT("GET"), Url, {}, FString(), GetDefaultHeaders(), OnSuccess, OnError);
}

void ServerUGC::PublicGetContentByContentIdV2(FString const& ContentId, THandler<FAccelByteModelsUGCContentResponseV2> const& OnSuccess, FErrorHandler const& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	if (ContentId.IsEmpty())
	{
		OnError.ExecuteIfBound(static_cast<int32>(ErrorCodes::InvalidRequest), TEXT("Invalid request, ContentId is empty."));
		return;
	}
	if (!ValidateAccelByteId(ContentId, EAccelByteIdHypensRule::NO_RULE
		, FAccelByteIdValidator::GetContentIdInvalidMessage(ContentId)
		, OnError))
	{
		return;
	}

	const FString Url = FString::Printf(TEXT("%s/v2/public/namespaces/%s/contents/%s")
		, *ServerSettingsRef.UGCServerUrl
		, *GetNamespace()
		, *ContentId);

	HttpClient.Request(TEXT("GET"), Url, {}, FString(), GetDefaultHeaders(), OnSuccess, OnError);
}

void ServerUGC::PublicGetUserContentsV2(FString const& UserId, THandler<FAccelByteModelsUGCSearchContentsPagingResponseV2> const& OnSuccess, FErrorHandler const& OnError, int32 Limit, int32 Offset, EAccelByteUGCContentUtilitiesSortByV2 SortBy)
{
	FReport::Log(FString(__FUNCTION__));

	if (UserId.IsEmpty())
	{
		OnError.ExecuteIfBound(static_cast<int32>(ErrorCodes::InvalidRequest), TEXT("Invalid request, UserId is empty."));
		return;
	}
	if (!ValidateAccelByteId(UserId, EAccelByteIdHypensRule::NO_HYPENS
		, FAccelByteIdValidator::GetUserIdInvalidMessage(UserId)
		, OnError))
	{
		return;
	}

	const FString Url = FString::Printf(TEXT("%s/v2/public/namespaces/%s/users/%s/contents")
		, *ServerSettingsRef.UGCServerUrl
		, *GetNamespace()
		, *UserId);

	const TMap<FString, FString> QueryParams = {
		{TEXT("offset"), Offset >= 0 ? FString::FromInt(Offset) : TEXT("")},
		{TEXT("limit"), Limit >= 0 ? FString::FromInt(Limit) : TEXT("")},
		{TEXT("sortBy"), SortBy == EAccelByteUGCContentUtilitiesSortByV2::NONE ? TEXT("") : FAccelByteUGCUtilities::ConvertUGCUtilitiesSortByToString(SortBy)}
	};

	HttpClient.Request(TEXT("GET"), Url, QueryParams, FString(), GetDefaultHeaders(), OnSuccess, OnError);
}

} // Namespace GameServerApi
} // Namespace AccelByte
