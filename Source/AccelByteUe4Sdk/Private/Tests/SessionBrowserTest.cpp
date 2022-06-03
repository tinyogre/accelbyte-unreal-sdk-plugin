﻿// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "Misc/AutomationTest.h"
#include "TestUtilities.h"
#include "UserTestAdmin.h"
#include "Core/AccelByteRegistry.h"
#include "Core/AccelByteMultiRegistry.h"
#include "GameServerApi/AccelByteServerOauth2Api.h"
#include "Models/AccelByteSessionBrowserModels.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAccelByteSessionBrowserTest, Log, All);
DEFINE_LOG_CATEGORY(LogAccelByteSessionBrowserTest);

const int32 AutomationFlagMaskSessionBrowser = (EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter | EAutomationTestFlags::CommandletContext | EAutomationTestFlags::ClientContext);

const auto SessionBrowserTestErrorHandler = FErrorHandler::CreateLambda([](int32 ErrorCode, FString const& ErrorMessage)
{
    UE_LOG(LogAccelByteSessionBrowserTest, Error, TEXT("Error code: %d\nError message:%s"), ErrorCode, *ErrorMessage);
});

const int SessionBrowserUserCount = 3;
TArray<FString> TestSessionIDs;
TArray<FApiClientPtr> TestSessionBrowserApiClients;
TArray<FTestUser> TestSessionBrowserUsers;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(SessionBrowserSetup, "AccelByte.Tests.SessionBrowser.A.Setup", AutomationFlagMaskSessionBrowser);
bool SessionBrowserSetup::RunTest(const FString& Parameters)
{
	TArray<bool> UsersCreationSuccess;
	UsersCreationSuccess.Init(false, SessionBrowserUserCount);

	TArray<bool> UsersLoginSuccess;
	UsersLoginSuccess.Init(false, SessionBrowserUserCount);

	TArray<FString> TestEmails;
	TestEmails.Init("", SessionBrowserUserCount);

	TArray<bool> UserStatCodeInitSuccess;
	UserStatCodeInitSuccess.Init(false, SessionBrowserUserCount);

	bool bServerClientLoginSuccess = false;

	// SERVER LOGIN
	FRegistry::ServerOauth2.LoginWithClientCredentials(
		FVoidHandler::CreateLambda(
			[&bServerClientLoginSuccess]()
	        {
	            bServerClientLoginSuccess = true;
	            UE_LOG(LogAccelByteSessionBrowserTest, Log, TEXT("\t\tClient Successfully Login."));
	        }), SessionBrowserTestErrorHandler);
	WaitUntil(bServerClientLoginSuccess, TEXT("Waiting for Client Login..."));
	
	for (int index = 0; index < SessionBrowserUserCount; index++)
	{
		// USER CREATION
		UE_LOG(LogAccelByteSessionBrowserTest, Log, TEXT("USER CREATION...%d out of %d"), index + 1, SessionBrowserUserCount);

		FTestUser TestUser(index);

		TestSessionBrowserUsers.Add(TestUser);
		FApiClientPtr ApiClient = FMultiRegistry::GetApiClient(TestUser.DisplayName);
		TestSessionBrowserApiClients.Add(ApiClient);
		
		bool bUsersCreationDone = false;
		ApiClient->User.Register(TestUser.Email, TestUser.Password, TestUser.DisplayName, TestUser.Country, TestUser.DateOfBirth,
			THandler<FRegisterResponse>::CreateLambda(
				[&index, &UsersCreationSuccess, &bUsersCreationDone](const FRegisterResponse& Response)
				{
					UsersCreationSuccess[index] = true;
					UE_LOG(LogAccelByteSessionBrowserTest, Log, TEXT("Test User is successfuly created."));
					bUsersCreationDone = true;
				}),
			FErrorHandler::CreateLambda(
				[&](int32 Code, FString Message)
				{
					if ((ErrorCodes)Code == ErrorCodes::UserEmailAlreadyUsedException)
					{
						UsersCreationSuccess[index] = true;
						UE_LOG(LogAccelByteSessionBrowserTest, Log, TEXT("Test User is already created."));
					}
					else
					{
						UE_LOG(LogAccelByteSessionBrowserTest, Log, TEXT("Test User can't be created"));
					}
					bUsersCreationDone = true;
				})
			);
		WaitUntil(bUsersCreationDone, TEXT("Waiting for User creation..."));

		// USER LOG IN
		UE_LOG(LogAccelByteSessionBrowserTest, Log, TEXT("LOG USER IN...%d out of %d"), index + 1, SessionBrowserUserCount);

		ApiClient->User.LoginWithUsername(
			TestUser.Email,
			TestUser.Password,
			FVoidHandler::CreateLambda([&index, &UsersLoginSuccess]()
			{
				UsersLoginSuccess[index] = true;
				UE_LOG(LogAccelByteSessionBrowserTest, Log, TEXT("\t\tSuccessfully Login."));
			}), FCustomErrorHandler::CreateLambda([](int32 Code, const FString& Message, const FJsonObject& ErrorJson)
			{
				UE_LOG(LogAccelByteSessionBrowserTest, Error, TEXT("Get User Data Failed..! Error: %d | Message: %s"), Code, *Message);
			}));
		WaitUntil(UsersLoginSuccess[index], TEXT("Waiting for User login"));
		UE_LOG(LogAccelByteSessionBrowserTest, Log, TEXT("User creds: %s"), *FRegistry::Credentials.GetUserId());
	}

	//User connect to lobby
	bool bConnected = false;
	TestSessionBrowserApiClients[0]->Lobby.SetConnectSuccessDelegate(FSimpleDelegate::CreateLambda([&bConnected]()
	{
		UE_LOG(LogAccelByteSessionBrowserTest, Log, TEXT("Connected to Lobby"));
		bConnected = true;
	}));
	TestSessionBrowserApiClients[0]->Lobby.Connect();
	WaitUntil(bConnected, TEXT("Waiting for User to connect to Lobby"));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(SessionBrowserTearDown, "AccelByte.Tests.SessionBrowser.Z.Teardown", AutomationFlagMaskSessionBrowser);
bool SessionBrowserTearDown::RunTest(const FString& Parameters)
{
	for (auto& SessionID : TestSessionIDs)
	{
		bool bDeleteSessionBrowser = false;
		FRegistry::SessionBrowser.RemoveGameSession(SessionID,
			THandler<FAccelByteModelsSessionBrowserData>::CreateLambda(
				[&bDeleteSessionBrowser](const FAccelByteModelsSessionBrowserData &Data)
			{
				bDeleteSessionBrowser = true;
			}), SessionBrowserTestErrorHandler);
		WaitUntil(bDeleteSessionBrowser, "Waiting for game sessions deletion...");
	}

	TestSessionBrowserApiClients[0]->Lobby.Disconnect();

	for (auto& ApiClient : TestSessionBrowserApiClients)
	{
		bool bDeleteUserSuccess = false;
		AdminDeleteUser(ApiClient->CredentialsRef->GetUserId(),
			FSimpleDelegate::CreateLambda([&bDeleteUserSuccess]()
				{
					UE_LOG(LogAccelByteSessionBrowserTest, Log, TEXT("Delete user by id success"));
					bDeleteUserSuccess = true;
				}), SessionBrowserTestErrorHandler);
		WaitUntil(bDeleteUserSuccess, TEXT("Waiting for User deletion..."));
	}

	TestSessionBrowserApiClients.Empty();
	TestSessionBrowserUsers.Empty();

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(SessionBrowserCreateSessionUnknownType, "AccelByte.Tests.SessionBrowser.B.CreateCustomGameWithUnknownType", AutomationFlagMaskSessionBrowser);
bool SessionBrowserCreateSessionUnknownType::RunTest(const FString& Parameters)
{
	bool bCreated = false;
	FString SessionType("unknown");
	FString GameMode("FFA");
	FString GameMap("Karimun Java Island");
	FString GameVersion("1.0.1");
	int UpdatedPlayerCount = 4;
	int UpdatedMaxPlayer = 8;
	auto SettingJson = MakeShared<FJsonObject>();
	SettingJson->SetStringField(TEXT("CUSTOM1"), TEXT("CUSTOM1"));
	SettingJson->SetNumberField(TEXT("CUSTOM2"), 20);

	bool bInvalidType = false;

	//Create game session
	TestSessionBrowserApiClients[0]->SessionBrowser.CreateGameSession(SessionType, GameMode, GameMap, GameVersion, 1, 8, 0, FString(), SettingJson,
		THandler<FAccelByteModelsSessionBrowserData>::CreateLambda([&bCreated](const FAccelByteModelsSessionBrowserData& Data)
			{
				bCreated = true;
			}), FErrorHandler::CreateLambda([&bInvalidType](int32, FString const&) 
			{
				bInvalidType = true;
			}));
	WaitUntil(bInvalidType, TEXT("Waiting for creating custom game session..."));
	AB_TEST_FALSE(bCreated);
	AB_TEST_TRUE(bInvalidType);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(SessionBrowserQuerySessionsUnknownType, "AccelByte.Tests.SessionBrowser.B.QueryCustomGamesWithUnknownType", AutomationFlagMaskSessionBrowser);
bool SessionBrowserQuerySessionsUnknownType::RunTest(const FString& Parameters)
{
	FString SessionType("unknown");
	FString GameMode("FFA");

	bool bInvalidType = false;

	//Create game session
	TestSessionBrowserApiClients[0]->SessionBrowser.GetGameSessions(SessionType, GameMode, 
		THandler<FAccelByteModelsSessionBrowserGetResult>::CreateLambda([](const FAccelByteModelsSessionBrowserGetResult&)
			{ }), FErrorHandler::CreateLambda([&bInvalidType](int32, FString const&)
			{
				bInvalidType = true;
			}));
	WaitUntil(bInvalidType, TEXT("Waiting for querying custom game session..."));
	AB_TEST_TRUE(bInvalidType);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(SessionBrowserCreateAndQueryP2PSession, "AccelByte.Tests.SessionBrowser.C.CreateAndQueryP2PSession", AutomationFlagMaskSessionBrowser);
bool SessionBrowserCreateAndQueryP2PSession::RunTest(const FString& Parameters)
{
	bool bCreated = false;
	FString SessionType("p2p");
	FString GameMode("FFA");
	FString GameMap("Karimun Java Island");
	FString GameVersion("1.0.1");
	FAccelByteModelsSessionBrowserData GameSession;
	int UpdatedPlayerCount = 4;
	int UpdatedMaxPlayer = 8;
	auto SettingJson = MakeShared<FJsonObject>();
	SettingJson->SetStringField(TEXT("CUSTOM1"), TEXT("CUSTOM1"));
	SettingJson->SetNumberField(TEXT("CUSTOM2"), 20);
	
	//Create game session
	TestSessionBrowserApiClients[0]->SessionBrowser.CreateGameSession(GameMode, GameMap, GameVersion, 1, 8, SettingJson,
		THandler<FAccelByteModelsSessionBrowserData>::CreateLambda([&bCreated, &GameSession](const FAccelByteModelsSessionBrowserData &Data)
	{
		bCreated = true;
		GameSession = Data;
	}), SessionBrowserTestErrorHandler);
	WaitUntil(bCreated, TEXT("Waiting for creating P2P game session..."));
	AB_TEST_TRUE(bCreated);

	//update game session
	bool bUpdatedSession = false;
	FAccelByteModelsSessionBrowserData ResultUpdated;
	TestSessionBrowserApiClients[0]->SessionBrowser.UpdateGameSession(GameSession.Session_id, UpdatedMaxPlayer, UpdatedPlayerCount,
        THandler<FAccelByteModelsSessionBrowserData>::CreateLambda([&bUpdatedSession, &ResultUpdated](const FAccelByteModelsSessionBrowserData &Data)
    {
		bUpdatedSession = true;
        ResultUpdated = Data;
    }), SessionBrowserTestErrorHandler);
	WaitUntil(bUpdatedSession, TEXT("Waiting for updating P2P game session ..."));
	AB_TEST_TRUE(bUpdatedSession);
	AB_TEST_EQUAL(ResultUpdated.Game_session_setting.Max_player, UpdatedMaxPlayer);
	AB_TEST_EQUAL(ResultUpdated.Game_session_setting.Current_player, UpdatedPlayerCount);

	//update game setting
	bool bUpdatedSettings = false;
	FAccelByteModelsSessionBrowserData ResultUpdated2;
	SettingJson->SetStringField(TEXT("CUSTOM3"), TEXT("CUSTOM3"));
	SettingJson->SetNumberField(TEXT("CUSTOM2"), 40);
	TestSessionBrowserApiClients[0]->SessionBrowser.UpdateGameSettings(GameSession.Session_id, SettingJson,
		THandler<FAccelByteModelsSessionBrowserData>::CreateLambda([&bUpdatedSettings, &ResultUpdated2](const FAccelByteModelsSessionBrowserData& Data)
			{
				bUpdatedSettings = true;
				ResultUpdated2 = Data;
			}), SessionBrowserTestErrorHandler);
	WaitUntil(bUpdatedSettings, TEXT("Waiting for updating P2P game settings ..."));
	AB_TEST_TRUE(bUpdatedSettings);
	AB_TEST_TRUE(ResultUpdated2.Game_session_setting.Settings.JsonObject->GetStringField("CUSTOM3").Equals(TEXT("CUSTOM3")));
	AB_TEST_EQUAL(ResultUpdated2.Game_session_setting.Settings.JsonObject->GetIntegerField("CUSTOM2"), 40);

	//Query game session
	bool bQueried = false;
	bool bFounded = false;
	FAccelByteModelsSessionBrowserGetResult QueryResult;
	FAccelByteModelsSessionBrowserData Founded;
	TestSessionBrowserApiClients[1]->SessionBrowser.GetGameSessions(SessionType, GameMode,
        THandler<FAccelByteModelsSessionBrowserGetResult>::CreateLambda([&bQueried, &QueryResult](const FAccelByteModelsSessionBrowserGetResult &QueryData)
		{
			bQueried = true;
			QueryResult = QueryData;
		}), SessionBrowserTestErrorHandler);
	WaitUntil(bQueried, TEXT("Waiting for querying P2P game sessions..."));
	AB_TEST_TRUE(bQueried);
	AB_TEST_TRUE(QueryResult.Sessions.Num() > 0);

	for(int i = 0; i < QueryResult.Sessions.Num(); i++)
	{
		if(QueryResult.Sessions[i].Session_id == ResultUpdated.Session_id)
		{
			bFounded = true;
			Founded = QueryResult.Sessions[i];
			break;
		}
	}
	AB_TEST_TRUE(Founded.Game_session_setting.Settings.JsonObject->GetStringField("CUSTOM1").Equals(TEXT("CUSTOM1")));
	AB_TEST_EQUAL(Founded.Game_session_setting.Settings.JsonObject->GetIntegerField("CUSTOM2"), 40);
	AB_TEST_TRUE(Founded.Game_session_setting.Settings.JsonObject->GetStringField("CUSTOM3").Equals(TEXT("CUSTOM3")));
	AB_TEST_TRUE(bFounded);
	AB_TEST_EQUAL(Founded.Game_session_setting.Current_player, ResultUpdated.Game_session_setting.Current_player);

	//Get game session by session id
	bool bGameSessionGet = false;
	FAccelByteModelsSessionBrowserData getByIdResult;
	TestSessionBrowserApiClients[1]->SessionBrowser.GetGameSession(QueryResult.Sessions[0].Session_id,
		THandler<FAccelByteModelsSessionBrowserData>::CreateLambda([&bGameSessionGet, &getByIdResult](const FAccelByteModelsSessionBrowserData& result)
		{
			bGameSessionGet = true;
			getByIdResult = result;
		}), SessionBrowserTestErrorHandler);
	WaitUntil(bGameSessionGet, TEXT("Waiting to get game session by id"));
	AB_TEST_EQUAL(getByIdResult.Session_id, QueryResult.Sessions[0].Session_id);

	//Remove game session
	bool bRemoved = false;
	TestSessionBrowserApiClients[0]->SessionBrowser.RemoveGameSession(GameSession.Session_id,
        THandler<FAccelByteModelsSessionBrowserData>::CreateLambda([&bRemoved](const FAccelByteModelsSessionBrowserData &Data)
	    {
	        bRemoved = true;
	    }), SessionBrowserTestErrorHandler);
	WaitUntil(bRemoved, TEXT("Waiting for removing game session..."));
	AB_TEST_TRUE(bRemoved);

	//Check again if game session really removed
	bQueried = false;
	TestSessionBrowserApiClients[1]->SessionBrowser.GetGameSessions(SessionType, GameMode,
        THandler<FAccelByteModelsSessionBrowserGetResult>::CreateLambda([&bQueried, &QueryResult](const FAccelByteModelsSessionBrowserGetResult &QueryData)
	    {
	        bQueried = true;
	        QueryResult = QueryData;
	    }), SessionBrowserTestErrorHandler);
	WaitUntil(bQueried, TEXT("Waiting for querying game sessions..."));
	AB_TEST_TRUE(bQueried);

	bFounded = false;
	for(int i = 0; i < QueryResult.Sessions.Num(); i++)
	{
		if(QueryResult.Sessions[i].Session_id == ResultUpdated.Session_id)
		{
			bFounded = true;
			break;
		}
	}
	AB_TEST_FALSE(bFounded);
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(SessionBrowserCreateCustomGame, "AccelByte.Tests.SessionBrowser.D.DedicatedCreateCustomGame", AutomationFlagMaskSessionBrowser);
bool SessionBrowserCreateCustomGame::RunTest(const FString& Parameters)
{
	bool bCreated = false;
	FString SessionType("dedicated");
	FString GameMode("FFA");
	FString GameMap("Karimun Java Island");
	FString GameVersion("1.0.1");
	FAccelByteModelsSessionBrowserData GameSession;
	int UpdatedPlayerCount = 4;
	int UpdatedMaxPlayer = 8;
	auto SettingJson = MakeShared<FJsonObject>();
	SettingJson->SetStringField(TEXT("CUSTOM1"), TEXT("CUSTOM1"));
	SettingJson->SetNumberField(TEXT("CUSTOM2"), 20);
	
	// Create game session
	TestSessionBrowserApiClients[0]->SessionBrowser.CreateGameSession(EAccelByteSessionType::dedicated, GameMode,
		GameMap, GameVersion, 1, 8, 4, FString(), SettingJson,
		THandler<FAccelByteModelsSessionBrowserData>::CreateLambda([&bCreated, &GameSession](FAccelByteModelsSessionBrowserData const& Data)
		{
			bCreated = true;
			GameSession = Data;
		}), SessionBrowserTestErrorHandler);
	WaitUntil(bCreated, TEXT("Waiting for creating dedicated game session..."));
	AB_TEST_TRUE(bCreated);

	// Waiting for DS Notif
	bool bDSReady = false;
	FAccelByteModelsDsNotice DsNotif;
	TestSessionBrowserApiClients[0]->Lobby.SetDsNotifDelegate(
		THandler<const FAccelByteModelsDsNotice>::CreateLambda([&bDSReady, &DsNotif](FAccelByteModelsDsNotice const& result)
		{
			UE_LOG(LogAccelByteSessionBrowserTest, Log, TEXT("[DS Notif] Match Id: %s - Status: %s"), *result.MatchId, *result.Status);
			bDSReady = true;
			DsNotif = result;
		}));

	TestSessionBrowserApiClients[0]->Lobby.RequestDS(GameSession.Session_id, GameMode, GameSession.Server.Game_version, GameSession.Server.Region, GameSession.Server.Deployment);
	WaitUntil(bDSReady, TEXT("Waiting for DS notif..."), 120);

	AB_TEST_EQUAL(DsNotif.MatchId, GameSession.Session_id);
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(DSCreateGameSession, "AccelByte.Tests.SessionBrowser.D.DSCreateGameSession", AutomationFlagMaskSessionBrowser);
bool DSCreateGameSession::RunTest(const FString& Parameters)
{
	bool bCreated = false;
	FString SessionType("dedicated");
	FString GameMode("FFA");
	FString GameMap("Karimun Java Island");
	FString GameVersion("1.0.1");
	FString ServerName {"TestLocal"};
	FAccelByteModelsSessionBrowserData GameSession;
	int UpdatedPlayerCount = 4;
	int UpdatedMaxPlayer = 8;
	auto SettingJson = MakeShared<FJsonObject>();
	SettingJson->SetStringField(TEXT("CUSTOM1"), TEXT("CUSTOM1"));
	SettingJson->SetNumberField(TEXT("CUSTOM2"), 20);

	// Local DS create server
	FServerApiClientPtr ServerApiClient = FMultiRegistry::GetServerApiClient();
	bool ServerLogin {false};
	ServerApiClient->ServerOauth2.LoginWithClientCredentials(FVoidHandler::CreateLambda([&]()
	{
		ServerLogin = true;
	}), SessionBrowserTestErrorHandler);

	WaitUntil(ServerLogin, "Waiting server login");

	bool RegisterSuccess {false};
	ServerApiClient->ServerDSM.RegisterLocalServerToDSM("127.0.0.1", 7777, ServerName, FVoidHandler::CreateLambda(
		[&]()
		{
			RegisterSuccess = true;
		}), SessionBrowserTestErrorHandler);

	WaitUntil(RegisterSuccess, "Waiting Register server");

	FAccelByteModelsServerInfo ServerInfo;
	bool bGetServerInfoDone {false};
	ServerApiClient->ServerDSM.GetServerInfo(THandler<FAccelByteModelsServerInfo>::CreateLambda(
		[&](const FAccelByteModelsServerInfo& Result)
		{
			ServerInfo = Result;
			bGetServerInfoDone = true;
		}), SessionBrowserTestErrorHandler);

	WaitUntil(bGetServerInfoDone, "Waiting get server info");
	
	// Create game session
	ServerApiClient->ServerSessionBrowser.CreateGameSession(EAccelByteSessionType::dedicated, GameMode,
		GameMap, GameVersion, 1, 8, 4, FString(), SettingJson,
		THandler<FAccelByteModelsSessionBrowserData>::CreateLambda([&bCreated, &GameSession](FAccelByteModelsSessionBrowserData const& Data)
		{
			bCreated = true;
			GameSession = Data;
		}), SessionBrowserTestErrorHandler);
	WaitUntil(bCreated, TEXT("Waiting for creating dedicated game session..."));
	AB_TEST_TRUE(bCreated);

	// DS Register game session
	FAccelByteModelsUser partyMember;
	partyMember.User_id = "0";

	FAccelByteModelsMatchingParty party;
	party.Party_id = "0";
	party.Party_members = {partyMember};

	bool bRegisterServerSessionDone {false};
	FAccelByteModelsServerCreateSessionResponse CreateSessionResponse;
	ServerApiClient->ServerDSM.RegisterServerGameSession(GameSession.Session_id, GameMode,
		THandler<FAccelByteModelsServerCreateSessionResponse>::CreateLambda(
		[&](const FAccelByteModelsServerCreateSessionResponse& Result)
		{
			bRegisterServerSessionDone = true;
			CreateSessionResponse = Result;
		}), SessionBrowserTestErrorHandler);
	
	WaitUntil(bRegisterServerSessionDone, "Waiting register server session");
	DelaySeconds(3, "Waiting 3 sec");


	// Client query session from Session browser
	FAccelByteModelsSessionBrowserGetResult GetResult;
	bool bGetResultDone {false};
	TestSessionBrowserApiClients[0]->SessionBrowser.GetGameSessions(EAccelByteSessionType::dedicated, GameMode,
	THandler<FAccelByteModelsSessionBrowserGetResult>::CreateLambda([&](const FAccelByteModelsSessionBrowserGetResult& Result)
	{
		GetResult = Result;
		bGetResultDone = true;
	}), SessionBrowserTestErrorHandler);

	WaitUntil(bGetResultDone, "Waiting client get session");

	bool bSessionFound {false};
	for(auto SessionData : GetResult.Sessions)
	{
		if(SessionData.Session_id == GameSession.Session_id)
		{
			bSessionFound = true;
		}
	}

	AB_TEST_TRUE(bSessionFound);

	// Join the created session
	bool bJoinDone {false};
	FAccelByteModelsSessionBrowserData JoinResult;
	TestSessionBrowserApiClients[0]->SessionBrowser.JoinSession(GameSession.Session_id, "",
		THandler<FAccelByteModelsSessionBrowserData>::CreateLambda([&](const FAccelByteModelsSessionBrowserData& Result)
		{
			bJoinDone = true;
			JoinResult = Result;
		}), SessionBrowserTestErrorHandler);

	WaitUntil(bJoinDone, "Waiting join session result");

	AB_TEST_FALSE(JoinResult.Server.Ip.IsEmpty());
	AB_TEST_FALSE(CreateSessionResponse.Session.Namespace.IsEmpty());
	return true;
}