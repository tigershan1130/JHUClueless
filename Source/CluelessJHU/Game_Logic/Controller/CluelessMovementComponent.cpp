// Fill out your copyright notice in the Description page of Project Settings.

#include "CluelessMovementComponent.h"
#include "CluelessJHU/Utilities/GameplayAPI.h"
#include "CluelessJHU/Game_Logic/State/CGameStateBase.h"

// Sets default values for this component's properties
UCluelessMovementComponent::UCluelessMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCluelessMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	ACGameStateBase* GameState = GetWorld()->GetGameState<ACGameStateBase>();
	

	if (GameState == nullptr)
		return;
	
	CluelessMovementStateCompCache = (UCluelessMovementStateComponent*)(GameState->GetComponentByClass(UCluelessMovementStateComponent::StaticClass()));

	if (CluelessMovementStateCompCache == nullptr)
		return;
	
	CluelessMovementStateCompCache->GetDynamicMovmentCache();

	TArray<FPlayerSetupStaticData> SetupDatas = UGameplayAPI::GetPlayerStaticSetupData(GetWorld());

	for (auto& Entry : SetupDatas)
	{
		int RoleID = Entry.ID - 1;

		CluelessMovementStateCompCache->ServerUpdateOccupied(Entry.InitialBlockID, RoleID);

		UE_LOG(LogTemp, Error, TEXT("Setting up Block ID %d for Role %d"), Entry.InitialBlockID, RoleID);
	}
}


void UCluelessMovementComponent::RoleMakeTeleport(int BlockID, int CurrentRoleID)
{
	if (CluelessMovementStateCompCache == nullptr)
	{
		ACGameStateBase* GameState = GetWorld()->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return;

		CluelessMovementStateCompCache = (UCluelessMovementStateComponent*)(GameState->GetComponentByClass(UCluelessMovementStateComponent::StaticClass()));
	}

	if (CluelessMovementStateCompCache == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Movement State Component"));
	}

	CluelessMovementStateCompCache->ServerUpdateOccupied(BlockID, CurrentRoleID);
}


void UCluelessMovementComponent::OnPlayerMakeMovement(int BlockID, int CurrentRoleID)
{	
	if (CluelessMovementStateCompCache == nullptr)
	{
		ACGameStateBase* GameState = GetWorld()->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return;

		CluelessMovementStateCompCache = (UCluelessMovementStateComponent*)(GameState->GetComponentByClass(UCluelessMovementStateComponent::StaticClass()));
	}

	if (CluelessMovementStateCompCache == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Movement State Component"));
	}

	// Get our current dynamic data.
	FDynamicMovementEntry FoundMoveToBlock = CluelessMovementStateCompCache->GetMovementBlock(BlockID);
	
	// Get current Role's Location.
	ACharacter* CurrentCharacter = UGameplayAPI::GetCharacterFromRoleID(GetWorld(), CurrentRoleID);

	if (CurrentCharacter == nullptr)	
		return;
	
	AClueless_PlayerState* CurrentPlayerState = (AClueless_PlayerState*)CurrentCharacter->GetPlayerState();

	if (CurrentPlayerState == nullptr)
		return;

	int CurrentLocationID = UGameplayAPI::GetBlockIDFromRoleID(CurrentRoleID, GetWorld());

	//UE_LOG(LogTemp, Warning, TEXT("Player %d is Making a move from Block %d to Block %d"), CurrentRoleID, CurrentLocationID, BlockID);
	FDynamicMovementEntry FromBlock = CluelessMovementStateCompCache->GetMovementBlock(CurrentLocationID);


	FString msg = "[Server: CluelessGameLogic]Player " + FString::FromInt(CurrentRoleID) + " Making Move From Block[" + FString::FromInt(CurrentLocationID) + "]["+ FString::FromInt(FromBlock.BlockID)+ "] To Block[" + FString::FromInt(BlockID) + "]";
	print(msg, FColor::Green);

	FStaticMovementBlock FromBlockInfo = UGameplayAPI::GetBlockInfo(FromBlock.BlockID, GetWorld());

	// if contains let's make a move
	if (FromBlockInfo.NeighborBlocks.Contains(BlockID))
	{
		if (FoundMoveToBlock.OccupiedRoles.Num() > 0 && FromBlockInfo.IsHallWay == true)
		{
			print("[Server: CluelessGameLogic] Player Movement Validated[False] Hall way Full", FColor::Red);
		}
		else
		{	
			CluelessMovementStateCompCache->ServerUpdateOccupied(BlockID, CurrentRoleID);
		}
	}
	else
	{
		print("[Server: CluelessGameLogic] Player Movement Validated[False] Is not Neighbor", FColor::Red);
	}


}


