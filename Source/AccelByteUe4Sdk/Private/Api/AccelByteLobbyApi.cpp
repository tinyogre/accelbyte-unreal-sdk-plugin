// Copyright (c) 2018 - 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "Api/AccelByteLobbyApi.h"
#include "Modules/ModuleManager.h"
#include "IWebSocket.h"
#include "Api/AccelByteQos.h"
#include "Core/AccelByteCredentials.h"
#include "Core/AccelByteHttpClient.h"
#include "Core/AccelByteRegistry.h"
#include "Core/AccelByteReport.h"
#include "Core/AccelByteHttpRetryScheduler.h"
#include "Core/AccelByteSettings.h"
#include "Core/IWebSocketFactory.h"
#include "Core/FUnrealWebSocketFactory.h"
#include "Core/IAccelByteTokenGenerator.h"
#include "Core/AccelByteError.h"
#include "Core/AccelByteMessageParser.h"
#include "Core/AccelByteUtilities.h"
#include "JsonUtilities.h"
#include "Engine.h"

DEFINE_LOG_CATEGORY(LogAccelByteLobby);

namespace AccelByte
{
namespace Api
{
	namespace LobbyRequest
	{
		// Party
		const FString PartyInfo = TEXT("partyInfoRequest");
		const FString PartyCreate = TEXT("partyCreateRequest");
		const FString PartyLeave = TEXT("partyLeaveRequest");
		const FString PartyInvite = TEXT("partyInviteRequest");
		const FString PartyJoin = TEXT("partyJoinRequest");
		const FString PartyReject = TEXT("partyRejectRequest");
		const FString PartyKick = TEXT("partyKickRequest");
		const FString PartyGenerateCode = TEXT("partyGenerateCodeRequest");
		const FString PartyGetCode = TEXT("partyGetCodeRequest");
		const FString PartyDeleteCode = TEXT("partyDeleteCodeRequest");
		const FString PartyJoinViaCode = TEXT("partyJoinViaCodeRequest");
		const FString PartyPromoteLeader = TEXT("partyPromoteLeaderRequest");
		const FString PartySendNotif = TEXT("partySendNotifRequest");

		// Chat
		const FString PersonalChat = TEXT("personalChatRequest");
		const FString PartyChat = TEXT("partyChatRequest");
		const FString JoinDefaultChannelChat = TEXT("joinDefaultChannelRequest");
		const FString ChannelChat = TEXT("sendChannelChatRequest");

		// Presence
		const FString SetUserPresence = TEXT("setUserStatusRequest");
		const FString GetAllFriendsStatus = TEXT("friendsStatusRequest");

		// Notification

		// Matchmaking
		const FString MatchmakingStart = TEXT("startMatchmakingRequest");
		const FString MatchmakingCancel = TEXT("cancelMatchmakingRequest");
		const FString ReadyConsent = TEXT("setReadyConsentRequest");
		const FString RejectConsent = TEXT("setRejectConsentRequest");

		// Custom Game
		const FString CreateDS = TEXT("createDSRequest");

		// Friends
		const FString RequestFriends = TEXT("requestFriendsRequest");
		const FString RequestFriendsByPublicId = TEXT("requestFriendsByPublicIDRequest");
		const FString Unfriend = TEXT("unfriendRequest");
		const FString ListOutgoingFriends = TEXT("listOutgoingFriendsRequest");
		const FString CancelFriends = TEXT("cancelFriendsRequest");
		const FString ListIncomingFriends = TEXT("listIncomingFriendsRequest");
		const FString AcceptFriends = TEXT("acceptFriendsRequest");
		const FString RejectFriends = TEXT("rejectFriendsRequest");
		const FString LoadFriendList = TEXT("listOfFriendsRequest");
		const FString GetFriendshipStatus = TEXT("getFriendshipStatusRequest");

		// Block Users
		const FString BlockPlayer = TEXT("blockPlayerRequest");
		const FString UnblockPlayer = TEXT("unblockPlayerRequest");

		//Signaling
		const FString SignalingP2PNotif = TEXT("signalingP2PNotif");

		//Session Attribute
		const FString SetSessionAttribute = TEXT("setSessionAttributeRequest");
		const FString GetSessionAttribute = TEXT("getSessionAttributeRequest");
		const FString GetAllSessionAttribute = TEXT("getAllSessionAttributeRequest");

		// Refresh Token
		const FString RefreshToken = TEXT("refreshTokenRequest");
	}

	namespace LobbyResponse
	{
		// default
		const FString ConnectedNotif = TEXT("connectNotif");
		const FString DisconnectNotif = TEXT("disconnectNotif");

		// Party
		const FString PartyInfo = TEXT("partyInfoResponse");
		const FString PartyCreate = TEXT("partyCreateResponse");
		const FString PartyLeave = TEXT("partyLeaveResponse");
		const FString PartyMemberLeaveNotif = TEXT("partyLeaveNotif");
		const FString PartyInvite = TEXT("partyInviteResponse");
		const FString PartyInviteNotif = TEXT("partyInviteNotif");
		const FString PartyGetInvitedNotif = TEXT("partyGetInvitedNotif");
		const FString PartyJoin = TEXT("partyJoinResponse");
		const FString PartyJoinNotif = TEXT("partyJoinNotif");
		const FString PartyReject = TEXT("partyRejectResponse");
		const FString PartyRejectNotif = TEXT("partyRejectNotif");
		const FString PartyKick = TEXT("partyKickResponse");
		const FString PartyKickNotif = TEXT("partyKickNotif");
		const FString PartyDataUpdateNotif = TEXT("partyDataUpdateNotif");
		const FString PartyMemberConnectNotif = TEXT("partyConnectNotif");
		const FString PartyMemberDisconnectNotif = TEXT("partyDisconnectNotif");
		const FString PartyGenerateCode = TEXT("partyGenerateCodeResponse");
		const FString PartyGetCode = TEXT("partyGetCodeResponse");
		const FString PartyDeleteCode = TEXT("partyDeleteCodeResponse");
		const FString PartyJoinViaCode = TEXT("partyJoinViaCodeResponse");
		const FString PartyPromoteLeader = TEXT("partyPromoteLeaderResponse");
		const FString PartySendNotif = TEXT("partySendNotifResponse");
		const FString PartyNotif = TEXT("partyNotif");

		// Chat
		const FString PersonalChat = TEXT("personalChatResponse");
		const FString PersonalChatNotif = TEXT("personalChatNotif");
		const FString PartyChat = TEXT("partyChatResponse");
		const FString PartyChatNotif = TEXT("partyChatNotif");
		const FString JoinChannelChat = TEXT("joinDefaultChannelResponse");
		const FString ChannelChat = TEXT("sendChannelChatResponse");
		const FString ChannelChatNotif = TEXT("channelChatNotif");

		// Presence
		const FString SetUserPresence = TEXT("setUserStatusResponse");
		const FString FriendStatusNotif = TEXT("userStatusNotif");
		const FString GetAllFriendsStatus = TEXT("friendsStatusResponse");

		// Notification
		const FString MessageNotif = TEXT("messageNotif");
		const FString UserBannedNotification = TEXT("userBannedNotification");
		const FString UserUnbannedNotification = TEXT("userUnbannedNotification");

		// Matchmaking
		const FString MatchmakingStart = TEXT("startMatchmakingResponse");
		const FString MatchmakingCancel = TEXT("cancelMatchmakingResponse");
		const FString ReadyConsent = TEXT("setReadyConsentResponse");
		const FString RejectConsent = TEXT("setRejectConsentResponse");
		const FString MatchmakingNotif = TEXT("matchmakingNotif");
		const FString ReadyConsentNotif = TEXT("setReadyConsentNotif");
		const FString RejectConsentNotif = TEXT("setRejectConsentNotif");
		const FString RematchmakingNotif = TEXT("rematchmakingNotif");
		const FString DsNotif = TEXT("dsNotif");

		// Custom Game
		const FString CreateDS = TEXT("createDSResponse");

		// Friends
		const FString RequestFriends = TEXT("requestFriendsResponse");
		const FString RequestFriendsByPublicId = TEXT("requestFriendsByPublicIDResponse");
		const FString Unfriend = TEXT("unfriendResponse");
		const FString ListOutgoingFriends = TEXT("listOutgoingFriendsResponse");
		const FString CancelFriends = TEXT("cancelFriendsResponse");
		const FString ListIncomingFriends = TEXT("listIncomingFriendsResponse");
		const FString AcceptFriends = TEXT("acceptFriendsResponse");
		const FString RejectFriends = TEXT("rejectFriendsResponse");
		const FString LoadFriendList = TEXT("listOfFriendsResponse");
		const FString GetFriendshipStatus = TEXT("getFriendshipStatusResponse");

		// Friends + Notification
		const FString AcceptFriendsNotif = TEXT("acceptFriendsNotif");
		const FString RequestFriendsNotif = TEXT("requestFriendsNotif");
		const FString UnfriendNotif = TEXT("unfriendNotif");
		const FString CancelFriendsNotif = TEXT("cancelFriendsNotif");
		const FString RejectFriendsNotif = TEXT("rejectFriendsNotif");

		// Block
		const FString BlockPlayer = TEXT("blockPlayerResponse");
		const FString UnblockPlayer = TEXT("unblockPlayerResponse");

		// Block + Notification
		const FString BlockPlayerNotif = TEXT("blockPlayerNotif");
		const FString UnblockPlayerNotif = TEXT("unblockPlayerNotif");

		//Signaling
		const FString SignalingP2PNotif = TEXT("signalingP2PNotif");

		// Error Notif
		const FString ErrorNotif = TEXT("errorNotif");
		
		//Session Attribute
		const FString SetSessionAttribute = TEXT("setSessionAttributeResponse");
		const FString GetSessionAttribute = TEXT("getSessionAttributeResponse");
		const FString GetAllSessionAttribute = TEXT("getAllSessionAttributeResponse");
		
		// Refresh Token
		const FString RefreshToken = TEXT("refreshTokenResponse");

		// V2 session notif
		const FString SessionNotif = TEXT("messageSessionNotif");
	}

	namespace Prefix
	{
		const FString Party = TEXT("party");
		const FString Chat = TEXT("chat");
		const FString Presence = TEXT("presence");
		const FString Matchmaking = TEXT("matchmaking");
		const FString Friends = TEXT("friends");
		const FString Block = TEXT("blocks");
		const FString Signaling = TEXT("signaling");
		const FString Attribute = TEXT("attribute");
		const FString Token = TEXT("token");
	}

	namespace Suffix
	{
		const FString Response = TEXT("Response");
		const FString Notif = TEXT("Notif"); // Note: current usage is not yet uniformized -> Notif/Notification
	}

	enum Response : uint8
	{
		Invalid_Response,

		// Party
		PartyInfo,
		PartyCreate,
		PartyLeave,
		PartyInvite,
		PartyJoin,
		PartyReject,
		PartyKick,
		PartyGenerateCode,
		PartyGetCode,
		PartyDeleteCode,
		PartyJoinViaCode,
		PartyPromoteLeader,
		PartySendNotif,

		// Chat
		PersonalChat,
		PartyChat,
		JoinChannelChat,
		ChannelChat,

		// Presence
		SetUserPresence,
		GetAllFriendsStatus,

		// Matchmaking
		MatchmakingStart,
		MatchmakingCancel,
		ReadyConsent,
		RejectConsent,

		// Custom Game
		CreateDS,

		// Friends
		RequestFriends,
		RequestFriendsByPublicId,
		Unfriend,
		ListOutgoingFriends,
		CancelFriends,
		ListIncomingFriends,
		AcceptFriends,
		RejectFriends,
		LoadFriendList,
		GetFriendshipStatus,

		// Block
		BlockPlayer,
		UnblockPlayer,

		//Session Attribute
		SetSessionAttribute,
		GetSessionAttribute,
		GetAllSessionAttribute,

		// Refresh Token
		RefreshToken,

		MAX_Response,
	};

	enum Notif : uint8
	{
		Invalid_Notif,

		// default
		ConnectedNotif,
		DisconnectNotif,

		// Party
		PartyMemberLeaveNotif,
		PartyInviteNotif,
		PartyGetInvitedNotif,
		PartyJoinNotif,
		PartyRejectNotif,
		PartyKickNotif,
		PartyDataUpdateNotif,
		PartyMemberConnectNotif,
		PartyMemberDisconnectNotif,
		PartyNotif,

		// Chat
		PersonalChatNotif,
		PartyChatNotif,
		ChannelChatNotif,

		// Presence
		FriendStatusNotif,

		// Notification
		MessageNotif,
		UserBannedNotification,
		UserUnbannedNotification,

		// Matchmaking
		MatchmakingNotif,
		ReadyConsentNotif,
		RejectConsentNotif,
		RematchmakingNotif,
		DsNotif,

		// Friends
		AcceptFriendsNotif,
		RequestFriendsNotif,
		UnfriendNotif,
		CancelFriendsNotif,
		RejectFriendsNotif,

		// Block
		BlockPlayerNotif,
		UnblockPlayerNotif,

		// Signaling
		SignalingP2PNotif,

		// Error Notif
		ErrorNotif,

		MAX_Notif,
	};

/**
* Helper macro to enforce uniform naming, easier pair initialization, and readibility
*/
#define FORM_STRING_ENUM_PAIR(Type, MessageType) \
	{ LobbyResponse::MessageType, Type::MessageType } \

