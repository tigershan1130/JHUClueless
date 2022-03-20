// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorBlock.h"
#include "BlockMovementSubSystem.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Sets default values
AActorBlock::AActorBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AActorBlock::BeginPlay()
{
	Super::BeginPlay();
	
	// regiter subystem
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UBlockMovementSubSystem* Clueless_BlockMovementSubsystem = GameInstance->GetSubsystem<UBlockMovementSubSystem>();

	if (Clueless_BlockMovementSubsystem != nullptr)
	{
		Clueless_BlockMovementSubsystem->RegisterActorBlock(this);
	}
}


