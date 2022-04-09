// Fill out your copyright notice in the Description page of Project Settings.
// This is server logic only
// This class handles player movement logics
// Tracks if certain blocks is occupied and so on.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "UObject/NameTypes.h"
#include "CluelessMovementComponent.generated.h"

#define print(text, color) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, color,text)


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
};


// This is Dynamic Movement Info cache that's been allocated in runtime 
USTRUCT()
struct CLUELESSJHU_API FDynamicMovementInfo
{
	GENERATED_USTRUCT_BODY();
public:

	UPROPERTY()
		FStaticMovementBlock BlockInfo; 

	UPROPERTY()
		TArray<int> OccupiedRoles;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLUELESSJHU_API UCluelessMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCluelessMovementComponent();

#pragma region Server RPC calls
	// this function will make player to make a movement
	UFUNCTION()
		void OnPlayerMakeMovement();



#pragma endregion Server RPC calls

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Map of <BlockID, FDynamicMovementInfo>
	UPROPERTY()
	TMap<int, FDynamicMovementInfo> MovementInformation;
		
};
