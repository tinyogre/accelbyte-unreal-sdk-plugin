// Copyright (c) 2017-2018 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.
#pragma once

#include "Misc/ConfigCacheIni.h"
#include "JusticePlatformFunctions.h"
#include "JusticeSDK.h"
#include "Utilities/AWSXRayJustice.h"
#include "Utilities/HTTPJustice.h"
#include "Utilities/JusticeLog.h"
#include "Models/Category.h"
#include "Models/ItemInfo.h"
#include "Models/ItemPagingSlicedResult.h"

DECLARE_DELEGATE_ThreeParams(FGetRootCategoryCompleteDelegate, bool, FString, TArray<Category>);
DECLARE_DELEGATE_ThreeParams(FItemCompleteDelegate, bool, FString, TArray<ItemInfo>);

class JUSTICESDK_API JusticeCatalog
{
public:
	static void GetRootCategory(FString Language, FGetRootCategoryCompleteDelegate OnComplete);
	static void GetCategory(FString ParentPath, FString Language, FGetRootCategoryCompleteDelegate OnComplete);
	static void GetItemByQuery(FString Language, FString Region, FString CategoryPath, FString ItemType, FString Status, int32 Page, int32 Size, FItemCompleteDelegate OnComplete);

private:
	static void OnGetRootCategoryResponse(FJusticeHttpResponsePtr Response, FGetRootCategoryCompleteDelegate OnComplete);
	static void OnGetCategoryResponse(FJusticeHttpResponsePtr Response, FGetRootCategoryCompleteDelegate OnComplete);
	static void OnGetItemByQueryResponse(FJusticeHttpResponsePtr Response, FItemCompleteDelegate OnComplete);
};