// Fill out your copyright notice in the Description page of Project Settings.
// This is server logic only
// This class handles player movement logics
// Tracks if certain blocks is occupied and so on.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CluelessJHU/Game_Logic/State/CluelessMovementStateComponent.h"
#include "CluelessMovementComponent.generated.h"

#define print(text, color) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, color,text)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLUELESSJHU_API UCluelessMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCluelessMovementComponent();

#pragma region Server RPC calls
	// this function will make player to make a movement
	UFUNCTION()
		void OnPlayerMakeMovement(int BlockID, int CurrentRoleID);



#pragma endregion Server RPC calls

protected:
	// Called when the game starts
	virtual void BeginPlay() override;		

	UPROPERTY()
		UCluelessMovementStateComponent* CluelessMovementStateCompCache = nullptr;
};
