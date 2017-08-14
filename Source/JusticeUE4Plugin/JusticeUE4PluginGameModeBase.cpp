// Fill out your copyright notice in the Description page of Project Settings.

#include "JusticeUE4PluginGameModeBase.h"
#include "Misc/ConfigCacheIni.h"

#define DO_CHECK 1

AJusticeUE4PluginGameModeBase::AJusticeUE4PluginGameModeBase()
{
}

void AJusticeUE4PluginGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Load Identity Interface
	OnlineSub = IOnlineSubsystem::Get();
	check(OnlineSub);

	Identity = OnlineSub->GetIdentityInterface();
	check(Identity.IsValid());

	// Setup Login delegates
	LoginCompleteHandle = Identity->AddOnLoginCompleteDelegate_Handle(
		DefaultLocalUserNum,
		FOnLoginCompleteDelegate::CreateUObject(this, &AJusticeUE4PluginGameModeBase::OnLoginCompleteDelegate));
}

void AJusticeUE4PluginGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// User login credentials
	FOnlineAccountCredentials user;
	user.Id = TEXT("test@example.com");
	user.Token = TEXT("123456");
	user.Type = TEXT("PasswordGrant");

	Identity->Login(DefaultLocalUserNum, user);
}

void AJusticeUE4PluginGameModeBase::OnLoginCompleteDelegate(int32 LocalUserNum, bool bSuccessful, const FUniqueNetId& UserId, const FString& ErrorStr)
{
	Identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, LoginCompleteHandle);
	UE_LOG(LogOnline, Log, TEXT("Game received login %s. %s"), bSuccessful ? TEXT("successful") : TEXT("failed"), *UserId.ToString());

	/*
	//Query Example
	FString CheckedResource = TEXT("NAMESPACE:justice:USER:t9sk283j21i8:ORDER:rec231:action:2");  //check permission of certain user to read certain order record	
	FString CheckedResource = TEXT("NAMESPACE:evil:USER:*:ORDER:*:action:15");                    //Check any user's permission to do anything on any order record
	FString CheckedResource = TEXT("NAMESPACE:justice:ROLEID:GODMODE");                           //check if a client/user have godmode role	
	FString CheckedResource = TEXT("NAMESPACE:justice:ROLEID:GODMODE*");                          //check if a client/user have role prefixed with GODMODE
	*/
	FString CheckedResource = TEXT("NAMESPACE:justice:PAYMENT:action:8");
	FString Result;	

	if (Identity->GetUserAccount(UserId)->GetUserAttribute(CheckedResource, Result))
	{
		//granting permissions
		UE_LOG(LogTemp, Warning, TEXT("Granting permission of %s"), *CheckedResource);
	}
	else
	{
		//denying permissions
		UE_LOG(LogTemp, Warning, TEXT("Denying permission of %s"), *CheckedResource);
	}	
}

