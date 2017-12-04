// Copyright (c) 2017 AccelByte Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Core/Public/Misc/Guid.h"
#include "OnlineSubsystemJusticeModule.h"
#include "UnrealString.h"

class FAWSXRayJustice
{
public:
	FAWSXRayJustice()
	{
		TraceId = FGuid::NewGuid();
	}

	~FAWSXRayJustice()
	{ }

	bool IsValid() const { return TraceId.IsValid(); };
	FGuid GetTraceId() const { return TraceId; };
	
	FString XRayTraceID() const {
		const int version = 1;
		int64 UnixTime = FDateTime::UtcNow().ToUnixTimestamp();
		FString TraceID = FString::Printf(TEXT("%x%x%x%x"), TraceId.A, TraceId.B, TraceId.C, TraceId.D);
		return FString::Printf(TEXT("%i-%i-%s"), version, UnixTime, *TraceID);
	}

	FString ToString() const {	
		return FString::Printf(TEXT("Root=%s"), *XRayTraceID());
	}


private:

	FGuid TraceId;

};