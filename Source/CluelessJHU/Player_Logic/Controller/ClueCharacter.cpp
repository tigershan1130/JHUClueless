// Fill out your copyright notice in the Description page of Project Settings.


#include "ClueCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "CluelessJHU/Utilities/GameplayAPI.h"
#include "CluelessJHU/Player_Logic/State/Clueless_PlayerState.h"
#include "CollisionQueryParams.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "CluelessJHU/Game_Logic/State/CGameStateBase.h"

#define print(text, color) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, color,text)

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

	//SetMouseDeprojection(true);
}

// Check Server RPC set Game Start Implementation
void AClueCharacter::ServerRPCSetGameStart_Implementation()
{
	// send mssage to server, here this message is already on server
	UGameplayAPI::ChangeClueGameState_Server(ClueGameState::Gaming, (UObject*)this);

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Server RPC set game start!"));
}

void AClueCharacter::ServerRPCMakeTeleport_Implementation(int BlockID)
{
	AClueless_PlayerState* CPlayerState = (AClueless_PlayerState*)this->GetPlayerState();

	// this is server side Clue Character call
	if (CPlayerState)
		UGameplayAPI::MakeTeleport_Server((UObject*)this, CPlayerState->GetRoleID(), BlockID);
}

void AClueCharacter::ServerRPCMakeMovement_Implementation(int BlockID)
{
	AClueless_PlayerState* CPlayerState = (AClueless_PlayerState*)this->GetPlayerState();

	// this is server side Clue Character call
	if (CPlayerState)
		UGameplayAPI::MakeMovement_Server((UObject*)this, BlockID, CPlayerState->GetRoleID());
}

void AClueCharacter::ServerRPCMakeSuggestion_Implementation(const FString& CWeaponID, const FString& CRoleID)
{
	AClueless_PlayerState* CPlayerState = (AClueless_PlayerState*)this->GetPlayerState();

	// this is server side Clue Character call
	if (CPlayerState)
		UGameplayAPI::MakeSuggestion_Server((UObject*)this, CPlayerState->GetRoleID(), CWeaponID, CRoleID);
}

void AClueCharacter::ServerRPCSkipShowCard_Implementation()
{
	AClueless_PlayerState* CPlayerState = (AClueless_PlayerState*)this->GetPlayerState();

	// this is server side Clue Character call
	if (CPlayerState)
		UGameplayAPI::SkipShowCard_Server((UObject*)this, CPlayerState->GetRoleID());
}

void AClueCharacter::ServerRPCShowCard_Implementation(const FString& CardID)
{
	AClueless_PlayerState* CPlayerState = (AClueless_PlayerState*)this->GetPlayerState();

	// this is server side Clue Character call
	if (CPlayerState)
		UGameplayAPI::ShowCard_Server((UObject*)this, CPlayerState->GetRoleID(), CardID);
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

void AClueCharacter::OnLeftMouseClick()
{
	if (CheckClick && IsLocallyControlled())
	{
		FVector2D MousePostion = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());

		UGameViewportClient* GameViewPort = GetWorld()->GetGameViewport();

		float DPIScale = GameViewPort->GetDPIScale();

		MousePostion = MousePostion * UWidgetLayoutLibrary::GetViewportScale(GameViewPort);

		FVector WorldPosition;
		FVector WorldDirection;

		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

		UGameplayStatics::DeprojectScreenToWorld(PC, MousePostion, WorldPosition, WorldDirection);

		FCollisionQueryParams TraceParams(FName(TEXT("InterTrace")), true, NULL);
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = true;

		FVector EndPosition = WorldPosition + (WorldDirection * 10000);

		FHitResult HitDetails = FHitResult(ForceInit);

		bool bIsHit = GetWorld()->LineTraceSingleByChannel(
			HitDetails,
			WorldPosition,
			EndPosition,
			ECC_PhysicsBody,
			TraceParams
		);


		if (bIsHit)
		{
			AActor* OwnerActor = HitDetails.GetComponent()->GetOwner();

			ABlockActor* BlockVisualActor = (ABlockActor*)OwnerActor;

			if (BlockVisualActor != nullptr)
			{
				if (BlockVisualActor->BlockID > 0 && BlockVisualActor->BlockID <= 55)
				{
					FString Msg = "I Hit Visual Block: " + FString::FromInt(BlockVisualActor->BlockID);

					TArray<ABlockActor*> VisualBlocks = UGameplayAPI::GetCurrentPlayerNeighborVisualBlocks(GetWorld());

					TArray<int> VisualBlockIDs;

					for (auto& Entry : VisualBlocks)
					{
						VisualBlockIDs.Add(Entry->BlockID);
					}

					if (VisualBlockIDs.Contains(BlockVisualActor->BlockID))
					{
						ServerRPCMakeMovement(BlockVisualActor->BlockID);
						print(Msg, FColor::Blue);
					}
				}
			}
		}
	}
}

// Called to bind functionality to input
void AClueCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind Mouse left button
	InputComponent->BindAction("MouseLeftClick", IE_Pressed, this, &AClueCharacter::OnLeftMouseClick);

}