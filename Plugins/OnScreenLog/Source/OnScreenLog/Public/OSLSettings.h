// Copyright 2021 Justin Kiesskalt, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "OSLSettings.generated.h"

class UOSLWidget;

/**
 *
 */
UCLASS(config = OSL, defaultconfig)
class ONSCREENLOG_API UOSLSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UOSLSettings(const FObjectInitializer& OI);
	bool GetIsDisabled() const;

	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;

#if WITH_EDITOR
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
#endif

protected:
	/// <summary>
	/// If true, the plugin will be disabled in shipping and wont show on the screen.
	/// </summary>
	UPROPERTY(EditAnywhere, config, Category = OSL)
		bool bDisableInShipping = true;
};
