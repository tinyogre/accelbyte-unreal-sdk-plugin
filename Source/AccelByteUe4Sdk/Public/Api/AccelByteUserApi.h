﻿// Copyright (c) 2018-2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Core/AccelByteError.h"
#include "Core/AccelByteHttpRetryScheduler.h"
#include "Core/AccelByteSettings.h"
#include "Core/AccelByteApiBase.h"
#include "Models/AccelByteUserModels.h"

namespace AccelByte
{

namespace Api
{

class Entitlement;
class Item;

/**
 * @brief User management API for creating user, verifying user, and resetting password.
 */
class ACCELBYTEUE4SDK_API User : public FApiBase
{
public:
	User(Credentials& Credentials, Settings& Settings, FHttpRetryScheduler& InHttpRef);
	~User();
private:
	TSharedRef<Credentials, ESPMode::ThreadSafe> UserCredentialsRef;
public:
	/**
	* @brief delegate for handling upgrade headless account notification.
	*/
	DECLARE_DELEGATE(FUpgradeNotif);

	/**
	 * @brief These events happen before every login event, such as clearing CredentialRef post-auth info.
	 * - Call this last, just before the actual login call.
	 */
	void FinalPreLoginEvents() ;


#pragma region Login Methods
	/**
	 * @brief Log in with email/username account.
	 *
	 * @param Username User email address or username.
	 * @param Password Password.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LoginWithUsername(const FString& Username
		, const FString& Password
		, const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief Log in with email/username account with 2FA enable
	 *
	 * @param Username User email address or username.
	 * @param Password Password.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LoginWithUsername(const FString& Username
		, const FString& Password
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief Log in with email/username account using v3 endpoint.
	 *
	 * @param Username User email address or username.
	 * @param Password Password.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 * @param bRememberMe This will use for refresh token expiration extension, default value is false.
	 */
	void LoginWithUsernameV3(const FString& Username
		, const FString& Password
		, const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError
		, const bool bRememberMe = false);

	/**
	 * @brief Log in with email/username account using v3 endpoint with 2FA enable
	 *
	 * @param Username User email address or username.
	 * @param Password Password.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 * @param bRememberMe This will use for refresh token expiration extension, default value is false.
	 */
	void LoginWithUsernameV3(const FString& Username
		, const FString& Password
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError
		, const bool bRememberMe = false);

	/**
	 * @brief Log in with email/username account using v4 endpoint with 2FA enable
	 *
	 * @param Username User email address or username.
	 * @param Password Password.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 * @param bRememberMe This will use for refresh token expiration extension, default value is false.
	 */
	void LoginWithUsernameV4(const FString& Username
		, const FString& Password
		, const THandler<FAccelByteModelsLoginQueueTicketInfo>& OnSuccess
		, const FOAuthErrorHandler& OnError
		, const bool bRememberMe = false);

	/**
	 * @brief Log in with device ID (anonymous log in).
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LoginWithDeviceId(const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError
		, bool bCreateHeadless = true);

	/**
	 * @brief Log in with device ID (anonymous log in).
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LoginWithDeviceId(const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError
		, bool bCreateHeadless = true);

	/**
	 * @brief Log in with device ID (anonymous log in).
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LoginWithDeviceIdV4(const THandler<FAccelByteModelsLoginQueueTicketInfo>& OnSuccess
		, const FOAuthErrorHandler& OnError
		, bool bCreateHeadless = true);

	/**
	 * @brief Log in with another platform account e.g. Steam, Google, Facebook, Twitch, etc.
	 *
	 * @param PlatformType Specify platform type that chosen by user to log in.
	 * @param PlatformToken Authentication code that provided by another platform.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LoginWithOtherPlatform(EAccelBytePlatformType PlatformType
		, const FString& PlatformToken
		, const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief Log in with another platform account e.g. Steam, Google, Facebook, Twitch, etc. with 2FA enable
	 *
	 * @param PlatformType Specify platform type that chosen by user to log in.
	 * @param PlatformToken Authentication code that provided by another platform.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 * @param bCreateHeadless If directly create new account when not linked yet, default value is true
	 */
	void LoginWithOtherPlatform(EAccelBytePlatformType PlatformType
		, const FString& PlatformToken
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError
		, bool bCreateHeadless = true);

	/**
	 * @brief Log in with another platform Id account e.g. Steam, Google, Twitch, etc especially to support OIDC (with 2FA enable)
	 *
	 * @param PlatformId Specify platform type, string type of this field makes support OpenID Connect (OIDC)
	 * @param PlatformToken Authentication code that provided by another platform.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 * @param bCreateHeadless If directly create new account when not linked yet, default value is true
	 */
	void LoginWithOtherPlatformId(const FString& PlatformId
		, const FString& PlatformToken
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError
		, bool bCreateHeadless = true);

	/**
	 * @brief Log in with another platform account e.g. Steam, Google, Facebook, Twitch, etc. with 2FA enable
	 *
	 * @param PlatformType Specify platform type that chosen by user to log in.
	 * @param PlatformToken Authentication code that provided by another platform.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 * @param bCreateHeadless If directly create new account when not linked yet, default value is true
	 */
	void LoginWithOtherPlatformV4(EAccelBytePlatformType PlatformType
		, const FString& PlatformToken
		, const THandler<FAccelByteModelsLoginQueueTicketInfo>& OnSuccess
		, const FOAuthErrorHandler& OnError
		, bool bCreateHeadless = true);

