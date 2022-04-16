// Fill out your copyright notice in the Description page of Project Settings.


#include "CluelessMovementStateComponent.h"
#include "Net/UnrealNetwork.h"
#include "StaticDataTableManager/Public/StaticDataSubSystem.h"
#include "CluelessJHU/Graphical_API/BlockPointActor.h"
#include "CluelessJHU/Player_Logic/Controller/CPawn.h"
#include "CluelessJHU/Utilities/GameplayAPI.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

#define print(text, color) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, color,text)

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


	SetIsReplicated(true);

	// pre-load our data
	GetDynamicMovmentCache();
}

void UCluelessMovementStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME(UCluelessMovementStateComponent, DynamicMovementInfoJson);
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
	FDynamicMovementInfo DynamicMovementInfo = GetCurrentDynamicInfo();

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
					DynamicMovEntry.OccupiedRoles.Empty();

					DynamicMovementInfo.DynamicMovementInfoCache.Add(DynamicMovEntry);
				}

				//DynamicMovementInfo.MarkArrayDirty();
				// Reset json for notification
				SetDynamicInfo_Server(DynamicMovementInfo);
			}
		}
	}

	return DynamicMovementInfo.DynamicMovementInfoCache;
}

void UCluelessMovementStateComponent::ServerUpdateOccupied(int BlockID, int RoleID)
{
	bool Changed = false;

	FDynamicMovementInfo DynamicMovementInfo = GetCurrentDynamicInfo();


	for (int i = 0; i < DynamicMovementInfo.DynamicMovementInfoCache.Num(); i++)
	{
		if (DynamicMovementInfo.DynamicMovementInfoCache[i].BlockID == BlockID)
		{
			if (!DynamicMovementInfo.DynamicMovementInfoCache[i].OccupiedRoles.Contains(RoleID))
			{
				DynamicMovementInfo.DynamicMovementInfoCache[i].OccupiedRoles.Add(RoleID);

				Changed = true;
				DynamicMovementInfo.DynamicMovementInfoCache[i].IncrementCount++;
			}
		}
		else
		{
			if (DynamicMovementInfo.DynamicMovementInfoCache[i].OccupiedRoles.Contains(RoleID))
			{
				DynamicMovementInfo.DynamicMovementInfoCache[i].OccupiedRoles.Remove(RoleID);
				Changed = true;
				DynamicMovementInfo.DynamicMovementInfoCache[i].IncrementCount++;
			}
		}
	}
	
	if (Changed)
	{
		SetDynamicInfo_Server(DynamicMovementInfo);

		// Listen Server patch.
		if (GetNetMode() == ENetMode::NM_ListenServer)
		{
			OnRep_DynamicMovementInfoChanged();
		}
	
	}
}

void UCluelessMovementStateComponent::OnRep_DynamicMovementInfoChanged()
{
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

	FDynamicMovementInfo DynamicMovementInfo = GetCurrentDynamicInfo();

	for (auto& Entry : DynamicMovementInfo.DynamicMovementInfoCache)
	{
		for (int i = 0; i < Entry.OccupiedRoles.Num(); i++)
		{
			int OccupiedRoleID = Entry.OccupiedRoles[i];

			if (VisualPawns.Contains(OccupiedRoleID) && ClientBlockPoints.Contains(Entry.BlockID))
			{
				FVector BlockLocation = ClientBlockPoints[Entry.BlockID]->GetActorLocation();

			    FStaticMovementBlock MovementStaticBlockInfo = UGameplayAPI::GetBlockInfo(Entry.BlockID, GetWorld());

				if (!MovementStaticBlockInfo.IsHallWay)
				{
					float Range = FMath::RandRange(-200, 200);

					BlockLocation = FVector(BlockLocation.X + Range, BlockLocation.Y + Range, BlockLocation.Z);
				}


				VisualPawns[OccupiedRoleID]->SetActorLocation(BlockLocation);
			}
		}
	}
}