	TMap<FString, Response> Lobby::ResponseStringEnumMap{
		FORM_STRING_ENUM_PAIR(Response,PartyInfo),
		FORM_STRING_ENUM_PAIR(Response,PartyCreate),
		FORM_STRING_ENUM_PAIR(Response,PartyLeave),
		FORM_STRING_ENUM_PAIR(Response,PartyInvite),
		FORM_STRING_ENUM_PAIR(Response,PartyJoin),
		FORM_STRING_ENUM_PAIR(Response,PartyReject),
		FORM_STRING_ENUM_PAIR(Response,PartyKick),
		FORM_STRING_ENUM_PAIR(Response,PartyGenerateCode),
		FORM_STRING_ENUM_PAIR(Response,PartyGetCode),
		FORM_STRING_ENUM_PAIR(Response,PartyDeleteCode),
		FORM_STRING_ENUM_PAIR(Response,PartyJoinViaCode),
		FORM_STRING_ENUM_PAIR(Response,PartyPromoteLeader),
		FORM_STRING_ENUM_PAIR(Response,PartySendNotif),
		FORM_STRING_ENUM_PAIR(Response,PersonalChat),
		FORM_STRING_ENUM_PAIR(Response,PartyChat),
		FORM_STRING_ENUM_PAIR(Response,JoinChannelChat),
		FORM_STRING_ENUM_PAIR(Response,ChannelChat),
		FORM_STRING_ENUM_PAIR(Response,SetUserPresence),
		FORM_STRING_ENUM_PAIR(Response,GetAllFriendsStatus),
		FORM_STRING_ENUM_PAIR(Response,MatchmakingStart),
		FORM_STRING_ENUM_PAIR(Response,MatchmakingCancel),
		FORM_STRING_ENUM_PAIR(Response,ReadyConsent),
		FORM_STRING_ENUM_PAIR(Response,RejectConsent),
		FORM_STRING_ENUM_PAIR(Response,RequestFriends),
		FORM_STRING_ENUM_PAIR(Response,RequestFriendsByPublicId),
		FORM_STRING_ENUM_PAIR(Response,Unfriend),
		FORM_STRING_ENUM_PAIR(Response,ListOutgoingFriends),
		FORM_STRING_ENUM_PAIR(Response,CancelFriends),
		FORM_STRING_ENUM_PAIR(Response,ListIncomingFriends),
		FORM_STRING_ENUM_PAIR(Response,AcceptFriends),
		FORM_STRING_ENUM_PAIR(Response,RejectFriends),
		FORM_STRING_ENUM_PAIR(Response,LoadFriendList),
		FORM_STRING_ENUM_PAIR(Response,GetFriendshipStatus),
		FORM_STRING_ENUM_PAIR(Response,BlockPlayer),
		FORM_STRING_ENUM_PAIR(Response,UnblockPlayer),
		FORM_STRING_ENUM_PAIR(Response,SetSessionAttribute),
		FORM_STRING_ENUM_PAIR(Response,GetSessionAttribute),
		FORM_STRING_ENUM_PAIR(Response,GetAllSessionAttribute),
		FORM_STRING_ENUM_PAIR(Response,RefreshToken),
		FORM_STRING_ENUM_PAIR(Response,CreateDS),
	};

	TMap<FString, Notif> Lobby::NotifStringEnumMap{
		FORM_STRING_ENUM_PAIR(Notif,ConnectedNotif),
		FORM_STRING_ENUM_PAIR(Notif,DisconnectNotif),
		FORM_STRING_ENUM_PAIR(Notif,PartyMemberLeaveNotif),
		FORM_STRING_ENUM_PAIR(Notif,PartyInviteNotif),
		FORM_STRING_ENUM_PAIR(Notif,PartyGetInvitedNotif),
		FORM_STRING_ENUM_PAIR(Notif,PartyJoinNotif),
		FORM_STRING_ENUM_PAIR(Notif,PartyRejectNotif),
		FORM_STRING_ENUM_PAIR(Notif,PartyKickNotif),
		FORM_STRING_ENUM_PAIR(Notif,PartyDataUpdateNotif),
		FORM_STRING_ENUM_PAIR(Notif,PartyNotif),
		FORM_STRING_ENUM_PAIR(Notif,PartyMemberConnectNotif),
		FORM_STRING_ENUM_PAIR(Notif,PartyMemberDisconnectNotif),
		FORM_STRING_ENUM_PAIR(Notif,PersonalChatNotif),
		FORM_STRING_ENUM_PAIR(Notif,PartyChatNotif),
		FORM_STRING_ENUM_PAIR(Notif,ChannelChatNotif),
		FORM_STRING_ENUM_PAIR(Notif,FriendStatusNotif),
		FORM_STRING_ENUM_PAIR(Notif,MessageNotif),
		FORM_STRING_ENUM_PAIR(Notif,UserBannedNotification),
		FORM_STRING_ENUM_PAIR(Notif,UserUnbannedNotification),
		FORM_STRING_ENUM_PAIR(Notif,MatchmakingNotif),
		FORM_STRING_ENUM_PAIR(Notif,ReadyConsentNotif),
		FORM_STRING_ENUM_PAIR(Notif,RejectConsentNotif),
		FORM_STRING_ENUM_PAIR(Notif,RematchmakingNotif),
		FORM_STRING_ENUM_PAIR(Notif,DsNotif),
		FORM_STRING_ENUM_PAIR(Notif,AcceptFriendsNotif),
		FORM_STRING_ENUM_PAIR(Notif,RequestFriendsNotif),
		FORM_STRING_ENUM_PAIR(Notif,UnfriendNotif),
		FORM_STRING_ENUM_PAIR(Notif,CancelFriendsNotif),
		FORM_STRING_ENUM_PAIR(Notif,RejectFriendsNotif),
		FORM_STRING_ENUM_PAIR(Notif,BlockPlayerNotif),
		FORM_STRING_ENUM_PAIR(Notif,UnblockPlayerNotif),
		FORM_STRING_ENUM_PAIR(Notif,SignalingP2PNotif),
		FORM_STRING_ENUM_PAIR(Notif,ErrorNotif),
	};

#undef FORM_STRING_ENUM_PAIR

void Lobby::Connect(const FString& Token)
{
	FReport::Log(FString(__FUNCTION__));

	if(TokenGenerator.IsValid() && Token.IsEmpty())
	{
		TokenGenerator->RequestToken();
		return;
	}

	if (!WebSocket.IsValid())
	{
		CreateWebSocket(Token);
	}

	if (WebSocket->IsConnected())
	{
		return;
	}

	WebSocket->Connect();
	UE_LOG(LogAccelByteLobby, Display, TEXT("Connecting to %s"), *SettingsRef.LobbyServerUrl);

	// if shipping build, skip fetching lobby error messages.
	// avoid fetching large json file to store lobby error messages.
#if !UE_BUILD_SHIPPING
	if(LobbyErrorMessages.Num() == 0)
	{
		FetchLobbyErrorMessages();
	}
#endif
}

void Lobby::Disconnect(bool ForceCleanup)
{
	FReport::Log(FString(__FUNCTION__));

	ChannelSlug = "";
	LobbyCredentialsRef.OnTokenRefreshed().Remove(TokenRefreshDelegateHandle);
	TokenRefreshDelegateHandle.Reset();
	if(WebSocket.IsValid())
	{
		WebSocket->Disconnect(ForceCleanup);
	}

	if (GEngine) UE_LOG(LogAccelByteLobby, Display, TEXT("Disconnected"));
}


bool Lobby::IsConnected() const
{
	FReport::Log(FString(__FUNCTION__));

	return WebSocket.IsValid() && WebSocket->IsConnected();
}

void Lobby::SendPing()
{
	FReport::Log(FString(__FUNCTION__));

	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		WebSocket->Send(FString());
	}
}

/**
Helper macro for ErrorHandler, SuccessHandler and IdResponseMap variable name. Made as separate macro to prevent linux-compiler error
*/
#define ID_RESPONSE_MAP(MessageType) MessageId ## MessageType ## ResponseMap
#define MESSAGE_ERROR_HANDLER(MessageType) On ## MessageType ## Error
#define MESSAGE_SUCCESS_HANDLER(MessageType) MessageType ## Response

/**
* @brief will auto cache MessageId-Response pair into the target map after sending the raw request
* @see SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED
*/
#define SEND_RAW_REQUEST_CACHED_RESPONSE(MessageType, MessageIDPrefix, CustomPayload) \
	const FString MessageId = SendRawRequest(LobbyRequest::MessageType, Prefix::MessageIDPrefix, CustomPayload); \
	if (!MessageId.IsEmpty()) \
	{ \
		ID_RESPONSE_MAP(MessageType).Emplace(MessageId, MESSAGE_SUCCESS_HANDLER(MessageType)); \
	} \

/**
* @brief will auto cache MessageId-Response pair into the target map after sending the raw request
* @param MessageType - request message type
* @return MessageId
*/
#define SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(MessageType, MessageIDPrefix, CustomPayload) \
	SEND_RAW_REQUEST_CACHED_RESPONSE(MessageType, MessageIDPrefix, CustomPayload) \
	return MessageId; \

//-------------------------------------------------------------------------------------------------
// Chat
//-------------------------------------------------------------------------------------------------
FString Lobby::SendPrivateMessage(const FString& UserId
	, const FString& Message)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(PersonalChat
		, Chat
		, FString::Printf(TEXT("to: %s\npayload: %s\n"), *UserId, *Message));
}

FString Lobby::SendPartyMessage(const FString& Message)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(PartyChat
		, Chat
		, FString::Printf(TEXT("payload: %s\n"), *Message));
}

FString Lobby::SendJoinDefaultChannelChatRequest()
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(JoinDefaultChannelChat
		, Chat
		, {});
}

FString Lobby::SendChannelMessage(const FString& Message)
{
	FReport::Log(FString(__FUNCTION__));

	if (!ChannelSlug.IsEmpty())
	{
		SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(ChannelChat, Chat,
			FString::Printf(TEXT("channelSlug: %s\npayload: %s\n"), *ChannelSlug, *Message));
	}
	else
	{
		FAccelByteModelsChannelMessageResponse ErrorResult;
		ErrorResult.Code = FString::FromInt((int)ErrorCodes::InvalidRequest);
		ErrorResult.Message = TEXT("You're not in any chat channel.");
		ChannelChatResponse.ExecuteIfBound(ErrorResult);
		return "";
	}
}

//-------------------------------------------------------------------------------------------------
// Party
//-------------------------------------------------------------------------------------------------
FString Lobby::SendInfoPartyRequest()
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(PartyInfo
		, Party
		, {})
}

FString Lobby::SendInfoPartyRequest(const FPartyInfoResponse& OnInfoPartyResponse
	, const FErrorHandler& OnError)
{
	SetInfoPartyResponseDelegate(OnInfoPartyResponse, OnError);
	return SendInfoPartyRequest();
}

FString Lobby::SendCreatePartyRequest()
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(PartyCreate
		, Party
		, {})
}

FString Lobby::SendLeavePartyRequest()
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(PartyLeave
		, Party
		, {})
}

FString Lobby::SendInviteToPartyRequest(const FString& UserId)
{
	FReport::Log(FString(__FUNCTION__));
	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(PartyInvite
		, Party
		, FString::Printf(TEXT("friendID: %s"), *UserId))
}

FString Lobby::SendAcceptInvitationRequest(const FString& PartyId
	, const FString& InvitationToken)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(PartyJoin
		, Party
		, FString::Printf(TEXT("partyID: %s\ninvitationToken: %s"), *PartyId, *InvitationToken))
}

FString Api::Lobby::SendRejectInvitationRequest(const FString& PartyId
	, const FString& InvitationToken)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(PartyReject
		, Party
		, FString::Printf(TEXT("partyID: %s\ninvitationToken: %s"), *PartyId, *InvitationToken))
}

FString Lobby::SendKickPartyMemberRequest(const FString& UserId)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(PartyKick
		, Party
		, FString::Printf(TEXT("memberID: %s\n"), *UserId))
}

FString Lobby::SendPartyGenerateCodeRequest()
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(PartyGenerateCode
		, Party
		, {})
}

FString Lobby::SendPartyGetCodeRequest()
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(PartyGetCode
		, Party
		, {})
}

FString Lobby::SendPartyDeleteCodeRequest()
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(PartyDeleteCode
		, Party
		, {})
}

FString Lobby::SendPartyJoinViaCodeRequest(const FString& partyCode)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(PartyJoinViaCode
		, Party
		, FString::Printf(TEXT("partyCode: %s\n"), *partyCode))
}

FString Lobby::SendPartyPromoteLeaderRequest(const FString& UserId)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(PartyPromoteLeader
		, Party
		, FString::Printf(TEXT("newLeaderUserId: %s\n"), *UserId))
}

FString Lobby::SendNotificationToPartyMember(const FString& Topic
	, const FString& Payload) 
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(PartySendNotif
		, Party
		, FString::Printf(TEXT("topic: %s\npayload: %s"), *Topic, *Payload));
}

