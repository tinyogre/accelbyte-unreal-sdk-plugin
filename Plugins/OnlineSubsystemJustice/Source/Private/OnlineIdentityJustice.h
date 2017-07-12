// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
 
#include "CoreMinimal.h"
#include "UObject/CoreOnline.h"
#include "OnlineSubsystemTypes.h"
#include "Online.h"
#include "Runtime/Core/Public/Misc/DateTime.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystemJusticeTypes.h"

class FOAuthTokenJustice :
public FOnlineJsonSerializable
{
	
public:
	
	FOAuthTokenJustice() :
	ExpiresIn(0),
	LastTokenRefreshUtc(0),
	NextTokenRefreshUtc(0),
	TokenRefreshBackoff(0)
	{ }

	bool ShouldRefresh()
	{
		return NextTokenRefreshUtc < 1 || RefreshToken.IsEmpty() ? false : NextTokenRefreshUtc <= FDateTime::UtcNow();
	};
	void ScheduleNormalRefresh()
	{
		NextTokenRefreshUtc = LastTokenRefreshUtc + FTimespan::FromSeconds((ExpiresIn + 1) / 11);
		TokenRefreshBackoff = 0;
	};
	void ScheduelBackoffRefresh()
	{
		if (TokenRefreshBackoff < 1)
		{
			TokenRefreshBackoff = FTimespan::FromSeconds(10);
		}
		TokenRefreshBackoff *= 2;
		NextTokenRefreshUtc = FDateTime::UtcNow() + TokenRefreshBackoff + FTimespan::FromSeconds(FMath::RandRange(1, 60));
	};

	FDateTime GetExpireTime()    { return LastTokenRefreshUtc - FTimespan::FromSeconds(ExpiresIn); };
	FString   GetExpireTimeStr() { return GetExpireTime().ToIso8601(); };
	FString   GetRefreshStr()    { return FString::Printf(TEXT("expire=%s refresh=%s backoff=%s"),
								   *GetExpireTimeStr(), *NextTokenRefreshUtc.ToIso8601(), *TokenRefreshBackoff.ToString()); };
	void SetLastRefreshTimeToNow() { LastTokenRefreshUtc = FDateTime::UtcNow(); };
	
	FString AccessToken;
	FString RefreshToken;
	FString TokenType;
	double  ExpiresIn;
	
	/* TODO: add roles and perms
	 TArray Roles;
	 TArray Permissions;
	 */
	
	FString UserId;
	FString DisplayName;
	FString Namespace;
	
	BEGIN_ONLINE_JSON_SERIALIZER
	ONLINE_JSON_SERIALIZE("access_token", AccessToken);
	ONLINE_JSON_SERIALIZE("refresh_token", RefreshToken);
	ONLINE_JSON_SERIALIZE("token_type", TokenType);
	ONLINE_JSON_SERIALIZE("expires_in", ExpiresIn);
	ONLINE_JSON_SERIALIZE("user_id", UserId);
	ONLINE_JSON_SERIALIZE("display_name", DisplayName);
	ONLINE_JSON_SERIALIZE("namespace", Namespace);
	END_ONLINE_JSON_SERIALIZER
	
	// Tracking when to refresh the token
	FDateTime LastTokenRefreshUtc;
	FDateTime NextTokenRefreshUtc;
	FTimespan TokenRefreshBackoff;
};


/**
 * Info associated with an user account generated by this online service
 */
