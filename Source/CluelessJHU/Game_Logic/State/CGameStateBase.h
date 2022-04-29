// Fill out your copyright notice in the Description page of Project Settings.
// THis Class is both on server and Client.

#pragma once

#include "CoreMinimal.h"

#include "CluelessJHU/Data/Game_StaticData.h"
#include "GameFramework/GameStateBase.h"
#include "CGameStateBase.generated.h"

class AClueless_PlayerState;


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


#pragma region Client recieves state change from Server
	/**
	 * @brief Replication when game is about start host have to manually start
	*/
	UFUNCTION()
		virtual void OnRep_GameStartedChanged();

	/*
	* @brief Check Cards Changed
	*/
	UFUNCTION()
		virtual void OnRep_OnExtraCardsChanged();

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
		void OnRep_TurnInfoChanged();

	// when a show card index changed.
	UFUNCTION()
		void OnRep_ShowCardTurnChanged();

#pragma endregion Client recieves state change from Server


#pragma region Server GetAndSet Functions
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

	UFUNCTION()
		TArray<FCardEntityData> GetLeftoverCards()
	{
		return LeftoverDeck;
	}

	UFUNCTION()
		void SetupCards(TArray<FCardEntityData> MurderCards, TArray<FCardEntityData> LeftoverCards)
	{
		MurderDeck = MurderCards;

		for (auto& Entry : MurderCards)
		{
			UE_LOG(LogTemp, Error, TEXT("Murder Deck: %s"), *(Entry.CardName.ToString()));
		}

		LeftoverDeck = LeftoverCards;
	}


	// called by server only.
	// It will change our server' game state
	UFUNCTION()
		void ChangeGameState(ClueGameState CurrentGameState);

	// Called by server only.
	// Only server have murder deck info, 
	UFUNCTION()
		TArray<FCardEntityData> GetMurderDeck()
	{
		return MurderDeck;
	}

	UFUNCTION()
		void ChangeToNextTurnIndex();

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

	// we need to refresh our turn index for any game action update
	UFUNCTION()
		void CheckInitCurrentTurnActions();

	UFUNCTION()
		void CheckActionAfterMovement();

	//
	UFUNCTION()
		int GetShowCardTurnIndex(int ShowCardCounter);

	//
	UFUNCTION()
		FPlayerSuggestedData GetPlayerSuggestData()
	{
		return SuggestionCachedData;
	}

	//
	UFUNCTION()
		void SetPlayerSuggestData(FPlayerSuggestedData SuggestData)
	{
		SuggestionCachedData = SuggestData;
		SuggestionCachedData.IsInShowCardPhase = true;

		if (GetNetMode() == ENetMode::NM_ListenServer)		
			OnRep_ShowCardTurnChanged();		
	}

	//
	UFUNCTION()
		void ResetSuggestionData()
	{
		SuggestionCachedData.SuggestionTurnCounter = 0;
		SuggestionCachedData.IsInShowCardPhase = false;

		if (GetNetMode() == ENetMode::NM_ListenServer)
			OnRep_ShowCardTurnChanged();
	}

#pragma endregion Server GetAndSet Functions


#pragma region for Both server and client
	// Get game state called from GameAPI
	UFUNCTION()
	ClueGameState GetGameState()
	{
		return CGameState;
	}

	// Get current player's turn index. Who's turn is it?
	UFUNCTION(BlueprintCallable, Category = "CluelessGameState")
		int GetCurrentTurn()
	{
		return PlayerTurnCachedData.PlayerTurnIndex;
	}

	// get player Setup Data
	UFUNCTION(BlueprintCallable, Category = "CluelessGameState")
		TArray<FPlayerSetupStaticData>  GetPlayerSetupStaticData();

	// get current turn
	UFUNCTION()
		FPlayerTurnInfo GetCurrentTurnInfo()
	{
		return PlayerTurnCachedData;
	}

	// clear player game action in current turn
	UFUNCTION()
		void ClearCurrentPlayerGameAction(EPlayerGameAction GameAction)
	{
		PlayerTurnCachedData.ClearGameAction(GameAction);

		//if (GetNetMode() == ENetMode::NM_ListenServer)
		//	OnRep_TurnInfoChanged();
	}

#pragma endregion for Both server and client

public:
	UFUNCTION(NetMulticast, Reliable)
		void OnMulticast_RPCNotifyShowedCard(const FString& RoleName, const FString& CardID);
	void OnMulticast_RPCNotifyShowedCard_Implementation(const FString& RoleName, const FString& CardID);

	UFUNCTION(NetMulticast, Reliable)
		void OnMulticast_RPCNotifyGameWin(const FString& RoleName, const FString& Information);
	void OnMulticast_RPCNotifyGameWin_Implementation(const FString& RoleName, const FString& Information);


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
	UPROPERTY(ReplicatedUsing = OnRep_TurnInfoChanged)
		FPlayerTurnInfo PlayerTurnCachedData;

	/*
	* @brief When player makes suggestion, this index tracks when to make other players stop showing cards	
	*/
	UPROPERTY(ReplicatedUsing = OnRep_ShowCardTurnChanged)
		FPlayerSuggestedData SuggestionCachedData;

	/**
	 * @brief The murder deck.
	*/
	UPROPERTY()
		TArray<FCardEntityData> MurderDeck;

	/*
	* @brief Extra deck
	*/
	UPROPERTY(Replicated = OnRep_OnExtraCardsChanged)
		TArray<FCardEntityData> LeftoverDeck;

	/*
	* @brief This contains PlayerState to Deck Relation TODO: Add to replication
	*/
	UPROPERTY(Replicated)
		TArray<AClueless_PlayerState*> DistributedCardsPlayers;

}; 