// Fill out your copyright notice in the Description page of Project Settings.


#include "ClueCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "CluelessJHU/Utilities/GameplayAPI.h"
#include "CluelessJHU/Player_Logic/State/Clueless_PlayerState.h"
#include "CluelessJHU/Game_Logic/State/CGameStateBase.h"

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
	UGameplayAPI::ChangeClueGameState_Server(ClueGameState::Gaming, (UObject*)this);

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Server RPC set game start!"));
}

void AClueCharacter::ServerRPCMakeMovement_Implementation(int BlockID)
{
	AClueless_PlayerState* CPlayerState = (AClueless_PlayerState*)this->GetPlayerState();

	// this is server side Clue Character call
	if (CPlayerState)
		UGameplayAPI::MakeMovement_Server((UObject*)this, BlockID, CPlayerState->GetRoleID());
}

void AClueCharacter::ServerRPCMakeSuggestion_Implementation(const FString& CWeaponID, const FString& CRoleID, const FString& CRoomID)
{
	AClueless_PlayerState* CPlayerState = (AClueless_PlayerState*)this->GetPlayerState();

	// this is server side Clue Character call
	if (CPlayerState)
		UGameplayAPI::MakeSuggestion_Server((UObject*)this, CPlayerState->GetRoleID(), CWeaponID, CRoleID, CRoomID);
}


void AClueCharacter::ServerRPCMakeAccusation_Implementation(const FString& CWeaponID, const FString& CRoleID, const FString& CRoomID)
{
	AClueless_PlayerState* CPlayerState = (AClueless_PlayerState*)this->GetPlayerState();

	// this is server side Clue Character call
	if (CPlayerState)
		UGameplayAPI::MakeAccusation_Server((UObject*)this, CPlayerState->GetRoleID(), CWeaponID, CRoleID, CRoomID);
}

void AClueCharacter::ServerRPCEndTurn_Implementation()
{
	// this is server side clue character call
	UGameplayAPI::EndPlayerTurn_Server((UObject*)this);

	// Make sure our local character Option UI is closed.
	if (this->IsLocallyControlled())
	{
		OnEndCharacterTurn();
	}

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

void AClueCharacter::InitClientVisualCharacter(int RoleID)
{
	if (GIsServer && GetNetMode() != ENetMode::NM_ListenServer)
		return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPawn::StaticClass(), FoundActors);

	for (int i = 0; i < FoundActors.Num(); i++)
	{
		ACPawn* CurrentCharacter = (ACPawn*)FoundActors[i];
		if (RoleID == CurrentCharacter->RoleID)
			ClientControlledVisualActor = CurrentCharacter;
	}

	if (ClientBlockPoints.Num() <= 0)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlockPointActor::StaticClass(), FoundActors);

		for (int i = 0; i < FoundActors.Num(); i++)
		{
			ABlockPointActor* BlockPoint = (ABlockPointActor*)FoundActors[i];
			if (BlockPoint)
				ClientBlockPoints.Add(BlockPoint);
		}
	}
}

void AClueCharacter::SetVisualPawnToBlock(int BlockID, int RoleID)
{
	if (ClientBlockPoints.Num() <= 0)
	{
		TArray<AActor*> FoundActors;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlockPointActor::StaticClass(), FoundActors);

		for (int i = 0; i < FoundActors.Num(); i++)
		{
			ABlockPointActor* BlockPoint = (ABlockPointActor*)FoundActors[i];
			if (BlockPoint)
				ClientBlockPoints.Add(BlockPoint);
		}
	}

	if (ClientControlledVisualActor == nullptr)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPawn::StaticClass(), FoundActors);

		for (int i = 0; i < FoundActors.Num(); i++)
		{
			ACPawn* CurrentCharacter = (ACPawn*)FoundActors[i];
			if (RoleID == CurrentCharacter->RoleID)
				ClientControlledVisualActor = CurrentCharacter;
		}
	}

	ABlockPointActor* FoundBlock = nullptr;

	for (int i = 0; i < ClientBlockPoints.Num(); i++)
	{
		if (ClientBlockPoints[i]->BlockID == BlockID)
			FoundBlock = ClientBlockPoints[i];

	}

	if (FoundBlock && ClientControlledVisualActor != nullptr)
	{
		// TODO: move current Visual Character to Block Location
		ClientControlledVisualActor->SetActorLocation(FoundBlock->GetActorLocation());
	}

}

