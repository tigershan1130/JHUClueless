// Copyright 2021 Justin Kiesskalt, All Rights Reserved.

#include "OSLWidget.h"
#include "OSLWidgetEntry.h"
#include "OSLLogger.h"
#include "OSLSettings.h"

#include "Components/PanelWidget.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ScrollBox.h"

void UOSLWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SeverityColors.SetNum(static_cast<int32>(ELogSeverity::COUNT));
	SeverityColors[static_cast<int32>(ELogSeverity::Info)] = InfoTextColor;
	SeverityColors[static_cast<int32>(ELogSeverity::Warning)] = WarningTextColor;
	SeverityColors[static_cast<int32>(ELogSeverity::Error)] = ErrorTextColor;

	UOSLSettings* Settings = GetMutableDefault<UOSLSettings>();

	if (Settings->GetIsDisabled())
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	Entries.SetNum(MaxMessages);
	for (int32 i = 0; i < MaxMessages; ++i)
	{
		UOSLWidgetEntry* NewEntry = CreateWidget<UOSLWidgetEntry>(GetOwningPlayer(), EntryClass);
		NewEntry->SetVisibility(ESlateVisibility::Collapsed);
		EntryParent->AddChild(NewEntry);
		Entries[i] = NewEntry;
	}

	for (int32 i = 0; i < CategoriesToShow.Num(); ++i)
	{
		SetCategoryEnabled(CategoriesToShow[i], true);
	}

	if (bReadBacklog)
	{
		for (int32 i = 0; i < UOSLLogger::OSLGetNumMessages(); ++i)
		{
			FLogEntry Entry;
			UOSLLogger::OSLGetMessage(i, Entry);

			Logs.Add(Entry);
		}
	}

	EntryParent->ForceLayoutPrepass();
	EntryParent->SetScrollOffset(EntryParent->GetScrollOffsetOfEnd());

	UOSLLogger::OnLogAdded.AddDynamic(this, &UOSLWidget::OnLogAdded);

	if (ClearLogButton) ClearLogButton->OnClicked.AddDynamic(this, &UOSLWidget::OnClearLogPressed);
	if (MinimizeButton) MinimizeButton->OnClicked.AddDynamic(this, &UOSLWidget::OnMinimizePressed);
	if (MaximizeButton) MaximizeButton->OnClicked.AddDynamic(this, &UOSLWidget::OnMaximizePressed);

	if (InfoCheckbox) InfoCheckbox->OnCheckStateChanged.AddDynamic(this, &UOSLWidget::OnInfoCheckBoxStateChanged);
	if (WarningCheckbox) WarningCheckbox->OnCheckStateChanged.AddDynamic(this, &UOSLWidget::OnWarningCheckBoxStateChanged);
	if (ErrorCheckbox) ErrorCheckbox->OnCheckStateChanged.AddDynamic(this, &UOSLWidget::OnErrorCheckBoxStateChanged);

	SetSeverityEnabled(ELogSeverity::Info, bInfosStartEnabled);
	SetSeverityEnabled(ELogSeverity::Warning, bWarningsStartEnabled);
	SetSeverityEnabled(ELogSeverity::Error, bErrorsStartEnabled);

	if (bStartMinimized)
		Minimize();
	else
		Maximize();
}

void UOSLWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	{
		QUICK_SCOPE_CYCLE_COUNTER(OSL___Tick_Widget);
		for (int32 i = 0; i < MaxMessages; ++i)
		{
			Entries[i]->SetVisibility(ESlateVisibility::Collapsed);
		}

		int32 VisCounter = MaxMessages - 1;
		for (int32 i = Logs.Num() - 1; i >= 0; --i)
		{
			if(VisCounter < 0)
				break;

			bool bVisible = IsMessageVisible(i);

			FLogEntry Entry = Logs[i];
			FSlateColor SeverityColor = GetSeverityColor(Entry.Severity);

			FLinearColor ZebraColor = VisCounter % 2 != 0 ? ZebraColor1 : ZebraColor2;
			Entries[VisCounter]->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
			Entries[VisCounter]->SetBackgroundColor(ZebraColor);
			Entries[VisCounter]->SetTextColor(SeverityColor);

			FString TimeStr = FString::FromInt(Entry.Time.GetHour()) + ":" + FString::FromInt(Entry.Time.GetMinute()) + ":" + FString::FromInt(Entry.Time.GetSecond());
			FText MessageText = FText::FromString("[" + TimeStr + "]" + "[" + Entry.Category.ToString() + "]" + " " + Entry.Message);
			Entries[VisCounter]->SetText(MessageText);

			if (bVisible)
				VisCounter--;
		}

		InfoCheckbox->SetIsChecked(IsSeverityEnabled(ELogSeverity::Info));
		WarningCheckbox->SetIsChecked(IsSeverityEnabled(ELogSeverity::Warning));
		ErrorCheckbox->SetIsChecked(IsSeverityEnabled(ELogSeverity::Error));

		//Get the new end offset
		float EndOffset = EntryParent->GetScrollOffsetOfEnd();
		EntryParent->ForceLayoutPrepass();
		float NewEndOffset = EntryParent->GetScrollOffsetOfEnd();

		float ScrollOffset = EntryParent->GetScrollOffset();
		EntryParent->SetScrollOffset(ScrollOffset + (NewEndOffset - EndOffset));
	}
}