void Lobby::SetPartySizeLimit(const FString& PartyId
	, const int32 Limit
	, const FVoidHandler& OnSuccess
	, const FErrorHandler& OnError)
{
	if(Limit <= 0)
	{
		FReport::Log("Party size limit should be above 0");
		return;
	}
	
	FString Authorization = FString::Printf(TEXT("Bearer %s"), *CredentialsRef.GetAccessToken());
	FString Url = FString::Printf(TEXT("%s/lobby/v1/public/party/namespaces/%s/parties/%s/limit")
		, *SettingsRef.BaseUrl
		, *CredentialsRef.GetNamespace()
		, *PartyId);
	FString Verb = TEXT("PUT");
	FString ContentType = TEXT("application/json");
	FString Accept = TEXT("application/json");

	FAccelByteModelsPartySetLimitRequest Content;
	Content.Limit = Limit;

	HttpClient.ApiRequest(TEXT("PUT"), Url, {}, Content, OnSuccess, OnError);
}

//-------------------------------------------------------------------------------------------------
// Presence
//-------------------------------------------------------------------------------------------------
FString Lobby::SendSetPresenceStatus(const EAvailability Availability
	, const FString& Activity)
{
	FReport::Log(FString(__FUNCTION__));
	const FString EscapedActivity = MessageParser::EscapeString(Activity);
	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(SetUserPresence
		, Presence
		, FString::Printf(TEXT("availability: %s\nactivity: %s\n"), *FAccelByteUtilities::GetUEnumValueAsString(Availability).ToLower(), *EscapedActivity))
}

FString Lobby::SendGetOnlineUsersRequest()
{
	return Lobby::SendGetOnlineFriendPresenceRequest();
}

FString Lobby::SendGetOnlineFriendPresenceRequest()
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(GetAllFriendsStatus
		, Presence
		, {})
}

//-------------------------------------------------------------------------------------------------
// Notification
//-------------------------------------------------------------------------------------------------
void Lobby::GetAllAsyncNotification()
{	
	FReport::Log(FString(__FUNCTION__));
	FReport::LogDeprecated(FString(__FUNCTION__), TEXT("Lobby version 2.4.0 and above doesn't support this anymore"));

	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		FString Content = FString::Printf(TEXT("type: offlineNotificationRequest\nid:%s")
			, *FGuid::NewGuid().ToString(EGuidFormats::Digits));
		WebSocket->Send(Content);
		UE_LOG(LogAccelByteLobby, Display, TEXT("Get async notification (id=%s)"), *Content)
	}
}

//-------------------------------------------------------------------------------------------------
// Matchmaking
//-------------------------------------------------------------------------------------------------
FString Lobby::SendStartMatchmaking(FString GameMode
	, FString ServerName
	, FString ClientVersion
	, TArray<TPair<FString, float>> Latencies
	, TMap<FString, FString> PartyAttributes
	, TArray<FString> TempPartyUserIds
	, TArray<FString> ExtraAttributes)
{
	FMatchmakingOptionalParams Params;
	Params.ServerName = ServerName;
	Params.ClientVersion = ClientVersion;
	Params.Latencies = Latencies;
	Params.PartyAttributes = PartyAttributes;
	Params.TempPartyUserIds = TempPartyUserIds;
	Params.ExtraAttributes = ExtraAttributes;

	return SendStartMatchmaking(GameMode, Params);
}

FString Lobby::SendStartMatchmaking(FString GameMode
	, TArray<FString> TempPartyUserIds
	, FString ServerName
	, FString ClientVersion
	, TArray<TPair<FString, float>> Latencies
	, TMap<FString, FString> PartyAttributes
	, TArray<FString> ExtraAttributes)
{
	return SendStartMatchmaking(GameMode
		, ServerName
		, ClientVersion
		, Latencies
		, PartyAttributes
		, TempPartyUserIds
		, ExtraAttributes);
}

FString Lobby::SendStartMatchmaking(FString GameMode
	, TMap<FString, FString> PartyAttributes
	, FString ServerName
	, FString ClientVersion
	, TArray<TPair<FString, float>> Latencies
	, TArray<FString> TempPartyUserIds
	, TArray<FString> ExtraAttributes)
{
	return SendStartMatchmaking(GameMode
		, ServerName
		, ClientVersion
		, Latencies
		, PartyAttributes
		, TempPartyUserIds
		, ExtraAttributes);
}

FString Lobby::SendStartMatchmaking(FString GameMode
	, TMap<FString, FString> PartyAttributes
	, TArray<FString> TempPartyUserIds
	, FString ServerName
	, FString ClientVersion
	, TArray<TPair<FString, float>> Latencies
	, TArray<FString> ExtraAttributes)
{
	return SendStartMatchmaking(GameMode
		, ServerName
		, ClientVersion
		, Latencies
		, PartyAttributes
		, TempPartyUserIds
		, ExtraAttributes);
}

FString Lobby::GetServerLatenciesJsonStr(TArray<TPair<FString, float>> SelectedLatencies)
{
	FString ServerLatencies = TEXT("{");
	for (int i = 0; i < SelectedLatencies.Num(); i++)
	{
		ServerLatencies.Append(FString::Printf(TEXT("\"%s\":%.0f"), *SelectedLatencies[i].Key, SelectedLatencies[i].Value));
		if (i + 1 < SelectedLatencies.Num())
		{
			ServerLatencies.Append(TEXT(","));
		}
	}
	ServerLatencies.Append(TEXT("}"));
	return FString::Printf(TEXT("latencies: %s\n"), *ServerLatencies);	
}

FString Lobby::SendStartMatchmaking(const FString& GameMode
	, const FMatchmakingOptionalParams& OptionalParams)
{
	FReport::Log(FString(__FUNCTION__));
	FString Contents = FString::Printf(TEXT("gameMode: %s\n"), *GameMode);
		
	if (!OptionalParams.ServerName.IsEmpty())
	{
		Contents.Append(FString::Printf(TEXT("serverName: %s\n"), *OptionalParams.ServerName));
	}
		
	if (!OptionalParams.ClientVersion.IsEmpty())
	{
		Contents.Append(FString::Printf(TEXT("clientVersion: %s\n"), *OptionalParams.ClientVersion));
	}

	// Use Qos cached Latencies, or were valid Latencies already provided?
	const bool bUseCustomLatencies = OptionalParams.Latencies.Num() > 0;
	const TArray<TPair<FString, float>> SelectedLatencies = bUseCustomLatencies
		? OptionalParams.Latencies
		: FRegistry::Qos.GetCachedLatencies();

	// Validate Latencies (now initially set, immediately after login success): Important for multi-regioning.
	if (SelectedLatencies.Num() == 0)
	{
		// There are reports of weirdness when !Latencies && Multi-Regioning: This should also already be set.
		UE_LOG(LogAccelByteLobby, Warning, TEXT("No latencies data provided either from cached latencies or optional params. "
			"Empty latencies will be passed to the server (possibly problematic, if multi-regioning)."));
	}

	// Latencies should be set: Convert to json str
	if (SelectedLatencies.Num() > 0)
		Contents.Append(GetServerLatenciesJsonStr(SelectedLatencies));

	auto PartyAttribute = OptionalParams.PartyAttributes;
	if (OptionalParams.NewSessionOnly)
		PartyAttribute.Add("new_session_only", "true");

	FString partyAttributeSerialized = "";
	if (PartyAttribute.Num() > 0)
	{
		TArray<FString> keys;
		PartyAttribute.GetKeys(keys);
		for (int i = 0 ; i < keys.Num() ; i++)
		{
			FString key = keys[i];
			FString value = PartyAttribute[keys[i]];
			key = key.ReplaceCharWithEscapedChar();
			value = value.ReplaceCharWithEscapedChar();

			//Convert to this format [ "key":"value" ]
			partyAttributeSerialized.Append(FString::Printf(TEXT("\"%s\":\"%s\""), *key, *value));

			//If there's more attribute, append a delimiter
			if (i < keys.Num() - 1)
			{
				partyAttributeSerialized.Append(", ");
			}
		}
	}

	if(OptionalParams.SubGameModes.Num() > 0)
	{
		// if there is party attribute already, prepend a comma
		if(PartyAttribute.Num() > 0)
		{
			partyAttributeSerialized.Append(", ");
		}

		FString SubGameModeValue {"["};
		for(int i = 0; i < OptionalParams.SubGameModes.Num(); i++)
		{
			if(i > 0)
				SubGameModeValue.Append(", ");

			SubGameModeValue.Append(FString::Printf(TEXT("\"%s\""), *OptionalParams.SubGameModes[i]));
		}
		SubGameModeValue.Append("]");

		partyAttributeSerialized.Append(FString::Printf(TEXT("\"sub_game_mode\": %s"), *SubGameModeValue));
	}

	if(!partyAttributeSerialized.IsEmpty())
	{
		Contents.Append(FString::Printf(TEXT("partyAttributes: {%s}\n"), *partyAttributeSerialized));
	}

	if (OptionalParams.TempPartyUserIds.Num() > 0)
	{
		FString STempParty = TEXT("");
		for (int i = 0; i < OptionalParams.TempPartyUserIds.Num(); i++)
		{
			STempParty.Append(FString::Printf(TEXT("%s"), *OptionalParams.TempPartyUserIds[i]));
			if (i + 1 < OptionalParams.TempPartyUserIds.Num())
			{
				STempParty.Append(TEXT(","));
			}
		}
		
		Contents.Append(FString::Printf(TEXT("tempParty: %s\n"), *STempParty));
	}

	if (OptionalParams.ExtraAttributes.Num() > 0)
	{
		FString SExtraAttributes = TEXT("");
		for (int i = 0; i < OptionalParams.ExtraAttributes.Num(); i++)
		{
			SExtraAttributes.Append(FString::Printf(TEXT("%s"), *OptionalParams.ExtraAttributes[i]));
			if (i + 1 < OptionalParams.ExtraAttributes.Num())
			{
				SExtraAttributes.Append(TEXT(","));
			}
		}
		Contents.Append(FString::Printf(TEXT("extraAttributes: %s\n"), *SExtraAttributes));
	}

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(MatchmakingStart, Matchmaking, Contents)
}


FString Lobby::SendCancelMatchmaking(FString GameMode
	, bool IsTempParty)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(MatchmakingCancel
		, Matchmaking
		, FString::Printf(TEXT("gameMode: %s\nisTempParty: %s"), *GameMode, (IsTempParty ? TEXT("true") : TEXT("false"))))
}
	
FString Lobby::SendReadyConsentRequest(FString MatchId)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(ReadyConsent
		, Matchmaking
		, FString::Printf(TEXT("matchId: %s\n"), *MatchId));
}

FString Lobby::SendRejectConsentRequest(FString MatchId)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(RejectConsent
		, Matchmaking
		, FString::Printf(TEXT("matchId: %s\n"), *MatchId));
}

FString Lobby::RequestDS(FString const& SessionID
	, FString const& GameMode
	, FString const& ClientVersion
	, FString const& Region
	, FString const& Deployment
	, FString const& ServerName)
{
	FReport::Log(FString(__FUNCTION__));
	return SendRawRequest(LobbyRequest::CreateDS
		, Prefix::Matchmaking
		, FString::Printf(TEXT("matchId: %s\ngameMode: %s\nserverName: %s\nclientVersion: %s\nregion: %s\ndeployment: %s\n"), *SessionID, *GameMode, *ServerName, *ClientVersion, *Region, *Deployment));
}

//-------------------------------------------------------------------------------------------------
// Friends
//-------------------------------------------------------------------------------------------------
void Lobby::RequestFriend(FString UserId)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE(RequestFriends
		, Friends
		, FString::Printf(TEXT("friendId: %s"), *UserId));
}

void Lobby::RequestFriendByPublicId(FString PublicId)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE(RequestFriendsByPublicId
		, Friends
		, FString::Printf(TEXT("friendPublicId: %s"), *PublicId));
}

void Lobby::Unfriend(FString UserId)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE(Unfriend
		, Friends
		, FString::Printf(TEXT("friendId: %s"), *UserId));
}

void Lobby::ListOutgoingFriends()
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE(ListOutgoingFriends
		, Friends
		, {});
}

void Lobby::CancelFriendRequest(FString UserId)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE(CancelFriends
		, Friends
		, FString::Printf(TEXT("friendId: %s"), *UserId));
}

void Lobby::ListIncomingFriends()
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE(ListIncomingFriends
		, Friends
		, {});
}

void Lobby::AcceptFriend(FString UserId)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE(AcceptFriends
		, Friends
		, FString::Printf(TEXT("friendId: %s"), *UserId));
}

void Lobby::RejectFriend(FString UserId)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE(RejectFriends
		, Friends
		, FString::Printf(TEXT("friendId: %s"), *UserId));
}

void Lobby::LoadFriendsList()
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE(LoadFriendList
		, Friends
		, {});
}

void Lobby::GetFriendshipStatus(FString UserId)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE(GetFriendshipStatus
		, Friends
		, FString::Printf(TEXT("friendId: %s"), *UserId));
}

void Lobby::BulkFriendRequest(FAccelByteModelsBulkFriendsRequest UserIds
	, FVoidHandler OnSuccess
	, FErrorHandler OnError)
{
	FReport::Log(FString(__FUNCTION__));

	const FString Url = FString::Printf(TEXT("%s/friends/namespaces/%s/users/%s/add/bulk")
		, *SettingsRef.BaseUrl
		, *CredentialsRef.GetNamespace()
		, *CredentialsRef.GetUserId());

	HttpClient.ApiRequest(TEXT("POST"), Url, {}, UserIds, OnSuccess, OnError);
}

