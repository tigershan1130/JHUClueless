// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorBlock.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BlockMovementSubSystem.generated.h"

/**
 * 
 */
UCLASS()
class BLOCKMOVEMENTSYSTEM_API UBlockMovementSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UFUNCTION()
		void RegisterActorBlock(AActorBlock* Block);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Block Movement Data")
	TArray<AActorBlock*> ActorBlockList;	
};
