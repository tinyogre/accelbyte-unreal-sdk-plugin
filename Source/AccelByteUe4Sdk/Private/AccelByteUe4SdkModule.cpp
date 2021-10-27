// Copyright (c) 2018 - 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteUe4SdkModule.h"
#include "Core/AccelByteRegistry.h"
#include "Core/AccelByteHttpRetryScheduler.h"
#include "CoreUObject.h"
#include "Api/AccelByteGameTelemetryApi.h"
#include "Core/AccelByteReport.h"
#include "Core/Version.h"
#include "Interfaces/IPluginManager.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#endif

FString GetDefaultServerAPIUrl(const FString& SpecificServerUrl, const FString& DefaultServerPath)
{
	if (SpecificServerUrl.IsEmpty())
	{
		return FString::Printf(TEXT("%s/%s"), *FRegistry::ServerSettings.BaseUrl, *DefaultServerPath);
	}

	return SpecificServerUrl;
}

class FAccelByteUe4SdkModule : public IAccelByteUe4SdkModuleInterface
{
    virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// For registering settings in UE4 editor
	void RegisterSettings();
	void UnregisterSettings();

	bool LoadSettingsFromConfigUobject();
	bool LoadServerSettingsFromConfigUobject();
	void NullCheckConfig(FString value, FString configField);
	static FVersion GetPluginVersion();
	void GetVersionInfo(FString const& Url, TFunction<void(FVersionInfo)> Callback) const;
	void CheckServicesCompatibility() const;
};

void FAccelByteUe4SdkModule::StartupModule()
{
#if WITH_EDITOR
	FModuleManager::Get().LoadModuleChecked("Settings");
#endif
	FModuleManager::Get().LoadModuleChecked("Http");
	FModuleManager::Get().LoadModuleChecked("Websockets");
	FModuleManager::Get().LoadModuleChecked("Json");
	FModuleManager::Get().LoadModuleChecked("JsonUtilities");
	FModuleManager::Get().LoadModuleChecked("Projects");

	RegisterSettings();
	LoadSettingsFromConfigUobject();
	LoadServerSettingsFromConfigUobject();

#if UE_BUILD_DEVELOPMENT
	CheckServicesCompatibility();
#endif

	FRegistry::HttpRetryScheduler.Startup();
	FRegistry::Credentials.Startup();
	FRegistry::GameTelemetry.Startup();
	FRegistry::ServerCredentials.Startup();
}

void FAccelByteUe4SdkModule::ShutdownModule()
{
	FRegistry::ServerCredentials.Shutdown();
	FRegistry::GameTelemetry.Shutdown();
	FRegistry::Credentials.Shutdown();
	FRegistry::HttpRetryScheduler.Shutdown();

	UnregisterSettings();
}

void FAccelByteUe4SdkModule::RegisterSettings()
{

#if WITH_EDITOR
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings(TEXT("Project"), TEXT("Plugins"), TEXT("AccelByte Unreal Engine 4 Client SDK"),
			FText::FromName(TEXT("AccelByte Unreal Engine 4 Client SDK")),
			FText::FromName(TEXT("Setup your plugin.")),
			GetMutableDefault<UAccelByteSettings>()
		);
		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FAccelByteUe4SdkModule::LoadSettingsFromConfigUobject);
		}

		ISettingsSectionPtr ServerSettingsSection = SettingsModule->RegisterSettings(TEXT("Project"), TEXT("Plugins"), TEXT("AccelByte Unreal Engine 4 Server SDK"),
			FText::FromName(TEXT("AccelByte Unreal Engine 4 Server SDK")),
			FText::FromName(TEXT("Setup your plugin.")),
			GetMutableDefault<UAccelByteServerSettings>()
		);
		if (ServerSettingsSection.IsValid())
		{
			ServerSettingsSection->OnModified().BindRaw(this, &FAccelByteUe4SdkModule::LoadServerSettingsFromConfigUobject);
		}
	}
#endif
}

void FAccelByteUe4SdkModule::UnregisterSettings()
{
#if WITH_EDITOR
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings(TEXT("Project"), TEXT("Plugins"), TEXT("AccelByte Unreal Engine 4 SDK"));
	}
#endif
}

bool FAccelByteUe4SdkModule::LoadSettingsFromConfigUobject()
{
	FRegistry::Settings.Reset(ESettingsEnvironment::Default);

	NullCheckConfig(*FRegistry::Settings.ClientId, "Client ID");
	NullCheckConfig(*FRegistry::Settings.Namespace, "Namespace");
	NullCheckConfig(*FRegistry::Settings.BaseUrl, "Base URL");
	
	return true;
}