void Lobby::GetPartyData(const FString& PartyId
	, const THandler<FAccelByteModelsPartyData>& OnSuccess
	, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	const FString Url = FString::Printf(TEXT("%s/lobby/v1/public/party/namespaces/%s/parties/%s")
		, *SettingsRef.BaseUrl
		, *CredentialsRef.GetNamespace()
		, *PartyId);

	HttpClient.ApiRequest(TEXT("GET"), Url, {}, FString(), OnSuccess, OnError);
}

void Lobby::BulkGetUserPresence(const TArray<FString>& UserIds
	, const THandler<FAccelByteModelsBulkUserStatusNotif>& OnSuccess
	, const FErrorHandler& OnError
	, bool CountOnly)
{
	FReport::Log(FString(__FUNCTION__));

	if (UserIds.Num() <= 0)
	{
		OnError.ExecuteIfBound(static_cast<int32>(ErrorCodes::InvalidRequest), TEXT("UserIds cannot be empty!"));
		return;
	}

	TArray<FString> ProcessedUserIds{};
	TArray<FString> NotProcessedUserIds{};

	if (UserIds.Num() > UserIdsURLLimit)
	{
		ProcessedUserIds.Append(UserIds.GetData(), UserIdsURLLimit);
		NotProcessedUserIds.Append(UserIds.GetData() + UserIdsURLLimit, UserIds.Num() - UserIdsURLLimit);
	}
	else
	{
		ProcessedUserIds = UserIds;
	}

	TMultiMap<FString, FString> QueryParams = {
		{TEXT("countOnly"), CountOnly ? TEXT("true") : TEXT("false")},
		{ TEXT("userIds"), FString::Join(ProcessedUserIds, TEXT(",")) }
	};

	const FString Url = FString::Printf(TEXT("%s/lobby/v1/public/presence/namespaces/%s/users/presence")
		, *SettingsRef.BaseUrl
		, *CredentialsRef.GetNamespace());

	HttpClient.ApiRequest(TEXT("GET"), Url, QueryParams, THandler<FAccelByteModelsBulkUserStatusNotif>::CreateLambda([OnSuccess, NotProcessedUserIds](const FAccelByteModelsBulkUserStatusNotif& Result)
		{
			FAccelByteModelsBulkUserStatusNotif FinalResult = Result;
			FinalResult.NotProcessed = NotProcessedUserIds;
			OnSuccess.ExecuteIfBound(FinalResult);
		}), OnError);
}

void Lobby::GetPartyStorage(const FString& PartyId
	, const THandler<FAccelByteModelsPartyDataNotif>& OnSuccess
	, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	const FString Url = FString::Printf(TEXT("%s/lobby/v1/public/party/namespaces/%s/parties/%s")
		, *SettingsRef.BaseUrl
		, *CredentialsRef.GetNamespace()
		, *PartyId);

	HttpClient.ApiRequest(TEXT("GET"), Url, {}, FString(), OnSuccess, OnError);
}

void Lobby::GetListOfBlockedUsers(const FString& UserId
	, const THandler<FAccelByteModelsListBlockedUserResponse> OnSuccess
	, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));
	FReport::LogDeprecated(
		FString(__FUNCTION__),
		TEXT("Please use GetListOfBlockedUsers(const THandler<FAccelByteModelsListBlockedUserResponse> OnSuccess, const FErrorHandler& OnError)"));

	const FString Url = FString::Printf(TEXT("%s/lobby/v1/public/player/namespaces/%s/users/%s/blocked")
		, *SettingsRef.BaseUrl
		, *CredentialsRef.GetNamespace()
		, *UserId);

	HttpClient.ApiRequest(TEXT("GET"), Url, {}, FString(), OnSuccess, OnError);
}

void Lobby::GetListOfBlockedUsers(const THandler<FAccelByteModelsListBlockedUserResponse> OnSuccess
	, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	const FString Url = FString::Printf(TEXT("%s/lobby/v1/public/player/namespaces/%s/users/me/blocked")
		, *SettingsRef.BaseUrl
		, *CredentialsRef.GetNamespace());

	HttpClient.ApiRequest(TEXT("GET"), Url, {}, FString(), OnSuccess, OnError);

}

void Lobby::GetListOfBlockers(const FString& UserId
	, const THandler<FAccelByteModelsListBlockerResponse> OnSuccess
	, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));
	FReport::LogDeprecated(
		FString(__FUNCTION__),
		TEXT("please use GetListOfBlockers(const THandler<FAccelByteModelsListBlockerResponse> OnSuccess, const FErrorHandler& OnError)"));

	const FString Url = FString::Printf(TEXT("%s/lobby/v1/public/player/namespaces/%s/users/%s/blocked-by")
		, *SettingsRef.BaseUrl
		, *CredentialsRef.GetNamespace()
		, *UserId);

	HttpClient.ApiRequest(TEXT("GET"), Url, {}, FString(), OnSuccess, OnError);
}

void Lobby::GetListOfBlockers(const THandler<FAccelByteModelsListBlockerResponse> OnSuccess
	, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	const FString Url = FString::Printf(TEXT("%s/lobby/v1/public/player/namespaces/%s/users/me/blocked-by")
		, *SettingsRef.BaseUrl
		, *CredentialsRef.GetNamespace());

	HttpClient.ApiRequest(TEXT("GET"), Url, {}, FString(), OnSuccess, OnError);
}

void Lobby::SendNotificationToUser(const FString& SendToUserId
	, const FAccelByteModelsFreeFormNotificationRequest& Message
	, bool bAsync
	, const FVoidHandler& OnSuccess
	, const FErrorHandler& OnError)
{
	FReport::Log(FString(__FUNCTION__));

	const FString Url = FString::Printf(TEXT("%s/notification/namespaces/%s/users/%s/freeform")
		, *SettingsRef.BaseUrl
		, *CredentialsRef.GetNamespace()
		, *SendToUserId);

	const TMultiMap<FString, FString> QueryParams = {
		{TEXT("async"), bAsync ? TEXT("true") : TEXT("false")}
	};

	HttpClient.ApiRequest(TEXT("POST"), Url, QueryParams, Message, OnSuccess, OnError);
}

void Lobby::WritePartyStorage(const FString& PartyId
	, TFunction<FJsonObjectWrapper(FJsonObjectWrapper)> PayloadModifier
	, const THandler<FAccelByteModelsPartyDataNotif>& OnSuccess
	, const FErrorHandler& OnError
	, uint32 RetryAttempt)
{
	TSharedPtr<PartyStorageWrapper> Wrapper = MakeShared<PartyStorageWrapper>();
	Wrapper->PartyId = PartyId;
	Wrapper->OnSuccess = OnSuccess;
	Wrapper->OnError = OnError;
	Wrapper->RemainingAttempt = RetryAttempt;
	Wrapper->PayloadModifier = PayloadModifier;
	WritePartyStorageRecursive(Wrapper);
}

//-------------------------------------------------------------------------------------------------
// Block
//-------------------------------------------------------------------------------------------------
void Lobby::BlockPlayer(const FString& UserId)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE(BlockPlayer
		, Block
		, FString::Printf(TEXT("userId: %s\nblockedUserId: %s\nnamespace: %s"), *CredentialsRef.GetUserId(), *UserId, *CredentialsRef.GetNamespace()));
}

void Lobby::UnblockPlayer(const FString& UserId)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE(UnblockPlayer
		, Friends
		, FString::Printf(TEXT("userId: %s\nunblockedUserId: %s\nnamespace: %s"), *CredentialsRef.GetUserId(), *UserId, *CredentialsRef.GetNamespace()));
}

//-------------------------------------------------------------------------------------------------
// Signaling
//-------------------------------------------------------------------------------------------------
FString Lobby::SendSignalingMessage(const FString& UserId
	, const FString& Message)
{
	FReport::Log(FString(__FUNCTION__));

	return SendRawRequest(LobbyRequest::SignalingP2PNotif
		, Prefix::Signaling
		, FString::Printf(TEXT("destinationId: %s\nmessage: %s\n"), *UserId, *Message));
}

//-------------------------------------------------------------------------------------------------
// Session Attributes
//-------------------------------------------------------------------------------------------------
FString Lobby::SetSessionAttribute(const FString& Key
	, const FString& Value)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(SetSessionAttribute
		, Attribute
		, FString::Printf(TEXT("namespace: %s\nkey: %s\nvalue: %s"), *CredentialsRef.GetNamespace(), *Key, *Value));
}

FString Lobby::GetSessionAttribute(const FString& Key)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(GetSessionAttribute
		, Attribute
		, FString::Printf(TEXT("namespace: %s\nkey: %s"), *CredentialsRef.GetNamespace(), *Key));
}

FString Lobby::GetAllSessionAttribute()
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(GetAllSessionAttribute
		, Attribute
		, FString::Printf(TEXT("namespace: %s"), *CredentialsRef.GetNamespace()));
}

//-------------------------------------------------------------------------------------------------
// Refresh Token
//-------------------------------------------------------------------------------------------------

FString Lobby::RefreshToken(const FString& AccessToken)
{
	FReport::Log(FString(__FUNCTION__));

	SEND_RAW_REQUEST_CACHED_RESPONSE_RETURNED(RefreshToken
		, Token
		, FString::Printf(TEXT("token: %s"), *AccessToken));
}
	
void Lobby::UnbindEvent()
{
	FReport::Log(FString(__FUNCTION__));

	MessageNotif.Unbind();
	PartyDataUpdateNotif.Unbind();

	UnbindFriendNotifEvents();
	UnbindFriendResponseEvents();

	UnbindPartyNotifEvents();
	UnbindPartyResponseEvents();

	UnbindMatchmakingNotifEvents();
	UnbindMatchmakingResponseEvents();

	UnbindChatNotifEvents();
	UnbindChatResponseEvents();

	UnbindBlockNotifEvents();
	UnbindBlockResponseEvents();

	UnbindSessionAttributeEvents();

	UnbindV2PartyEvents();
	UnbindV2GameSessionEvents();
	UnbindV2MatchmakingEvents();
	
	UserBannedNotification.Unbind();
	UserUnbannedNotification.Unbind();
}

void Lobby::UnbindFriendNotifEvents()
{
	FReport::Log(FString(__FUNCTION__));

	FriendStatusNotif.Unbind();
	AcceptFriendsNotif.Unbind();
	RequestFriendsNotif.Unbind();
	UnfriendNotif.Unbind();
	CancelFriendsNotif.Unbind();
	RejectFriendsNotif.Unbind();
}

void Lobby::UnbindFriendResponseEvents()
{
	FReport::Log(FString(__FUNCTION__));

	SetUserPresenceResponse.Unbind();
	GetAllFriendsStatusResponse.Unbind();
	RequestFriendsResponse.Unbind();
	RequestFriendsByPublicIdResponse.Unbind();
	UnfriendResponse.Unbind();
	ListOutgoingFriendsResponse.Unbind();
	CancelFriendsResponse.Unbind();
	ListIncomingFriendsResponse.Unbind();
	AcceptFriendsResponse.Unbind();
	RejectFriendsResponse.Unbind();
	LoadFriendListResponse.Unbind();
	GetFriendshipStatusResponse.Unbind();

	OnSetUserPresenceError.Unbind();
	OnGetAllFriendsStatusError.Unbind();
	OnRequestFriendsError.Unbind();
	OnUnfriendError.Unbind();
	OnListOutgoingFriendsError.Unbind();
	OnCancelFriendsError.Unbind();
	OnListIncomingFriendsError.Unbind();
	OnAcceptFriendsError.Unbind();
	OnRejectFriendsError.Unbind();
	OnLoadFriendListError.Unbind();
	OnGetFriendshipStatusError.Unbind();
}

void Lobby::UnbindPartyNotifEvents()
{
	FReport::Log(FString(__FUNCTION__));

	PartyLeaveNotif.Unbind(); // This Unbind is DEPRECATED
	PartyMemberLeaveNotif.Unbind();
	PartyInviteNotif.Unbind();
	PartyGetInvitedNotif.Unbind();
	PartyJoinNotif.Unbind();
	PartyRejectNotif.Unbind();
	PartyKickNotif.Unbind();
	PartyNotif.Unbind();
	PartyMemberConnectNotif.Unbind();
	PartyMemberDisconnectNotif.Unbind();
}

void Lobby::UnbindPartyResponseEvents()
{
	FReport::Log(FString(__FUNCTION__));

	PartyInfoResponse.Unbind();
	PartyCreateResponse.Unbind();
	PartyLeaveResponse.Unbind();
	PartyInviteResponse.Unbind();
	PartyJoinResponse.Unbind();
	PartyRejectResponse.Unbind();
	PartyKickResponse.Unbind();
	PartyGetCodeResponse.Unbind();
	PartyGenerateCodeResponse.Unbind();
	PartyDeleteCodeResponse.Unbind();
	PartyJoinViaCodeResponse.Unbind();
	PartyPromoteLeaderResponse.Unbind();

	OnPartyInfoError.Unbind();
	OnPartyCreateError.Unbind();
	OnPartyLeaveError.Unbind();
	OnPartyInviteError.Unbind();
	OnPartyJoinError.Unbind();
	OnPartyRejectError.Unbind();
	OnPartyKickError.Unbind();
	OnPartyGenerateCodeError.Unbind();
	OnPartyGetCodeError.Unbind();
	OnPartyDeleteCodeError.Unbind();
	OnPartyJoinViaCodeError.Unbind();
	OnPartyPromoteLeaderError.Unbind();
}

