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


	/**
	 * @brief UnRegister player Controller
	*/
	UFUNCTION(BlueprintCallable, Category = "CluelessGameState")
		void UnReigsterPlayerControllerOnServer(APlayerController* PlayerController);

	/**
	 * @brief Update player
	*/

	UFUNCTION(BlueprintCallable, Category = "CluelessGameState")
		void UpdatePlayerControllerWithCharacterOnServer(APlayerController* PlayerController, ACharacter* Character);


	UFUNCTION()
		virtual void OnRep_GameStartedChanged();

	UFUNCTION()
		void OnRep_PlayerCharacterMappingChanged();


	UFUNCTION()
		TArray<FPlayerCharacterRelationEntry> GetCharatersRelationMapping()
	{
		return PlayerRelationMapping.PlayerRelationMapping;
	}

	UFUNCTION()
		TArray<ACharacter*> GetActivePlayerCharacters();


protected:

	UFUNCTION()
		void CheckGameIsReadyToStart();


	/**
	 * @brief Data table for character and role mapping
	*/
	UPROPERTY()
		TArray<FPlayerSetupStaticData> PlayerSetupStaticData;


	/**
	 * @brief  This holds registered
	*/
	UPROPERTY()
		TMap<APlayerController*, ACharacter*> ServerMapPlayerCharacters;


	/**
	 * @brief This use to set if our game is ready to start, if it is
	 * on Host Client will show StartGame button, this is used with listen server option.
	*/
	UPROPERTY(ReplicatedUsing = OnRep_GameStartedChanged)
		bool HostReadyToStartGame;

	/**
	 * @brief GameState that modifies current game
	*/
	UPROPERTY()
		int GameState;

	/**
	 * @brief Use to replicate between server and client, holds relation mapping between PlayerSetupStaticData and PlayerCharacter
	*/
	UPROPERTY(ReplicatedUsing = OnRep_PlayerCharacterMappingChanged)
		FPlayerRelationsContainer PlayerRelationMapping;
};