bool FAccelByteUe4SdkModule::LoadServerSettingsFromConfigUobject()
{
#if WITH_EDITOR || UE_SERVER
	FRegistry::ServerSettings.ClientId = GetDefault<UAccelByteServerSettings>()->ClientId;
	FRegistry::ServerSettings.ClientSecret = GetDefault<UAccelByteServerSettings>()->ClientSecret;
	FRegistry::ServerSettings.Namespace = GetDefault<UAccelByteServerSettings>()->Namespace;
	FRegistry::ServerSettings.PublisherNamespace = GetDefault<UAccelByteServerSettings>()->PublisherNamespace;
	FRegistry::ServerSettings.RedirectURI = GetDefault<UAccelByteServerSettings>()->RedirectURI;
	FRegistry::ServerSettings.BaseUrl = GetDefault<UAccelByteServerSettings>()->BaseUrl;

	NullCheckConfig(*FRegistry::ServerSettings.ClientId, "Client ID");
	NullCheckConfig(*FRegistry::ServerSettings.ClientSecret, "Client Secret");

	FRegistry::ServerSettings.IamServerUrl = GetDefaultServerAPIUrl(GetDefault<UAccelByteServerSettings>()->IamServerUrl, TEXT("iam"));
	FRegistry::ServerSettings.DSMControllerServerUrl = GetDefaultServerAPIUrl(GetDefault<UAccelByteServerSettings>()->DSMControllerServerUrl, TEXT("dsmcontroller"));
	FRegistry::ServerSettings.StatisticServerUrl = GetDefaultServerAPIUrl(GetDefault<UAccelByteServerSettings>()->StatisticServerUrl, TEXT("social"));
	FRegistry::ServerSettings.PlatformServerUrl = GetDefaultServerAPIUrl(GetDefault<UAccelByteServerSettings>()->PlatformServerUrl, TEXT("platform"));
	FRegistry::ServerSettings.QosManagerServerUrl = GetDefaultServerAPIUrl(GetDefault<UAccelByteServerSettings>()->QosManagerServerUrl, TEXT("qosm"));
	FRegistry::ServerSettings.GameTelemetryServerUrl = GetDefaultServerAPIUrl(GetDefault<UAccelByteServerSettings>()->GameTelemetryServerUrl, TEXT("game-telemetry"));
	FRegistry::ServerSettings.AchievementServerUrl = GetDefaultServerAPIUrl(GetDefault<UAccelByteServerSettings>()->AchievementServerUrl, TEXT("achievement"));
	FRegistry::ServerSettings.MatchmakingServerUrl = GetDefaultServerAPIUrl(GetDefault<UAccelByteServerSettings>()->MatchmakingServerUrl, TEXT("matchmaking"));
	FRegistry::ServerSettings.LobbyServerUrl = GetDefaultServerAPIUrl(GetDefault<UAccelByteServerSettings>()->LobbyServerUrl, TEXT("lobby"));
	FRegistry::ServerSettings.CloudSaveServerUrl = GetDefaultServerAPIUrl(GetDefault<UAccelByteServerSettings>()->CloudSaveServerUrl, TEXT("cloudsave"));
	FRegistry::ServerSettings.SeasonPassServerUrl = GetDefaultServerAPIUrl(GetDefault<UAccelByteServerSettings>()->SeasonPassServerUrl, TEXT("seasonpass"));
	FRegistry::ServerCredentials.SetClientCredentials(FRegistry::ServerSettings.ClientId, FRegistry::ServerSettings.ClientSecret);

#endif
	return true;
}

void FAccelByteUe4SdkModule::NullCheckConfig(FString value, FString configField)
{
	if (value.IsEmpty())
	{
		UE_LOG(LogAccelByte, Warning, TEXT("\"%s\" is not configured yet.\nCheck DefaultEngine.ini or Edit/ProjectSettings/Plugins/"), *configField);
	}
}

FVersion FAccelByteUe4SdkModule::GetPluginVersion()
{
	FString const PluginName = "AccelByteUe4Sdk";
 
	TSharedPtr<IPlugin> const Plugin = IPluginManager::Get().FindPlugin("AccelByteUe4Sdk");
	const FPluginDescriptor& Descriptor = Plugin->GetDescriptor();

	return Descriptor.VersionName;
}

void FAccelByteUe4SdkModule::GetVersionInfo(FString const& Url, TFunction<void(FVersionInfo)> Callback) const
{
	FHttpRequestPtr const Request = FHttpModule::Get().CreateRequest();
	Request->SetVerb("GET");
	Request->SetURL(FString::Printf(TEXT("%s/version"), *Url));
	Request->OnProcessRequestComplete().BindLambda(
		[Callback](FHttpRequestPtr const RequestPtr, FHttpResponsePtr const ResponsePtr, bool const bFinished)
		{
			if (!bFinished || !ResponsePtr.IsValid())
			{
				UE_LOG(LogAccelByte, Warning, TEXT("Getting version info failed:%s"), *RequestPtr->GetURL());
				return;
			}

			FString const Content = ResponsePtr->GetContentAsString();

			ensureAlwaysMsgf(!Content.IsEmpty(), TEXT("Version info empty: %s"), *RequestPtr->GetURL());

			FVersionInfo VersionInfo;
			FJsonObjectConverter::JsonObjectStringToUStruct(Content, &VersionInfo, 0, 0);

			if (Callback)
			{
				Callback(VersionInfo);
			}
		});

	Request->ProcessRequest();

}

void FAccelByteUe4SdkModule::CheckServicesCompatibility() const
{
	if (GetPluginVersion().Compare(FVersion{TEXT("4.0.0")}) <= 0)
	{
		return;
	}

	FString const Path = FPaths::ProjectPluginsDir() / "AccelByteUe4Sdk/Content/CompatibilityMap.json";
	FString String;
	FFileHelper::LoadFileToString(String, *Path);
	auto CompatibilityMapPtr = MakeShared<FServiceCompatibilityMap>(FServiceCompatibilityMap::FromJson(String));

	for (auto const& ServiceName : CompatibilityMapPtr->GetServices())
	{
		if (ServiceName.IsEmpty())
		{
			return;
		}

		GetVersionInfo(
			FRegistry::Settings.BaseUrl / ServiceName,
			[CompatibilityMapPtr, ServiceName](FVersionInfo const VersionInfo)
			{
				FResult const Result = CompatibilityMapPtr->Check(ServiceName, VersionInfo.Version);
				if (Result.bIsError)
				{
					UE_LOG(LogAccelByte, Warning, TEXT("[Compatibility] %s"), *Result.Message);
				}
			});
	}
}

IMPLEMENT_MODULE(FAccelByteUe4SdkModule, AccelByteUe4Sdk)