void Lobby::UnbindMatchmakingNotifEvents()
{
	FReport::Log(FString(__FUNCTION__));

	MatchmakingNotif.Unbind();
	ReadyConsentNotif.Unbind();
	RejectConsentNotif.Unbind();
	RematchmakingNotif.Unbind();
	DsNotif.Unbind();
}

void Lobby::UnbindMatchmakingResponseEvents()
{
	FReport::Log(FString(__FUNCTION__));

	MatchmakingStartResponse.Unbind();
	MatchmakingCancelResponse.Unbind();
	ReadyConsentResponse.Unbind();
	RejectConsentResponse.Unbind();

	OnMatchmakingStartError.Unbind();
	OnMatchmakingCancelError.Unbind();
	OnReadyConsentError.Unbind();
	OnRejectConsentError.Unbind();
}

void Lobby::UnbindChatNotifEvents()
{
	FReport::Log(FString(__FUNCTION__));

	PersonalChatNotif.Unbind();
	PartyChatNotif.Unbind();
	ChannelChatNotif.Unbind();
}

void Lobby::UnbindChatResponseEvents()
{
	FReport::Log(FString(__FUNCTION__));

	PersonalChatResponse.Unbind();
	PartyChatResponse.Unbind();
	JoinDefaultChannelChatResponse.Unbind();
	ChannelChatResponse.Unbind();

	OnPersonalChatError.Unbind();
	OnPartyChatError.Unbind();
	OnJoinDefaultChannelChatError.Unbind();
	OnChannelChatError.Unbind();
}

void Lobby::UnbindBlockNotifEvents()
{
	BlockPlayerNotif.Unbind();
	UnblockPlayerNotif.Unbind();
}
	
void Lobby::UnbindBlockResponseEvents()
{
	OnBlockPlayerError.Unbind();
	OnUnblockPlayerError.Unbind();
}

void Lobby::UnbindSessionAttributeEvents()
{
	OnGetSessionAttributeError.Unbind();
	OnSetSessionAttributeError.Unbind();
	OnGetAllSessionAttributeError.Unbind();
}

void Lobby::UnbindV2PartyEvents()
{
	V2PartyInvitedNotif.Unbind();
	V2PartyJoinedNotif.Unbind();
	V2PartyKickedNotif.Unbind();
	V2PartyRejectedNotif.Unbind();
	V2PartyMembersChangedNotif.Unbind();
	V2PartyUpdatedNotif.Unbind();
}

void Lobby::UnbindV2GameSessionEvents()
{
	V2GameSessionInvitedNotif.Unbind();
	V2GameSessionJoinedNotif.Unbind();
	V2GameSessionMembersChangedNotif.Unbind();
	V2GameSessionUpdatedNotif.Unbind();
	V2DSStatusChangedNotif.Unbind();
	V2GameSessionKickedNotif.Unbind();
	V2GameSessionRejectedNotif.Unbind();
}

void Lobby::UnbindV2MatchmakingEvents()
{
	V2MatchmakingMatchFoundNotif.Unbind();
	V2MatchmakingExpiredNotif.Unbind();
	V2MatchmakingStartNotif.Unbind();
}

void Lobby::OnConnected()
{
	UE_LOG(LogAccelByteLobby, Display, TEXT("Connected"));

	TokenRefreshDelegateHandle = LobbyCredentialsRef.OnTokenRefreshed().AddLambda(
		[this](bool bSuccess)
		{
			if (bSuccess)
			{
				RefreshToken(CredentialsRef.GetAccessToken());
			}
		});
		
	ConnectSuccess.ExecuteIfBound();
}

void Lobby::OnConnectionError(const FString& Error)
{
	UE_LOG(LogAccelByteLobby, Display, TEXT("Error connecting: %s"), *Error);
	ConnectError.ExecuteIfBound(static_cast<std::underlying_type<ErrorCodes>::type>(ErrorCodes::WebSocketConnectFailed), ErrorMessages::Default.at(static_cast<std::underlying_type<ErrorCodes>::type>(ErrorCodes::WebSocketConnectFailed)) + TEXT(" Reason: ") + Error);
}

void Lobby::OnClosed(int32 StatusCode
	, const FString& Reason
	, bool WasClean)
{
	bool bIsReconnecting {true};
	if (StatusCode > 4000 && !BanNotifReceived)
	{
		bIsReconnecting = false;
		Disconnect();
	}

	LobbyCredentialsRef.OnTokenRefreshed().Remove(TokenRefreshDelegateHandle);
	TokenRefreshDelegateHandle.Reset();

	BanNotifReceived = false;
	
	UE_LOG(LogAccelByteLobby, Display, TEXT("Connection closed. Status code: %d  Reason: %s Clean: %s Reconnecting: %s"),
		StatusCode, *Reason, WasClean? TEXT("true") : TEXT("false"), bIsReconnecting? TEXT("true") : TEXT("false"));
	
	if(!bIsReconnecting)
	{
		ConnectionClosed.ExecuteIfBound(StatusCode, Reason, WasClean);
	}
	else
	{
		Reconnecting.ExecuteIfBound(StatusCode, Reason, WasClean);
	}
}
	
FString Lobby::SendRawRequest(const FString& MessageType
	, const FString& MessageIDPrefix
	, const FString& CustomPayload)
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		const FString MessageID = GenerateMessageID(MessageIDPrefix);
		FString Content = FString::Printf(TEXT("type: %s\nid: %s"), *MessageType, *MessageID);
		if (!CustomPayload.IsEmpty())
		{
			Content.Append(FString::Printf(TEXT("\n%s"), *CustomPayload));
		}
		WebSocket->Send(Content);
		UE_LOG(LogAccelByteLobby, Display, TEXT("Sending request: %s"), *Content);
		return MessageID;
	}
	return TEXT("");
}

FString Lobby::GenerateMessageID(const FString& Prefix) const
{
	return FString::Printf(TEXT("%s-%d"), *Prefix, FMath::RandRange(1000, 9999));
}

void Lobby::CreateWebSocket(const FString& Token)
{
	if(WebSocket.IsValid())
	{
		WebSocket.Reset();
	}

	TMap<FString, FString> Headers;
	Headers.Add(LobbySessionHeaderName, LobbySessionId.LobbySessionID);
	if(!Token.IsEmpty())
	{
		Headers.Add("Entitlement", Token);
	}
	FModuleManager::Get().LoadModuleChecked(FName(TEXT("WebSockets")));

	WebSocket = AccelByteWebSocket::Create(*SettingsRef.LobbyServerUrl
		, TEXT("wss")
		, CredentialsRef
		, Headers
		, TSharedRef<IWebSocketFactory>(new FUnrealWebSocketFactory())
		, PingDelay
		, InitialBackoffDelay
		, MaxBackoffDelay
		, TotalTimeout);

	WebSocket->OnConnected().AddRaw(this, &Lobby::OnConnected);
	WebSocket->OnMessageReceived().AddRaw(this, &Lobby::OnMessage);
	WebSocket->OnConnectionError().AddRaw(this, &Lobby::OnConnectionError);
	WebSocket->OnConnectionClosed().AddRaw(this, &Lobby::OnClosed);
}

FString Lobby::LobbyMessageToJson(const FString& Message)
{
	bool bFirst = true;
	FString JsonString = TEXT("{");
	TArray<FString> Lines;
	Message.ParseIntoArray(Lines, TEXT("\n"), true);
	for (const auto& Line : Lines)
	{
		FString Name;
		FString Value;
		if (bFirst)
		{
			bFirst = false;
		}
		else
		{
			JsonString.Append(",");
		}
		Line.Split(": ", &Name, &Value);
		JsonString.Appendf(TEXT("\"%s\":"), *Name);

		Value.TrimStartAndEndInline();

		const TCHAR* Cursor = GetData(Value);

		if (Cursor == nullptr)
		{
			JsonString.Append("null");
			continue;
		}
		
		// make sure it's null terminated
		checkf(*(Cursor + Value.Len()) == 0, TEXT("Invalid value: '%s' length: %d"), *Value, Value.Len());

		// Array
		if (*Cursor == '[')
		{
			++Cursor;
			// skip spaces
			while (*Cursor && *Cursor == ' ') ++Cursor;
			bool bWasArrayParsed;
			FString JsonArrayString;
			// array of JSON object
			if (*Cursor == '{')
			{
				bWasArrayParsed = MessageParser::ParseArrayOfObject(Cursor, JsonArrayString);
			}
			// array of string
			else
			{
				bWasArrayParsed = MessageParser::ParseArrayOfString(Cursor, JsonArrayString);
			}
			
			if (bWasArrayParsed)
			{
				JsonString.Append(JsonArrayString);
			}
			else
			{
				// if the array was not parsed, set to empty array
				JsonString.Append("[]");
				UE_LOG(LogAccelByte, Warning, TEXT("[LobbyMessageToJson] Invalid array for field '%s', set to empty array"), *Name);
			}
		}
		// JSON
		else if (*Cursor == '{')
		{
			FString ObjectString;
			// only append valid object
			if (MessageParser::ParseObject(Cursor, ObjectString))
			{
				JsonString.Append(ObjectString);
			}
			else
			{
				JsonString.Append("{}");
				UE_LOG(LogAccelByte, Warning, TEXT("[LobbyMessageToJson] Invalid object for field '%s', set to empty object"), *Name);
			}
		}
		// everything else
		else
		{
			MessageParser::ParseString(Cursor, JsonString);
		}
	}

	JsonString += TEXT("}");
	return JsonString;
}

/**
* Default Response handler as templated function (compile time checked)
* @see HandleNotif
*/
template <typename DataStruct, typename ResponseCallbackType, typename ErrorCallbackType>
void HandleResponse(const FString& MessageType
	, ResponseCallbackType ResponseCallback
	, ErrorCallbackType ErrorCallback
	, const FString& ParsedJsonString
	, const FString& ReceivedMessageType
	, int lobbyResponseCode
	, const TMap<FString, FString>& LobbyErrorMessages)
{
	ensure(ReceivedMessageType.Equals(MessageType));
	DataStruct Result;
	bool bSuccess = false;
	if (lobbyResponseCode == 0)
	{
		bSuccess = FAccelByteJsonConverter::JsonObjectStringToUStruct(ParsedJsonString, &Result);
	}
	else
	{
		const FString ErrorCodeString = FString::FromInt(lobbyResponseCode);

		auto it = ErrorMessages::Default.find(lobbyResponseCode);

		FString ErrorCodeName;
		if(LobbyErrorMessages.Contains(ErrorCodeString))
		{
			ErrorCodeName = LobbyErrorMessages[ErrorCodeString];
		}
		else if(it != ErrorMessages::Default.cend())
		{
			ErrorCodeName = ErrorMessages::Default.at(lobbyResponseCode);
		}
		else
		{
			ErrorCodeName = TEXT("Error name not defined");
		}
		
		UE_LOG(LogAccelByteLobby, Display, TEXT("%s returned non zero error code, code is %d with codename %s"), *MessageType, lobbyResponseCode, *ErrorCodeName);
		if (ErrorCallback.IsBound())
		{
			ErrorCallback.ExecuteIfBound(lobbyResponseCode, *ErrorCodeName);
			bSuccess = false;
		}
		else
		{
			Result.Code = FString::FromInt(lobbyResponseCode);
			bSuccess = true;
		}
	}

	if (bSuccess)
	{
		ResponseCallback.ExecuteIfBound(Result);
	}
}

/**
* @brief convenient switch case for RESPONSE context; will call the singular assigned delegate on success with no caching/message-Id matching
* @see CASE_NOTIF
**/
#define CASE_RESPONSE(MessageType, Model) \
	case (Response::MessageType) : \
		{ \
			HandleResponse<Model>(LobbyResponse::MessageType, MESSAGE_SUCCESS_HANDLER(MessageType), MESSAGE_ERROR_HANDLER(MessageType), ParsedJsonString, ReceivedMessageType, lobbyResponseCode, LobbyErrorMessages);\
			break; \
		} \

/**
* Some delegate types can be derived from MessageType
*/
#define DELEGATE_TYPE(MessageType) F ## MessageType ## Response

/**
* @brief convenient switch case for RESPONSE context; will cache the (MessageId,SuccessDelegate) pair
* @param DelegateType explicit param for cases/responses that has different format naming between MessageType and DelegateType
*/
#define CASE_RESPONSE_MESSAGE_ID_DELEGATE_TYPE(MessageType, Model, DelegateType) \
	case (Response::MessageType) : \
		{ \
			if(ID_RESPONSE_MAP(MessageType).Contains(ReceivedMessageId)) \
			{ \
				DelegateType* IdResponse = ID_RESPONSE_MAP(MessageType).Find(ReceivedMessageId); \
				check(IdResponse); \
				HandleResponse<Model>(LobbyResponse::MessageType, *IdResponse, MESSAGE_ERROR_HANDLER(MessageType), ParsedJsonString, ReceivedMessageType, lobbyResponseCode, LobbyErrorMessages);\
				ID_RESPONSE_MAP(MessageType).Remove(ReceivedMessageId); \
			} \
			else \
			{ \
				UE_LOG(LogAccelByteLobby, Warning, TEXT("Message Id response not found! type %s, id %s"), *LobbyResponse::MessageType, *ReceivedMessageId); \
			} \
			break; \
		} \

