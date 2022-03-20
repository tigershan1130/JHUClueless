// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorBlock.generated.h"

UCLASS()
class BLOCKMOVEMENTSYSTEM_API AActorBlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorBlock();

	// -1 means we can have as many as we want
	// 1 means we can only occupy one player
	UPROPERTY(EditAnywhere, Category = "Block Setting")
	int MaxCharactersAllowed;

	// current players in the list
	UPROPERTY(BlueprintReadOnly)
	TArray<int> CurrentRoles;

	// Neigbhor Blocks
	UPROPERTY(EditAnywhere, Category = "Block Setting")
	TArray<AActorBlock*> NeighborBlocks;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
