// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "UObject/NameTypes.h"
#include "GameFramework/GameStateBase.h"
#include "CluelessJHU/Data/Game_StaticData.h"
#include <Runtime/JsonUtilities/Public/JsonObjectConverter.h>
#include "CluelessMovementStateComponent.generated.h"


/*
* This is the main class to handle all the data change for the Clueless Movement Logic.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLUELESSJHU_API UCluelessMovementStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCluelessMovementStateComponent();

	// for replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Get Movable Blocks
	UFUNCTION()
	TArray<int> GetMovableBlocks(int CurrentBlockIDs);

	UFUNCTION()
		TArray<FDynamicMovementEntry> GetDynamicMovmentCache();

	UFUNCTION()
		void ServerUpdateOccupied(int BlockID, int RoleID);

	UFUNCTION()
		TArray<FStaticMovementBlock> GetStaticMovementCache();

	UFUNCTION()
		FDynamicMovementEntry GetMovementBlock(int BlockID)
	{
		TArray<FDynamicMovementEntry> DynamicMovementEntry = GetDynamicMovmentCache();
		FDynamicMovementEntry FoundEntry;
		FoundEntry.BlockID = -1;

		// Find found Entry.
		for (auto& Entry : DynamicMovementEntry)
		{
			if (Entry.BlockID == BlockID)
				FoundEntry = Entry;
		}

		return FoundEntry;
	}

	UFUNCTION()		
		FDynamicMovementInfo GetCurrentDynamicInfo()
	{
		FDynamicMovementInfo DynamicMovementInfo;
		FJsonObjectConverter::JsonObjectStringToUStruct(DynamicMovementInfoJson, &DynamicMovementInfo, 0, 0);

		return DynamicMovementInfo;

	}

	UFUNCTION()
		void SetDynamicInfo_Server(FDynamicMovementInfo ModifiedInfoData)
	{
		FString JSONPayload;
		FJsonObjectConverter::UStructToJsonObjectString(ModifiedInfoData, JSONPayload, 0, 0);

		DynamicMovementInfoJson = JSONPayload;
	}

	UFUNCTION()
		int GetBlockIDFromRoleID(int RoleID)
	{
		FDynamicMovementInfo DynamicMovementInfo = GetCurrentDynamicInfo();

		TArray<FDynamicMovementEntry> MovementBlocks = DynamicMovementInfo.DynamicMovementInfoCache;

		for (auto& Entry : MovementBlocks)
		{
			if (Entry.OccupiedRoles.Contains(RoleID))
			{
				return Entry.BlockID;
			}
		}

		return -1;
	}

#pragma region Client recieves state change from Server
	// On Dynamic Movement Info changed.
	UFUNCTION()
	void OnRep_DynamicMovementInfoChanged();

#pragma endregion Client recieves state change from Server

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	UPROPERTY(ReplicatedUsing = OnRep_DynamicMovementInfoChanged)
	FString DynamicMovementInfoJson;

	//UPROPERTY(ReplicatedUsing = OnRep_Test)
	//	int Increment;

	UPROPERTY()
		TArray<FStaticMovementBlock> StaticMovementInfocache;

};
