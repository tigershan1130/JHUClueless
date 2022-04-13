// Fill out your copyright notice in the Description page of Project Settings.


#include "ClueGameTurnBasedComponent.h"
#include "Kismet/KismetArrayLibrary.h"
#include "CluelessJHU/Utilities/GameplayAPI.h"
#include "CluelessJHU/Player_Logic/State/Clueless_PlayerState.h"
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

	MurderDeck.Add(WeaponCards[MurderWeaponIndex]);
	MurderDeck.Add(CharacterCards[MurderCharIndex]);
	MurderDeck.Add(RoomCards[MurderRoomIndex]);

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
}


void UClueGameTurnBasedComponent::OnPlayerMakeAccusation(int RoleID, FString CWeaponID, FString CRoleID, FString CRoomID)
{
	ACGameStateBase* GameState = GetWorld()->GetGameState<ACGameStateBase>();

	if (GameState == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[CluelessGameLogic] Can't Find GameState"));
		return;
	}

	TArray<FCardEntityData> MurderDeck = GameState->GetMurderDeck();

	// go through our murder deck to check our cards
	int CorrectCounter = 0;
	for (int i = 0; i < MurderDeck.Num(); i++)
	{
		if (MurderDeck[i].CardID == CRoleID)
		{
			CorrectCounter++;
		}
		if(MurderDeck[i].CardID == CWeaponID)
		{
			CorrectCounter++;
		}
		if (MurderDeck[i].CardID == CRoomID)
		{
			CorrectCounter++;
		}
	}

	//TODO: make current role clear accusation action

	if (CorrectCounter >= 3)
	{
		print("[Server: CluelessGameLogic] Player Made Correct Accusation", FColor::Green);

		print("[Server: CluelessGameLogic] Notifies all Clients, Game Won!", FColor::Green);
	}
	else
	{
		print("[Server: CluelessGameLogic] Player Made False Accusation", FColor::Green);

		print("[Server: CluelessGameLogic] Marking Player As Audience(Move,EndTurns)", FColor::Green);
	}

	

	
}

void UClueGameTurnBasedComponent::OnPlayerMakeSuggestion(int RoleID, FString CWeaponID, FString CRoleID, FString CRoomID)
{
	FString DebugText = "[Server: CluelessGameLogic] Player making suggestion: [" + CWeaponID + "," + CRoleID + "," + CRoomID + "]";

	print(DebugText, FColor::Green);

	// Check if Current role is actually inside a room before he or she can make a suggestion
    ACharacter* SuggestionInstigator = UGameplayAPI::GetCharacterFromRoleID(GetWorld(), RoleID);

	AClueless_PlayerState* CPlayerState = SuggestionInstigator->GetPlayerState<AClueless_PlayerState>();

	int CPlayerBlockID = UGameplayAPI::GetBlockIDFromRoleID(CPlayerState->GetRoleID(), GetWorld());

	if (CPlayerBlockID == -1)
	{
		print("[Server: CluelessGameLogic] Player Location Not Valid", FColor::Red);
		return;
	}

	FStaticMovementBlock MovementStaticInfo = UGameplayAPI::GetBlockInfo(CPlayerBlockID, GetWorld());

	if (!MovementStaticInfo.IsHallWay && (!MovementStaticInfo.IsSpawnPoint))
	{
		print("[Server: CluelessGameLogic] Player Location Valid, Next Move Suspect into This Block", FColor::Green);
	}
	else
	{
		print("[Server: CluelessGameLogic] Player Location Not Valid", FColor::Red);
		return;
	}

	FCardEntityData CardStaticInfo = UGameplayAPI::GetCardStaticData(CRoleID, GetWorld());

	AClueless_PlayerState* SuspectPlayerState = (AClueless_PlayerState*)UGameplayAPI::FindPlayerFromCharacterID(CardStaticInfo.RelationID, GetWorld());
	
	if (SuspectPlayerState == nullptr)
	{
		print("[Server: CluelessGameLogic] Suspect Can't be Found", FColor::Red);
		return;
	}
	else
	{
		print("[Server: CluelessGameLogic] Suspect Found Move player to Location", FColor::Green);


	}



	// Making suggestion move suggested character into this room.

	// TODO: Make current role clear suggestion action

	// In Order to make show card logic.
	// 1. we need a replication variable called ShowCardPlayerTurnIndex(This will continue go up until somebody shows a suggested card or  our ShowCardPlayerIndex becomes the same player turn index again)
	// 2. As each turn changes, other players will need to select a card to show, or no cards then skip suggestion // need to add those two to game actions...

}

