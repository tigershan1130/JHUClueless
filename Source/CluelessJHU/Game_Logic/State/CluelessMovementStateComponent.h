// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "UObject/NameTypes.h"
#include "GameFramework/GameStateBase.h"
#include "CluelessMovementStateComponent.generated.h"

// This is static data pre-defined in UE's Datatable
USTRUCT(BlueprintType, Blueprintable)
struct CLUELESSJHU_API FStaticMovementBlock : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // block Id for internal referencing.
		int BlockID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // name of the current block for message info displaying
		FText BlockName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // This contains all connecting neighborBlocks they can travel to.
		TArray<int> NeighborBlocks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // because hall way can only occupied once.
		bool IsHallWay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // check if it is a spawn point, if it is, it can go back to this point
		bool IsSpawnPoint;
};


// This is Dynamic Movement Info cache that's been allocated in runtime
USTRUCT(BlueprintType)
struct FDynamicMovementEntry : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		FStaticMovementBlock BlockInfo;

	UPROPERTY(BlueprintReadWrite)
		TArray<int> OccupiedRoles;

	UPROPERTY(BlueprintReadWrite)
		int BlockID;

	void PreReplicatedRemove(const struct FDynamicMovementInfoContainer& ArraySerializer) {}
	void PostReplicatedAdd(const struct FDynamicMovementInfoContainer& ArraySerializer) {}
	void PostReplicatedChange(const struct FDynamicMovementInfoContainer& ArraySerializer) {}


	bool operator==(const FDynamicMovementEntry& lhs) const
	{
		return (BlockID == lhs.BlockID);
	}

};


USTRUCT()
struct FDynamicMovementInfoContainer : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		TArray<FDynamicMovementEntry> DynamicMovementInfoCache;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FDynamicMovementEntry>(DynamicMovementInfoCache, DeltaParms, *this);
	}
};


template<>
struct TStructOpsTypeTraits<FDynamicMovementInfoContainer> : public TStructOpsTypeTraitsBase2<FDynamicMovementInfoContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};


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
		void ServerUpdateOccupied(int BlockID, int PreviousBlockID, int RoleID);

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
	FDynamicMovementInfoContainer DynamicMovementInfo;

	UPROPERTY()
		TArray<FStaticMovementBlock> StaticMovementInfocache;

};
