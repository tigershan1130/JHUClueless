// Fill out your copyright notice in the Description page of Project Settings.


#include "ClueCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "CluelessJHU/Utilities/GameplayAPI.h"
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

	SetReplicates(true);
}

// Check Server RPC set Game Start Implementation
void AClueCharacter::ServerRPCSetGameStart_Implementation()
{
	// send mssage to server, here this message is already on server
	UGameplayAPI::ChangeClueGameState(ClueGameState::Gaming, (UObject*)this);

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Server RPC set game start!"));
}


// check if client is ready
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

