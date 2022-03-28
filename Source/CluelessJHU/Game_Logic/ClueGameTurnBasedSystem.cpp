// Fill out your copyright notice in the Description page of Project Settings.


#include "ClueGameTurnBasedSystem.h"
#include "CluelessJHU/Player/Clueless_PlayerState.h"
#include "CluelessJHU/Data/Game_StaticData.h"

// Sets default values for this component's properties
UClueGameTurnBasedSystem::UClueGameTurnBasedSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UClueGameTurnBasedSystem::OnPlayerEndTurn()
{
	print("[Server: CluelessGameLogic] TODO: End Current Players Turn->Move Turn To Next Player", FColor::Green);

}

// Called when the game starts
void UClueGameTurnBasedSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UClueGameTurnBasedSystem::OnGameInit()
{
	print("[Server: CluelessGameLogic] Initializing Game...", FColor::Green);
	
	//1. Spawn and distributing cards
	ACGameStateBase* GameState = GetWorld()->GetGameState<ACGameStateBase>();

	if (GameState == nullptr)
		return;

	TArray<FCardEntityData> InitedCards = GameState->GetCardsSetupData();

	// put our initied cards into those cards.
	TArray<FCardEntityData> WeaponCards = InitedCards.FilterByPredicate([](const FCardEntityData& Data) { return Data.Type == 1; });
	TArray<FCardEntityData> CharacterCards = InitedCards.FilterByPredicate([](const FCardEntityData& Data) {return Data.Type == 0;  });
	TArray<FCardEntityData> RoomCards = InitedCards.FilterByPredicate([](const FCardEntityData& Data) {return Data.Type == 2;  });

	// Get current valid players.
	TArray<APlayerController*> ActiveCharacterControls = GameState->GetActiveController_Server();
	TArray<AClueless_PlayerState*> ActivePlayerStates;

	// Populate our current active player states for distributing cards to players
	for (auto& Entry : ActiveCharacterControls)
	{
		AClueless_PlayerState* CurrentPS = Entry->GetPlayerState<AClueless_PlayerState>();
		if (CurrentPS)
			ActivePlayerStates.Add(CurrentPS);
	}

	// Check if our initialization data is ok, if it is, continue with our logic.
	if (InitedCards.Num() <= 0 || ActivePlayerStates.Num() <= 0)
	{
		print("[Server: CluelessGameLogic] Bad Generating and Distributing Cards...", FColor::Red);
		return;		
	}

	// Init our data before cards distribution
	int NumberOfPlayers = ActivePlayerStates.Num();

	// 1. Murder Deck

	// 2. Player Cards

	// 3. Extra Cards



	// Spawn Player Character


}

void UClueGameTurnBasedSystem::OnPlayerMakeAccusation()
{
	print("[Server: CluelessGameLogic] TODO: Check Accusation Cards->Player Status Adjustment", FColor::Green);

	
}

void UClueGameTurnBasedSystem::OnPlayerMakeMovement()
{
	print("[Server: CluelessGameLogic] TODO: Validate Player Movement->Make Player Movement", FColor::Green);

}

void UClueGameTurnBasedSystem::OnPlayerMakeSuggestion()
{
	print("[Server: CluelessGameLogic] TODO: check Player Suggestion", FColor::Green);

}

