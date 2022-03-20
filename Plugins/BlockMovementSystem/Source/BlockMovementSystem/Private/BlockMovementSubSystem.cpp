// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockMovementSubSystem.h"


void UBlockMovementSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Warning, TEXT("Block Movement SubSystem Initialized"));

}


void UBlockMovementSubSystem::Deinitialize()
{

}

void UBlockMovementSubSystem::RegisterActorBlock(AActorBlock* Block)
{
	if (!ActorBlockList.Contains(Block))
	{
		ActorBlockList.Add(Block);
	}

}
