// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CluelessJHU/CActor.h"
#include "BlockPointActor.generated.h"

UCLASS()
class CLUELESSJHU_API ABlockPointActor : public ACActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlockPointActor();

	UPROPERTY(EditAnywhere, Category = "Parameters")
		int BlockID;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
