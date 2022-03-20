// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MovementRuleSet.generated.h"


class AActorBlock;
/**
 * 
 */
UCLASS()
class BLOCKMOVEMENTSYSTEM_API UMovementRuleSet : public UObject
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintImplementableEvent, Category = "Block Movement ")
		void CheckMovementBlocks(TArray<AActorBlock*>& Blocks, TArray<AActorBlock*>& ValidBlocks);

	
};
