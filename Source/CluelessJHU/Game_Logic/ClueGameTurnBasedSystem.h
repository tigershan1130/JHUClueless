// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClueGameTurnBasedSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLUELESSJHU_API UClueGameTurnBasedSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClueGameTurnBasedSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

//public:	
//	// Called every frame
//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	
	// This function will start the clue game
	// 1. it will spawn all the corresponding players (Game logic-> Server->Client-> Rendering System)
	// 2. Initialize cards in the deck?  (Game Logic Self update between client and server)
	UFUNCTION()
	void OnGameInit();



};
