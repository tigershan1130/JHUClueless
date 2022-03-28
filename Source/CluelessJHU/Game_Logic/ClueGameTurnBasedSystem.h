// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CGameStateBase.h"
#include "ClueGameTurnBasedSystem.generated.h"

#define print(text, color) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, color,text)


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLUELESSJHU_API UClueGameTurnBasedSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClueGameTurnBasedSystem();

	// This function will start the clue game
	// 1. it will spawn all the corresponding players (Game logic-> Server->Client-> Rendering System)
	// 2. Initialize cards in the deck?  (Game Logic Self update between client and server)
	UFUNCTION()
		void OnGameInit();

	// This function will make player make an accusation
	UFUNCTION()
		void OnPlayerMakeAccusation();

	// this function will make player to make a movement
	UFUNCTION()
		void OnPlayerMakeMovement();

	// This function will make player to make a suggestion
	UFUNCTION()
		void OnPlayerMakeSuggestion();

	// this function will make player end turn
	UFUNCTION()
		void OnPlayerEndTurn();



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};
