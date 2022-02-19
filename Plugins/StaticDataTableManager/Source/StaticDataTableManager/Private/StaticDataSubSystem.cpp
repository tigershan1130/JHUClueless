// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticDataSubSystem.h"
#include "StaticDataSettings.h"



void UStaticDataSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	//InitStaticData();
	//FString fileName;
	//const UStaticDataSettings& Settings = *GetDefault<UStaticDataSettings>();
	//TMap<FString, FString> CommandMap = Settings.CommandList;
	//TArray<FString> CommandList;
	//CommandMap.GenerateValueArray(CommandList);
	//for (int i = 0; i < CommandList.Num(); i++)
	//{
	//	UKismetSystemLibrary::ExecuteConsoleCommand(this, CommandList[i]);
	//}
	//if (CommandList.Contains("DisableAllDebugUI"))
	//{
	//	IsHideDebugUI = true;
	//}
	/*this->ServerUrl = Settings.ServerUrl;
	this->ClientBuildBranch = Settings.BuildBranch;
	this->IntranetServerUrlList = Settings.IntranetServerUrlList;*/
}


void UStaticDataSubSystem::InitStaticData()
{
	if (!IsDataBeenInit)
	{
		LoadDataTableStaticData();
		//Call Other Load Func
		IsDataBeenInit = true;
	}

}


void UStaticDataSubSystem::LoadDataTableStaticData()
{
	const UStaticDataSettings& Settings = *GetDefault<UStaticDataSettings>();
	TMap<FName, TSoftObjectPtr<UDataTable>> DataTableMapFromSettings = Settings.DataTableMap;

	if (DataTableMapFromSettings.Num() == 0)
	{
		return;
	}

	for (auto item : DataTableMapFromSettings)
	{
		UDataTable* Temp = nullptr;
		if (item.Value.IsValid())
		{
			Temp = item.Value.Get();
		}
		else
		{
			Temp = item.Value.LoadSynchronous();
		}
		DataTableMap.Add(item.Key, Temp);
	}
}

bool UStaticDataSubSystem::Generic_GetDataTableRowByName(const UDataTable* Table, FName RowName, void* OutRowPtr)
{
	bool bFoundRow = false;

	if (OutRowPtr && Table)
	{
		void* RowPtr = Table->FindRowUnchecked(RowName);

		if (RowPtr != nullptr)
		{
			const UScriptStruct* StructType = Table->GetRowStruct();

			if (StructType != nullptr)
			{
				StructType->CopyScriptStruct(OutRowPtr, RowPtr);
				bFoundRow = true;
			}
		}
	}

	return bFoundRow;
}

UDataTable* UStaticDataSubSystem::GetDataTableByName(const FName& TableName)
{
	const UStaticDataSettings& Settings = *GetDefault<UStaticDataSettings>();
	TMap<FName, TSoftObjectPtr<UDataTable>> DataTableMap = Settings.DataTableMap;
	TSoftObjectPtr<UDataTable>* Table = nullptr;

	Table = DataTableMap.Find(TableName);
	UDataTable* Result = nullptr;

	if (Table)
	{
		Result = Table->Get();
	}
	return Result;
}

bool UStaticDataSubSystem::GetDataTableRowByName(FName TableName, FName RowName, FTableRowBase& OutRow)
{
	check(0);

	return false;
}
