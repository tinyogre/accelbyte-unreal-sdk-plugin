// Copyright (c) 2017-2018 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteBlueprintsUser.h"
#include "AccelByteServicesUser.h"

using namespace AccelByte::Services;
using namespace AccelByte;

#if 0

#pragma region RegisterWithEmail

URegisterWithEmail * URegisterWithEmail::RegisterWithEmail(FString Email, FString Password, FString DisplayName)
{
	URegisterWithEmail* Node = NewObject<URegisterWithEmail>();
	Node->Email = Email;
	Node->Password = Password;
	Node->DisplayName = DisplayName;

	return Node;
}

void URegisterWithEmail::Activate()
{
	JusticeUser::Register(this->Email, this->Password, this->DisplayName, FDefaultCompleteDelegate::CreateLambda([&](bool bSuccessful, FString ErrorStr) {
		if (bSuccessful)
		{
			OnSuccess.Broadcast(TEXT(""));
		}
		else
		{
			OnFailed.Broadcast(ErrorStr);
		}
	}));
}

#pragma endregion RegisterWithEmail

#pragma region LoginWithEmailAccount

ULoginWithEmail * ULoginWithEmail::LoginWithEmailAccount(FString Email, FString Password)
{
	ULoginWithEmail* Node = NewObject<ULoginWithEmail>();
	Node->Email = Email;
	Node->Password = Password;
	
	return Node;
}

void ULoginWithEmail::Activate()
{
	JusticeUser::Login(this->Email, this->Password, FDefaultCompleteDelegate::CreateLambda([&](bool bSuccessful, FString ErrorStr) {
		if (bSuccessful)
		{
			OnSuccess.Broadcast(TEXT(""));
		}
		else
		{
			OnFailed.Broadcast(ErrorStr);
		}
	}));
}

#pragma endregion LoginWithEmailAccount

#pragma region VerifyEmail

UVerifyEmail * UVerifyEmail::VerifyEmail(FString VerificationCode)
{
	UVerifyEmail* Node = NewObject<UVerifyEmail>();
	Node->VerificationCode = VerificationCode;

	return Node;
}

void UVerifyEmail::Activate()
{
	JusticeUser::Verify(this->VerificationCode, FDefaultCompleteDelegate::CreateLambda([&](bool bSuccessful, FString ErrorStr) {
		if (bSuccessful)
		{
			OnSuccess.Broadcast(TEXT(""));
		}
		else
		{
			OnFailed.Broadcast(ErrorStr);
		}
	}));
}

#pragma endregion VerifyEmail

#pragma region SendVerificationCodeForPasswordReset

UForgotPassword * UForgotPassword::SendVerificationCodeForPasswordReset(FString Email)
{
	UForgotPassword* Node = NewObject<UForgotPassword>();
	Node->Email = Email;

	return Node;
}

void UForgotPassword::Activate()
{
	JusticeUser::SendVerificationCodeForPasswordReset(this->Email, FDefaultCompleteDelegate::CreateLambda([&](bool bSuccessful, FString ErrorStr) {
		if (bSuccessful)
		{
			OnSuccess.Broadcast(TEXT(""));
		}
		else
		{
			OnFailed.Broadcast(ErrorStr);
		}
	}));
}

#pragma endregion SendVerificationCodeForPasswordReset

#pragma region ResetPasswordWithCode

UResetPassword * UResetPassword::ResetPasswordWithCode(FString VerificationCode, FString Email, FString NewPassword)
{
	UResetPassword* Node = NewObject<UResetPassword>();
	Node->VerificationCode = VerificationCode;
	Node->Email = Email;
	Node->NewPassword = NewPassword;

	return Node;
}

void UResetPassword::Activate()
{
	JusticeUser::ResetPasswordWithCode(this->VerificationCode, this->Email, this->NewPassword, FDefaultCompleteDelegate::CreateLambda([&](bool bSuccessful, FString ErrorStr) {
		if (bSuccessful)
		{
			OnSuccess.Broadcast(TEXT(""));
		}
		else
		{
			OnFailed.Broadcast(ErrorStr);
		}
	}));
}

#pragma endregion ResetPasswordWithCode

#pragma region LoginFromLauncher

ULoginFromLauncher * ULoginFromLauncher::LoginFromLauncher()
{
	ULoginFromLauncher* Node = NewObject<ULoginFromLauncher>();
	return Node;
}

void ULoginFromLauncher::Activate()
{
	JusticeUser::LoginFromLauncher(FDefaultCompleteDelegate::CreateLambda([&](bool bSuccessful, FString ErrorStr) {
		if (bSuccessful)
		{
			OnSuccess.Broadcast(TEXT(""));
		}
		else
		{
			OnFail.Broadcast(ErrorStr);
		}
	}));
}

#pragma endregion LoginFromLauncher

#pragma region GetAccessTokenWithPlatformGrant

ULoginWithPlatform * ULoginWithPlatform::GetAccessTokenWithPlatformGrant(UPlatformType PlatformType, FString PlatformToken)
{
	ULoginWithPlatform* Node = NewObject<ULoginWithPlatform>();
	Node->PlatformType = (EPlatformType)PlatformType;
	Node->PlatformToken = PlatformToken;
	return Node;
}

