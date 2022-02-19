// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "UObject/NameTypes.h"
#include "Game_StaticData.generated.h"

/**
 *
 */
USTRUCT(BlueprintType, Blueprintable)
struct FPlayerSetupStaticData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* texture;
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
