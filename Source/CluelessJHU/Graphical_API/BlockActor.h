// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockActor.generated.h"

UCLASS()
class CLUELESSJHU_API ABlockActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlockActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Block Actor")
		void TurnOnVisualBlock();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Block Actor")
		void TurnOffVisualBlock();

	UPROPERTY(EditAnywhere, Category = "Block Actor")
		int BlockID;
};
