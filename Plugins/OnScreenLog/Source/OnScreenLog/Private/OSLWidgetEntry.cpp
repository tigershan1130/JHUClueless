// Copyright 2021 Justin Kiesskalt, All Rights Reserved.

#include "OSLWidgetEntry.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "OSLLogger.h"
#include "OSLSettings.h"
#include "Misc/DateTime.h"

void UOSLWidgetEntry::SetTextColor(FSlateColor& Color)
{
	MessageText->SetColorAndOpacity(Color);
}

void UOSLWidgetEntry::SetBackgroundColor(FLinearColor& Color)
{
	BackgroundImage->SetColorAndOpacity(Color);
}

void UOSLWidgetEntry::SetText(const FText& Text)
{
	MessageText->SetText(Text);
}