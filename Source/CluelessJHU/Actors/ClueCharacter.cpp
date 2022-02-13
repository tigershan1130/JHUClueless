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
	

	// initialize Player stuff
}

void AClueCharacter::SetPlayerCharacterReady()
{	
	// if client Tell server actor we are ready	
	ENetMode NetMode = GetNetOwner()->GetNetMode();

	if (NetMode == ENetMode::NM_Client || NetMode == ENetMode::NM_Standalone)
		ServerRPCClientActorReady();
	//else if (NetMode == ENetMode::NM_ListenServer)
	//	ClientActorReady();
}

// Called every frame
void AClueCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AClueCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AClueCharacter::ServerConnectPlayerControllerWithActor(APlayerController* PlayerController)
{
	ACGameStateBase* const MyGameState = GetWorld() != NULL ? GetWorld()->GetGameState<ACGameStateBase>() : NULL;

	MyGameState->UpdatePlayerControllerWithCharacterOnServer(PlayerController, this);

}

// this is RPC from client ->server 
void AClueCharacter::ServerRPCClientActorReady_Implementation()
{
	ClientActorReady();
}

void AClueCharacter::ClientActorReady()
{
	ACGameStateBase* const MyGameState = GetWorld() != NULL ? GetWorld()->GetGameState<ACGameStateBase>() : NULL;

	MyGameState->ReportPlayerClientReady(this);
}



