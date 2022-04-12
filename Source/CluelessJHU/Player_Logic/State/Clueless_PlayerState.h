// Fill out your copyright notice in the Description page of Project Settings.
// This Class Contains Both on Server and Client, Handles each player's MVC's Model

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CluelessJHU/CActor.h"
#include "CluelessJHU/Data/Game_StaticData.h"
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



#pragma region Recieves message from server, these states have changed.
	UFUNCTION()
		virtual void OnRep_RoleID();

	UFUNCTION()
		virtual void OnRep_GameActionChanged();

	UFUNCTION()
		virtual void OnRep_ChangedPawn();

	UFUNCTION()
		virtual void OnRep_CardsDistributed();

	UFUNCTION()
		virtual void OnRep_BlockChanged();

#pragma endregion Recieves message from server, these states have changed.


#pragma region Server Functions

	UFUNCTION(BlueprintCallable)
		void ChangeGameState(int State);

	UFUNCTION()
		void SetControlledPawn(APawn* Pawn)
	{
		CurrentControlledPawn = Pawn;
	}


	//#define TEST_BIT(Bitmask, Bit) (((Bitmask) & (1 << static_cast<uint8>(Bit))) > 0)
	//#define SET_BIT(Bitmask, Bit) (Bitmask |= 1 << static_cast<uint8>(Bit))
	//#define CLEAR_BIT(Bitmask, Bit) (Bitmask &= ~(1 << static_cast<uint8>(Bit)))

	UFUNCTION()
		void SetAllowGameAction(EPlayerGameAction Action)
	{
		CurrentGameAction |= 1 << static_cast<uint8>(Action);

		// for special case of listen server
		if (GetNetMode() == ENetMode::NM_ListenServer)
			OnRep_GameActionChanged();
	}

	UFUNCTION()
		bool IsActionAllowed(EPlayerGameAction Action)
	{
		return (CurrentGameAction & (1 << static_cast<uint8>(Action))) > 0;
	}

	UFUNCTION()
		void ClearGameAction(EPlayerGameAction Action)
	{
		CurrentGameAction &= ~(1 << static_cast<uint8>(Action));

		// for special case of listen server
		if (GetNetMode() == ENetMode::NM_ListenServer)
			OnRep_GameActionChanged();
	}

	UFUNCTION()
		void ClearAllGameAction()
	{
		if (CurrentGameAction != 0)
		{
			CurrentGameAction = 0;

			// for special case of listen server
			if (GetNetMode() == ENetMode::NM_ListenServer)
				OnRep_GameActionChanged();
		}
	}

	UFUNCTION()
		void SetCardsInHand(TArray<FCardEntityData> CardsForThisPlayer);

	UFUNCTION()
		TArray<FCardEntityData> GetCardsInHand()
	{
		return HandCards;
	}

	UFUNCTION(BlueprintCallable)
		int GetCurrentGameState()
	{
		return CurrentPlayerState;
	}

	UFUNCTION()
		int GetRoleID()
	{
		return RoleID;
	}

	UFUNCTION()
		void SetRoleID(int ID)
	{
		RoleID = ID;
	}

	UFUNCTION()
		void SetBlockID(int BlockID)
	{
		CurrentBlockID = BlockID;

		// for special case of listen server
		if (GetNetMode() == ENetMode::NM_ListenServer)
			OnRep_BlockChanged();
	}

	UFUNCTION()
		int GetBlockID()
	{
		return CurrentBlockID;
	}

#pragma endregion Server Functions

protected:
	/**
	 * @brief Check this player posses which character should be -1 when player is connected
	 * Will only assign a number when user starts the game.
	*/
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_RoleID)
		int RoleID;


	/**
	 * @brief check player current state it is in, Playing, Waiting or Game End.
	*/
	UPROPERTY(BlueprintReadWrite, Replicated)
		int CurrentPlayerState;


	/**
	 * @brief In Game, what kind of state are we in, should we make our move, should we make a suggestion, this will control player's current action
	 * or should we wait for other's players turn.
	*/
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_GameActionChanged)
		uint8 CurrentGameAction; 


	/**
	 * @brief This is the current pawn we are controlling.
	*/
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_ChangedPawn)
		APawn* CurrentControlledPawn;

	/*
	* @brief this holds all cards current player have
	*/
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_CardsDistributed)
		TArray<FCardEntityData> HandCards;

	/*
	* @brief Current Location of the Player
	*/
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_BlockChanged)
		int CurrentBlockID;

	
};