	/**
	 * @brief Log in with another platform Id account e.g. Steam, Google, Twitch, etc especially to support OIDC (with 2FA enable)
	 *
	 * @param PlatformId Specify platform type, string type of this field makes support OpenID Connect (OIDC)
	 * @param PlatformToken Authentication code that provided by another platform.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 * @param bCreateHeadless If directly create new account when not linked yet, default value is true
	 */
	void LoginWithOtherPlatformIdV4(const FString& PlatformId
		, const FString& PlatformToken
		, const THandler<FAccelByteModelsLoginQueueTicketInfo>& OnSuccess
		, const FOAuthErrorHandler& OnError
		, bool bCreateHeadless = true);

	/**
	 * @brief Login with native platform and secondary platform. Currently support Windows only.
	 *
	 * @param NativePlatform From the native subsystem
	 * @param NativePlatformToken The auth ticket from native identity interface
	 * @param SecondaryPlatform From the secondary platform subsystem
	 * @param SecondaryPlatformToken The auth ticket from secondary platform interface
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LoginWithSimultaneousPlatform(EAccelBytePlatformType NativePlatform
		, const FString& NativePlatformToken
		, EAccelBytePlatformType SecondaryPlatform
		, const FString& SecondaryPlatformToken
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief Login with native platform and secondary platform. Currently support Windows only.
	 *
	 * @param NativePlatform From the native subsystem
	 * @param NativePlatformToken The auth ticket from native identity interface
	 * @param SecondaryPlatform From the secondary platform subsystem
	 * @param SecondaryPlatformToken The auth ticket from secondary platform interface
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LoginWithSimultaneousPlatform(const FString& NativePlatform
		, const FString& NativePlatformToken
		, const FString& SecondaryPlatform
		, const FString& SecondaryPlatformToken
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief Login with native platform and secondary platform. Currently support Windows only.
	 *
	 * @param NativePlatform From the native subsystem
	 * @param NativePlatformToken The auth ticket from native identity interface
	 * @param SecondaryPlatform From the secondary platform subsystem
	 * @param SecondaryPlatformToken The auth ticket from secondary platform interface
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LoginWithSimultaneousPlatformV4(EAccelBytePlatformType NativePlatform
		, const FString& NativePlatformToken
		, const EAccelBytePlatformType& SecondaryPlatform
		, const FString& SecondaryPlatformToken
		, const THandler<FAccelByteModelsLoginQueueTicketInfo>& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief Login with native platform and secondary platform. Currently support Windows only.
	 *
	 * @param NativePlatform From the native subsystem
	 * @param NativePlatformToken The auth ticket from native identity interface
	 * @param SecondaryPlatform From the secondary platform subsystem
	 * @param SecondaryPlatformToken The auth ticket from secondary platform interface
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LoginWithSimultaneousPlatformV4(const FString& NativePlatform
		, const FString& NativePlatformToken
		, const FString& SecondaryPlatform
		, const FString& SecondaryPlatformToken
		, const THandler<FAccelByteModelsLoginQueueTicketInfo>& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief Verify log in with new device when user enabled 2FA.
	 *
	 * @param MfaToken return from BE when user login with new device and 2FA enabled.
	 * @param AuthFactorType 2FA factor used. Could be "authenticator" or "backupCode". User should make sure what type used
	 * @param Code auth code from 3rd party authenticator or backupCode.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 * @param bRememberDevice This will use for refresh token expiration extension, default value is false.
	 */
	void VerifyLoginWithNewDevice2FAEnabled(const FString& MfaToken
		, EAccelByteLoginAuthFactorType AuthFactorType
		, const FString& Code
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError
		, bool bRememberDevice = false);

	/**
	 * @brief Verify log in with new device when user enabled 2FA.
	 *
	 * @param MfaToken return from BE when user login with new device and 2FA enabled.
	 * @param AuthFactorType 2FA factor used. Could be "authenticator" or "backupCode". User should make sure what type used
	 * @param Code auth code from 3rd party authenticator or backupCode.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 * @param bRememberDevice This will use for refresh token expiration extension, default value is false.
	 */
	void VerifyLoginWithNewDevice2FAEnabledV4(const FString& MfaToken
		, EAccelByteLoginAuthFactorType AuthFactorType
		, const FString& Code
		, const THandler<FAccelByteModelsLoginQueueTicketInfo>& OnSuccess
		, const FOAuthErrorHandler& OnError
		, bool bRememberDevice = false);

	/**
	 * @brief Log in from AccelByte Launcher.
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LoginWithLauncher(const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief Log in from AccelByte Launcher.
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LoginWithLauncher(const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief Log in from AccelByte Launcher.
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LoginWithLauncherV4(const THandler<FAccelByteModelsLoginQueueTicketInfo>& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief login with refresh token
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LoginWithRefreshToken(const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief login with refresh token
	 *
	 * @param RefreshToken the refresh token for login.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LoginWithRefreshToken(const FString& RefreshToken
		, const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief login with refresh token
	 *
	 * @param RefreshToken the refresh token for login.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 * @param PlatformUserId Target platform user id to cache.
	 */
	void LoginWithRefreshToken(const FString& RefreshToken
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError
		, const FString& PlatformUserId = TEXT(""));

	/**
	 * @brief login with refresh token
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 * @param RefreshToken the refresh token for login.
	 * @param PlatformUserId Target platform user id to cache.
	 */
	void LoginWithRefreshTokenV4(const THandler<FAccelByteModelsLoginQueueTicketInfo>& OnSuccess
		, const FOAuthErrorHandler& OnError
		, const FString& RefreshToken = TEXT("")
		, const FString& PlatformUserId = TEXT(""));

