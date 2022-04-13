// Copyright 2021 Justin Kiesskalt, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OSLLogger.h"
#include "OSLWidgetEntry.generated.h"

class UTextBlock;
class UImage;

/**
 *
 */
UCLASS()
class ONSCREENLOG_API UOSLWidgetEntry : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetBackgroundColor(FLinearColor& Color);
	void SetTextColor(FSlateColor& Color);
	void SetText(const FText& Text);

protected:
	UPROPERTY(Meta = (BindWidget)) UTextBlock* MessageText;
	UPROPERTY(Meta = (BindWidget)) UImage* BackgroundImage;
	UPROPERTY(Meta = (BindWidget)) UPanelWidget* EntryPanel;
};
