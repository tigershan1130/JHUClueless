// Copyright 2021 Justin Kiesskalt, All Rights Reserved.

#include "OnScreenLog.h"
#include "OSLLogger.h"
#include "OSLSettings.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogDefault);
#define LOCTEXT_NAMESPACE "FOnScreenLogModule"

void FOnScreenLogModule::StartupModule()
{
	LogDevice = MakeShareable<FOSLOutputDevice<FOnScreenLogModule>>(new FOSLOutputDevice<FOnScreenLogModule>());
	LogDevice->SetContextObject(this);

	Settings = GetMutableDefault<UOSLSettings>();

	UOSLLogger::OSLStartup();
}

void FOnScreenLogModule::ShutdownModule()
{
	UOSLLogger::OSLShutdown();
}

void FOnScreenLogModule::OnLog(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category)
{
	if(!Settings.IsValid() || Settings->GetIsDisabled())
		return;

	UOSLLogger::OSLLog(V, Verbosity, Category);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOnScreenLogModule, OnScreenLog)