// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "CluelessJHU/Player_Logic/State/Clueless_PlayerState.h"
#include "CluelessJHU/Player_Logic/Controller/ClueCharacter.h"
#include "CluelessJHU/Game_Logic/Controller/ClueGameTurnBasedComponent.h"
#include "CluelessJHU/Utilities/GameplayAPI.h"
#include "CluelessJHU/Data/Game_StaticData.h"
#include "StaticDataTableManager/Public/StaticDataSubSystem.h"
#include "Misc/MessageDialog.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>


#define print(text, color) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, color,text)


ACGameStateBase::ACGameStateBase()
{

}

// what's all the data we need to sync.
void ACGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGameStateBase, HostReadyToStartGame);
	DOREPLIFETIME(ACGameStateBase, PlayerRelationMapping);
	DOREPLIFETIME(ACGameStateBase, CGameState);
	DOREPLIFETIME(ACGameStateBase, LeftoverDeck);
	DOREPLIFETIME(ACGameStateBase, DistributedCardsPlayers);
	DOREPLIFETIME(ACGameStateBase, PlayerTurnCachedData);
	DOREPLIFETIME(ACGameStateBase, SuggestionCachedData);
}

void ACGameStateBase::PreInitializeComponents()
{

}

void ACGameStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UWorld* World = GetWorld();

	CGameState = ClueGameState::PreGaming;

}

void ACGameStateBase::BeginPlay()
{
	Super::BeginPlay();

}

void ACGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

/**
 * @brief 
 * @return 
*/
TArray<FPlayerSetupStaticData> ACGameStateBase::GetPlayerSetupStaticData()
{
	if (PlayerSetupStaticData.Num() == 0)
	{
		// 1.  Load Static Data
		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());

		UStaticDataSubSystem* StaticDataGameSystem = GameInstance->GetSubsystem<UStaticDataSubSystem>();

		StaticDataGameSystem->InitStaticData();

		UDataTable* CharacterSetupStaticData = UStaticDataSubSystem::GetDataTableByName(TEXT("CharacterSetup"));

		FString _Context;

		for (auto& RowName : CharacterSetupStaticData->GetRowNames())
		{
			FPlayerSetupStaticData* RowData = CharacterSetupStaticData->FindRow<FPlayerSetupStaticData>(RowName, _Context);
			PlayerSetupStaticData.Add(*RowData);
		}

	}

	return PlayerSetupStaticData;
}

void ACGameStateBase::OnMulticast_RPCNotifyGameWin_Implementation(const FString& RoleName, const FString& Information)
{
	TArray<AClueless_PlayerState*> ActivePlayerStates = UGameplayAPI::GetActivePlayerStates(GetWorld());

	for (auto& Entry : ActivePlayerStates)
	{
		APawn* CurrentPawn = Entry->GetCurrentControlledPawn();

		if (CurrentPawn->IsLocallyControlled())
		{
			AClueCharacter* CluelessCharacter = (AClueCharacter*)CurrentPawn;
			if (CluelessCharacter)
			{
				FString Msg = " Have Won The Game!";
				CluelessCharacter->OnPlayerWonGame(RoleName, Msg);
			}
		}
	}

}

/* This is been called from server to client, this is executed on server and client*/
void ACGameStateBase::OnMulticast_RPCNotifyShowedCard_Implementation(const FString& RoleName, const FString& CardID)
{
	// TODO: Show a notification box saying we blah blah blah
	//FString Message = "Player Showed Card: " + CardID;
	//print(Message, FColor::Yellow);

    FCardEntityData CardData = UGameplayAPI::GetCardFromCardID(CardID, GetWorld());

	//FString Msg = "Player Showed Card: " + CardData.CardName.ToString();

	TArray<AClueless_PlayerState*> ActivePlayerStates = UGameplayAPI::GetActivePlayerStates(GetWorld());

	for (auto& Entry : ActivePlayerStates)
	{
		APawn* CurrentPawn = Entry->GetCurrentControlledPawn();
		
		if (CurrentPawn->IsLocallyControlled())
		{
			AClueCharacter* CluelessCharacter = (AClueCharacter*)CurrentPawn;
			if (CluelessCharacter)
			{
				FString Msg = " Showed Card: " + CardData.CardName.ToString();
				CluelessCharacter->OnPlayerShowCard(RoleName, Msg);
			}
		}
	}
}

