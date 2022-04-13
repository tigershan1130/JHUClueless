// Copyright 2021 Justin Kiesskalt, All Rights Reserved.

#include "OSLLogger.h"

int32 UOSLLogger::MessageIDCount;
TArray<FLogEntry> UOSLLogger::Logs;

FOnLogAddedSignature UOSLLogger::OnLogAdded;
FOnLogClearedSignature UOSLLogger::OnLogCleared;

void UOSLLogger::OSLStartup()
{

}

void UOSLLogger::OSLShutdown()
{
	OSLClearBacklog();
}

void UOSLLogger::OSLClearBacklog()
{
	Logs.Empty();
	MessageIDCount = 0;

	OnLogCleared.Broadcast();
}

void UOSLLogger::OSLLogInfo(const FString& Message)
{
	UE_LOG(LogDefault, Log, TEXT("%s"), *Message);
}

void UOSLLogger::OSLLogWarning(const FString& Message)
{
	UE_LOG(LogDefault, Warning, TEXT("%s"), *Message);
}

void UOSLLogger::OSLLogError(const FString& Message)
{
	UE_LOG(LogDefault, Error, TEXT("%s"), *Message);
}

int32 UOSLLogger::OSLGetNumMessages()
{
	return Logs.Num();
}

void UOSLLogger::OSLLog(const TCHAR* Message, ELogVerbosity::Type Verbosity, const FName& Category)
{
	FString StrMessage = Message;
	
	if(StrMessage.Len() <= 0)
		return;

	ELogSeverity Severity = ELogSeverity::Info;

	switch (Verbosity)
	{
	case ELogVerbosity::Error:
	case ELogVerbosity::Fatal:
		Severity = ELogSeverity::Error;
		break;

	case ELogVerbosity::Warning:
		Severity = ELogSeverity::Warning;
		break;
	}

	FLogEntry Entry;
	Entry.MessageID = MessageIDCount++;
	Entry.Message = StrMessage;
	Entry.Severity = Severity;
	Entry.Category = Category;
	Entry.Time = FDateTime::Now();

	Logs.Add(Entry);
	OnLogAdded.Broadcast(Entry);
}

bool UOSLLogger::OSLGetMessage(int32 MessageID, FLogEntry& OutMessage)
{
	if(!Logs.IsValidIndex(MessageID))
		return false;

	OutMessage = Logs[MessageID];
	return true;
}