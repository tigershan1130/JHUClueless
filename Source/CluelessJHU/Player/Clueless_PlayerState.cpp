// Fill out your copyright notice in the Description page of Project Settings.


#include "Clueless_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "CluelessJHU/Actors/ClueCharacter.h"

AClueless_PlayerState::AClueless_PlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AClueless_PlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AClueless_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AClueless_PlayerState, CurrentState);


}

void AClueless_PlayerState::ChangeGameState(int State)
{
	CurrentState = State;

	// if it is listen server, we need to call this seperately to invoke replication for state change.
	if (GetNetMode() == NM_Standalone)	
		OnRep_StateChanged();
	
}

void AClueless_PlayerState::OnRep_RoleID()
{
}

void AClueless_PlayerState::OnRep_StateChanged()
{
	// called on client, get possessed character
	AClueCharacter* Character = (AClueCharacter*)GetPawn();

	// check if character is valid
	if (Character != nullptr)
	{
		Character->OnPlayerCharacterInitalized();
	}

}

void AClueless_PlayerState::OnRep_GameActionChanged()
{
}

void AClueless_PlayerState::OnRep_ChangedPawn()
{
}
