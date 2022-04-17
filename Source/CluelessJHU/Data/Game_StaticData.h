// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "UObject/NameTypes.h"
#include "Game_StaticData.generated.h"

UENUM()
enum ClueGameState
{
	PreGaming	UMETA(DisplayName = "Pre Gaming"),
	Gaming		UMETA(DisplayName = "Gaming"),
	PostGaming	UMETA(DisplayName = "Post Gaming"),
};

/**
 *
 */
UENUM(BlueprintType, meta = (Bitflags))
enum class EPlayerGameAction : uint8
{
	None = 0x00,
	Movement = 0x01,
	Accusation = 0x02,
	Suggestion = 0x04,
	EndTurn = 0x05,
};

ENUM_CLASS_FLAGS(EPlayerGameAction);



USTRUCT(BlueprintType, Blueprintable)
struct FPlayerSetupStaticData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName CharacterID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int InitialBlockID;
};

USTRUCT(BlueprintType, Blueprintable)
struct FPlayerRelationMappingEntry
{
	GENERATED_USTRUCT_BODY();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FPlayerSetupStaticData PlayerStaticData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsYou;

};


USTRUCT(BlueprintType, Blueprintable)
struct FPlayerSuggestedData
{
	GENERATED_USTRUCT_BODY();
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString SuspectID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString BlockID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString WeaponID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int SuggestionTurnCounter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool AllowShowCardOption;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsInShowCardPhase = false;
};

USTRUCT(BlueprintType, Blueprintable)
struct FPlayerTurnInfo
{
	GENERATED_USTRUCT_BODY();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int PlayerTurnIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 PlayerGamingAction;


	void SetAllowGameAction(EPlayerGameAction Action)
	{
		PlayerGamingAction |= 1 << static_cast<uint8>(Action);
	}

	bool IsActionAllowed(EPlayerGameAction Action)
	{
		return (PlayerGamingAction & (1 << static_cast<uint8>(Action))) > 0;
	}

	void ClearGameAction(EPlayerGameAction Action)
	{
		PlayerGamingAction &= ~(1 << static_cast<uint8>(Action));
	}

	void ClearAllGameAction()
	{
		PlayerGamingAction = 0;
	}

};



USTRUCT(BlueprintType,  Blueprintable)
struct FCardEntityData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

public:

	// Card ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ID;

	// 0 for character
	// 1 for item
	// 2 for Room
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Type;

	// so if 0 for character, we can then use RelationID to go to character table to find more data.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName RelationID;

	// card name if needed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText CardName;

	// this is same ID as the RowName
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString CardID;

	// card description if needed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Description;

	bool operator==(const FCardEntityData& lhs) const
	{
		return ((ID == lhs.ID) && (CardID == lhs.CardID));
	}
};

USTRUCT(BlueprintType)
struct FPlayerCharacterRelationEntry : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		int Index;

	UPROPERTY(BlueprintReadWrite)
		ACharacter* Character;

	void PreReplicatedRemove(const struct FPlayerRelationsContainer& ArraySerializer) {}
	void PostReplicatedAdd(const struct FPlayerRelationsContainer& ArraySerializer) {}
	void PostReplicatedChange(const struct FPlayerRelationsContainer& ArraySerializer) {}


	bool operator==(const FPlayerCharacterRelationEntry& lhs) const
	{
		return (Index == lhs.Index);
	}

};


USTRUCT()
struct FPlayerRelationsContainer : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		TArray<FPlayerCharacterRelationEntry> PlayerRelationMapping;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FPlayerCharacterRelationEntry>(PlayerRelationMapping, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FPlayerRelationsContainer> : public TStructOpsTypeTraitsBase2<FPlayerRelationsContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};


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
struct FDynamicMovementEntry
{
	GENERATED_USTRUCT_BODY()
public:
	//UPROPERTY()
	//	FStaticMovementBlock BlockInfo;

	UPROPERTY()
		TArray<int> OccupiedRoles;

	UPROPERTY()
		int BlockID;

	UPROPERTY()
		int IncrementCount;
};


USTRUCT(BlueprintType)
struct FDynamicMovementInfo
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY()
		TArray<FDynamicMovementEntry> DynamicMovementInfoCache;

};