//@brief convenient switch case for RESPONSE context; will cache the (MessageId,SuccessDelegate) pair
#define CASE_RESPONSE_MESSAGE_ID(MessageType, Model) CASE_RESPONSE_MESSAGE_ID_DELEGATE_TYPE(MessageType, Model, DELEGATE_TYPE(MessageType))

void Lobby::HandleMessageResponse(const FString& ReceivedMessageType
	, const FString& ParsedJsonString
	, const TSharedPtr<FJsonObject>& ParsedJsonObj
	, const TSharedPtr<FLobbyMessageMetaData>& MessageMeta = nullptr)
{
	int lobbyResponseCode{0};
	FString ReceivedMessageId{};

	if(MessageMeta.IsValid())
	{
		lobbyResponseCode = FCString::Atoi(*MessageMeta->Code);
		ReceivedMessageId = MessageMeta->Id;
	}
	else
	{
		lobbyResponseCode = ParsedJsonObj->GetIntegerField("code");
		ReceivedMessageId = ParsedJsonObj->GetStringField("id");
	}

	Response ResponseEnum = Response::Invalid_Response;
	Response* ResponseEnumPointer = ResponseStringEnumMap.Find(ReceivedMessageType);
	if (ResponseEnumPointer)
	{
		ResponseEnum = *ResponseEnumPointer;
	}

	switch (ResponseEnum)
	{
		// Party
		CASE_RESPONSE_MESSAGE_ID(PartyInfo			, FAccelByteModelsInfoPartyResponse);
		CASE_RESPONSE_MESSAGE_ID(PartyCreate		, FAccelByteModelsCreatePartyResponse);
		CASE_RESPONSE_MESSAGE_ID(PartyLeave			, FAccelByteModelsLeavePartyResponse);
		CASE_RESPONSE_MESSAGE_ID(PartyInvite		, FAccelByteModelsPartyInviteResponse);
		CASE_RESPONSE_MESSAGE_ID(PartyJoin			, FAccelByteModelsPartyJoinResponse);
		CASE_RESPONSE_MESSAGE_ID(PartyReject		, FAccelByteModelsPartyRejectResponse);
		CASE_RESPONSE_MESSAGE_ID(PartyKick			, FAccelByteModelsKickPartyMemberResponse);
		CASE_RESPONSE_MESSAGE_ID(PartyGetCode		, FAccelByteModelsPartyGetCodeResponse);
		CASE_RESPONSE_MESSAGE_ID(PartyGenerateCode	, FAccelByteModelsPartyGenerateCodeResponse);
		CASE_RESPONSE_MESSAGE_ID(PartyDeleteCode	, FAccelByteModelsPartyDeleteCodeResponse);
		CASE_RESPONSE_MESSAGE_ID(PartyJoinViaCode	, FAccelByteModelsPartyJoinResponse);
		CASE_RESPONSE_MESSAGE_ID(PartyPromoteLeader	, FAccelByteModelsPartyPromoteLeaderResponse);
		CASE_RESPONSE_MESSAGE_ID(PartySendNotif		, FAccelByteModelsPartySendNotifResponse);
		// Chat
		CASE_RESPONSE_MESSAGE_ID(PersonalChat	, FAccelByteModelsPersonalMessageResponse);
		CASE_RESPONSE_MESSAGE_ID(PartyChat		, FAccelByteModelsPartyMessageResponse);
		CASE_RESPONSE_MESSAGE_ID(ChannelChat	, FAccelByteModelsChannelMessageResponse);
		case (Response::JoinChannelChat):
		{
			FAccelByteModelsJoinDefaultChannelResponse Result;
			if (const bool bParseSuccess = FAccelByteJsonConverter::JsonObjectStringToUStruct(ParsedJsonString, &Result))
			{
				ChannelSlug = Result.ChannelSlug;
				JoinDefaultChannelChatResponse.ExecuteIfBound(Result);
			}
			break;
		}
		// Presence
		CASE_RESPONSE_MESSAGE_ID(SetUserPresence	, FAccelByteModelsSetOnlineUsersResponse);
		CASE_RESPONSE_MESSAGE_ID(GetAllFriendsStatus, FAccelByteModelsGetOnlineUsersResponse);
		// Matchmaking
		CASE_RESPONSE_MESSAGE_ID_DELEGATE_TYPE(MatchmakingStart	, FAccelByteModelsMatchmakingResponse, FMatchmakingResponse);
		CASE_RESPONSE_MESSAGE_ID_DELEGATE_TYPE(MatchmakingCancel, FAccelByteModelsMatchmakingResponse, FMatchmakingResponse);
		CASE_RESPONSE_MESSAGE_ID(ReadyConsent, FAccelByteModelsReadyConsentRequest);
		CASE_RESPONSE_MESSAGE_ID(RejectConsent, FAccelByteModelsRejectConsentRequest);
		// Custom Game
		CASE_RESPONSE_MESSAGE_ID_DELEGATE_TYPE(CreateDS			, FAccelByteModelsLobbyBaseResponse, FBaseResponse);
		// Friends
		CASE_RESPONSE_MESSAGE_ID(RequestFriends		, FAccelByteModelsRequestFriendsResponse);
		CASE_RESPONSE_MESSAGE_ID_DELEGATE_TYPE(RequestFriendsByPublicId		, FAccelByteModelsRequestFriendsResponse, FRequestFriendsResponse);
		CASE_RESPONSE_MESSAGE_ID(Unfriend			, FAccelByteModelsUnfriendResponse);
		CASE_RESPONSE_MESSAGE_ID(ListOutgoingFriends, FAccelByteModelsListOutgoingFriendsResponse);
		CASE_RESPONSE_MESSAGE_ID(CancelFriends		, FAccelByteModelsCancelFriendsResponse);
		CASE_RESPONSE_MESSAGE_ID(ListIncomingFriends, FAccelByteModelsListIncomingFriendsResponse);
		CASE_RESPONSE_MESSAGE_ID(AcceptFriends		, FAccelByteModelsAcceptFriendsResponse);
		CASE_RESPONSE_MESSAGE_ID(RejectFriends		, FAccelByteModelsRejectFriendsResponse);
		CASE_RESPONSE_MESSAGE_ID(LoadFriendList		, FAccelByteModelsLoadFriendListResponse);
		case (Response::GetFriendshipStatus):
		{
			FAccelByteModelsGetFriendshipStatusStringResponse StringResult;
			bool bParseSuccess = FAccelByteJsonConverter::JsonObjectStringToUStruct(ParsedJsonString, &StringResult);
			if (bParseSuccess)
			{
				FAccelByteModelsGetFriendshipStatusResponse Result;
				Result.Code = StringResult.Code;
				Result.friendshipStatus = (ERelationshipStatusCode)FCString::Atoi(*StringResult.friendshipStatus);
				GetFriendshipStatusResponse.ExecuteIfBound(Result);
			}
			break;
		}
		// Block
		CASE_RESPONSE_MESSAGE_ID(BlockPlayer	, FAccelByteModelsBlockPlayerResponse);
		CASE_RESPONSE_MESSAGE_ID(UnblockPlayer	, FAccelByteModelsUnblockPlayerResponse);
		// Session Attribute
		CASE_RESPONSE_MESSAGE_ID(SetSessionAttribute	, FAccelByteModelsSetSessionAttributesResponse);
		CASE_RESPONSE_MESSAGE_ID(GetSessionAttribute	, FAccelByteModelsGetSessionAttributesResponse);
		CASE_RESPONSE_MESSAGE_ID(GetAllSessionAttribute	, FAccelByteModelsGetAllSessionAttributesResponse);
		CASE_RESPONSE_MESSAGE_ID(RefreshToken			, FAccelByteModelsRefreshTokenResponse);
		default:
		{
			ParsingError.ExecuteIfBound(-1, FString::Printf(TEXT("Error; Detected of type Response but no specific handler case assigned. %s, Raw: %s"), *ReceivedMessageType, *ParsedJsonString));
			break;
		}

	}
}

#undef MESSAGE_SUCCESS_HANDLER
#undef MESSAGE_ERROR_HANDLER
#undef CASE_RESPONSE
#undef CASE_RESPONSE_MESSAGE_ID
#undef ID_RESPONSE_MAP
#undef DELEGATE_TYPE
#undef CASE_RESPONSE_MESSAGE_ID_DELEGATE_TYPE

/**
* Default Notif handler as templated function (compile time checked)
* @see HandleResponse
*/
template <typename DataStruct, typename ResponseCallbackType>
void HandleNotif(const FString& MessageType
	, ResponseCallbackType ResponseCallback
	, const FString& ParsedJsonString
	, const FString& ReceivedMessageType)
{
	ensure(ReceivedMessageType.Equals(MessageType));
	DataStruct Result;
	if (const bool bSuccess = FAccelByteJsonConverter::JsonObjectStringToUStruct(ParsedJsonString, &Result)) {
		ResponseCallback.ExecuteIfBound(Result);
	}

};

/**
*  Programmer-friendly macro for switch case NOTIF scenario
* @see CASE_RESPONSE
*
**/
#define CASE_NOTIF(MessageType, Model) \
	case (Notif::MessageType) : \
		{ \
			HandleNotif<Model>(LobbyResponse::MessageType, MessageType, ParsedJsonString, ReceivedMessageType);\
			break; \
		} \
		
template <typename PayloadType, typename CallbackType>
void DispatchV2JsonNotif(const FString& Payload
	, CallbackType ResponseCallback)
{
	FString PayloadJsonString;
	if(!FBase64::Decode(Payload, PayloadJsonString))
	{
		UE_LOG(LogAccelByteLobby, Warning, TEXT("Unable to decode payload from notification\n%s"), *Payload);
		return;
	}

	UE_LOG(LogAccelByteLobby, Log, TEXT("MPv2 notif json:\n%s"), *PayloadJsonString);
	
	PayloadType Result;
	if(FAccelByteJsonConverter::JsonObjectStringToUStruct(PayloadJsonString, &Result))
	{
		ResponseCallback.ExecuteIfBound(Result);
	}
	else
	{
		UE_LOG(LogAccelByteLobby, Warning, TEXT("Unable to json parse payload from notification\n%s"), *Payload);
	}
}

void Lobby::HandleV2SessionNotif(const FString& ParsedJsonString)
{
	FAccelByteModelsSessionNotificationMessage Notif;
	if (FAccelByteJsonConverter::JsonObjectStringToUStruct(ParsedJsonString, &Notif) == false)
	{
		UE_LOG(LogAccelByteLobby, Log, TEXT("Cannot deserialize sessionMessageNotif to struct\nNotification: %s"), *ParsedJsonString);
		return;
	}

	switch(FAccelByteUtilities::GetUEnumValueFromString<EV2SessionNotifTopic>(Notif.Topic))
	{
	case EV2SessionNotifTopic::OnPartyInvited:
	{
		DispatchV2JsonNotif<FAccelByteModelsV2PartyInvitedEvent>(Notif.Payload, V2PartyInvitedNotif);
		break;
	}
	case EV2SessionNotifTopic::OnPartyMembersChanged:
	{
		DispatchV2JsonNotif<FAccelByteModelsV2PartyMembersChangedEvent>(Notif.Payload, V2PartyMembersChangedNotif);
		break;
	}
	case EV2SessionNotifTopic::OnPartyJoined:
	{
		DispatchV2JsonNotif<FAccelByteModelsV2PartyUserJoinedEvent>(Notif.Payload, V2PartyJoinedNotif);
		break;
	}
	case EV2SessionNotifTopic::OnPartyRejected:
	{
		DispatchV2JsonNotif<FAccelByteModelsV2PartyUserRejectedEvent>(Notif.Payload, V2PartyRejectedNotif);
		break;
	}
	case EV2SessionNotifTopic::OnPartyKicked:
	{
		DispatchV2JsonNotif<FAccelByteModelsV2PartyUserKickedEvent>(Notif.Payload, V2PartyKickedNotif);
		break;
	}
	case EV2SessionNotifTopic::OnSessionInvited:
	{
		DispatchV2JsonNotif<FAccelByteModelsV2GameSessionUserInvitedEvent>(Notif.Payload, V2GameSessionInvitedNotif);
		break;
	}
	case EV2SessionNotifTopic::OnSessionJoined:
	{
		DispatchV2JsonNotif<FAccelByteModelsV2GameSessionUserJoinedEvent>(Notif.Payload, V2GameSessionJoinedNotif);
		break;
	}
	case EV2SessionNotifTopic::OnSessionMembersChanged:
	{
		DispatchV2JsonNotif<FAccelByteModelsV2GameSessionMembersChangedEvent>(Notif.Payload, V2GameSessionMembersChangedNotif);
		break;
	}
	case EV2SessionNotifTopic::OnSessionKicked:
	{
		DispatchV2JsonNotif<FAccelByteModelsV2GameSessionUserKickedEvent>(Notif.Payload, V2GameSessionKickedNotif);
		break;
	}
	case EV2SessionNotifTopic::OnSessionRejected:
	{
		DispatchV2JsonNotif<FAccelByteModelsV2GameSessionUserRejectedEvent>(Notif.Payload, V2GameSessionRejectedNotif);
		break;
	}
	case EV2SessionNotifTopic::OnDSStatusChanged:
	{
		DispatchV2JsonNotif<FAccelByteModelsV2DSStatusChangedNotif>(Notif.Payload, V2DSStatusChangedNotif);
		break;
	}
	case EV2SessionNotifTopic::OnPartyUpdated:
	{
		DispatchV2JsonNotif<FAccelByteModelsV2PartySession>(Notif.Payload, V2PartyUpdatedNotif);
		break;
	}
	case EV2SessionNotifTopic::OnGameSessionUpdated:
	{
		DispatchV2JsonNotif<FAccelByteModelsV2GameSession>(Notif.Payload, V2GameSessionUpdatedNotif);
		break;
	}
	default: UE_LOG(LogAccelByteLobby, Log, TEXT("Unknown session notification topic\nNotification: %s"), *ParsedJsonString);
	}
}

