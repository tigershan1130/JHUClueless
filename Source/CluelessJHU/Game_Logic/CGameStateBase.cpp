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

	DOREPLIFETIME(ACGameStateBase, IsReadyToStartGame);
	DOREPLIFETIME(ACGameStateBase, PlayerRelationMapping);
}

void ACGameStateBase::PreInitializeComponents()
{

}

void ACGameStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UWorld* World = GetWorld();
	
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
}

/**
 * @brief 
*/
void ACGameStateBase::OnRep_GameStartedChanged()
{


}

/**
 * @brief 
*/
void ACGameStateBase::OnRep_PlayerCharacterMappingChanged()
{
	for (auto& Entry : PlayerRelationMapping.PlayerRelationMapping)
	{
		AClueCharacter* ClueCharacter = (AClueCharacter*)Entry.Character;

		if (ClueCharacter != nullptr)
		{
			if (ClueCharacter->IsLocallyControlled())
			{
				ClueCharacter->OnPlayerCharacterJoinBinded();

			}
		}
	}


}

void ACGameStateBase::CheckGameIsReadyToStart()
{
	//if()
}