/**
 * @brief when player connects from game mode
 * @param PlayerController 
*/
void ACGameStateBase::ReigsterPlayerControllerOnServer(APlayerController* PlayerController)
{
	if (!ServerMapPlayerCharacters.Contains(PlayerController))
		ServerMapPlayerCharacters.Add(PlayerController, nullptr);
}


/**
 * @brief  when player disconnects from game mode
 * @param PlayerController 
*/
void ACGameStateBase::UnReigsterPlayerControllerOnServer(APlayerController* PlayerController)
{
	if (ServerMapPlayerCharacters.Contains(PlayerController))
		ServerMapPlayerCharacters.Remove(PlayerController);
}

/**
 * @brief When a player Controller posses a Character, both are passed in
 * @param PlayerController 
 * @param Character 
*/
void ACGameStateBase::UpdatePlayerControllerWithCharacterOnServer(APlayerController* PlayerController, ACharacter* Character)
{
	if (ServerMapPlayerCharacters.Contains(PlayerController))
	{
		if (Character != nullptr)
		{
			ServerMapPlayerCharacters[PlayerController] = Character;

			bool Found = false;

			for (auto& Entry : PlayerRelationMapping.PlayerRelationMapping)
			{
				Found = (Entry.Character == Character);
				if (Found)
					break;
			}

			if (!Found)
			{
				FPlayerCharacterRelationEntry CharacterRelationEntry;

				CharacterRelationEntry.Character = Character;
				CharacterRelationEntry.Index = PlayerRelationMapping.PlayerRelationMapping.Num();

				if (PlayerController->GetPlayerState<AClueless_PlayerState>())
				{
					PlayerController->GetPlayerState<AClueless_PlayerState>()->SetRoleID(CharacterRelationEntry.Index);
					PlayerController->GetPlayerState<AClueless_PlayerState>()->SetControlledPawn(Character);
				}

				PlayerRelationMapping.PlayerRelationMapping.Add(CharacterRelationEntry);

				PlayerRelationMapping.MarkArrayDirty();

				if (GetNetMode() == NM_ListenServer)
				{
					OnRep_PlayerCharacterMappingChanged();
				}
			}
		}
	}

	// get active characters
	TArray<ACharacter*> ActiveCharacters = GetActivePlayerCharacters_Server();

	// if current active characters is bigger than or equal to 3, we can start our game if I am the host
	if (ActiveCharacters.Num() >= 3)
	{
		HostReadyToStartGame = true;

		if (GetNetMode() == NM_ListenServer)
		{
			OnRep_GameStartedChanged();
		}
	}
}

// Refresh Turn Index.
// When refresh turn index, also set player's game action
// WHen Actions are set, we should toggle different GUI Options.(TODO)
void ACGameStateBase::CheckInitCurrentTurnActions()
{
	FPlayerSetupStaticData CurrentPlayerStaticData = UGameplayAPI::GetCurrentTurnCharacterInfo(PlayerTurnCachedData.PlayerTurnIndex, GetWorld());

	PlayerTurnCachedData.ClearAllGameAction();
	PlayerTurnCachedData.SetAllowGameAction(EPlayerGameAction::Accusation);
	PlayerTurnCachedData.SetAllowGameAction(EPlayerGameAction::Movement);
	PlayerTurnCachedData.SetAllowGameAction(EPlayerGameAction::EndTurn);

	AClueless_PlayerState* ActivePlayer = UGameplayAPI::GetPlayerStateFromTurnIndex(PlayerTurnCachedData.PlayerTurnIndex, GetWorld());

	if (ActivePlayer == nullptr)	
		return;
	
	int BlockID = UGameplayAPI::GetBlockIDFromRoleID(ActivePlayer->GetRoleID(), GetWorld());
	FStaticMovementBlock StaticMovementBlockInfo = UGameplayAPI::GetBlockInfo(BlockID, GetWorld());

	if (!StaticMovementBlockInfo.IsHallWay && (!StaticMovementBlockInfo.IsSpawnPoint))
	{
		PlayerTurnCachedData.SetAllowGameAction(EPlayerGameAction::Suggestion);
	}	

	if (GetNetMode() == ENetMode::NM_ListenServer)
		OnRep_TurnInfoChanged();
}