void Lobby::HandleV2MatchmakingNotif(const FAccelByteModelsNotificationMessage& Message)
{
	UE_LOG(LogAccelByteLobby, Log, TEXT("Received MMv2 notification with topic : %s"), *Message.Topic);
	
	switch (FAccelByteUtilities::GetUEnumValueFromString<EV2MatchmakingNotifTopic>(Message.Topic))
	{
		case EV2MatchmakingNotifTopic::OnMatchFound:
		{
			DispatchV2JsonNotif<FAccelByteModelsV2MatchFoundNotif>(Message.Payload, V2MatchmakingMatchFoundNotif);
			break;
		}
		case EV2MatchmakingNotifTopic::OnMatchmakingStarted:
		{
			DispatchV2JsonNotif<FAccelByteModelsV2StartMatchmakingNotif>(Message.Payload, V2MatchmakingStartNotif);
			break;
		}
		case EV2MatchmakingNotifTopic::OnMatchmakingTicketExpired:
		{
			DispatchV2JsonNotif<FAccelByteModelsV2MatchmakingExpiredNotif>(Message.Payload, V2MatchmakingExpiredNotif);
			break;
		}
		case EV2MatchmakingNotifTopic::OnMatchmakingTicketCanceled:
		{
			DispatchV2JsonNotif<FAccelByteModelsV2MatchmakingCanceledNotif>(Message.Payload, V2MatchmakingCanceledNotif);
			break;
		}
		default: UE_LOG(LogAccelByteLobby, Warning, TEXT("Unknown matchmaking v2 notification topic : %s"), *Message.Topic);
	}
}

void Lobby::InitializeV2MatchmakingNotifDelegates()
{
	MatchmakingV2NotifDelegates = {
		{EV2MatchmakingNotifTopic::OnMatchFound, FMessageNotif::CreateRaw(this, &Lobby::HandleV2MatchmakingNotif)},
		{EV2MatchmakingNotifTopic::OnMatchmakingStarted, FMessageNotif::CreateRaw(this, &Lobby::HandleV2MatchmakingNotif)},
		{EV2MatchmakingNotifTopic::OnMatchmakingTicketExpired, FMessageNotif::CreateRaw(this, &Lobby::HandleV2MatchmakingNotif)},
		{EV2MatchmakingNotifTopic::OnMatchmakingTicketCanceled, FMessageNotif::CreateRaw(this, &Lobby::HandleV2MatchmakingNotif)},
	};
}

void Lobby::HandleMessageNotif(const FString& ReceivedMessageType
	, const FString& ParsedJsonString
	, const TSharedPtr<FJsonObject>& ParsedJsonObj)
{
	Notif NotifEnum = Notif::Invalid_Notif;
	if (const Notif* NotifEnumPointer = NotifStringEnumMap.Find(ReceivedMessageType))
	{
		NotifEnum = *NotifEnumPointer;
	}

	switch (NotifEnum)
	{
		// Party
		case (Notif::ConnectedNotif):
		{
			FAccelByteModelsLobbySessionId SessionId;
			bool bSuccess = FAccelByteJsonConverter::JsonObjectStringToUStruct(ParsedJsonString, &SessionId);
			if (bSuccess)
			{
				LobbySessionId = SessionId;
				WebSocket->UpdateUpgradeHeaders(LobbySessionHeaderName, LobbySessionId.LobbySessionID);
			}
			break;
		}
		CASE_NOTIF(DisconnectNotif, FAccelByteModelsDisconnectNotif);
		case (Notif::PartyMemberLeaveNotif):
		{
				FAccelByteModelsLeavePartyNotice PartyLeaveResult;
				bool bSuccess = FAccelByteJsonConverter::JsonObjectStringToUStruct(ParsedJsonString, &PartyLeaveResult);
				if (bSuccess)
				{
					if (PartyLeaveNotif.IsBound())
					{
						PartyLeaveNotif.ExecuteIfBound(PartyLeaveResult);
					}
					else
					{
						PartyMemberLeaveNotif.ExecuteIfBound(PartyLeaveResult);
					}
				}
				break;
		}
		CASE_NOTIF(PartyInviteNotif, FAccelByteModelsInvitationNotice);
		CASE_NOTIF(PartyGetInvitedNotif, FAccelByteModelsPartyGetInvitedNotice);
		CASE_NOTIF(PartyJoinNotif, FAccelByteModelsPartyJoinNotice);
		CASE_NOTIF(PartyRejectNotif, FAccelByteModelsPartyRejectNotice);
		CASE_NOTIF(PartyKickNotif, FAccelByteModelsGotKickedFromPartyNotice);
		case Notif::PartyNotif:
		{
			const FString PayloadKey(TEXT("payload"));
			const TSharedPtr<FJsonObject>* ObjectValue;
			if (ParsedJsonObj->TryGetObjectField(PayloadKey, ObjectValue))
			{
				FString StringValue;
				TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&StringValue, /*Indent=*/0);
				if (!FJsonSerializer::Serialize(ObjectValue->ToSharedRef(), JsonWriter))
				{
					UE_LOG(LogAccelByteLobby, Log, TEXT("PartyNotif: unable to serialize payload as string: %s"), *ParsedJsonString);
					return;
				}
				ParsedJsonObj->SetStringField(PayloadKey, StringValue);
			}

			FAccelByteModelsPartyNotif Notification;
			if (!FJsonObjectConverter::JsonObjectToUStruct(ParsedJsonObj.ToSharedRef(), &Notification, 0, 0))
			{
				UE_LOG(LogAccelByteLobby, Log, TEXT("PartyNotif: unable to deserialize to struct: %s"), *ParsedJsonString);
				return;
			}
			PartyNotif.ExecuteIfBound(Notification);
			break;
		}
		CASE_NOTIF(PartyDataUpdateNotif, FAccelByteModelsPartyDataNotif);
		CASE_NOTIF(PartyMemberConnectNotif, FAccelByteModelsPartyMemberConnectionNotice);
		CASE_NOTIF(PartyMemberDisconnectNotif, FAccelByteModelsPartyMemberConnectionNotice);
		// Chat
		CASE_NOTIF(PersonalChatNotif, FAccelByteModelsPersonalMessageNotice);
		CASE_NOTIF(PartyChatNotif, FAccelByteModelsPartyMessageNotice);
		CASE_NOTIF(ChannelChatNotif, FAccelByteModelsChannelMessageNotice);
		// Presence
		CASE_NOTIF(FriendStatusNotif, FAccelByteModelsUsersPresenceNotice);
		// Notification
		case(Notif::MessageNotif):
		{
			FAccelByteModelsNotificationMessage NotificationMessage;
			FString PayloadKey = "payload";
			if (ParsedJsonObj->HasTypedField<EJson::Object>(PayloadKey))
			{
				TSharedPtr<FJsonObject> PayloadObject = ParsedJsonObj->GetObjectField(PayloadKey);
				if (PayloadObject == nullptr)
				{
					UE_LOG(LogAccelByteLobby, Log, TEXT("Cannot deserialize the whole MessageNotif to the struct\nNotification: %s"), *ParsedJsonString);
					return;
				}

				FString PayloadString;
				TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&PayloadString);
				if (FJsonSerializer::Serialize(PayloadObject.ToSharedRef(), JsonWriter) == false)
				{
					UE_LOG(LogAccelByteLobby, Log, TEXT("Cannot serialize payload field from MessageNotif\nNotification: %s"), *ParsedJsonString);
					return;
				}
				ParsedJsonObj->RemoveField(PayloadKey);
				ParsedJsonObj->SetStringField(PayloadKey, PayloadString);
				if (FJsonObjectConverter::JsonObjectToUStruct(ParsedJsonObj.ToSharedRef(), &NotificationMessage, 0, 0) == false)
				{
					UE_LOG(LogAccelByteLobby, Log, TEXT("Cannot deserialize the whole MessageNotif to the struct\nNotification: %s"), *ParsedJsonString);
					return;
				}
			}
			else
			{
				if (FAccelByteJsonConverter::JsonObjectStringToUStruct(ParsedJsonString, &NotificationMessage) == false)
				{
					UE_LOG(LogAccelByteLobby, Log, TEXT("Cannot deserialize the whole MessageNotif to the struct\nNotification: %s"), *ParsedJsonString);
					return;
				}
			}

			EV2MatchmakingNotifTopic MMNotifEnum = FAccelByteUtilities::GetUEnumValueFromString<EV2MatchmakingNotifTopic>(NotificationMessage.Topic);
			if(MMNotifEnum != EV2MatchmakingNotifTopic::Invalid && MatchmakingV2NotifDelegates.Contains(MMNotifEnum))
			{
				MatchmakingV2NotifDelegates[MMNotifEnum].ExecuteIfBound(NotificationMessage);
				break;
			}
				
			MessageNotif.ExecuteIfBound(NotificationMessage);
			break;
		}
		// Matchmaking
		CASE_NOTIF(MatchmakingNotif		, FAccelByteModelsMatchmakingNotice);
		CASE_NOTIF(ReadyConsentNotif	, FAccelByteModelsReadyConsentNotice);
		CASE_NOTIF(RejectConsentNotif	, FAccelByteModelsRejectConsentNotice);
		CASE_NOTIF(RematchmakingNotif	, FAccelByteModelsRematchmakingNotice);
		CASE_NOTIF(DsNotif				, FAccelByteModelsDsNotice);
		// Friends + Notification
		CASE_NOTIF(AcceptFriendsNotif	, FAccelByteModelsAcceptFriendsNotif);
		CASE_NOTIF(RequestFriendsNotif	, FAccelByteModelsRequestFriendsNotif);
		CASE_NOTIF(UnfriendNotif		, FAccelByteModelsUnfriendNotif);
		CASE_NOTIF(CancelFriendsNotif	, FAccelByteModelsCancelFriendsNotif);
		CASE_NOTIF(RejectFriendsNotif	, FAccelByteModelsRejectFriendsNotif);
		// Block + Notification
		CASE_NOTIF(BlockPlayerNotif		, FAccelByteModelsBlockPlayerNotif);
		CASE_NOTIF(UnblockPlayerNotif	, FAccelByteModelsUnblockPlayerNotif);
		// Shadow Ban
		case (Notif::UserBannedNotification): // intended fallthrough
		case (Notif::UserUnbannedNotification):
		{
			BanNotifReceived = true;
			FAccelByteModelsUserBannedNotification Result;
			//CredentialsRef.OnTokenRefreshed().Remove(TokenRefreshDelegateHandle);
			if (FAccelByteJsonConverter::JsonObjectStringToUStruct(ParsedJsonString, &Result))
			{
				if (Result.UserId == CredentialsRef.GetUserId())
				{
					HttpRef.BearerAuthRejected();
				}
				
				if (ReceivedMessageType.Equals(LobbyResponse::UserBannedNotification))
				{
					UserBannedNotification.ExecuteIfBound(Result);

					const int64 UtcNow = FDateTime::UtcNow().ToUnixTimestamp();
					FDateTime BanEndDate{0};
					FDateTime::ParseIso8601(*Result.EndDate, BanEndDate);
					float BanDuration = BanEndDate.ToUnixTimestamp() - UtcNow;
					double ScheduledTime = FPlatformTime::Seconds() + BanDuration;

					FString Key = FString::Printf(TEXT("%s-%s"), *Result.UserId, *FAccelByteUtilities::GetUEnumValueAsString(Result.Ban));

					FUnbanScheduleRef* Schedule = UnbanSchedules.Find(Key);
					if (!Schedule || ScheduledTime < (*Schedule)->ScheduledTime)
					{
						if (Schedule)
						{
							FTickerAlias::GetCoreTicker().RemoveTicker((*Schedule)->DelegateHandle);
							UnbanSchedules.Remove(Key);
						}

						TSharedRef<FAccelByteModelsUserBannedNotification> Data = MakeShared<FAccelByteModelsUserBannedNotification>(Result);
						FUnbanScheduleRef NewSchedule = MakeShared<FUnbanSchedule>();
						NewSchedule->DelegateHandle = FTickerAlias::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
							[this, Data, ScheduledTime](float DeltaTime)
							{
								double CurrentTime = FPlatformTime::Seconds();
								if (CurrentTime >= ScheduledTime)
								{
									Data->Enable = false;
									UserUnbannedNotification.ExecuteIfBound(*Data);
									return false;
								}
								return true;
							})
							, 0.2f);
						NewSchedule->ScheduledTime = ScheduledTime;
						UnbanSchedules.Add(Key, NewSchedule);
					}
				}
				else if (ReceivedMessageType.Equals(LobbyResponse::UserUnbannedNotification))
				{
					UserUnbannedNotification.ExecuteIfBound(Result);
				}
			}
			break;
		}
		case (Notif::ErrorNotif):
		{
			FString ErrorNotifRequestType = ParsedJsonObj->GetStringField(TEXT("requestType"));

			// Handle trigger ResponseDelegates when ErrorNotif arrived with "requestType" field 
			if(!ErrorNotifRequestType.IsEmpty())
			{
				// Replace "Request" at end of string with "Response"
				const FString RequestString = {TEXT("Request")};
				if(ErrorNotifRequestType.EndsWith(RequestString))
				{
					ErrorNotifRequestType = ErrorNotifRequestType.LeftChop(RequestString.Len()).Append(TEXT("Response"));
				}
				
				FAccelByteModelsLobbyBaseResponse ErrorRequestResponse;
				ErrorRequestResponse.Code = ParsedJsonObj->GetStringField(TEXT("code"));
				ErrorRequestResponse.Id = ParsedJsonObj->GetStringField("id");
				ErrorRequestResponse.Type = ErrorNotifRequestType;

				TSharedPtr<FJsonObject> ErrorRequestJsonObject = FJsonObjectConverter::UStructToJsonObject(ErrorRequestResponse);
				FString ErrorRequestJsonString;
				FJsonObjectConverter::UStructToJsonObjectString(ErrorRequestResponse, ErrorRequestJsonString);
				HandleMessageResponse(ErrorNotifRequestType,  ErrorRequestJsonString, ErrorRequestJsonObject);
			}
			else
			{
				ErrorNotif.ExecuteIfBound(ParsedJsonObj->GetIntegerField(TEXT("code")), ParsedJsonObj->GetStringField(TEXT("message")));
			}
			break;
		}
		case (Notif::SignalingP2PNotif):
		{
			SignalingP2PNotif.ExecuteIfBound(ParsedJsonObj->GetStringField(TEXT("destinationId")), ParsedJsonObj->GetStringField(TEXT("message")));
			break;
		}
		default:
		{
			ParsingError.ExecuteIfBound(-1, FString::Printf(TEXT("Error; Detected of type notif but no specific handler case assigned. %s, Raw: %s"), *ReceivedMessageType, *ParsedJsonString));
			break;
		}
	}

}
#undef CASE_NOTIF

