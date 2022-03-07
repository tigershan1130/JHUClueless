// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CluelessJHU/Game_Logic/CGameStateBase.h"
#include "CluelessJHU/Data/Game_StaticData.h"
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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	/*
	* this will be called by Player State to notify that character is ready to initialize data.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void OnPlayerCharacterInitalized();

	/**
	 * @brief Onplayer Joined Game With CharacterRelation
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void OnPlayerCharacterJoinBinded();

	/**
	 * @brief this is for base player setup when game state has changed.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void OnGameStateChanged(ClueGameState State);


	/**
	 * @brief
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void OnHostReadyStartGame();

	
	/**
	 * @brief Interface message, send message to server to ask server to start the game.
	*/
	UFUNCTION(BlueprintCallable, Reliable, Server)
		void ServerRPCSetGameStart();
	void ServerRPCSetGameStart_Implementation();

private:
	// this is private function called in tick to check if the player is ready.
	UFUNCTION()
		void ClientCheckPlayerReady();



};