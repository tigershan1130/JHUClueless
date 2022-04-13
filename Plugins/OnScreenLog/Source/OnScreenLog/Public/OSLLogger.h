// Copyright 2021 Justin Kiesskalt, All Rights Reserved.

#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Misc/DateTime.h"
#include "OSLLogger.generated.h"

UENUM(BlueprintType)
enum class ELogSeverity : uint8
{
	Info = 0,
	Warning,
	Error,
	COUNT UMETA(Hidden)
};

USTRUCT()
struct FLogEntry
{
	GENERATED_USTRUCT_BODY()

public:
	int32 MessageID;
	FString Message;
	FName Category;
	ELogSeverity Severity;
	FDateTime Time;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLogClearedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLogAddedSignature, const FLogEntry&, Entry);

UCLASS()
class UOSLLogger : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/// <summary>
	/// Clears the backlog, meaning Loggers with “Read From Backlog” Enabled will not read logs from before this point.
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "OSL", Meta = (DisplayName = "OSL Clear Backlog"))
		static void OSLClearBacklog();

	/// <summary>
	/// Logs a new Info Message to the “LogDefault” Category
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "OSL", Meta = (DisplayName = "OSL Log Info"))
		static void OSLLogInfo(const FString& Message);

	/// <summary>
	/// Logs a new Warning Message to the “LogDefault” Category
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "OSL", Meta = (DisplayName = "OSL Log Warning"))
		static void OSLLogWarning(const FString& Message);

	/// <summary>
	/// Logs a new Error Message to the “LogDefault” Category
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "OSL", Meta = (DisplayName = "OSL Log Error"))
		static void OSLLogError(const FString& Message);

	/// <summary>
	/// Returns the number of messages that are stored in the backlog.
	/// </summary>
	UFUNCTION(BlueprintPure, Category = "OSL", Meta = (DisplayName = "OSL Get Num Messages"))
		static int32 OSLGetNumMessages();

public:
	static void OSLStartup();
	static void OSLShutdown();

	static void OSLLog(const TCHAR* Message, ELogVerbosity::Type Verbosity, const FName& Category);
	static bool OSLGetMessage(int32 MessageID, FLogEntry& OutMessage);

	static FOnLogClearedSignature OnLogCleared;
	static FOnLogAddedSignature OnLogAdded;

private:
	static int32 MessageIDCount;
	static TArray<FLogEntry> Logs;
};