	/**
	 * @brief Refresh the platform token that is stored in the IAM backend.
	 * Therefore we can prevent expiration on the backend.
	 * This endpoint also not generate any event or AB Access/Refresh Token.
	 *
	 * @param Platform The targeted platform to be refreshed.
	 * @param NativePlatformToken The platform token that will used to refresh IAM storage.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void RefreshPlatformToken(EAccelBytePlatformType NativePlatform
		, const FString& NativePlatformToken
		, const THandler<FPlatformTokenRefreshResponse>& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief Refresh the platform token that is stored in the IAM backend.
	 * Therefore we can prevent expiration on the backend.
	 * This endpoint also not generate any event or AB Access/Refresh Token.
	 *
	 * @param Platform The targeted platform to be refreshed.
	 * @param NativePlatformToken The platform token that will used to refresh IAM storage.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void RefreshPlatformToken(const FString& NativePlatform
		, const FString& NativePlatformToken
		, const THandler<FPlatformTokenRefreshResponse>& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief Relogin using the previously logged-in platform account. WINDOWS ONLY
	 *
	 * @param PlatformUserID The user ID that used previously and used .
	 * @param OnSuccess This will be called when the provided Platform User Account is acknowledged and not expired yet. Continue the session.
	 * @param OnError This will be called when the Platform User Account that provided is not known yet — OR it existed but already expired.
	 */
	void TryRelogin(const FString& PlatformUserID
		, const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief Relogin using the previously logged-in platform account. WINDOWS ONLY
	 *
	 * @param PlatformUserID The user ID that used previously and used .
	 * @param OnSuccess This will be called when the provided Platform User Account is acknowledged and not expired yet. Continue the session.
	 * @param OnError This will be called when the Platform User Account that provided is not known yet — OR it existed but already expired.
	 */
	void TryRelogin(const FString& PlatformUserID
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief Relogin using the previously logged-in platform account. WINDOWS ONLY
	 *
	 * @param PlatformUserID The user ID that used previously and used .
	 * @param OnSuccess This will be called when the provided Platform User Account is acknowledged and not expired yet. Continue the session.
	 * @param OnError This will be called when the Platform User Account that provided is not known yet — OR it existed but already expired.
	 */
	void TryReloginV4(const FString& PlatformUserID
		, const THandler<FAccelByteModelsLoginQueueTicketInfo>& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief Create Headless Account And Login
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void CreateHeadlessAccountAndLogin(const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief Create Headless Account And Login
	 *
	 * @param LinkingToken LinkingToken
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void CreateHeadlessAccountAndLogin(const FString& LinkingToken
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief Create Headless Account And Login
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 * @param LinkingToken LinkingToken
	 */
	void CreateHeadlessAccountAndLoginV4(const THandler<FAccelByteModelsLoginQueueTicketInfo>& OnSuccess
		, const FOAuthErrorHandler& OnError
		, const FString& LinkingToken = TEXT(""));

	/**
	 * @brief Authentication With Platform Link And Login
	 *
	 * @param Username Username/user's email that will be validated.
	 * @param Password This is password's account exist
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void AuthenticationWithPlatformLinkAndLogin(const FString& Username
		, const FString& Password
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief Authentication With Platform Link And Login
	 *
	 * @param Username Username/user's email that will be validated.
	 * @param Password This is password's account exist
	 * @param LinkingToken Specified Linking token
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void AuthenticationWithPlatformLinkAndLogin(const FString& Username
		, const FString& Password
		, const FString& LinkingToken
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief Authentication With Platform Link And Login
	 *
	 * @param Username Username/user's email that will be validated.
	 * @param Password This is password's account exist
	 * @param LinkingToken Specified Linking token
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void AuthenticationWithPlatformLinkAndLoginV4(const FString& Username
		, const FString& Password
		, const THandler<FAccelByteModelsLoginQueueTicketInfo>& OnSuccess
		, const FOAuthErrorHandler& OnError
		, const FString& LinkingToken = TEXT(""));

	/**
	 * @brief Claim Access Token using Login Ticket.
	 *
	 * @param LoginTicket Login Ticket to claim the access token
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void ClaimAccessToken(const FString LoginTicket
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError);

#pragma endregion /Login Methods


	/**
	 * @brief Log out current user session. Access tokens, user ID, and other credentials from memory will be removed.
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void Logout(const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief Remove access tokens, user ID, and other credentials from memory.
	 */
	void ForgetAllCredentials();

	/**
	 * @brief This function will register a new user with email-based account.
	 *
	 * @param Username Email address of the user, can be used as login username.
	 * @param Password The Password.
	 * @param DisplayName The DisplayName.
	 * @param Country User's country, ISO3166-1 alpha-2 two letter, e.g. US.
	 * @param DateOfBirth User's date of birth, valid values are between 1905-01-01 until current date.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FAccountUserData.
	 * @param OnError This will be called when the operation failed.
	 * @param UniqueDisplayName If uniqueDisplayNameEnabled config is enabled, this param is required to be filled.
	 */
	void Register(const FString& Username
		, const FString& Password
		, const FString& DisplayName
		, const FString& Country
		, const FString& DateOfBirth
		, const THandler<FRegisterResponse>& OnSuccess
		, const FErrorHandler& OnError
		, const FString& UniqueDisplayName = TEXT(""));

	/**
	 * @brief This function will register a new user with email-based account.
	 *
	 * @param EmailAddress Email address of the user, can be used as login username.
	 * @param Username The username can be used as login username, case insensitive, alphanumeric with allowed symbols underscore (_) and dot (.).
	 * @param Password The Password.
	 * @param DisplayName The DisplayName.
	 * @param Country User's country, ISO3166-1 alpha-2 two letter, e.g. US.
	 * @param DateOfBirth User's date of birth, valid values are between 1905-01-01 until current date.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FAccountUserData.
	 * @param OnError This will be called when the operation failed.
	 * @param UniqueDisplayName If uniqueDisplayNameEnabled config is enabled, this param is required to be filled.
	 */
	void Registerv2(const FString& EmailAddress
		, const FString& Username
		, const FString& Password
		, const FString& DisplayName
		, const FString& Country
		, const FString& DateOfBirth
		, const THandler<FRegisterResponse>& OnSuccess
		, const FErrorHandler& OnError
		, const FString& UniqueDisplayName = TEXT(""));

	/**
	 * @brief This function will register a new user with email-based account and complete agreement.
	 *
	 * @param RegisterRequest invoke Accepted Policies, EmailAddress, Username, Password, Display Name, Country and Date of Birth
	 * @param OnSuccess This will be called when the operation succeeded. The result is FAccountUserData.
	 * @param OnError This will be called when the operation failed.
	 */
	void Registerv3(const FRegisterRequestv3& RegisterRequest
		, const THandler<FRegisterResponse>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will get data of currently logged in user.
	 *
	 * @param OnSuccess This will be called when the operation succeeded. The result is FAccountUserData.
	 * @param OnError This will be called when the operation failed.
	 * @param bIncludeAllPlatforms This will return all 3rd party platform accounts linked to account. Default is False
	 */
	void GetData(const THandler<FAccountUserData>& OnSuccess
		, const FErrorHandler& OnError
		, bool bIncludeAllPlatforms = false);

	/**
	 * @brief This function will upgrade user's headless account. You may call SendUserAccountVerificationCode afterwards.
	 * Headless account is an account that doesn't have an email and password.
	 * If user logs in with a device/platform and they cannot login with email-and-password, their account is considered as a headless account.
	 * Therefore, the function requests user’s Username and Password for parameters.
	 *
	 * @param Username The EmailAddress of the user.
	 * @param Password The Password.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 * @param bNeedVerificationCode Will send verification code to email if true, default false.
	 */
	void Upgrade(const FString& Username
		, const FString& Password
		, const THandler<FAccountUserData>& OnSuccess
		, const FErrorHandler& OnError
		, bool bNeedVerificationCode = false);

	/**
	 * @brief This function will upgrade user's headless account. You may call SendUserAccountVerificationCode afterwards.
	 * Headless account is an account that doesn't have an email and password.
	 * If user logs in with a device/platform and they cannot login with email-and-password, their account is considered as a headless account.
	 * Therefore, the function requests user’s Username and Password for parameters.
	 *
	 * @param EmailAddress The EmailAddress of the user.
	 * @param Username The Username.
	 * @param Password The Password.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void Upgradev2(const FString& EmailAddress
		, const FString& Username
		, const FString& Password
		, const THandler<FAccountUserData>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief Verify user's email. User should login with email and password first to get access token.
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void SendVerificationCode(const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will verify the registered email **after** user receives verification code sent with ::SendUserAccountVerificationCode() to their email.
	 *
	 * @param VerificationCode The VerificationCode.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void Verify(const FString& VerificationCode
		, const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief Send a request to reset user's password. For some reason the endpoint uses HTTP basic authentication instead of bearer, because you know... our IAM is very inconsistent.
	 *
	 * @param EmailAddress User email address or phone number.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void SendResetPasswordCode(const FString& EmailAddress
		, const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief Reset user's password with sent verification code.
	 *
	 * @ref SendPasswordResetCode()
	 * @param EmailAddress User email address or phone number.
	 * @param VerificationCode Code sent to the email address.
	 * @param NewPassword The new password.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void ResetPassword(const FString& VerificationCode
		, const FString& EmailAddress
		, const FString& NewPassword
		, const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function should be done before user upgrade their headless account. After this function successfully called, obtain the verification code from the submitted email. Then call UpgradeHeadlessAccountWithVerificationCode function afterwards.
	 *
	 * @param EmailAddress Email or phone number that will be used to upgrade the headless account.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	*/
	void SendUpgradeVerificationCode(const FString& EmailAddress
		, const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief Send request verification code with Change email context. User should login with email and password first to get access token.
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void SendUpdateEmailVerificationCode(const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function should be called after you call SendUserUpgradeVerificationCode and obtain verification code.
	 *
	 * @param Username Email or phone number that will be used to upgrade the headless account.
	 * @param Password User's password.
	 * @param VerificationCode User's verification code that obtained from email.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 * @param UniqueDisplayName If uniqueDisplayNameEnabled config is enabled, this param is required to be filled.
	 */
	void UpgradeAndVerify(const FString& Username
		, const FString& Password
		, const FString& VerificationCode
		, const THandler<FAccountUserData>& OnSuccess
		, const FErrorHandler& OnError
		, const FString& UniqueDisplayName = TEXT(""));

	/**
	* @brief This function should be called after you call SendUpgradeVerificationCode and obtain verification code.
	*
	* @param UpgradeAndVerifyRequest contain user data will be used to upgrade the headless account.
	* @param OnSuccess This will be called when the operation succeeded.
	* @param OnError This will be called when the operation failed.
	*/
	void UpgradeAndVerify2(const FUpgradeAndVerifyRequest& UpgradeAndVerifyRequest
		, const THandler<FAccountUserData>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function gets user's platform accounts linked to user’s account.
	 *
	 * @param OnSuccess This will be called when the operation succeeded. The result is TArray<FPlatformLink>.
	 * @param OnError This will be called when the operation failed.
	 */
	void GetPlatformLinks(const THandler<FPagedPlatformLinks>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function links user's current account to their other account in other platform.
	 * Ticket for each platform (PlatformToken) can be obtained from browser with platform linking URL (e.g. Facebook, Google, Twitch platform).
	 * The browser will redirect the URL to a site with a code in form of parameter URL.
	 *
	 * @param PlatformType The PlatformType (Steam, PS4, Xbox, etc).
	 * @param Ticket The Ticket.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LinkOtherPlatform(EAccelBytePlatformType PlatformType
		, const FString& Ticket
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief This function links user's current account to their other account in other platform.
	 * Ticket for each platform (PlatformToken) can be obtained from browser with platform linking URL (e.g. Facebook, Google, Twitch platform).
	 * The browser will redirect the URL to a site with a code in form of parameter URL.
	 *
	 * @param PlatformType The PlatformType (Steam, PS4, Xbox, etc).
	 * @param Ticket The Ticket.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LinkOtherPlatform(EAccelBytePlatformType PlatformType
		, const FString& Ticket
		, const FVoidHandler& OnSuccess
		, const FCustomErrorHandler& OnError);

	/**
	 * @brief This function links user's current account to their other account in other platform, especially to support OIDC
	 * Ticket for each platform (PlatformToken) can be obtained from browser with platform linking URL
	 * The browser will redirect the URL to a site with a code in form of parameter URL.
	 *
	 * @param PlatformId Specify platform type, string type of this field makes support OpenID Connect (OIDC)
	 * @param Ticket The Ticket.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LinkOtherPlatformId(const FString& PlatformId, const FString& Ticket, const FVoidHandler& OnSuccess,
	                       const FCustomErrorHandler& OnError);

	/**
	 * @brief This function forced links user's current account to their other account in other platform.
	 * Use this only if the general LinkOtherPlatform get conflicted and getting confirmation from user.
	 *
	 * @param PlatformType The PlatformType (Steam, PS4, Xbox, etc).
	 * @param PlatformUserId The UserId from the other platfrom you want to link.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void ForcedLinkOtherPlatform(EAccelBytePlatformType PlatformType
		, const FString& PlatformUserId
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief This function forced links user's current account to their other account in other platform.
	 * Use this only if the general LinkOtherPlatform get conflicted and getting confirmation from user.
	 *
	 * @param PlatformType The PlatformType (Steam, PS4, Xbox, etc).
	 * @param PlatformUserId The UserId from the other platfrom you want to link.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void ForcedLinkOtherPlatform(EAccelBytePlatformType PlatformType
		, const FString& PlatformUserId
		, const FVoidHandler& OnSuccess
		, const FCustomErrorHandler& OnError);

	/**
	 * @brief This function unlinks user's current account from their other account in other platform
	 *
	 * @param PlatformType The PlatformType (Steam, PS4, Xbox, etc).
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 *
	 * @deprecated This method will be removed in the future. Please use same function with the FCustomErrorHandler parameter instead.
	 */
	void UnlinkOtherPlatform(EAccelBytePlatformType PlatformType
		, const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function unlinks user's current account from their other account in other platform
	 *
	 * @param PlatformType The PlatformType (Steam, PS4, Xbox, etc).
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void UnlinkOtherPlatform(EAccelBytePlatformType PlatformType
		, const FVoidHandler& OnSuccess
		, const FCustomErrorHandler& OnError);

	/**
	 * @brief This function unlinks user's current account from their other account in other platform, especially to support OIDC
	 *
	 * @param PlatformId Specify platform type, string type of this field makes support OpenID Connect (OIDC)
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void UnlinkOtherPlatformId(const FString& PlatformId
		, const FVoidHandler& OnSuccess
		, const FCustomErrorHandler& OnError);

	/**
	 * @brief This function unlinks all the user's current account from their other accounts in other platforms.
	 *
	 * @param PlatformType The PlatformType (Steam, PS4, Xbox, etc).
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 *
	 * @note Use this API to unlink all the user's current account from their other accounts in other platforms within the game namespace.
	 * It resolves issues with the old API by ensuring successful unlinking across multiple namespaces.
	 * After calling this API, if a user logs in to any namespace with the same 3rd platform account,
	 * they will be logged in as a different account.
	 */
	void UnlinkAllOtherPlatform(EAccelBytePlatformType PlatformType
		, const FVoidHandler& OnSuccess
		, const FCustomErrorHandler& OnError);

	/**
	 * @brief This function unlinks all the user's current account from their other accounts in other platforms, especially to support OIDC.
	 *
	 * @param PlatformId Specify platform type, string type of this field makes support OpenID Connect (OIDC).
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 *
	 * @note Use this API to unlink all the user's current account from their other accounts in other platforms within the game namespace.
	 * It resolves issues with the old API by ensuring successful unlinking across multiple namespaces.
	 * After calling this API, if a user logs in to any namespace with the same 3rd platform account,
	 * they will be logged in as a different account.
	 */
	void UnlinkAllOtherPlatformId(const FString& PlatformId
		, const FVoidHandler& OnSuccess
		, const FCustomErrorHandler& OnError);

	/**
	 * @brief This function will search user by their Username or Display Name. The query will be used to find the user with the most approximate username or display name.
	 *
	 * @param Query Targeted user's Username or Display Name.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FPagedPublicUsersInfo.
	 * @param OnError This will be called when the operation failed.
	 */
	void SearchUsers(const FString& Query
		, const THandler<FPagedPublicUsersInfo>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will search user by their Username or Display Name. The query will be used to find the user with the most approximate username or display name.
	 *
	 * @param Query Targeted user's Username or Display Name.
	 * @param Offset Targeted offset query filter.
	 * @param Limit Targeted limit query filter.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FPagedPublicUsersInfo.
	 * @param OnError This will be called when the operation failed.
	 */
	void SearchUsers(const FString& Query
		, int32 Offset
		, int32 Limit
		, const THandler<FPagedPublicUsersInfo>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will search user by their Username or Display Name. The query will be used to find the user with the most approximate username or display name.
	 *
	 * @param Query Targeted user's Username or Display Name.
	 * @param PlatformType The PlatformType (Steam, PS4, Xbox, etc).
	 * @param PlatformBy Filter the responded PagedPublicUsersInfo by SearchPlatformType.
	 * @param Offset Targeted offset query filter.
	 * @param Limit Targeted limit query filter.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FPagedPublicUsersInfo.
	 * @param OnError This will be called when the operation failed.
	 */
	void SearchUsers(const FString& Query
		, EAccelBytePlatformType PlatformType
		, EAccelByteSearchPlatformType PlatformBy
		, const THandler<FPagedPublicUsersInfo>& OnSuccess
		, const FErrorHandler& OnError
		, const int32 Offset = 0
		, const int32 Limit = 100);

	/**
	 * @brief Searches for users on third-party platforms using their Username or Display Name.
	 * This function specifically targets users on platforms and utilizes the platform's DisplayName for the search.
	 *
	 * @param Query Targeted user's Username or Display Name.
	 * @param PlatformId Specify platform type, string type of this field makes support OpenID Connect (OIDC).
	 * @param PlatformBy Filter the responded PagedPublicUsersInfo by SearchPlatformType.
	 * @param Offset Targeted offset query filter.
	 * @param Limit Targeted limit query filter.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FPagedPublicUsersInfo.
	 * @param OnError This will be called when the operation failed.
	 */
	void SearchUsers(const FString& Query
		, const FString& PlatformId
		, EAccelByteSearchPlatformType PlatformBy
		, const THandler<FPagedPublicUsersInfo>& OnSuccess
		, const FErrorHandler& OnError
		, const int32 Offset = 0
		, const int32 Limit = 100);

	/**
	 * @brief Searches for users on third-party platforms using their Username or Display Name.
	 * This function specifically targets users on platforms and utilizes the platform's DisplayName for the search.
	 *
	 * @param Query Targeted user's Username or Display Name.
	 * @param By Filter the responded PagedPublicUsersInfo by SearchType. Choose the SearchType.ALL if you want to be responded with all query type.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FPagedPublicUsersInfo.
	 * @param OnError This will be called when the operation failed.
	 * @param Offset Targeted offset query filter.
	 * @param Limit Targeted limit query filter.
	 * @param PlatformId Specify platform type, string type of this field makes support OpenID Connect (OIDC).
	 * @param PlatformBy Filter the responded PagedPublicUsersInfo by SearchPlatformType.
	 */
	void SearchUsers(const FString& Query
		, EAccelByteSearchType By
		, const THandler<FPagedPublicUsersInfo>& OnSuccess
		, const FErrorHandler& OnError
		, const int32& Offset = 0
		, const int32& Limit = 100
		, const FString& PlatformId = TEXT("")
		, EAccelByteSearchPlatformType PlatformBy = EAccelByteSearchPlatformType::NONE);

	/**
	 * @brief This function will search user by userId.
	 *
	 * @param UserId Targeted user's ID.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FAccountUserData.
	 * @param OnError This will be called when the operation failed.
	 */
	void GetUserByUserId(const FString& UserId
		, const THandler<FSimpleUserData>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will get user by other platform user id it linked to.
	 * Note : For Nintendo Login you have to add Environment ID Suffix behind platform id with PlatformID:EnvironmentID as format
	 * e.g csg123jskansdk:dd1
	 *
	 * @param PlatformType Other platform type .
	 * @param OtherPlatformUserId Targeted user's ID.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FAccountUserData.
	 * @param OnError This will be called when the operation failed.
	 */
	void GetUserByOtherPlatformUserId(EAccelBytePlatformType PlatformType
		, const FString& OtherPlatformUserId
		, const THandler<FAccountUserData>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function for update user account info within the game.
	 *
	 * @param UpdateRequest The data you want to update. for DateOfBirth, the format is YYYY-MM-DD.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FAccountUserData.
	 * @param OnError This will be called when the operation failed.
	 */
	void UpdateUser(FUserUpdateRequest UpdateRequest
		, const THandler<FAccountUserData>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function for update user account info within the game.
	 *
	 * @param UpdateEmailRequest Set verify code and new user email address.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FUserData.
	 * @param OnError This will be called when the operation failed.
	 */
	void UpdateEmail(FUpdateEmailRequest UpdateEmailRequest
		, const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will get user(s) by other platform user id(s) it linked to.
	 * Note : For Nintendo Login you have to add Environment ID Suffix behind platform id with PlatformID:EnvironmentID as format
	 * e.g csg123jskansdk:dd1
	 *
	 * @param PlatformType Other platform type.
	 * @param OtherPlatformUserId Targeted user's ID(s).
	 * @param OnSuccess This will be called when the operation succeeded. The result is FBulkPlatformUserIdResponse.
	 * @param OnError This will be called when the operation failed.
	 * @param bRawPuid Show unencrypted platform user id in result (disabled by default).
	 */
	void BulkGetUserByOtherPlatformUserIds(EAccelBytePlatformType PlatformType
		, const TArray<FString>& OtherPlatformUserId
		, const THandler<FBulkPlatformUserIdResponse>& OnSuccess
		, const FErrorHandler& OnError
		, const bool bRawPuid = false);

	/**
	 * @brief This function will get country information from user's IP.
	 *
	 * @param OnSuccess This will be called when the operation succeeded. The result is FCountryInfo.
	 * @param OnError This will be called when the operation failed.
	 */
	void GetCountryFromIP(const THandler<FCountryInfo>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will check whether user can play the game by having it purchased or subscribed.
	 *
	 * @param OnSuccess This will be called when the operation succeeded. The result is boolean.
	 * @param OnError This will be called when the operation failed.
	 */
	void GetUserEligibleToPlay(const THandler<bool>& OnSuccess
		, const FErrorHandler & OnError);

	/**
	 * @brief This function will get user(s) information like user's DisplayName.
	 *
	 * @param UserIds List UserId(s) to get.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FListBulkUserInfo.
	 * @param OnError This will be called when the operation failed.
	 */
	void BulkGetUserInfo(const TArray<FString>& UserIds
		, const THandler<FListBulkUserInfo>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will get user input validation
	 *
	 * @param  LanguageCode Targeted Language Code, using ISO-639
	 * @param  OnSuccess This will be called when the operation succeeded. The result is FInputUserValidation.
	 * @param  OnError This will be called when the operation failed.
	 * @param  bDefaultOnEmpty Targeted DefaultOnEmpty.
	 */
	void GetInputValidations(const FString& LanguageCode
		, THandler<FInputValidation> const& OnSuccess
		, FErrorHandler const& OnError
		, bool bDefaultOnEmpty = true);

	/**
	 * @brief This function will enable 2FA with backupCode.
	 *
	 * @param OnSuccess This will be called when the operation succeeded. The result is FUser2FaBackupCode.
	 * @param OnError This will be called when the operation failed.
	 */
	void Enable2FaBackupCode(const THandler<FUser2FaBackupCode>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will disable 2FA with backupCode.
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void Disable2FaBackupCode(const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will generate backupCode.
	 *
	 * @param OnSuccess This will be called when the operation succeeded. The result is FUser2FaBackupCode.
	 * @param OnError This will be called when the operation failed.
	 */
	void GenerateBackupCode(const THandler<FUser2FaBackupCode>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will get backupCode.
	 *
	 * @param OnSuccess This will be called when the operation succeeded. The result is FUser2FaBackupCode.
	 * @param OnError This will be called when the operation failed.
	 */
	void GetBackupCode(const THandler<FUser2FaBackupCode>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will enable 3rd party authenticator.
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void Enable2FaAuthenticator(const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will disable 3rd party authenticator.
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void Disable2FaAuthenticator(const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will generate secret key for linking AB 2fa with 3rd party authenticator apps.
	 *
	 * @param OnSuccess This will be called when the operation succeeded. The result is FUser2FaSecretKey.
	 * @param OnError This will be called when the operation failed.
	 */
	void GenerateSecretKeyFor2FaAuthenticator(const THandler<FUser2FaSecretKey>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will get user 2FA factor enabled.
	 *
	 * @param OnSuccess This will be called when the operation succeeded. The result is FUser2FaMethod.
	 * @param OnError This will be called when the operation failed.
	 */
	void GetEnabled2FaFactors(const THandler<FUser2FaMethod>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function will make default factor for 2FA.
	 *
	 * @param AuthFactorType AuthFactorType user want to set as default factor
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void MakeDefault2FaFactors(EAccelByteLoginAuthFactorType AuthFactorType
		, const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief DEPRECATED: This function is no longer supported and will be removed in future releases.
	 * This function for update user account info within the game.
	 *
	 * @param UpdateRequest The data you want to update. for DateOfBirth, the format is YYYY-MM-DD.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FAccountUserData.
	 * @param OnError This will be called when the operation failed.
	 * Please use UpdateUser instead.
	 */
	void UpdateUserV3(FUserUpdateRequest UpdateRequest
		, const THandler<FAccountUserData>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function for getting publisher user.
	 *
	 * @param UserId Targeted User ID.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FGetPublisherUserResponse.
	 * @param OnError This will be called when the operation failed.
	 */
	void GetPublisherUser(const FString& UserId
		, const THandler<FGetPublisherUserResponse>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function for OAuth2 token verification API.
	 *
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void VerifyToken(const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function for getting user information .
	 *
	 * @param UserId Targeted User ID.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FGetUserInformationResponse.
	 * @param OnError This will be called when the operation failed.
	 */
	void GetUserInformation(const FString& UserId
		, const THandler<FGetUserInformationResponse>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function for generating one time code.
	 *
	 * @param PlatformType Specify platform type that chosen by user to log in.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FGeneratedOneTimeCode.
	 * @param OnError This will be called when the operation failed.
	 */
	void GenerateOneTimeCode(EAccelBytePlatformType PlatformType
		, const THandler<FGeneratedOneTimeCode>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief This function for generate publisher user's game token. required a code from request game token
	 *
	 * @param Code code from request game token..
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void GenerateGameToken(const FString& Code,
		const FVoidHandler& OnSuccess,
		const FOAuthErrorHandler & OnError);

	/**
	 * @brief This function for generate publisher user's game token. required a code from request game token
	 *
	 * @param Code code from request game token..
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void GenerateGameTokenV4(const FString& Code,
		const THandler<FAccelByteModelsLoginQueueTicketInfo>& OnSuccess,
		const FOAuthErrorHandler& OnError);
	
	/**
	 * @brief This function generate a code that can be exchanged into publisher namespace token (i.e. by web portal)
	 *
	 * @param PublisherClientID The targeted game's publisher ClientID.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void GenerateCodeForPublisherTokenExchange(const FString& PublisherClientID,
		const THandler<FCodeForTokenExchangeResponse>& OnSuccess,
		const FErrorHandler & OnError);

	/**
	 * @brief This function for link headless account to current full account.
	 *
	 * @param Request Request struct Containing chosen namespace and one time link code.
	 * @param OnSuccess This will be called when the operation succeeded.
	 * @param OnError This will be called when the operation failed.
	 */
	void LinkHeadlessAccountToCurrentFullAccount(const FLinkHeadlessAccountRequest& Request, const FVoidHandler& OnSuccess, const FErrorHandler& OnError);

	/**
	 * @brief This function to get conflict result when link headless account to current account by one time code
	 *
	 * @param OneTimeLinkCode One time link code value.
	 * @param OnSuccess This will be called when the operation succeeded. The result is THandler<FConflictLinkHeadlessAccountResult>&
	 * @param OnError This will be called when the operation failed.
	 */
	void GetConflictResultWhenLinkHeadlessAccountToFullAccount(const FString& OneTimeLinkCode, const THandler<FConflictLinkHeadlessAccountResult>& OnSuccess, const FErrorHandler& OnError);

	/**
	 * @brief Check users's account availability, available only using displayName field
	 * If the result is success or no error, it means the account already exists.
	 * If a new account is added with the defined display name, the service will be unable to perform the action.
	 *
	 * @param DisplayName User's display name value to be checked.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FVoidHandler
	 * @param OnError This will be called when the operation failed.
	 * @param bIsCheckUniqueDisplayName Check user availability by unique display name. Default as false.
	 */
	void CheckUserAccountAvailability(const FString& DisplayName, const FVoidHandler& OnSuccess, const FErrorHandler& OnError, bool bIsCheckUniqueDisplayName = false);

	/**
	 * @brief This function is used for retrieving third party platform token for user that login using third party,
	 * if user have not link requested platform in game namespace, will try to retrieving third party platform token from publisher namespace.
	 * Passing platform group name or it's member will return same access token that can be used across the platform members.
     * Note: The third party platform and platform group covered for this is:
     *    (psn) ps4web, (psn) ps4, (psn) ps5, epicgames, twitch, awscognito.
	 * @param PlatformType Platform type value.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FThirdPartyPlatformTokenData.
	 * @param OnError This will be called when the operation failed.
	 */
	void RetrieveUserThirdPartyPlatformToken(const EAccelBytePlatformType& PlatformType
		, const THandler<FThirdPartyPlatformTokenData>& OnSuccess
		, const FOAuthErrorHandler& OnError);

	/**
	 * @brief This function will get user basic and public info of 3rd party account
	 * @param Request Request struct containing chosen platformId and userIds.
	 * @param OnSuccess This will be called when the operation succeeded. The result is FAccountUserPlatformInfos.
	 * @param OnError This will be called when the operation failed.
	 */
	void GetUserOtherPlatformBasicPublicInfo(const FPlatformAccountInfoRequest& Request
		, const THandler<FAccountUserPlatformInfosResponse>& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief A request to retrieve such of information related to the current account configuration value.
	 * @param AccountConfiguration Target account configuration key to retrieve the information.
	 * @param OnSuccess This will be called when the operation succeeded. The result is bool.
	 * @param OnError This will be called when the operation failed.
	 */
	void GetAccountConfigurationValue(EAccountConfiguration AccountConfiguration
		, const THandler<bool>& OnSuccess
		, const FErrorHandler& OnError);

private:
	User() = delete;
	User(User const&) = delete;
	User(User&&) = delete;

	void SendVerificationCode(const FVerificationCodeRequest& Request
		, const FVoidHandler& OnSuccess
		, const FErrorHandler& OnError);

	/**
	 * @brief Validate Login response data and call the corresponding delegate function.
	 *
	 * @param Response the Login response data.
	 * @param OnSuccess delegate function for successful Login.
	 * @param OnError delegate function for error Login.
	 * @param CachedTokenKey The cached token key.
	 */
	void ProcessLoginResponse(const FOauth2Token& Response
		, const FVoidHandler& OnSuccess
		, const FOAuthErrorHandler& OnError
		, const FString& CachedTokenKey);

	/**
	 * @brief General handler for LoginWith* success; mostly a multicast callback handler.
	 * - Credentials: Inits Qos Latencies Scheduler.
	 * - Qos: Sets CredentialsRef Auth Token.
	 *
	 * @param OnSuccess delegate function for successful Login.
	 * @param Response the Login response data.
	 * @param CachedTokenKey The cached token key.
	 */
	void OnLoginSuccess(const FVoidHandler& OnSuccess
		, const FOauth2Token& Response
		, const FString& CachedTokenKey);

	/**
	 * @brief Saving chached token as a file to Local Data Storage.
	 * This functionality is working on Windows Platform, MAC and Linux only at the moment. 
	 *
	 * @param CachedTokenKey The cached token key.
	 * @param RefreshToken The refresh token.
	 * @param ExpireDate The expire date 
	 */
	void SaveCachedTokenToLocalDataStorage(const FString& CachedTokenKey
		, const FString& RefreshToken
		, FDateTime ExpireDate);
	/**
	 * @brief Internal utilities to trigger invalid request error for User API.
	 * 
	 * @param ErrorMessage message to pass for login error.
	 * @param OnError delegate function for error Login.
	 */
	void TriggerInvalidRequestError(const FString& ErrorMessage, const FOAuthErrorHandler& OnError);
};

} // Namespace Api
} // Namespace AccelByte
