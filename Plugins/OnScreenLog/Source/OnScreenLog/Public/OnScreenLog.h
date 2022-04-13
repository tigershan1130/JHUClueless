// Copyright 2021 Justin Kiesskalt, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OSLWidget.h"
#include "OSLOutputDevice.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDefault, Log, All);

class UOSLSettings;

class ONSCREENLOG_API FOnScreenLogModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OnLog(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category);

private:
	TWeakObjectPtr<UOSLSettings> Settings;
	TSharedPtr<UOSLWidget> Widget;
	TSharedPtr<FOSLOutputDevice<FOnScreenLogModule>> LogDevice = nullptr;
};