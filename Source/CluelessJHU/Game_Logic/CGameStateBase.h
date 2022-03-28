// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CluelessJHU/Data/Game_StaticData.h"
#include "GameFramework/GameStateBase.h"
#include "CGameStateBase.generated.h"

class AClueless_PlayerState;


UENUM()
enum ClueGameState
{
	PreGaming	UMETA(DisplayName = "Pre Gaming"),
	Gaming		UMETA(DisplayName = "Gaming"),
	PostGaming	UMETA(DisplayName = "Post Gaming"),
};

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

	/**
	 * @brief when player connects to game mode.
	*/
	UFUNCTION(BlueprintCallable, Category = "CluelessGameState")
		void ReigsterPlayerControllerOnServer(APlayerController* PlayerController);

	/**
	 * @brief UnRegister player Controller
	*/
	UFUNCTION(BlueprintCallable, Category = "CluelessGameState")
		void UnReigsterPlayerControllerOnServer(APlayerController* PlayerController);

	/**
	 * @brief Update player when player gets possed
	*/
	UFUNCTION(BlueprintCallable, Category = "CluelessGameState")
		void UpdatePlayerControllerWithCharacterOnServer(APlayerController* PlayerController, ACharacter* Character);

	/**
	 * @brief Replication when game is about start host have to manually start
	*/
	UFUNCTION()
		virtual void OnRep_GameStartedChanged();

	/**
	 * @brief Player character mapping changed to update our pre-game UI
	*/
	UFUNCTION()
		void OnRep_PlayerCharacterMappingChanged();

	/**
	 * @brief When this changed, we need to change our UI.
	*/
	UFUNCTION()
		void OnRep_GameStateChanged();

	/**
	 * @brief when turn has changed.
	*/
	UFUNCTION()
		void OnRep_TurnChanged();


	/**
	 * @brief Get Current player and character relation mapping, character are possed character
	*/
	UFUNCTION()
		TArray<FPlayerCharacterRelationEntry> GetCharatersRelationMapping()
	{
		return PlayerRelationMapping.PlayerRelationMapping;
	}

	/**
	 * @brief Get Current active players
	*/
	UFUNCTION()
	TArray<ACharacter*> GetActivePlayerCharacters_Server();

	UFUNCTION()
	TArray<APlayerController*> GetActiveController_Server();


	UFUNCTION()
	TArray<FCardEntityData> GetCardsSetupData();

	// Get game state called from GameAPI
	UFUNCTION()
	ClueGameState GetGameState()
	{
		return CGameState;
	}

	// called by server only.
	// It will change our server' game state
	UFUNCTION()
		void ChangeGameState(ClueGameState CurrentGameState);


protected:

	/**
	 * @brief Data table for character and role mapping
	*/
	UPROPERTY()
		TArray<FPlayerSetupStaticData> PlayerSetupStaticData;

	/* Clue Cards Setup Data*/
	UPROPERTY()
		TArray<FCardEntityData> ClueCardsSetupData;

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
	UPROPERTY(ReplicatedUsing = OnRep_GameStateChanged)
		TEnumAsByte<ClueGameState> CGameState;

	/**
	 * @brief Use to replicate between server and client, holds relation mapping between PlayerSetupStaticData and PlayerCharacter
	*/
	UPROPERTY(ReplicatedUsing = OnRep_PlayerCharacterMappingChanged)
		FPlayerRelationsContainer PlayerRelationMapping;


	/**
	 * @brief When turn changed we keep player's turn using this data.
	*/
	UPROPERTY(ReplicatedUsing = OnRep_TurnChanged)
		int PlayerTurn;

	
	/**
	 * @brief The murder deck.TODO: Add to replication
	*/
	UPROPERTY(Replicated)
		TArray<FCardEntityData> MurderDeck;

	/*
	* @brief Extra deck  TODO: Add to replication
	*/
	UPROPERTY(Replicated)
		TArray<FCardEntityData> ExtraDeck;

	/*
	* @brief This contains PlayerState to Deck Relation TODO: Add to replication
	*/
	UPROPERTY(Replicated)
		TArray<AClueless_PlayerState*> DistributedCardsPlayers;

}; 