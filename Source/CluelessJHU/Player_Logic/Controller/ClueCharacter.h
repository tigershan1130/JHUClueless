﻿// Fill out your copyright notice in the Description page of Project Settings.
// This Class Exist both on Client and Server, This class gets user input and send messages to server
// This class also recieve events from Clueless_PlayerState to distribute events for GUI and Rendering API.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "CluelessJHU/Graphical_API/BlockPointActor.h"
#include "CluelessJHU/Player_Logic/State/Clueless_PlayerState.h"
#include "CluelessJHU/Game_Logic/State/CGameStateBase.h"
#include "CluelessJHU/Data/Game_StaticData.h"
#include "CluelessJHU/Player_Logic/Controller/CPawn.h"
#include "ClueCharacter.generated.h"

UCLASS()
class CLUELESSJHU_API AClueCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AClueCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * @brief Client Ready is false
	*/
	UPROPERTY(BlueprintReadOnly)
		bool ClientReady = false;

		/* This holds the visual character for Graphical API*/
	UPROPERTY(BlueprintReadWrite)
		ACPawn* ClientControlledVisualActor;

	UPROPERTY(BlueprintReadWrite)
		TArray<ABlockPointActor*> ClientBlockPoints;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input, This is client only.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void InitClientVisualCharacter(int RoleID);

	UFUNCTION()
		void SetVisualPawnToBlock(int BlockID, int RoleID);

	UFUNCTION()
	ACPawn* GetClientVisualActor()
	{
		return ClientControlledVisualActor;
	}

#pragma region ServerStateChanged

	/*
	* this will be called by Player State to notify that character is ready to initialize data. Server->Client
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void OnPlayerCharacterInitalized();

	/**
	 * @brief Onplayer Joined Game With CharacterRelation. Server->Client
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void OnPlayerCharacterJoinBinded();

	// On Extra cards updated.
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void OnExtraCardsUpdated();

	/* 
	* @brief on player cards updated.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void OnPlayerCardsUpdated();

	/**
	 * @brief this is for base player setup when game state has changed. Server->Client
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void OnGameStateChanged(ClueGameState State);

	/**
	 * @brief server->Client, Server notifies Client that host is able to start game. Only Host should able to have Start-Game Button enabled.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void OnHostReadyStartGame();

	/*
	* @brief When character is at his/her turn.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void OnThisCharacterTurn();

	// when controlled player turn ends.
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void OnEndCharacterTurn();

	// when non-controlled Player ends turn
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void OnOtherCharacterTurn();

#pragma endregion ServerStateChanged
	

#pragma region Player Send messages to Server
	/**
	 * @brief Interface message, client send message to server to ask server to start the game.
	*/
	UFUNCTION(BlueprintCallable, Reliable, Server)
		void ServerRPCSetGameStart();
	void ServerRPCSetGameStart_Implementation();


	/**
	* @brief Driver: client send message to server to request for a movement
	*/
	UFUNCTION(BlueprintCallable, Reliable, Server)
		void ServerRPCMakeMovement(int BlockID);
	void ServerRPCMakeMovement_Implementation(int BlockID);

	/*
	* @brief Driver: client send message to server to make a suggestion
	*/
	UFUNCTION(Blueprintcallable, Reliable, Server)
		void ServerRPCMakeSuggestion(const FString& CWeaponID, const FString& CRoleID, const FString& CRoomID);
	void ServerRPCMakeSuggestion_Implementation(const FString& CWeaponID, const FString& CRoleID, const FString& CRoomID);

	/**
	* @brief Driver: client send mssage to server to make an accusation
	*/
	UFUNCTION(BlueprintCallable, Reliable, Server)
		void ServerRPCMakeAccusation(const FString& CWeaponID, const FString& CRoleID, const FString& CRoomID);
	void ServerRPCMakeAccusation_Implementation(const FString& CWeaponID, const FString& CRoleID, const FString& CRoomID);

	/*
	* @brief Driver: client send message to server to end his/her current turn.
	*/
	UFUNCTION(BlueprintCallable, Reliable, Server)
		void ServerRPCEndTurn();
	void ServerRPCEndTurn_Implementation();

#pragma endregion Player Send messages to Server

	// ======================= UNIT TESTS(DRIVERS) ============================
	UFUNCTION(Exec)
		void FunctionTestMakeAccusationByID(FString WeaponID, FString RoleID, FString RoomID)
	{
		ServerRPCMakeAccusation(WeaponID, RoleID, RoomID);
	}

	UFUNCTION(Exec)
		void FunctionTestMakeSuggestionByID(FString WeaponID, FString RoleID, FString RoomID)
	{
		ServerRPCMakeSuggestion(WeaponID, RoleID, RoomID);
	}

	UFUNCTION(Exec)
		void FunctionTestMakeAccusationByName(FString Suspect, FString Weapon, FString Location)
	{
		ACGameStateBase* GameState = GetWorld()->GetGameState<ACGameStateBase>();

		FString WeaponID;
		FString RoleID;
		FString RoomID;

		if (GameState)
		{
			TArray<FCardEntityData> SetupCards = GameState->GetCardsSetupData();
			
			for (auto& Entry : SetupCards)
			{
				if (Entry.CardName.ToString() == Suspect)
					RoleID = Entry.CardID;
				if (Entry.CardName.ToString() == Weapon)
					WeaponID = Entry.CardID;
				if (Entry.CardName.ToString() == Location)
					RoomID = Entry.CardID;
			}

		}

		ServerRPCMakeAccusation(WeaponID, RoleID, RoomID);
	}

	UFUNCTION(Exec)
		void FunctionTestMakeSuggestionByName(FString Suspect, FString Weapon, FString Location)
	{
		ACGameStateBase* GameState = GetWorld()->GetGameState<ACGameStateBase>();

		FString WeaponID;
		FString RoleID;
		FString RoomID;

		if (GameState)
		{
			TArray<FCardEntityData> SetupCards = GameState->GetCardsSetupData();

			for (auto& Entry : SetupCards)
			{
				if (Entry.CardName.ToString() == Suspect)
					RoleID = Entry.CardID;
				if (Entry.CardName.ToString() == Weapon)
					WeaponID = Entry.CardID;
				if (Entry.CardName.ToString() == Location)
					RoomID = Entry.CardID;
			}

		}

		ServerRPCMakeSuggestion(WeaponID, RoleID, RoomID);
	}



	UFUNCTION(Exec)
		void FunctionTestMakeMovement(int BlockID)
	{
		ServerRPCMakeMovement(BlockID);
	}

	UFUNCTION(Exec)
		void FunctionTestEndTurn()
	{
		ServerRPCEndTurn();
	}

	UFUNCTION(Exec)
		void FunctionTestInitGame()
	{
		ServerRPCSetGameStart();
	}


private:
	// this is private function called in tick to check if the player is ready.
	UFUNCTION()
		void ClientCheckPlayerReady();



};