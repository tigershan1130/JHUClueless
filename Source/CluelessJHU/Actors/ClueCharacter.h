// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		void ServerConnectPlayerControllerWithActor(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable) // maybe blueprint need to do some stuff before we can call player ready
		void SetPlayerCharacterReady();

	// Tell server character client is initialized.
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCClientActorReady();

	// Server validation once recieve rpc from client.
	// we can prevent invalid access token and DDOS.
	bool ServerRPCClientActorReady_Validate()
	{
		return true;                              // This will allow the RPC to be called
	}


	void ServerRPCClientActorReady_Implementation();

	UFUNCTION()
		void ClientActorReady();


	// this will be called by Player State to notify that character is ready to initialize data.
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
	void OnPlayerCharacterInitalized();
};