void UOSLWidget::NativeDestruct()
{
	Super::NativeDestruct();

	UOSLLogger::OnLogAdded.RemoveDynamic(this, &UOSLWidget::OnLogAdded);

	if(ClearLogButton) ClearLogButton->OnClicked.RemoveDynamic(this, &UOSLWidget::OnClearLogPressed);
	if(MinimizeButton) MinimizeButton->OnClicked.RemoveDynamic(this, &UOSLWidget::OnMinimizePressed);
	if(MaximizeButton) MaximizeButton->OnClicked.RemoveDynamic(this, &UOSLWidget::OnMaximizePressed);

	if(InfoCheckbox) InfoCheckbox->OnCheckStateChanged.RemoveDynamic(this, &UOSLWidget::OnInfoCheckBoxStateChanged);
	if(WarningCheckbox) WarningCheckbox->OnCheckStateChanged.RemoveDynamic(this, &UOSLWidget::OnWarningCheckBoxStateChanged);
	if(ErrorCheckbox) ErrorCheckbox->OnCheckStateChanged.RemoveDynamic(this, &UOSLWidget::OnErrorCheckBoxStateChanged);
}

void UOSLWidget::SetSeverityEnabled(ELogSeverity Severity, bool bEnabled)
{
	if (bEnabled)
	{
		SeverityStates |= (1 << static_cast<int32>(Severity));
	}
	else
	{
		SeverityStates = SeverityStates & ~(1 << static_cast<int32>(Severity));
	}
}

void UOSLWidget::SetCategoryEnabled(const FName& Category, bool bEnabled)
{
	int32 Hash = GetTypeHash(Category);

	if (bEnabled)
	{
		HashedCategoriesToShow.AddUnique(Hash);
	}
	else
	{
		HashedCategoriesToShow.Remove(Hash);
	}
}

void UOSLWidget::SetShowAllCategories(bool bEnabled)
{
	bShowAllCategories = bEnabled;
}

void UOSLWidget::OnLogAdded(const FLogEntry& Log)
{
	Logs.Add(Log);
}

void UOSLWidget::ClearLog()
{
	Logs.Empty();
}

void UOSLWidget::Minimize()
{
	bIsMinimized = true;

	MinimizedPanel->SetVisibility(bIsMinimized ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	MaximizedPanel->SetVisibility(bIsMinimized ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
}

void UOSLWidget::Maximize()
{
	bIsMinimized = false;

	MinimizedPanel->SetVisibility(bIsMinimized ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	MaximizedPanel->SetVisibility(bIsMinimized ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

	EntryParent->ScrollToEnd();
}

bool UOSLWidget::IsMinimized() const
{
	return bIsMinimized;
}

bool UOSLWidget::IsMaximized() const
{
	return !bIsMinimized;
}

void UOSLWidget::OnClearLogPressed()
{
	ClearLog();
}

void UOSLWidget::OnMinimizePressed()
{
	Minimize();
}

void UOSLWidget::OnMaximizePressed()
{
	Maximize();
}

void UOSLWidget::OnInfoCheckBoxStateChanged(bool bIsChecked)
{
	SetSeverityEnabled(ELogSeverity::Info, bIsChecked);
}

void UOSLWidget::OnWarningCheckBoxStateChanged(bool bIsChecked)
{
	SetSeverityEnabled(ELogSeverity::Warning, bIsChecked);
}

void UOSLWidget::OnErrorCheckBoxStateChanged(bool bIsChecked)
{
	SetSeverityEnabled(ELogSeverity::Error, bIsChecked);
}

bool UOSLWidget::IsMessageVisible(int32 MessageIndex) const
{
	if (!Logs.IsValidIndex(MessageIndex))
		return false;

	return IsSeverityEnabled(Logs[MessageIndex].Severity) && IsCategoryEnabled(Logs[MessageIndex].Category);
}

bool UOSLWidget::IsSeverityEnabled(ELogSeverity Severity) const
{
	return (SeverityStates & (1 << static_cast<int32>(Severity))) != 0;
}

bool UOSLWidget::IsCategoryEnabled(const FName& Category) const
{
	int32 Hash = GetTypeHash(Category);
	return bShowAllCategories || HashedCategoriesToShow.Contains(Hash);
}

bool UOSLWidget::IsShowingAllCategories() const
{
	return bShowAllCategories;
}

FSlateColor UOSLWidget::GetSeverityColor(ELogSeverity Severity) const
{
	return SeverityColors[static_cast<int32>(Severity)];
}