class FUserOnlineAccountJustice : 
	public FUserOnlineAccount
{

public:

	// FOnlineUser
	
	virtual TSharedRef<const FUniqueNetId> GetUserId() const override { return UserIdPtr; }
	virtual FString GetUserIdStr() const { return UserIdPtr->ToString(); }
	virtual FString GetRealName() const override { return TEXT("DummyRealName"); }
	virtual FString GetDisplayName(const FString& Platform = FString()) const override  { return TEXT("DummyDisplayName"); }
	virtual bool GetUserAttribute(const FString& AttrName, FString& OutAttrValue) const override;
	virtual bool SetUserAttribute(const FString& AttrName, const FString& AttrValue) override;

	// FUserOnlineAccount

	virtual FString GetAccessToken() const override { return TEXT("DummyAuthTicket"); }
	virtual bool GetAuthAttribute(const FString& AttrName, FString& OutAttrValue) const override;

	// FUserOnlineAccountJustice

	FUserOnlineAccountJustice(const FString& InUserId=TEXT("")) 
		: UserIdPtr(new FUniqueNetIdString(InUserId))
	{ }

	virtual ~FUserOnlineAccountJustice()
	{
	}

	/** User Id represented as a FUniqueNetId */
	TSharedRef<const FUniqueNetId> UserIdPtr;

	/** Additional key/value pair data related to auth */
	TMap<FString, FString> AdditionalAuthData;
	/** Additional key/value pair data related to user attribution */
	TMap<FString, FString> UserAttributes;

	/** Credentials */
	FOAuthTokenJustice Token;
};

/**
 * Justice service implementation of the online identity interface
 */
class FOnlineIdentityJustice : public IOnlineIdentity
{
public:

	// IOnlineIdentity

	virtual bool Login(int32 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials) override;
	virtual bool Logout(int32 LocalUserNum) override;
	virtual bool AutoLogin(int32 LocalUserNum) override;
	virtual TSharedPtr<FUserOnlineAccount> GetUserAccount(const FUniqueNetId& UserId) const override;
	virtual TArray<TSharedPtr<FUserOnlineAccount> > GetAllUserAccounts() const override;
	virtual TSharedPtr<const FUniqueNetId> GetUniquePlayerId(int32 LocalUserNum) const override;
	virtual TSharedPtr<const FUniqueNetId> CreateUniquePlayerId(uint8* Bytes, int32 Size) override;
	virtual TSharedPtr<const FUniqueNetId> CreateUniquePlayerId(const FString& Str) override;
	virtual ELoginStatus::Type GetLoginStatus(int32 LocalUserNum) const override;
	virtual ELoginStatus::Type GetLoginStatus(const FUniqueNetId& UserId) const override;
	virtual FString GetPlayerNickname(int32 LocalUserNum) const override;
	virtual FString GetPlayerNickname(const FUniqueNetId& UserId) const override;
	virtual FString GetAuthToken(int32 LocalUserNum) const override;
	virtual void GetUserPrivilege(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, const FOnGetUserPrivilegeCompleteDelegate& Delegate) override;
	virtual FPlatformUserId GetPlatformUserIdFromUniqueNetId(const FUniqueNetId& UniqueNetId) override;
	virtual FString GetAuthType() const override;

	// FOnlineIdentityJustice

	/**
	 * Constructor
	 *
	 * @param InSubsystem online subsystem being used
	 */
	FOnlineIdentityJustice(class FOnlineSubsystemJustice* InSubsystem);

	/**
	 * Destructor
	 */
	virtual ~FOnlineIdentityJustice();

private:

	void TokenPasswordGrantComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful,
						TSharedPtr<FUserOnlineAccountJustice> UserAccountPtr, int32 LocalUserNum);
	void TokenRefreshGrantComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful,
						TSharedPtr<FUserOnlineAccountJustice> UserAccountPtr, int32 LocalUserNum);

	void ScheduleNextTokenRefresh(FOAuthTokenJustice& OutToken);

	/**
	 * Should use the initialization constructor instead
	 */
	FOnlineIdentityJustice();

	/** Ids mapped to locally registered users */
	TMap<int32, TSharedPtr<const FUniqueNetId>> UserIds;

	/** Ids mapped to locally registered users */
	TMap<FUniqueNetIdString, TSharedRef<FUserOnlineAccountJustice>> UserAccounts;

	/** IAM Base URL Route */
	FString BaseURL;
	
	/** Client OAuth Client Credentials */
	FOnlineAccountCredentials Client;
	
	/** Client OAuth Token */
	FOAuthTokenJustice Token;

};

typedef TSharedPtr<FOnlineIdentityJustice, ESPMode::ThreadSafe> FOnlineIdentityJusticePtr;
