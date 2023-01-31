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

void UClueGameTurnBasedComponent::OnPlayerShowCard(int RoleID, FString CardID)
{
	print("[Server: CluelessGameLogic] Player Show Card", FColor::Green);
	// validate if this cardID is validate to show(Fake Client(hacker)can fake this CardID)
	AClueless_PlayerState* CPlayerState = UGameplayAPI::GetPlayerStateFromRoleID(RoleID, GetWorld());

	if (CPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[Server: CluelessGameLogic] Error can't find PlayerState"));
		return;
	}

	ACGameStateBase* GameState = GetWorld()->GetGameState<ACGameStateBase>();

	if (GameState == nullptr)
		return;

    FCardEntityData CardEntityData = UGameplayAPI::GetCardFromCardID(CardID, GetWorld());

	if (!CPlayerState->GetCardsInHand().Contains(CardEntityData))
	{
		return;
	}

	FPlayerSetupStaticData PlayerStaticData = UGameplayAPI::GetCurrentRoleData(RoleID, GetWorld());

	// 1. Player showed a card to everyone, this needs to be displayed as a message
	GameState->OnMulticast_RPCNotifyShowedCard(PlayerStaticData.CharacterName.ToString(), CardID);

	// end Suggestion restore back to original user
	OnFinishedSuggestion();
}

void UClueGameTurnBasedComponent::OnPlayerSkipShowCard(int RoleID)
{
	print("[Server: CluelessGameLogic] Player Skip Show Card", FColor::Green);

	ACGameStateBase* GameState = GetWorld()->GetGameState<ACGameStateBase>();

	if (GameState == nullptr)
		return;

	FPlayerSuggestedData CurrentSuggestData = GameState->GetPlayerSuggestData();

	CurrentSuggestData.SuggestionTurnCounter++;

	int CurrentTurn = GameState->GetCurrentTurn();
	int TotalPlayersNum = UGameplayAPI::GetActivePlayerStates(GetWorld()).Num();

	if (CurrentTurn + CurrentSuggestData.SuggestionTurnCounter >= TotalPlayersNum)
	{
		int AdditionalCounter = CurrentTurn + CurrentSuggestData.SuggestionTurnCounter % TotalPlayersNum;

		if (AdditionalCounter == CurrentTurn) // everybody have been skipping their cards....
		{
			GameState->ResetSuggestionData();
		}
		else
		{
			CurrentSuggestData.AllowShowCardOption = CheckSuggestCards(CurrentSuggestData.BlockID, CurrentSuggestData.WeaponID, CurrentSuggestData.SuspectID, CurrentSuggestData.SuggestionTurnCounter);
			GameState->SetPlayerSuggestData(CurrentSuggestData);
		}
	}
	else
	{
		CurrentSuggestData.AllowShowCardOption = CheckSuggestCards(CurrentSuggestData.BlockID, CurrentSuggestData.WeaponID, CurrentSuggestData.SuspectID, CurrentSuggestData.SuggestionTurnCounter);
		GameState->SetPlayerSuggestData(CurrentSuggestData);
	}

}

void UClueGameTurnBasedComponent::OnFinishedSuggestion()
{
	ACGameStateBase* GameState = GetWorld()->GetGameState<ACGameStateBase>();

	if (GameState == nullptr)
		return;

	GameState->ResetSuggestionData();

}

// Called when the game starts
void UClueGameTurnBasedComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UClueGameTurnBasedComponent::OnGameInit()
{
	print("[Server: CluelessGameLogic] Initialized Game...", FColor::Green);
	
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
			int CurrentIndex = i * (PlayerStartingCardsNum) + j;
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

	AClueless_PlayerState* CurrentPlayerState = UGameplayAPI::GetPlayerStateFromRoleID(RoleID, GetWorld());

	if (CurrentPlayerState->GetIsAudience())
	{
		UE_LOG(LogTemp, Error, TEXT("[CluelessGameLogic] Bad Action, Current Player is Audience"));
		return;
	}

	FPlayerSetupStaticData PlayerStaticData = UGameplayAPI::GetCurrentRoleData(RoleID, GetWorld());

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

	if (CorrectCounter >= 3)
	{
		print("[Server: CluelessGameLogic] Player Made Correct Accusation", FColor::Green);

		print("[Server: CluelessGameLogic] Notifies all Clients, Game Won!", FColor::Green);

		GameState->OnMulticast_RPCNotifyGameWin(PlayerStaticData.CharacterName.ToString(), "Won");
	}
	else
	{
		print("[Server: CluelessGameLogic] Player Made False Accusation", FColor::Yellow);
		print("[Server: CluelessGameLogic] Marking Player As Audience(Move,EndTurns)", FColor::Yellow);

		GameState->OnMulticast_RPCNotifyMarkedAsAudience(PlayerStaticData.CharacterName.ToString(), "Marked As Audience");

		if (CurrentPlayerState)
		{
			CurrentPlayerState->MarkAsAudience();
		}
	}
	

	//make current role clear accusation action
	GameState->ClearCurrentPlayerGameAction(EPlayerGameAction::Accusation);
}

