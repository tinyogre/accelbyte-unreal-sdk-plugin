// Copyright (c) 2017-2018 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Models/OAuthTokenJustice.h"
#include "ResetPassword.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FResetPasswordCodeOutputPin, FString, ErrorMessage);

UCLASS()
class JUSTICESDK_API UAsyncResetPassword : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
		FResetPasswordCodeOutputPin OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FResetPasswordCodeOutputPin OnFailed;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", Category = "Identity"))
		static UAsyncResetPassword* ResetPassword(FString UserID, FString VerificationCode, FString NewPassword);

	virtual void Activate() override;

private:
	FString UserID;
	FString VerificationCode;
	FString NewPassword;
};