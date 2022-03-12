﻿// Fill out your copyright notice in the Description page of Project Settings.


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

	DOREPLIFETIME(AClueless_PlayerState, CurrentPlayerState);
	DOREPLIFETIME(AClueless_PlayerState, RoleID);


}

void AClueless_PlayerState::ChangeGameState(int State)
{
	CurrentPlayerState = State;

}

void AClueless_PlayerState::OnRep_RoleID()
{

}


void AClueless_PlayerState::OnRep_GameActionChanged()
{

}


void AClueless_PlayerState::OnRep_ChangedPawn()
{

}
