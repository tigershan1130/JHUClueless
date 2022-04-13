// Fill out your copyright notice in the Description page of Project Settings.


#include "CluelessMovementStateComponent.h"
#include "Net/UnrealNetwork.h"
#include "StaticDataTableManager/Public/StaticDataSubSystem.h"
#include "CluelessJHU/Graphical_API/BlockPointActor.h"
#include "CluelessJHU/Player_Logic/Controller/CPawn.h"
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

TArray<FStaticMovementBlock> UCluelessMovementStateComponent::GetStaticMovementCache()
{
	if (StaticMovementInfocache.Num() <= 0)
	{
		UDataTable* MovementSetupStaticData = UStaticDataSubSystem::GetDataTableByName(TEXT("MovementSetup"));

		FString _Context;

		for (auto& RowName : MovementSetupStaticData->GetRowNames())
		{
			FStaticMovementBlock* RowData = MovementSetupStaticData->FindRow<FStaticMovementBlock>(RowName, _Context);
			StaticMovementInfocache.Add(*RowData);
		}
	}

	return StaticMovementInfocache;
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

void UCluelessMovementStateComponent::ServerUpdateOccupied(int BlockID, int RoleID)
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
		else
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
	// TODO: we need to upgrade our graphical from here.
	if (GIsServer && GetNetMode() != ENetMode::NM_ListenServer)
		return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPawn::StaticClass(), FoundActors);

	TMap<int, ACPawn*> VisualPawns;
	TMap<int, ABlockPointActor*> ClientBlockPoints;

	for (int i = 0; i < FoundActors.Num(); i++)
	{
		ACPawn* CurrentCharacter = (ACPawn*)FoundActors[i];

		if(!VisualPawns.Contains(CurrentCharacter->RoleID))
			VisualPawns.Add(CurrentCharacter->RoleID,CurrentCharacter);
	}

	if (ClientBlockPoints.Num() <= 0)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlockPointActor::StaticClass(), FoundActors);

		for (int i = 0; i < FoundActors.Num(); i++)
		{
			ABlockPointActor* BlockPoint = (ABlockPointActor*)FoundActors[i];
			if (BlockPoint)
			{
				if(!ClientBlockPoints.Contains(BlockPoint->BlockID))
					ClientBlockPoints.Add(BlockPoint->BlockID, BlockPoint);
			}
		}
	}

	for (auto& Entry : DynamicMovementInfo.DynamicMovementInfoCache)
	{
		for (int i = 0; i < Entry.OccupiedRoles.Num(); i++)
		{
			int OccupiedRoleID = Entry.OccupiedRoles[i];

			if (VisualPawns.Contains(OccupiedRoleID) && ClientBlockPoints.Contains(Entry.BlockID))
			{
				FVector BlockLocation = ClientBlockPoints[Entry.BlockID]->GetActorLocation();
				VisualPawns[OccupiedRoleID]->SetActorLocation(BlockLocation);
			}
		}
	}
}
