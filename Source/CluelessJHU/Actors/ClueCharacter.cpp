// Fill out your copyright notice in the Description page of Project Settings.


#include "ClueCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "CluelessJHU/Player/Clueless_PlayerState.h"
#include "CluelessJHU/Game_Logic/CGameStateBase.h"

// Sets default values
AClueCharacter::AClueCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// set replication to true
	bReplicates = true;

}

// Called when the game starts or when spawned
void AClueCharacter::BeginPlay()
{
	Super::BeginPlay();
}

//void AClueCharacter::ServerRPCSetGameStart()
//{
//
//
//}

void AClueCharacter::ClientCheckPlayerReady()
{
	if (IsLocallyControlled())
	{
		if (!ClientReady)
		{
			APlayerState* CPlayerState = GetPlayerState();

			AClueless_PlayerState* CluelessPlayerState = (AClueless_PlayerState*)CPlayerState;

			if (CluelessPlayerState != nullptr && CluelessPlayerState->GetCurrentGameState() > 0) // player state is already synced if it is not 0
			{
				OnPlayerCharacterInitalized();
				ClientReady = true;
			}
		}
	}
}

// Called every frame
void AClueCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ClientCheckPlayerReady();

}

// Called to bind functionality to input
void AClueCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

