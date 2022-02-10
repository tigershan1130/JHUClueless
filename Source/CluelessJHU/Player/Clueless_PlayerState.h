// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Clueless_PlayerState.generated.h"



/**
 *
 */

UCLASS(BlueprintType, Blueprintable, notplaceable)
class CLUELESSJHU_API AClueless_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AClueless_PlayerState();

	virtual void PostInitializeComponents() override;

	// for replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UFUNCTION()
		virtual void OnRep_RoleID();

	UFUNCTION()
		virtual void OnRep_StateChanged();

	UFUNCTION()
		virtual void OnRep_GameActionChanged();

	UFUNCTION()
		virtual void OnRep_ChangedPawn();

private:
	/**
	 * @brief Check this player posses which character should be -1 when player is connected
	 * Will only assign a number when user starts the game.
	*/
	UPROPERTY(ReplicatedUsing = OnRep_RoleID)
		int RoleID;


	/**
	 * @brief check player current state it is in, Playing, Waiting or Game End.
	*/
	UPROPERTY(ReplicatedUsing = OnRep_StateChanged)
		int CurrentState;


	/**
	 * @brief In Game, what kind of state are we in, should we make our move, should we make a suggestion, this will control player's current action
	 * or should we wait for other's players turn.
	*/
	UPROPERTY(ReplicatedUsing = OnRep_GameActionChanged)
		int CurrentGameAction;


	/**
	 * @brief This is the current pawn we are controlling.
	*/
	UPROPERTY(ReplicatedUsing = OnRep_ChangedPawn)
		APawn* CurrentControlledPawn;
};
