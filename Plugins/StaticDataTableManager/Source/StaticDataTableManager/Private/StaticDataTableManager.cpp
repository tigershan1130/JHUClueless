// Copyright Epic Games, Inc. All Rights Reserved.

#include "StaticDataTableManager.h"


#if WITH_EDITOR
#include "ISettingsModule.h"
#include "StaticDataSettings.h"
#endif


#define LOCTEXT_NAMESPACE "FStaticDataTableManagerModule"

void FStaticDataTableManagerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

#if WITH_EDITOR
	if (ISettingsModule* SettingModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingModule->RegisterSettings("Project", "Game", "StaticDataSetting",
			INVTEXT("StaticDataSetting"),
			INVTEXT("Configure StaticData"),
			GetMutableDefault<UStaticDataSettings>()
		);
	}
#endif
}

void FStaticDataTableManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FStaticDataTableManagerModule, StaticDataTableManager)