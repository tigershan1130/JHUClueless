// Fill out your copyright notice in the Description page of Project Settings.
// This is server logic only
// This class handles player movement logics
// Tracks if certain blocks is occupied and so on.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CluelessMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLUELESSJHU_API UCluelessMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCluelessMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
