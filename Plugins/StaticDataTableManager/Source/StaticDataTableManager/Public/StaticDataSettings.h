// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "StaticDataSettings.generated.h"

/**
 *
 */
UCLASS(config = DataManager, defaultconfig, Blueprintable)
class STATICDATATABLEMANAGER_API UStaticDataSettings : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(config, EditAnywhere, Category = StaticData)
		TMap<FName, TSoftObjectPtr<UDataTable>> DataTableMap;

};