// check action after movement
void ACGameStateBase::CheckActionAfterMovement()
{
	AClueless_PlayerState* ActivePlayer = UGameplayAPI::GetPlayerStateFromTurnIndex(PlayerTurnCachedData.PlayerTurnIndex, GetWorld());

	if (ActivePlayer == nullptr)
		return;

	int BlockID = UGameplayAPI::GetBlockIDFromRoleID(ActivePlayer->GetRoleID(), GetWorld());
	FStaticMovementBlock StaticMovementBlockInfo = UGameplayAPI::GetBlockInfo(BlockID, GetWorld());

	if (!StaticMovementBlockInfo.IsHallWay && (!StaticMovementBlockInfo.IsSpawnPoint))
	{
		PlayerTurnCachedData.SetAllowGameAction(EPlayerGameAction::Suggestion);
	}

	if (GetNetMode() == ENetMode::NM_ListenServer)
		OnRep_TurnInfoChanged();
}

// get show card Turn index
int ACGameStateBase::GetShowCardTurnIndex(int ShowCardCounter)
{
	int TotalActivePlayers = UGameplayAPI::GetActivePlayerStates(GetWorld()).Num();

	if (TotalActivePlayers <= 0)
		return -1 ;
	
	int AccumulatedIndex = PlayerTurnCachedData.PlayerTurnIndex + ShowCardCounter;

	
	if (AccumulatedIndex >= TotalActivePlayers)
		return AccumulatedIndex % TotalActivePlayers;

	return AccumulatedIndex;
}

/**
 * @brief Ok, 3 Players are inside game, let's start game.
*/
void ACGameStateBase::OnRep_GameStartedChanged()
{
	// If Host Ready to Start game
	if (HostReadyToStartGame)
	{
		// The only logic can reach here is because I am the host...
		// let's enable our start game button
		for (auto& Entry : PlayerRelationMapping.PlayerRelationMapping)
		{
			AClueCharacter* ClueCharacter = (AClueCharacter*)Entry.Character;
			
			if (ClueCharacter != nullptr && Entry.Index == 0)
			{
				if (ClueCharacter->IsLocallyControlled())
					ClueCharacter->OnHostReadyStartGame();
			}
		}
	}
}

/*
* @brief When Game cards changed.
*/
void ACGameStateBase::OnRep_OnExtraCardsChanged()
{
	for (auto& Entry : PlayerRelationMapping.PlayerRelationMapping)
	{
		AClueCharacter* ClueCharacter = (AClueCharacter*)Entry.Character;

		if (ClueCharacter != nullptr)
		{
			if (ClueCharacter->IsLocallyControlled())
				ClueCharacter->OnPlayerCharacterJoinBinded();
		}
	}
}

/**
 * @brief This mapping is updated every time relation is changed or player joined.
 * when refreshing character and role relation
*/
void ACGameStateBase::OnRep_PlayerCharacterMappingChanged()
{
	for (auto& Entry : PlayerRelationMapping.PlayerRelationMapping)
	{
		AClueCharacter* ClueCharacter = (AClueCharacter*)Entry.Character;

		if (ClueCharacter != nullptr)
		{
			if (ClueCharacter->IsLocallyControlled())			
				ClueCharacter->OnPlayerCharacterJoinBinded();			
		}
	}


}

