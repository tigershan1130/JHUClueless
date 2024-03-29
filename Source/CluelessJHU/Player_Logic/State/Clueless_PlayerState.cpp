﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Clueless_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "CluelessJHU/Player_Logic/Controller/ClueCharacter.h"

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

	DOREPLIFETIME(AClueless_PlayerState, CurrentPlayerState);
	DOREPLIFETIME(AClueless_PlayerState, RoleID);
	DOREPLIFETIME(AClueless_PlayerState, CurrentControlledPawn);
	//DOREPLIFETIME(AClueless_PlayerState, CurrentGameAction);
	//TODO: FIx bug, currently all players cards all send to each of the clients
	// So technically this can be send to other players, so we only need to rep_notify to owner.
	DOREPLIFETIME(AClueless_PlayerState, IsAudience);
	DOREPLIFETIME(AClueless_PlayerState, HandCards);

}

void AClueless_PlayerState::ChangeGameState(int State)
{
	CurrentPlayerState = State;
}

void AClueless_PlayerState::OnRep_RoleID()
{
	if (GIsServer && GetNetMode() != ENetMode::NM_ListenServer)
		return;

	if (CurrentControlledPawn == nullptr)
		return;
}

void AClueless_PlayerState::OnRep_ChangedPawn()
{

}

void AClueless_PlayerState::OnRep_CardsDistributed()
{
	//TODO: FIx bug, currently all players cards all send to each of the clients
	// So technically this can be send to other players, so we only need to rep_notify to owner.

	if (CurrentControlledPawn != nullptr)
	{
		AClueCharacter* Character = (AClueCharacter*)CurrentControlledPawn;

		if (Character != nullptr)		
			Character->OnPlayerCardsUpdated();		
	}
}

void AClueless_PlayerState::OnRep_BecomeAudience()
{

	if (CurrentControlledPawn != nullptr)
	{
		AClueCharacter* Character = (AClueCharacter*)CurrentControlledPawn;

		if (Character != nullptr)
			Character->OnBecomeAudience();
	}
}


void AClueless_PlayerState::SetCardsInHand(TArray<FCardEntityData> CardsForThisPlayer)
{
	HandCards = CardsForThisPlayer; // This will then replicate to all clients.
}
