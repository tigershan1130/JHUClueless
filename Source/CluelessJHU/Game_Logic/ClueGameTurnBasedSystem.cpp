// Fill out your copyright notice in the Description page of Project Settings.


#include "ClueGameTurnBasedSystem.h"

// Sets default values for this component's properties
UClueGameTurnBasedSystem::UClueGameTurnBasedSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UClueGameTurnBasedSystem::OnPlayerEndTurn()
{
	print("[Server: CluelessGameLogic] TODO: End Current Players Turn->Move Turn To Next Player");

}

// Called when the game starts
void UClueGameTurnBasedSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UClueGameTurnBasedSystem::OnGameInit()
{
	print("[Server: CluelessGameLogic] TODO: Generate Cards->Spawn Characters->InitiatePlayerTurn");
	

	// Spawn Cards

	// Spawn Player Character


}

void UClueGameTurnBasedSystem::OnPlayerMakeAccusation()
{
	print("[Server: CluelessGameLogic] TODO: Check Accusation Cards->Player Status Adjustment");

	
}

void UClueGameTurnBasedSystem::OnPlayerMakeMovement()
{
	print("[Server: CluelessGameLogic] TODO: Validate Player Movement->Make Player Movement");

}

void UClueGameTurnBasedSystem::OnPlayerMakeSuggestion()
{
	print("[Server: CluelessGameLogic] TODO: check Player Suggestion");

}

