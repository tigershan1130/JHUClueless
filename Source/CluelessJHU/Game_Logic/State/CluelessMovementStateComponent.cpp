// Fill out your copyright notice in the Description page of Project Settings.


#include "CluelessMovementStateComponent.h"
#include "Net/UnrealNetwork.h"
#include "StaticDataTableManager/Public/StaticDataSubSystem.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Sets default values for this component's properties
UCluelessMovementStateComponent::UCluelessMovementStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCluelessMovementStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// pre-load our data
	GetDynamicMovmentCache();
}

void UCluelessMovementStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCluelessMovementStateComponent, DynamicMovementInfo);
}

TArray<int> UCluelessMovementStateComponent::GetMovableBlocks(int CurrentBlockIDs)
{
	return TArray<int>();
}

// Get CluelessMovement Data
TArray<FDynamicMovementEntry> UCluelessMovementStateComponent::GetDynamicMovmentCache()
{
	if (DynamicMovementInfo.DynamicMovementInfoCache.Num() <= 0)
	{
		if (GIsServer) // only server can populate this data.
		{
			UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());

			UStaticDataSubSystem* StaticDataGameSystem = GameInstance->GetSubsystem<UStaticDataSubSystem>();

			if (StaticDataGameSystem != nullptr)
			{
				StaticDataGameSystem->InitStaticData();

				UDataTable* MovementSetupStaticData = UStaticDataSubSystem::GetDataTableByName(TEXT("MovementSetup"));


				FString _Context;

				for (auto& RowName : MovementSetupStaticData->GetRowNames())
				{
					FStaticMovementBlock* RowData = MovementSetupStaticData->FindRow<FStaticMovementBlock>(RowName, _Context);

					// TODO: populate our cache.
					FDynamicMovementEntry DynamicMovEntry;
					DynamicMovEntry.BlockID = RowData->BlockID;
					DynamicMovEntry.BlockInfo = *RowData;
					DynamicMovEntry.OccupiedRoles.Empty();

					DynamicMovementInfo.DynamicMovementInfoCache.Add(DynamicMovEntry);
				}

				DynamicMovementInfo.MarkArrayDirty();
			}
		}
	}

	return DynamicMovementInfo.DynamicMovementInfoCache;
}

void UCluelessMovementStateComponent::ServerUpdateOccupied(int BlockID, int PreviousBlockID, int RoleID)
{
	bool Changed = false;
	for (auto& Entry : DynamicMovementInfo.DynamicMovementInfoCache)
	{
		if (Entry.BlockID == BlockID)
		{
			if(!Entry.OccupiedRoles.Contains(RoleID))
				Entry.OccupiedRoles.Add(RoleID);

			Changed = true;
		}

		if (Entry.BlockID == RoleID)
		{
			if (Entry.OccupiedRoles.Contains(RoleID))
				Entry.OccupiedRoles.Remove(RoleID);

			Changed = true;
		}

	}
	
	if (Changed)
	{
		// Listen Server patch.
		if (GetNetMode() == ENetMode::NM_ListenServer)
		{
			OnRep_DynamicMovementInfoChanged();
		}

		DynamicMovementInfo.MarkArrayDirty();
	}
}


void UCluelessMovementStateComponent::OnRep_DynamicMovementInfoChanged()
{
	
}

