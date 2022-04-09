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
class CLUELESSJHU_API UCluelessMovementStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCluelessMovementStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:
	// Map of <BlockID, FDynamicMovementInfo>
	UPROPERTY()
		TMap<int, FDynamicMovementInfo> MovementInformation;
		
};
