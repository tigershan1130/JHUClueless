// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveRoleActor.h"

// Sets default values
AMoveRoleActor::AMoveRoleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMoveRoleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMoveRoleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