void UClueGameTurnBasedComponent::OnPlayerMakeSuggestion(int RoleID, FString CWeaponID, FString CRoleID)
{
	//Set first person to make suggestion
	ACGameStateBase* GameState = GetWorld()->GetGameState<ACGameStateBase>();

	if (GameState == nullptr)
		return;

	// Check if Current role is actually inside a room before he or she can make a suggestion
    ACharacter* SuggestionInstigator = UGameplayAPI::GetCharacterFromRoleID(GetWorld(), RoleID);

	AClueless_PlayerState* CPlayerState = SuggestionInstigator->GetPlayerState<AClueless_PlayerState>();

	if (CPlayerState->GetIsAudience())
		return;

	int CPlayerBlockID = UGameplayAPI::GetBlockIDFromRoleID(CPlayerState->GetRoleID(), GetWorld());

	if (CPlayerBlockID == -1)
	{
		print("[Server: CluelessGameLogic] Player Location Not Valid", FColor::Red);
		return;
	}

	//FString DebugText = "[Server: CluelessGameLogic] Player making suggestion: [" + CWeaponID + "," + CRoleID + "," + FString::FromInt(CPlayerBlockID) + "]";

	//print(DebugText, FColor::Green);

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

	int SuspectRoleID = UGameplayAPI::FindRoleIDFromCharacterID(CardStaticInfo.RelationID, GetWorld());
	
	if (SuspectRoleID == -1)
	{
		print("[Server: CluelessGameLogic] Suspect Can't be Found", FColor::Red);
		return;
	}

	// Making suggestion move suggested character into this room.
	//call RoleMakeTeleport to teleport token into our room
	UGameplayAPI::MakeTeleport_Server(GetWorld(), SuspectRoleID, MovementStaticInfo.BlockID);
	
	// In Order to make show card logic.
	// 1. we need a replication variable called ShowCardPlayerTurnIndex(This will continue go up until somebody shows a suggested card or  our ShowCardPlayerIndex becomes the same player turn index again)

	FPlayerSuggestedData PlayerSuggestData;

	PlayerSuggestData.BlockID =  UGameplayAPI::GetCardIDFromBlockID(CPlayerBlockID, GetWorld());
	PlayerSuggestData.SuspectID = CRoleID;
	PlayerSuggestData.WeaponID = CWeaponID;
	PlayerSuggestData.SuggestionTurnCounter = 1;

	PlayerSuggestData.AllowShowCardOption = CheckSuggestCards(PlayerSuggestData.BlockID, PlayerSuggestData.WeaponID, PlayerSuggestData.SuspectID, PlayerSuggestData.SuggestionTurnCounter);

	GameState->SetPlayerSuggestData(PlayerSuggestData);
	// 2. As each turn changes, other players will need to select a card to show, or no cards then skip suggestion // need to add those two to game actions...
	// All data needed is inside our data now, we will then send it to 

	GameState->ClearCurrentPlayerGameAction(EPlayerGameAction::Suggestion);
}

// private helper function
bool UClueGameTurnBasedComponent::CheckSuggestCards(FString BlockID, FString WeaponID, FString RoleID, int PlayerSuggestionCounter)
{
	ACGameStateBase* GameState = GetWorld()->GetGameState<ACGameStateBase>();

	if (GameState == nullptr)
		return false;


	int PlayerTurnIndex = GameState->GetShowCardTurnIndex(PlayerSuggestionCounter);

	AClueless_PlayerState* ShowCardPlayer = UGameplayAPI::GetPlayerStateFromTurnIndex(PlayerTurnIndex, GetWorld());

	if (ShowCardPlayer == nullptr)
		return false;
	
	TArray<FString> CardsToCheck;
	CardsToCheck.Add(BlockID);
	CardsToCheck.Add(RoleID);
	CardsToCheck.Add(WeaponID);

	bool ContainCards = ShowCardPlayer->ContainsCards(CardsToCheck);

	return ContainCards;
}
