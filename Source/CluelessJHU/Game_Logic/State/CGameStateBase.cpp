// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "CluelessJHU/Player_Logic/State/Clueless_PlayerState.h"
#include "CluelessJHU/Player_Logic/Controller/ClueCharacter.h"
#include "CluelessJHU/Game_Logic/Controller/ClueGameTurnBasedComponent.h"
#include "CluelessJHU/Utilities/GameplayAPI.h"
#include "StaticDataTableManager/Public/StaticDataSubSystem.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>



ACGameStateBase::ACGameStateBase()
{

}

// what's all the data we need to sync.
void ACGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACGameStateBase, HostReadyToStartGame, COND_OwnerOnly);
	DOREPLIFETIME(ACGameStateBase, PlayerRelationMapping);
	DOREPLIFETIME(ACGameStateBase, CGameState);
	DOREPLIFETIME(ACGameStateBase, LeftoverDeck);
	DOREPLIFETIME(ACGameStateBase, DistributedCardsPlayers);
	DOREPLIFETIME(ACGameStateBase, PlayerTurnIndex);
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
		if (GetNetMode() == NM_ListenServer)
		{
			HostReadyToStartGame = true;
			OnRep_GameStartedChanged();
		}
	}
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

			if (ClueCharacter != nullptr)
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
void ACGameStateBase::OnRep_TurnChanged()
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
				if (CGameState == ClueGameState::Gaming && CPlayerRelationMapping[i].Index == PlayerTurnIndex)
				{
					ClueCharacter->OnThisCharacterTurn();

					//UE_LOG(LogTemp, Error, TEXT("Local Name Same Turn: %s"), *(PlayerStaticSetupData[CPlayerRelationMapping[i].Index].CharacterName.ToString()));
				}
				else if (CGameState == ClueGameState::Gaming && CPlayerRelationMapping[i].Index != PlayerTurnIndex)
				{
					//UE_LOG(LogTemp, Error, TEXT("Other Name: %s"), *(PlayerStaticSetupData[CPlayerRelationMapping[i].Index].CharacterName.ToString()));
					ClueCharacter->OnOtherCharacterTurn();
				}
			}
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

	PlayerTurnIndex = PlayerTurnIndex + 1;

	// clamp
	if (PlayerTurnIndex >= TotalActivePlayers)
		PlayerTurnIndex = 0;

	// listen server requires special networking notifications for Replication
	if (GetNetMode() == NM_ListenServer)
		OnRep_TurnChanged();

}