/**
 * @brief When game state is changed
*/
void ACGameStateBase::OnRep_GameStateChanged()
{
	int index = 0;

	TArray<FPlayerSetupStaticData> PlayerStaticSetupData = UGameplayAPI::GetPlayerStaticSetupData(GetWorld());

	// game state has changed, we notify all the clue character players
	for (auto& Entry : PlayerRelationMapping.PlayerRelationMapping)
	{
		AClueCharacter* ClueCharacter = (AClueCharacter*)Entry.Character;

		if (ClueCharacter != nullptr)
		{
			if (ClueCharacter->IsLocallyControlled())
			{
				ClueCharacter->OnGameStateChanged(CGameState);

				if (CGameState == ClueGameState::Gaming && Entry.Index == 0)
				{
					//UE_LOG(LogTemp, Error, TEXT("Changing UI Character: %s"), *(PlayerStaticSetupData[Entry.Index].CharacterName.ToString()));
					ClueCharacter->OnThisCharacterTurn();
				}
				else if (CGameState == ClueGameState::Gaming && Entry.Index != 0)
				{
					ClueCharacter->OnOtherCharacterTurn();
				}
			}			
		}
		index++;
	}
}


/**
 * @brief When somebody ended her turn to next player
*/
void ACGameStateBase::OnRep_TurnInfoChanged()
{
	// when a Player's turn is changed
	// We need to find the correct player to notify its
	// PlayerTurnIndex;
	TArray<FPlayerCharacterRelationEntry> CPlayerRelationMapping = GetCharatersRelationMapping();

	TArray<FPlayerSetupStaticData> PlayerStaticSetupData = UGameplayAPI::GetPlayerStaticSetupData(GetWorld());

	// game state has changed, we notify all the clue character players
	for (int i = 0; i < CPlayerRelationMapping.Num(); i++)
	{
		AClueCharacter* ClueCharacter = (AClueCharacter*)CPlayerRelationMapping[i].Character;

		if (ClueCharacter != nullptr)
		{
			if (ClueCharacter->IsLocallyControlled())
			{
				if (CGameState == ClueGameState::Gaming && CPlayerRelationMapping[i].Index == PlayerTurnCachedData.PlayerTurnIndex)
				{
					ClueCharacter->OnThisCharacterTurn();

					//UE_LOG(LogTemp, Error, TEXT("Local Name Same Turn: %s"), *(PlayerStaticSetupData[CPlayerRelationMapping[i].Index].CharacterName.ToString()));
				}
				else if (CGameState == ClueGameState::Gaming && CPlayerRelationMapping[i].Index != PlayerTurnCachedData.PlayerTurnIndex)
				{
					//UE_LOG(LogTemp, Error, TEXT("Other Name: %s"), *(PlayerStaticSetupData[CPlayerRelationMapping[i].Index].CharacterName.ToString()));
					ClueCharacter->OnOtherCharacterTurn();
				}
			}
		}
	}

}

// Ok our Suggestion Turn Changed.
void ACGameStateBase::OnRep_ShowCardTurnChanged()
{
	int ActualPlayerShowCardTurn = GetShowCardTurnIndex(SuggestionCachedData.SuggestionTurnCounter);

	AClueless_PlayerState* CPlayerState = UGameplayAPI::GetPlayerStateFromTurnIndex(ActualPlayerShowCardTurn, GetWorld());

	if (CPlayerState == nullptr)
		return;

	TArray<AClueless_PlayerState*> AllActivePlayers = UGameplayAPI::GetActivePlayerStates(GetWorld());

	if (SuggestionCachedData.IsInShowCardPhase == false)
	{
		// refresh turn ui
		//print("[Game Logic Client] Show Card Turn Is Done", FColor::Green);

		OnRep_TurnInfoChanged();
		return;
	}

	for (auto& Entry : AllActivePlayers)
	{
		AClueCharacter* ClueCharacter = (AClueCharacter*)Entry->GetCurrentControlledPawn();
	
		if (ClueCharacter->IsLocallyControlled())
		{
			if (Entry == CPlayerState)			
				ClueCharacter->OnSelfToProveSuggestion();			
			else			
				ClueCharacter->OnOtherToProveSuggestion();			
		}
	}
}

