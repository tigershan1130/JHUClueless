// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "CluelessJHU/Player/Clueless_PlayerState.h"
#include "StaticDataTableManager/Public/StaticDataSubSystem.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>



ACGameStateBase::ACGameStateBase()
{

}

void ACGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGameStateBase,IsReadyToStartGame);
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
		if(Character != nullptr)
			ServerMapPlayerCharacters[PlayerController] = Character;
	}
}

void ACGameStateBase::ReportPlayerClientReady(ACharacter* PlayerCharacter)
{
	if (!ReportedClientPlayers.Contains(PlayerCharacter))
		ReportedClientPlayers.Add(PlayerCharacter);

	if (ReportedClientPlayers.Num() >= 0)
	{
		int PossedNumbers = 0;
		APlayerController* FoundController = nullptr;
		// TODO: optimize, currently O(N), but our max players should be not greater than 8, 64 execution is ok.
		for (auto& PCharacter : ReportedClientPlayers)
		{
			for (auto& Elem : ServerMapPlayerCharacters)
			{
				if (Elem.Value == PCharacter)
				{
					if (PCharacter == PlayerCharacter)
						FoundController = Elem.Key;

					PossedNumbers++;
				}
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Connect Possed Players Numbers  %d"), PossedNumbers);
		if (FoundController != nullptr)
		{
			APlayerState* CurrentPlayerState = FoundController->GetPlayerState<APlayerState>();

			if (CurrentPlayerState != nullptr)
			{
				AClueless_PlayerState* CPlayerState = (AClueless_PlayerState*)(CurrentPlayerState);

				if (CPlayerState != nullptr)
					CPlayerState->ChangeGameState(1);
			}

		}

		// we are ready to start our game.
		if (PossedNumbers > 0)
		{
			IsReadyToStartGame = true;
		}
	}
}

void ACGameStateBase::OnRep_GameStartedChanged()
{


}

void ACGameStateBase::CheckGameIsReadyToStart()
{
	//if()
}
