// Copyright (c) 2017-2018 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Private/Models/OAuthTokenJustice.h"
#include "Private/Models/UserCreateResponse.h"
#include "Private/Models/UserCreateRequest.h"
#include "Private/Models/ResetPasswordRequest.h"
#include "Private/AWSXRayJustice.h"
#include "Private/HTTPJustice.h"

DECLARE_DELEGATE_ThreeParams(FUserLoginCompleteDelegate, bool, FString, UOAuthTokenJustice*);
DECLARE_DELEGATE_TwoParams(FUserLogoutCompleteDelegate, bool, FString);
DECLARE_DELEGATE_ThreeParams(FRegisterPlayerCompleteDelegate, bool, FString, UUserCreateResponse*);
DECLARE_DELEGATE_TwoParams(FVerifyNewPlayerCompleteDelegate, bool, FString);
DECLARE_DELEGATE_TwoParams(FForgotPasswordStep1CompleteDelegate, bool, FString);
DECLARE_DELEGATE_TwoParams(FForgotPasswordStep2CompleteDelegate, bool, FString);

enum FTaskTypeJustice
{
	IdentityRefresh = 1
};

enum FGrantTypeJustice
{
	PasswordGrant = 1,
	RefreshGrant = 2,
	ClientCredentialGrant = 3,
	Anonymous = 4,
};

class JUSTICESDK_API JusticeIdentity 
{
	
public:
	static void Login(FString LoginId, FString Password, FGrantTypeJustice GrantType, FUserLoginCompleteDelegate OnComplete);
	
	static void UserLogout(FUserLogoutCompleteDelegate OnComplete);
	static void RegisterNewPlayer(FString UserId, FString Password, FString DisplayName, FString AuthType, FRegisterPlayerCompleteDelegate OnComplete);
	static void VerifyNewPlayer(FString UserId, FString VerificationCode, FVerifyNewPlayerCompleteDelegate OnComplete);
	static void ForgotPasswordStep1(FString LoginId, FForgotPasswordStep1CompleteDelegate onComplete);
	static void ForgotPasswordStep2(FString UserId, FString VerificationCode, FString NewPassword, FForgotPasswordStep2CompleteDelegate onComplete);
	
	// Client specific
	static void ClientLogin();
	static void ClientLogout();

	static OAuthTokenJustice* GetUserToken();
	static OAuthTokenJustice* GetClientToken();
	static FString GetUserId();

private:
	static void OnLoginComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful, TSharedRef<FAWSXRayJustice> RequestTrace, FUserLoginCompleteDelegate OnComplete);
	static void OnRefreshComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful, TSharedRef<FAWSXRayJustice> RequestTrace);
	static void OnClientCredentialComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful, TSharedRef<FAWSXRayJustice> RequestTrace);
	static void OnLogoutComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful, TSharedRef<FAWSXRayJustice> RequestTrace, FUserLogoutCompleteDelegate OnComplete);	
	static void OnClientLogoutComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful, TSharedRef<FAWSXRayJustice> RequestTrace);
	static void OnRegisterNewPlayerComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful, TSharedRef<FAWSXRayJustice> RequestTrace, FRegisterPlayerCompleteDelegate OnComplete);
	static void OnVerifyNewPlayerComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful, TSharedRef<FAWSXRayJustice> RequestTrace, FVerifyNewPlayerCompleteDelegate OnComplete);	
	static void OnForgotPasswordStep1Complete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful, TSharedRef<FAWSXRayJustice> RequestTrace, FForgotPasswordStep1CompleteDelegate OnComplete);
	static void OnForgotPasswordStep2Complete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccessful, TSharedRef<FAWSXRayJustice> RequestTrace, FForgotPasswordStep2CompleteDelegate OnComplete);


};