/*
* Get current characters roles that's already been possessed.
*/
TArray<ACharacter*> ACGameStateBase::GetActivePlayerCharacters_Server()
{
	TArray<ACharacter*> ActiveCharacters;

	for (auto& Entry : ServerMapPlayerCharacters)
	{
		if (Entry.Value != nullptr)
			ActiveCharacters.Add(Entry.Value);

	}

	return ActiveCharacters;
}


TArray<APlayerController*> ACGameStateBase::GetActiveController_Server()
{

	TArray<APlayerController*> ActiveControllers;

	for (auto& Entry : ServerMapPlayerCharacters)
	{
		if (Entry.Value != nullptr)
			ActiveControllers.Add(Entry.Key);

	}

	return ActiveControllers;
}

// Get Cards Inited 21 cards total 6 + 6 + 9
TArray<FCardEntityData> ACGameStateBase::GetCardsSetupData()
{
	if (ClueCardsSetupData.Num() <= 0)
	{
		// populate our data
		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());

		UStaticDataSubSystem* StaticDataGameSystem = GameInstance->GetSubsystem<UStaticDataSubSystem>();

		StaticDataGameSystem->InitStaticData();

		UDataTable* CardsSetupStaticData = UStaticDataSubSystem::GetDataTableByName(TEXT("CardsSetup"));

		FString _Context;

		for (auto& RowName : CardsSetupStaticData->GetRowNames())
		{
			FCardEntityData* RowData = CardsSetupStaticData->FindRow<FCardEntityData>(RowName, _Context);
			ClueCardsSetupData.Add(*RowData);
		}
	}

	return ClueCardsSetupData;
}


/**
 * @brief Change Current game state.
 * @param CurrentGameState 
*/
void ACGameStateBase::ChangeGameState(ClueGameState CurrentGameState)
{
	if (GIsServer || GetNetMode() == ENetMode::NM_ListenServer)
	{
		// if we are switching from pregaming state to start gaming start
		// THIS IS VERY IMPORTANT FUNCTION, it is invoked when client starts the game.
		if (CurrentGameState == ClueGameState::Gaming && CGameState == ClueGameState::PreGaming)
		{
			// TODO: init game on server
			AGameModeBase* GM = UGameplayStatics::GetGameMode(GetWorld());
			if (!GM) {
				UE_LOG(LogTemp, Warning, TEXT("GM from Client is not Valid"));
			}
			else
			{
				UClueGameTurnBasedComponent* TurnBasedGameModeComp = (UClueGameTurnBasedComponent*)GM->GetComponentByClass(UClueGameTurnBasedComponent::StaticClass());

				if (TurnBasedGameModeComp)
				{
					TurnBasedGameModeComp->OnGameInit();

					PlayerTurnCachedData.PlayerTurnIndex = 0;
					// Update what this turn can do
					CheckInitCurrentTurnActions();
				}
			}
		}
		
		CGameState = CurrentGameState;

		OnRep_GameStateChanged();
	}

}

// switch to next turn
void ACGameStateBase::ChangeToNextTurnIndex()
{
	int TotalActivePlayers = GetActivePlayerCharacters_Server().Num();

	if (TotalActivePlayers <= 0)
		return;

	PlayerTurnCachedData.PlayerTurnIndex = PlayerTurnCachedData.PlayerTurnIndex + 1;

	// clamp
	if (PlayerTurnCachedData.PlayerTurnIndex >= TotalActivePlayers)
		PlayerTurnCachedData.PlayerTurnIndex = 0;

	// when move to next players update what this turn can do.
	CheckInitCurrentTurnActions();

	// listen server requires special networking notifications for Replication
	if (GetNetMode() == NM_ListenServer)
		OnRep_TurnInfoChanged();
}