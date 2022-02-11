// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StaticDataSubSystem.generated.h"

/**
 *
 */
UCLASS()
class STATICDATATABLEMANAGER_API UStaticDataSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
		void InitStaticData();

	//GetDataTableRowByName
	template <class T>
	T* FindRow(FName TableName, FName RowName)
	{
		UDataTable* Table = GetDataTableByName(TableName);

		if (RowName == NAME_None)
		{
			return nullptr;
		}

		uint8* const* RowDataPtr = nullptr;
		if (Table)
		{
			RowDataPtr = Table->GetRowMap().Find(RowName);
		}

		if (RowDataPtr == nullptr)
		{
			return nullptr;
		}

		uint8* RowData = *RowDataPtr;
		check(RowData);

		return reinterpret_cast<T*>(RowData);
	}

	//GetDataTableRowByName for BP
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "StaticData", meta = (CustomStructureParam = "OutRow"))
		bool GetDataTableRowByName(FName TableName, FName RowName, FTableRowBase& OutRow);

	static bool Generic_GetDataTableRowByName(const UDataTable* Table, FName RowName, void* OutRowPtr);

	DECLARE_FUNCTION(execGetDataTableRowByName)
	{
		P_GET_PROPERTY(FNameProperty, TableName)
			UDataTable* Table = GetDataTableByName(TableName);
		P_GET_PROPERTY(FNameProperty, RowName);

		Stack.StepCompiledIn<FStructProperty>(NULL);
		void* OutRowPtr = Stack.MostRecentPropertyAddress;

		P_FINISH;
		bool bSuccess = false;

		FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
		if (!Table)
		{
			FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				NSLOCTEXT("GetDataTableRow", "MissingTableInput", "Failed to resolve the table input. Be sure the DataTable is valid.")
			);
		}
		else if (StructProp && OutRowPtr)
		{
			UScriptStruct* OutputType = StructProp->Struct;
			const UScriptStruct* TableType = Table->GetRowStruct();

			const bool bCompatible = (OutputType == TableType) ||
				(OutputType->IsChildOf(TableType) && FStructUtils::TheSameLayout(OutputType, TableType));
			if (bCompatible)
			{
				P_NATIVE_BEGIN;
				bSuccess = Generic_GetDataTableRowByName(Table, RowName, OutRowPtr);
				P_NATIVE_END;
			}
			else
			{
				FBlueprintExceptionInfo ExceptionInfo(
					EBlueprintExceptionType::AccessViolation,
					NSLOCTEXT("GetDataTableRow", "IncompatibleProperty", "Incompatible output parameter; the data table's type is not the same as the return type.")
				);
			}
		}
		else
		{
			FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				NSLOCTEXT("GetDataTableRow", "MissingOutputProperty", "Failed to resolve the output parameter for GetDataTableRow.")
			);
		}
		*(bool*)RESULT_PARAM = bSuccess;
	}

private:
	UFUNCTION(BlueprintCallable)
		static UDataTable* GetDataTableByName(const FName& TableName);

	UFUNCTION()
		void LoadDataTableStaticData();

	UPROPERTY()
		bool IsDataBeenInit;

	//Store DataTableMap, preventing from GC
	UPROPERTY()
		TMap<FName, UDataTable*> DataTableMap;


};
