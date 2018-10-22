// Copyright (c) 2018 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AccelByteItemModels.h"
#include "AccelByteItemBlueprints.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UAccelByteBlueprintsItem : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FBlueprintErrorHandler, int32, ErrorCode, FString, ErrorMessage);
	
	DECLARE_DYNAMIC_DELEGATE_OneParam(FGetItemByIdSuccess, const FAccelByteModelsItemInfo&, Result);
	UFUNCTION(BlueprintCallable, Category = "AccelByte | Item | Api ")
	static void GetItemById(const FString& AccessToken, const FString& Namespace, const FString& UserId, const FString& ItemId, const FString& Region, const FString& Language, const FGetItemByIdSuccess& OnSuccess, const FBlueprintErrorHandler& OnError);
	UFUNCTION(BlueprintCallable, Category = "AccelByte | Item | Api ")
	static void GetItemByIdEasy(const FString& ItemId, const FString& Region, const FString& Language, const FGetItemByIdSuccess& OnSuccess, const FBlueprintErrorHandler& OnError);

	DECLARE_DYNAMIC_DELEGATE_OneParam(FGetItemsByCriteriaSuccess, const FAccelByteModelsItemPagingSlicedResult&, Result);
	UFUNCTION(BlueprintCallable, Category = "AccelByte | Item | Api ")
	static void GetItemsByCriteria(const FString& AccessToken, const FString& Namespace, const FString& UserId, const FString& Language, const FString& Region, const FString& CategoryPath, const FString& ItemType, const FString& Status, int32 Page, int32 Size, const FGetItemsByCriteriaSuccess& OnSuccess, const FBlueprintErrorHandler& OnError);
	UFUNCTION(BlueprintCallable, Category = "AccelByte | Item | Api ")
	static void GetItemsByCriteriaEasy(const FString& Language, const FString& Region, const FString& CategoryPath, const FString& ItemType, const FString& Status, int32 Page, int32 Size, const FGetItemsByCriteriaSuccess& OnSuccess, const FBlueprintErrorHandler& OnError);
};
