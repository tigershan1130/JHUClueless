// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "CluelessJHU/Player/Clueless_PlayerState.h"
#include "CluelessJHU/Actors/ClueCharacter.h"
#include "CluelessJHU/Utilities/GameplayAPI.h"
#include "StaticDataTableManager/Public/StaticDataSubSystem.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>



ACGameStateBase::ACGameStateBase()
{

}

void ACGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACGameStateBase, HostReadyToStartGame, COND_OwnerOnly);
	DOREPLIFETIME(ACGameStateBase, PlayerRelationMapping);
	DOREPLIFETIME(ACGameStateBase, CGameState);
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

void ACGameStateBase::ReigsterPlayerControllerOnServer(APlayerController* PlayerController)
{
	if (!ServerMapPlayerCharacters.Contains(PlayerController))
		ServerMapPlayerCharacters.Add(PlayerController, nullptr);



}

void ACGameStateBase::UnReigsterPlayerControllerOnServer(APlayerController* PlayerController)
{
	if (ServerMapPlayerCharacters.Contains(PlayerController))
		ServerMapPlayerCharacters.Remove(PlayerController);
}

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
	TArray<ACharacter*> ActiveCharacters = GetActivePlayerCharacters();

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
	for (auto& Entry : PlayerRelationMapping.PlayerRelationMapping)
	{
		AClueCharacter* ClueCharacter = (AClueCharacter*)Entry.Character;

		if (ClueCharacter != nullptr)
		{
			if (ClueCharacter->IsLocallyControlled())			
				ClueCharacter->OnGameStateChanged();
			
		}
	}
}

/*
* Get current characters roles that's already been possessed.
*/
TArray<ACharacter*> ACGameStateBase::GetActivePlayerCharacters()
{
	TArray<ACharacter*> ActiveCharacters;

	for (auto& Entry : ServerMapPlayerCharacters)
	{
		if (Entry.Value != nullptr)
			ActiveCharacters.Add(Entry.Value);

	}

	return ActiveCharacters;
}
