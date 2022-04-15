// Fill out your copyright notice in the Description page of Project Settings.
// This Class is Server Control Logic ONLY
// This class handles Accusation Check
// This class handles Suggestion Check
// This class handles Turn Check
// This class handles Winning and Losing Conditions

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CluelessJHU/Game_Logic/State/CGameStateBase.h"
#include "ClueGameTurnBasedComponent.generated.h"

#define print(text, color) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, color,text)


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLUELESSJHU_API UClueGameTurnBasedComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClueGameTurnBasedComponent();

#pragma region Server Events Recieved From Client
	// This function will start the clue game
	// 1. it will spawn all the corresponding players (Game logic-> Server->Client-> Rendering System)
	// 2. Initialize cards in the deck?  (Game Logic Self update between client and server)
	UFUNCTION()
		void OnGameInit();

	// This function will make player make an accusation
	UFUNCTION()
		void OnPlayerMakeAccusation(int RoleID, FString CWeaponID, FString CRoleID, FString CRoomID);

	// This function will make player to make a suggestion
	UFUNCTION()
		void OnPlayerMakeSuggestion(int RoleID, FString CWeaponID, FString CRoleID);

	// this function will make player end turn
	UFUNCTION()
		void OnPlayerEndTurn();

	// Player show card
	UFUNCTION()
		void OnPlayerShowCard(int RoleID, FString CardID);

	// player skip show card progress because he/she doesn't have a card to show.
	UFUNCTION()
		void OnPlayerSkipShowCard(int RoleID);


#pragma endregion Server Events Recieved From Client


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};
