// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CluelessJHU/Data/Game_StaticData.h"
#include "GameFramework/GameStateBase.h"
#include "CGameStateBase.generated.h"

class AClueless_PlayerState;
/**
 * 
 */
UCLASS()
class CLUELESSJHU_API ACGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	

public:
	// Constructor
	ACGameStateBase();

	// C++11 destructor
	virtual ~ACGameStateBase() = default;

	// for replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	// get player Setup Data
	UFUNCTION(BlueprintCallable, Category = "CluelessGameState")
		TArray<FPlayerSetupStaticData>  GetPlayerSetupStaticData();

	// Check if game is ready
	UFUNCTION(BlueprintCallable, Category = "CluelessGameState")
		void ReigsterPlayerControllerOnServer(APlayerController* PlayerController);


	UFUNCTION(BlueprintCallable, Category = "CluelessGameState")
		void UnReigsterPlayerControllerOnServer(APlayerController* PlayerController);


	UFUNCTION(BlueprintCallable, Category = "CluelessGameState")
		void UpdatePlayerControllerWithCharacterOnServer(APlayerController* PlayerController, ACharacter* Character);


	UFUNCTION(BlueprintCallable, Category = "CluelessGameState")
	void ReportPlayerClientReady(ACharacter* PlayerCharacter);


	UFUNCTION()
	virtual void OnRep_GameStartedChanged();



protected:

	UFUNCTION()
	void CheckGameIsReadyToStart();


	// localized data
	UPROPERTY()
	TArray<FPlayerSetupStaticData> PlayerSetupStaticData;



	UPROPERTY()
	TMap<APlayerController*, ACharacter*> ServerMapPlayerCharacters;


	UPROPERTY()
	TArray<ACharacter*> ReportedClientPlayers;

	// Is our data ready
	// Is our connected players loaded
	UPROPERTY()
	TMap<FString, bool> SystemStatusMap;

	
	UPROPERTY(ReplicatedUsing = OnRep_GameStartedChanged)
	bool IsReadyToStartGame;


};
