// Fill out your copyright notice in the Description page of Project Settings.


#include "ClueGameTurnBasedComponent.h"
#include "Kismet/KismetArrayLibrary.h"
#include "CluelessJHU/Player_Logic/Clueless_PlayerState.h"
#include "CluelessJHU/Data/Game_StaticData.h"

// Sets default values for this component's properties
UClueGameTurnBasedComponent::UClueGameTurnBasedComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UClueGameTurnBasedComponent::OnPlayerEndTurn()
{
	print("[Server: CluelessGameLogic] End Current Players Turn", FColor::Green);

	//1. Spawn and distributing cards
	ACGameStateBase* GameState = GetWorld()->GetGameState<ACGameStateBase>();

	if (GameState == nullptr)
		return;

	GameState->ChangeToNextTurnIndex();
}

// Called when the game starts
void UClueGameTurnBasedComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UClueGameTurnBasedComponent::OnGameInit()
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

	// Data 
	TArray<FCardEntityData> MurderDeck;
	TMap<int, TArray<FCardEntityData>> PlayerHands;
	TArray<FCardEntityData> ExtraCards;

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
	int MurderWeaponIndex = FMath::RandRange(0, WeaponCards.Num()-1);
	int MurderCharIndex = FMath::RandRange(0, CharacterCards.Num()-1);
	int MurderRoomIndex = FMath::RandRange(0, RoomCards.Num()-1);


	// Remove Murder Cards from initial stack.
	WeaponCards.RemoveAt(MurderWeaponIndex);
	CharacterCards.RemoveAt(MurderCharIndex);
	RoomCards.RemoveAt(MurderRoomIndex);

	// 2. Player Cards
	// Distributing of player cards would not be necessary for Player Cards
	TArray<FCardEntityData> ShuffleCards;
	ShuffleCards.Append(WeaponCards);
	ShuffleCards.Append(CharacterCards);
	ShuffleCards.Append(RoomCards);

	int TotalCardsNum = ShuffleCards.Num();
	int PlayerStartingCardsNum = TotalCardsNum / NumberOfPlayers; // integer automatically rounds down.

	// shuffle cards
	for (int32 i = ShuffleCards.Num() - 1; i > 0; i--) {
		int32 j = FMath::Floor(FMath::Rand() * (i + 1)) % ShuffleCards.Num();
		FCardEntityData temp = ShuffleCards[i];
		ShuffleCards[i] = ShuffleCards[j];
		ShuffleCards[j] = temp;
	}


	// putting different cards into player's hands
	for (int i = 0; i < NumberOfPlayers; i++)
	{
		PlayerHands.Add(i, TArray<FCardEntityData>());

		for (int j = 0; j < PlayerStartingCardsNum; j++)
		{
			int CurrentIndex = i * (PlayerStartingCardsNum-1) + j;
			PlayerHands[i].Add(ShuffleCards[CurrentIndex]);
		}
	}

	// 3. Extra Cards
	int RemainingCards = TotalCardsNum - PlayerStartingCardsNum * NumberOfPlayers;

	if (RemainingCards > 0)
	{
		for (int i = PlayerStartingCardsNum * NumberOfPlayers; i < TotalCardsNum; i++)
		{
			ExtraCards.Add(ShuffleCards[i]);
		}
	}

	// 4. Setup all Cards Data.
	GameState->SetupCards(MurderDeck, ExtraCards);
	 
	for (int i = 0; i < ActivePlayerStates.Num(); i++)
	{
		if (PlayerHands.Contains(i))
			ActivePlayerStates[i]->SetCardsInHand(PlayerHands[i]);
	}

	// Spawn Player Character
	print("[Server: CluelessGameLogic] TODO: Set Player Character Spawn and Initialization", FColor::Red);
}


void UClueGameTurnBasedComponent::OnPlayerMakeAccusation()
{
	print("[Server: CluelessGameLogic] TODO: Check Accusation Cards->Player Status Adjustment", FColor::Red);

	
}

void UClueGameTurnBasedComponent::OnPlayerMakeSuggestion()
{
	print("[Server: CluelessGameLogic] TODO: check Player Suggestion", FColor::Red);

}