void ULoginWithPlatform::Activate()
{
	JusticeUser::Login(this->PlatformType, this->PlatformToken, FDefaultCompleteDelegate::CreateLambda([&](bool bSuccessful, FString ErrorStr)
	{
		if (bSuccessful)
		{
			OnSuccess.Broadcast(TEXT(""));
		}
		else
		{
			OnFail.Broadcast(ErrorStr);
		}
	}));
}

#pragma endregion GetAccessTokenWithPlatformGrant


#pragma region LoginWithDeviceId

ULoginWithDeviceId * ULoginWithDeviceId::LoginWithDeviceId()
{
	ULoginWithDeviceId* Node = NewObject<ULoginWithDeviceId>();
	return Node;
}

void ULoginWithDeviceId::Activate()
{
	User::LoginWithDeviceId(FDefaultCompleteDelegate::CreateLambda([&](bool bSuccessful, FString ErrorStr)
	{
		if (bSuccessful)
		{
			OnSuccess.Broadcast(TEXT(""));
		}
		else
		{
			OnFail.Broadcast(ErrorStr);
		}
	}));
}

#pragma endregion LoginWithDeviceId

#pragma region UpgradeHeadlessUserAccount

UUpgradeHeadlessAccount * UUpgradeHeadlessAccount::UpgradeHeadlessUserAccount(FString Email, FString Password)
{
	UUpgradeHeadlessAccount* Node = NewObject<UUpgradeHeadlessAccount>();
	Node->Email = Email;
	Node->Password = Password;
	return Node;
}

void UUpgradeHeadlessAccount::Activate()
{
	JusticeUser::UpgradeHeadlessUserAccount(this->Email, this->Password, FDefaultCompleteDelegate::CreateLambda([&](bool bSuccessful, FString ErrorStr)
	{
		if (bSuccessful)
		{
			OnSuccess.Broadcast(TEXT(""));
		}
		else
		{
			OnFail.Broadcast(ErrorStr);
		}
	}));
}

#pragma endregion UpgradeHeadlessUserAccount

#pragma region GetProfile

UGetProfile * UGetProfile::GetProfile()
{
	UGetProfile* Node = NewObject<UGetProfile>();
	return Node;
}

void UGetProfile::Activate()
{
	JusticeUser::GetProfile(FRequestCurrentPlayerProfileCompleteDelegate::CreateLambda([&](bool bSuccessful, FString ErrorStr, FUserProfileInfo * Result)
	{
		if (bSuccessful)
		{
			UUserProfileJustice* UserProfile = UUserProfileJustice::Deserialize(Result);
			OnSuccess.Broadcast(TEXT(""), UserProfile);
		}
		else
		{
			OnFail.Broadcast(ErrorStr, nullptr);
		}
	}));
}

#pragma endregion GetProfile

#pragma region UpdateProfile

UUpdateProfile * UUpdateProfile::UpdateProfile(FString DisplayName, FString FirstName, FString LastName, FString AvatarSmallUrl, FString AvatarUrl, FString AvatarLargeUrl, FString Email, FString Language, FString Timezone, FString DateOfBirth, FString Country, TMap<FString, FString> CustomAttributes)
{
	UUpdateProfile* Node = NewObject<UUpdateProfile>();
	Node->DisplayName = DisplayName;
	Node->FirstName = FirstName;
	Node->LastName = LastName;
	Node->AvatarSmallUrl = AvatarSmallUrl;
	Node->AvatarUrl = AvatarUrl;
	Node->AvatarLargeUrl = AvatarLargeUrl;
	Node->Email = Email;
	Node->Language = Language;
	Node->Timezone = Timezone;
	Node->DateOfBirth = DateOfBirth;
	Node->Country = Country;
	Node->CustomAttributes = CustomAttributes;
	return Node;
}

void UUpdateProfile::Activate()
{
	UserProfileInfoUpdate ProfileUpdate;
	ProfileUpdate.DisplayName = this->DisplayName;
	ProfileUpdate.FirstName = this->FirstName;
	ProfileUpdate.LastName = this->LastName;
	ProfileUpdate.AvatarSmallURL = this->AvatarSmallUrl;
	ProfileUpdate.AvatarURL = this->AvatarUrl;
	ProfileUpdate.AvatarSmallURL = this->AvatarSmallUrl;
	ProfileUpdate.AvatarLargeURL = this->AvatarLargeUrl;
	ProfileUpdate.Email = this->Email;
	ProfileUpdate.Language = this->Language;
	ProfileUpdate.Timezone = this->Timezone;
	ProfileUpdate.DateOfBirth = this->DateOfBirth;
	ProfileUpdate.Country = this->Country;
	ProfileUpdate.CustomAttributes = this->CustomAttributes;
	JusticeUser::UpdateProfile(ProfileUpdate, FDefaultCompleteDelegate::CreateLambda([&](bool bSuccessful, FString ErrorStr)
	{
		if (bSuccessful)
		{
			OnSuccess.Broadcast(TEXT(""));
		}
		else
		{
			OnFail.Broadcast(ErrorStr);
		}
	}));
}

#pragma endregion UpdateProfile

#endif
