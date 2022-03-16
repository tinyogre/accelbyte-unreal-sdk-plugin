// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Core/AccelByteError.h"
#include "Models/AccelByteLobbyModels.h"
#include "Core/AccelByteMultiRegistry.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ABSessionAttribute.generated.h"

using namespace AccelByte;

USTRUCT(BlueprintType)
struct FSetSessionAttributeRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Key;

	UPROPERTY(BlueprintReadWrite)
	FString Value;
};

USTRUCT(BlueprintType)
struct FGetSessionAttributeRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Key;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FDSetSessionAttributeResponse, FAccelByteModelsSetSessionAttributesResponse, Response);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDGetSessionAttributeResponse, FAccelByteModelsGetSessionAttributesResponse, Response);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDGetAllSessionAttributeResponse, FAccelByteModelsGetAllSessionAttributesResponse, Response);

//Sub-API from AccelByte Lobby
UCLASS(Blueprintable, BlueprintType)
class UABSessionAttribute : public UObject
{
	GENERATED_BODY()
public:
	void SetApiClient(FApiClientPtr const& NewApiClientPtr);

//Request-Response
public:
	UFUNCTION(BlueprintCallable, Category = "AccelByte | SessionAttribute")
	FString SetSessionAttribute(
		FSetSessionAttributeRequest const& Request,
		FDSetSessionAttributeResponse OnResponse,
		FDErrorHandler OnError) const;

	UFUNCTION(BlueprintCallable, Category = "AccelByte | SessionAttribute")
	FString GetSessionAttribute(
		FGetSessionAttributeRequest const& Request,
		FDGetSessionAttributeResponse OnResponse,
		FDErrorHandler OnError) const;

	UFUNCTION(BlueprintCallable, Category = "AccelByte | SessionAttribute")
	FString GetAllSessionAttribute(
		FDGetAllSessionAttributeResponse OnResponse,
		FDErrorHandler OnError) const;

private:
	FApiClientPtr ApiClientPtr;
};