bool Lobby::ExtractLobbyMessageMetaData(const FString& InLobbyMessage
	, TSharedRef<FLobbyMessageMetaData>& OutLobbyMessageMetaData)
{
	TArray<FString> MessageLines;
	InLobbyMessage.ParseIntoArrayLines(MessageLines);

	const FString CodeKey {"code: "};
	const FString TypeKey {"type: "};
	const FString IdKey {"id: "};
	
	for(const FString& Line : MessageLines)
	{
		if(Line.StartsWith(CodeKey))
		{
			OutLobbyMessageMetaData->Code = Line.RightChop(CodeKey.Len());
		}
		else if(Line.StartsWith(TypeKey))
		{
			OutLobbyMessageMetaData->Type = Line.RightChop(TypeKey.Len());
		}
		else if(Line.StartsWith(IdKey))
		{
			OutLobbyMessageMetaData->Id = Line.RightChop(IdKey.Len());
		}
	}

	UE_LOG(LogAccelByteLobby, Display, TEXT("Metadata found type %s, id %s, code %s"), *OutLobbyMessageMetaData->Type, *OutLobbyMessageMetaData->Id, *OutLobbyMessageMetaData->Code);

	return true;
}
	
void Lobby::OnMessage(const FString& Message)
{
	UE_LOG(LogAccelByteLobby, Display, TEXT("Raw Lobby Response\n%s"), *Message);

	if (Message.IsEmpty())
	{
		return;
	}

	// Conversion : Custom -> Json
	const FString ParsedJsonString = LobbyMessageToJson(Message);
	
	UE_LOG(LogAccelByteLobby, Display, TEXT("JSON Version: %s"), *ParsedJsonString);

	TSharedPtr<FJsonObject> ParsedJsonObj;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ParsedJsonString);
	if (!FJsonSerializer::Deserialize(JsonReader, ParsedJsonObj))
	{
		UE_LOG(LogAccelByteLobby, Display, TEXT("Failed to Deserialize. Json: %s"), *ParsedJsonString);

		TSharedRef<FLobbyMessageMetaData> MetaData = MakeShared<FLobbyMessageMetaData>();
		ExtractLobbyMessageMetaData(Message, MetaData);
		MetaData->Code = FString::FromInt(static_cast<int>(ErrorCodes::JsonDeserializationFailed));

		// handle error message if message type is response. if notif then ignore.
		if (MetaData->Type.Contains(Suffix::Response))
		{
			HandleMessageResponse(MetaData->Type, ParsedJsonString, ParsedJsonObj, MetaData);
		}
		
		return;
	}

	const FString JsonTypeIdentifier = TEXT("type");
	if (!ParsedJsonObj->HasTypedField<EJson::String>(JsonTypeIdentifier))
	{
		return;
	}

	const FString ReceivedMessageType = ParsedJsonObj->GetStringField(JsonTypeIdentifier);
	UE_LOG(LogAccelByteLobby, Display, TEXT("Type: %s"), *ReceivedMessageType);

	if (ReceivedMessageType.Equals(LobbyResponse::SessionNotif))
	{
		HandleV2SessionNotif(ParsedJsonString);
	}
	else if (ReceivedMessageType.Contains(Suffix::Response))
	{
		HandleMessageResponse(ReceivedMessageType, ParsedJsonString, ParsedJsonObj);
	}
	else if (ReceivedMessageType.Contains(Suffix::Notif))
	{
		HandleMessageNotif(ReceivedMessageType, ParsedJsonString, ParsedJsonObj);
	}
	else // undefined; not Response nor Notif
	{
		ParsingError.ExecuteIfBound(-1, FString::Printf(TEXT("Error cannot parse message. Neither a response nor a notif type. %s, Raw: %s"), *ReceivedMessageType, *ParsedJsonString));
	}

}

void Lobby::RequestWritePartyStorage(const FString& PartyId
	, const FAccelByteModelsPartyDataUpdateRequest& Data
	, const THandler<FAccelByteModelsPartyDataNotif>& OnSuccess
	, const FErrorHandler& OnError
	, FSimpleDelegate OnConflicted)
{
	FReport::Log(FString(__FUNCTION__));

	const FString Url = FString::Printf(TEXT("%s/lobby/v1/public/party/namespaces/%s/parties/%s/attributes")
		, *SettingsRef.BaseUrl
		, *CredentialsRef.GetNamespace()
		, *PartyId);

	FString Contents = "{\n";
	FString CustomAttribute;
	FJsonObjectConverter::UStructToJsonObjectString(Data.Custom_attribute, CustomAttribute);
	FString UpdatedAt = FString::Printf(TEXT("\"updatedAt\": %lld"), Data.UpdatedAt);
	FString CustomString = FString::Printf(TEXT("\"custom_attribute\": %s"), *CustomAttribute);
	Contents += UpdatedAt;
	Contents += ",\n";
	Contents += CustomString;
	Contents += "}";

	const FErrorHandler ErrorHandler = FErrorHandler::CreateLambda(
		[OnError, OnConflicted](int32 Code, FString Message)
		{
			if (Code == static_cast<int32>(ErrorCodes::StatusPreconditionFailed) || Code == (int32)
				ErrorCodes::PartyStorageOutdatedUpdateData)
			{
				OnConflicted.ExecuteIfBound();
			}
			else
			{
				OnError.ExecuteIfBound(Code, Message);
			}
		});

	HttpClient.ApiRequest(TEXT("PUT"), Url, {}, Contents, OnSuccess, ErrorHandler);
}

void Lobby::WritePartyStorageRecursive(TSharedPtr<PartyStorageWrapper> DataWrapper)
{
	if (DataWrapper->RemainingAttempt <= 0)
	{
		DataWrapper->OnError.ExecuteIfBound(412, TEXT("Exhaust all retry attempt to modify party storage.."));
		return;
	}

	GetPartyStorage(DataWrapper->PartyId
		, THandler<FAccelByteModelsPartyDataNotif>::CreateLambda(
			[this, DataWrapper](FAccelByteModelsPartyDataNotif Result)
			{
				Result.Custom_attribute = DataWrapper->PayloadModifier(Result.Custom_attribute);

				FAccelByteModelsPartyDataUpdateRequest PartyStorageBodyRequest;

				PartyStorageBodyRequest.UpdatedAt = FCString::Atoi64(*Result.UpdatedAt);
				PartyStorageBodyRequest.Custom_attribute = Result.Custom_attribute;

				RequestWritePartyStorage(DataWrapper->PartyId
					, PartyStorageBodyRequest
					, DataWrapper->OnSuccess
					, DataWrapper->OnError
					, FSimpleDelegate::CreateLambda(
						[this, DataWrapper]()
						{
							DataWrapper->RemainingAttempt--;
							WritePartyStorageRecursive(DataWrapper);
						}));
			})
		, FErrorHandler::CreateLambda(
			[DataWrapper](int32 ErrorCode, FString ErrorMessage)
			{
				DataWrapper->OnError.ExecuteIfBound(ErrorCode, ErrorMessage);
			})
		);
}

void Lobby::SetRetryParameters(int32 NewTotalTimeout
	, int32 NewBackoffDelay
	, int32 NewMaxDelay)
{
	FReport::Log(FString(__FUNCTION__));

	if (WebSocket.IsValid())
	{
		UE_LOG(LogAccelByteLobby, Log, TEXT("Can't change retry parameters! Lobby is already connected."));
		return;
	}

	Lobby::TotalTimeout = NewTotalTimeout;
	Lobby::InitialBackoffDelay = NewBackoffDelay;
	Lobby::MaxBackoffDelay = NewMaxDelay;
}

void Lobby::FetchLobbyErrorMessages()
{
	FString Url = FString::Printf(TEXT("%s/lobby/v1/messages"), *SettingsRef.BaseUrl);

	TMap<FString, FString> Headers;
	Headers.Add(GHeaderABLogSquelch, TEXT("true"));

	HttpClient.Request(TEXT("GET"), Url, {}, TEXT(""), Headers
		, THandler<TArray<FLobbyMessages>>::CreateLambda(
			[&](const TArray<FLobbyMessages>& Result)
			{
				for(const FLobbyMessages& LobbyMessages : Result)
				{
					LobbyErrorMessages.Add(LobbyMessages.Code, LobbyMessages.CodeName);
				}

				UE_LOG(LogAccelByteLobby, Log, TEXT("fetching lobby error messages DONE! %d lobby messages has been cached"), LobbyErrorMessages.Num());
			})
		, FErrorHandler::CreateLambda(
			[](const int32 code, const FString& message)
			{
				UE_LOG(LogAccelByteLobby, Warning, TEXT("Error fetching lobby error messages! code %d, message %s"), code, *message);
			}));
}

void Lobby::ClearLobbyErrorMessages()
{
	if(LobbyErrorMessages.Num() > 0)
	{
		LobbyErrorMessages.Empty();
	}
}

void Lobby::SetTokenGenerator(TSharedPtr<IAccelByteTokenGenerator> TokenGeneratorRef)
{
	if(IsConnected())
	{
		UE_LOG(LogAccelByteLobby, Warning, TEXT("Cannot set token generator, lobby already connected"));
		return;
	}
		
	Lobby::TokenGenerator = TokenGeneratorRef;
	
	if(TokenGeneratorRef.IsValid())
	{
		Lobby::TokenGenerator->OnTokenReceived().Add(OnTokenReceived);
	}
}

Lobby::Lobby(Credentials & InCredentialsRef
	, Settings const& InSettingsRef
	, FHttpRetryScheduler& InHttpRef
	, float InPingDelay
	, float InInitialBackoffDelay
	, float InMaxBackoffDelay
	, float InTotalTimeout
	, TSharedPtr<IWebSocket> InWebSocket)
	: FApiBase(InCredentialsRef, InSettingsRef, InHttpRef)
	, LobbyCredentialsRef{InCredentialsRef}
	, PingDelay{InPingDelay}
	, InitialBackoffDelay{InInitialBackoffDelay}
	, MaxBackoffDelay{InMaxBackoffDelay}
	, TotalTimeout{InTotalTimeout}
	, BackoffDelay{InInitialBackoffDelay}
	, RandomizedBackoffDelay{InInitialBackoffDelay}
	, TimeSinceLastPing{.0f}
	, TimeSinceLastReconnect{.0f}
	, TimeSinceConnectionLost{.0f}
{
	InitializeV2MatchmakingNotifDelegates();
}

Lobby::~Lobby()
{
	// only disconnect when engine is still valid
	if(UObjectInitialized())
	{
		Disconnect(true);
		UnbindEvent();
		ConnectSuccess.Unbind();
		ConnectError.Unbind();
		ConnectionClosed.Unbind();
		Reconnecting.Unbind();
	}
}

TMap<FString, FString> Lobby::LobbyErrorMessages{};

} // Namespace Api
} // Namespace AccelByte
