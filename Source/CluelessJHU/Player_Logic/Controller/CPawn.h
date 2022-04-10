// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CluelessJHU/CActor.h"
#include "CPawn.generated.h"

UCLASS()
class CLUELESSJHU_API ACPawn : public ACActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Binding ID")
		int RoleID;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	

};
