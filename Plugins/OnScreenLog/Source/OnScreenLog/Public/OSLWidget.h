// Copyright 2021 Justin Kiesskalt, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OSLWidget.generated.h"

class UButton;
class UCheckBox;
class UPanelWidget;
class UOSLWidgetEntry;
class UScrollBox;

/**
 *
 */
UCLASS()
class ONSCREENLOG_API UOSLWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/// <summary>
	/// The widget to create for each new entry in the log
	/// </summary>
	UPROPERTY(EditDefaultsOnly, Category = "OSL")
		TSubclassOf<UOSLWidgetEntry> EntryClass;

	/// <summary>
	/// The number of messages the log can show, setting this too high may cause lag
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OSL")
		int32 MaxMessages = 999;

	/// <summary>
	/// The text color of logs that have the severity of “Info”
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OSL")
		FSlateColor InfoTextColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));

	/// <summary>
	/// The text color of logs that have the severity of “Warning”
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OSL")
		FSlateColor WarningTextColor = FSlateColor(FLinearColor(1.0f, 1.0f, 0.0f, 1.0f));

	/// <summary>
	/// The text color of logs that have the severity of “Error”
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OSL")
		FSlateColor ErrorTextColor = FSlateColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));

	/// <summary>
	/// The background color to show on every second entry
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OSL")
		FLinearColor ZebraColor1 = FLinearColor(0.03f, 0.03f, 0.03f, 0.3f);

	/// <summary>
	/// The background color to show on every other entry
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OSL")
		FLinearColor ZebraColor2 = FLinearColor(0.0f, 0.0f, 0.0f, 0.3f);

	/// <summary>
	/// If true, the On Screen Log will start minimized
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OSL")
		bool bStartMinimized = false;

	/// <summary>
	/// If true, the On Screen Log will show messages from before it was added to the screen
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OSL")
		bool bReadBacklog = true;

	/// <summary>
	/// If true, Info Messages will show by default
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OSL")
		bool bInfosStartEnabled = true;

	/// <summary>
	/// If true, Warning Messages will show by default
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OSL")
		bool bWarningsStartEnabled = true;

	/// <summary>
	/// If true, Error Messages will show by default
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OSL")
		bool bErrorsStartEnabled = true;

	/// <summary>
	/// If true, This On Screen Log will show messages from all categories.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OSL")
		bool bShowAllCategories = true;

	/// <summary>
	/// A list of categories to show in this On Screen Log
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OSL", Meta = (EditCondition = "!bShowAllCategories", EditConditionHides))
		TArray<FName> CategoriesToShow;

	/// <summary>
	/// Toggles filter on the specified severity
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "OSL")
		void SetSeverityEnabled(ELogSeverity Severity, bool bEnabled);

	/// <summary>
	/// Toggles whether or not the category will show (overriden by bShowAllCategories)
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "OSL")
		void SetCategoryEnabled(const FName& Category, bool bEnabled);

	/// <summary>
	/// Toggles whether or not this logger should show every category of message
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "OSL")
		void SetShowAllCategories(bool bEnabled);

	/// <summary>
	/// Clears this logger and removes all messages
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "OSL")
		void ClearLog();

	/// <summary>
	/// Minimizes this logger so its hidden on the screen
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "OSL")
		void Minimize();

	/// <summary>
	/// Opens this logger so its visible on the screen
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "OSL")
		void Maximize();

	/// <summary>
	/// returns true if this logger is minimized
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "OSL")
		bool IsMinimized() const;

	/// <summary>
	/// returns true if this logger is maximized
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "OSL")
		bool IsMaximized() const;

	/// <summary>
	/// returns true if the message in this Loggers' log is visible (not being filtered out)
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "OSL")
		bool IsMessageVisible(int32 MessageIndex) const;

	/// <summary>
	/// returns true if this logger is showing messages with the given severity
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "OSL")
		bool IsSeverityEnabled(ELogSeverity Severity) const;

	/// <summary>
	/// returns true if this logger is showing messages with the given category
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "OSL")
		bool IsCategoryEnabled(const FName& Category) const;

	/// <summary>
	/// returns true if this logger is showing messages from every category
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "OSL")
		bool IsShowingAllCategories() const;

	/// <summary>
	/// returns the text color for the given severity
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "OSL")
		FSlateColor GetSeverityColor(ELogSeverity Severity) const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

	UFUNCTION() void OnClearLogPressed();
	UFUNCTION() void OnMinimizePressed();
	UFUNCTION() void OnMaximizePressed();
	UFUNCTION() void OnInfoCheckBoxStateChanged(bool bIsChecked);
	UFUNCTION() void OnWarningCheckBoxStateChanged(bool bIsChecked);
	UFUNCTION() void OnErrorCheckBoxStateChanged(bool bIsChecked);

	UFUNCTION() void OnLogAdded(const FLogEntry& Log);

protected:
	UPROPERTY(Meta = (BindWidget)) UPanelWidget* MinimizedPanel;
	UPROPERTY(Meta = (BindWidget)) UPanelWidget* MaximizedPanel;
	UPROPERTY(Meta = (BindWidget)) UScrollBox* EntryParent;
	UPROPERTY(Meta = (BindWidget)) UButton* ClearLogButton;
	UPROPERTY(Meta = (BindWidget)) UButton* MinimizeButton;
	UPROPERTY(Meta = (BindWidget)) UButton* MaximizeButton;
	UPROPERTY(Meta = (BindWidget)) UCheckBox* InfoCheckbox;
	UPROPERTY(Meta = (BindWidget)) UCheckBox* WarningCheckbox;
	UPROPERTY(Meta = (BindWidget)) UCheckBox* ErrorCheckbox;

private:
	UPROPERTY() TArray<UOSLWidgetEntry*> Entries;
	UPROPERTY() TArray<FSlateColor> SeverityColors;
	UPROPERTY() TArray<FLogEntry> Logs;
	UPROPERTY() TArray<int32> HashedCategoriesToShow;
	uint8 SeverityStates;
	bool bIsMinimized = false;
	float LastScrollPosition = 0.0f;
};