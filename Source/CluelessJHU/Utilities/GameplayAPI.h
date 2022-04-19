// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CluelessJHU/Data/Game_StaticData.h"
#include "GameFramework/Character.h"
#include "CluelessJHU/Player_Logic/State/Clueless_PlayerState.h"
#include "CluelessJHU/Game_Logic/Controller/ClueGameTurnBasedComponent.h"
#include "CluelessJHU/Game_Logic/Controller/CluelessMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "CluelessJHU/Game_Logic/State/CGameStateBase.h"
#include "GameplayAPI.generated.h"
	

#define print(text, color) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, color,text)
/**
 * 
 */
UCLASS()
class CLUELESSJHU_API UGameplayAPI : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

#pragma region Server and Client calls	
	/*Get CardID from block ID using SetupCard Static Data*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static FString GetCardIDFromBlockID(int BlockID, UObject* WorldContextObj)
	{
		FString BlockCardID = "Error";

		FCardEntityData CardData;
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return BlockCardID;

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		TArray<FCardEntityData> SetupCards = GameState->GetCardsSetupData();

		for (auto& Entry : SetupCards)
		{
			if (Entry.RelationID.ToString() == FString::FromInt(BlockID))
			{
				UE_LOG(LogTemp, Warning, TEXT("Entry Relation ID: %s, %d"), *(Entry.RelationID.ToString()), *(Entry.CardID));
				BlockCardID = Entry.CardID;
			}
		}

		return BlockCardID;
	}

	/*
	* Get Static Block Info From Datatable by passing in BlockID
	*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static FStaticMovementBlock GetBlockInfo(int BlockID, UObject* WorldContextObj)
	{
		FStaticMovementBlock FoundBlock;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return FoundBlock;

		UCluelessMovementStateComponent* CluelessMovementStateCompCache = (UCluelessMovementStateComponent*)(GameState->GetComponentByClass(UCluelessMovementStateComponent::StaticClass()));


		if (CluelessMovementStateCompCache == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid Movement State Component"));
		}

		TArray<FStaticMovementBlock> StaticMovementDataList = CluelessMovementStateCompCache->GetStaticMovementCache();

		for (auto& Entry : StaticMovementDataList)
		{
			if (Entry.BlockID == BlockID)
			{
				FoundBlock = Entry;
			}
		}

		return FoundBlock;
	}

	/*
	* Get Block ID from Role ID(meaning which block is the role currently at?)
	*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static int GetBlockIDFromRoleID(int RoleID, UObject* WorldContextObj)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return -1;

		UCluelessMovementStateComponent* CluelessMovementStateCompCache = (UCluelessMovementStateComponent*)(GameState->GetComponentByClass(UCluelessMovementStateComponent::StaticClass()));

		if (!CluelessMovementStateCompCache)
			return -1;


		return CluelessMovementStateCompCache->GetBlockIDFromRoleID(RoleID);
	}

	/*
	* Get Static Card Info From Dattable by passing in CardID
	*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static FCardEntityData GetCardStaticData(FString CardID, UObject* WorldContextObj)
	{
		FCardEntityData CardData;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return CardData;

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		TArray<FCardEntityData> SetupCards = GameState->GetCardsSetupData();

		for (auto& Entry : SetupCards)
		{
			if (Entry.CardID == CardID)
			{
				CardData = Entry;
			}
		}

		return CardData;
	}

	/*
	* Get RoleID From Character Relation ID
	*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static int FindRoleIDFromCharacterID(FName CharacterID, UObject* WorldContextObj)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return -1;

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return -1;

		TArray<FPlayerSetupStaticData> PlayerStaticSetupData = GetPlayerStaticSetupData(World);

		int FoundIndex = -1;
		for (int i = 0; i < PlayerStaticSetupData.Num(); i++)
		{
			if (PlayerStaticSetupData[i].CharacterID == CharacterID)
			{
				return i;
			}
		}

		return -1;
	}

	/*
	Get current role data for both client and server
	*/
	UFUNCTION(BlueprintCallable, Category = "GamePlay API", meta = (WorldContext = "WorldContextObj"))
		static FPlayerSetupStaticData GetCurrentRoleData(int RoleID, UObject* WorldContextObj)
	{
		FPlayerSetupStaticData RoleEntry;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return RoleEntry;


		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		TArray<FPlayerCharacterRelationEntry> PlayerRelationMapping = GameState->GetCharatersRelationMapping();

		if (PlayerRelationMapping.Num() <= 0)
			return RoleEntry;

		TArray<FPlayerSetupStaticData> PlayerStaticSetupData = GetPlayerStaticSetupData(World);

		RoleID = FMath::Clamp(RoleID, 0, PlayerStaticSetupData.Num() - 1);


		RoleEntry = PlayerStaticSetupData[RoleID];

		return RoleEntry;
	}

	/*
	* Find Player State from Character ID, note, this doesn't 100% grantee a sucessful PlayerState is returned
	* only those who controls the correponsding role ID will be returned
	* if you only need role ID, you should use FindRoleIDFromCharacterID
	*/
	UFUNCTION(BlueprintCallable, Category = "GamePlay API", meta = (WorldContext = "WorldContextObj"))
		static APlayerState* FindPlayerFromCharacterID(FName CharacterID, UObject* WorldContextObj)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return nullptr;

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return nullptr;

		TArray<FPlayerSetupStaticData> PlayerStaticSetupData = GetPlayerStaticSetupData(World);

		int FoundIndex = -1;
		for (int i = 0; i < PlayerStaticSetupData.Num(); i++)
		{
			if (PlayerStaticSetupData[i].CharacterID == CharacterID)
			{
				FoundIndex = i;
			}
		}

		if (FoundIndex == -1)
			return nullptr;

		TArray<AClueless_PlayerState*> CPlayerStates = GetActivePlayerStates(World);

		for (auto& Entry : CPlayerStates)
		{
			if (Entry->GetRoleID() == FoundIndex)
				return Entry;
		}

		return nullptr;
	}

	/*Get current Controlled character*/
	UFUNCTION(BlueprintCallable, Category = "GamePlay API", meta = (WorldContext = "WorldContextObj"))
		static AClueCharacter* GetCurrentControlledCharacter(UObject* WorldContextObj)
	{
		AClueCharacter* ClueCharacter = nullptr;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return ClueCharacter;

		TArray<AClueless_PlayerState*> ActivePlayerStates = GetActivePlayerStates(WorldContextObj);
		
		for (auto& Entry : ActivePlayerStates)
		{
			APawn* CPawn = Entry->GetCurrentControlledPawn();

			if (CPawn->IsLocallyControlled())
			{
				ClueCharacter = (AClueCharacter*)CPawn;
			}
		}

		return ClueCharacter;

	}

	/* Get current */
	UFUNCTION(BlueprintCallable, Category = "GamePlay API", meta = (WorldContext = "WorldContextObj"))
		static FPlayerSuggestedData GetSuggestCachedData(UObject* WorldContextObj)
	{
		FPlayerSuggestedData SuggestedCacheData;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return SuggestedCacheData;

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return SuggestedCacheData;

		return GameState->GetPlayerSuggestData();

	}

	/**
	* Get Game State, Readonly
	*/
	UFUNCTION(BlueprintCallable, Category = "GamePlay API", meta = (WorldContext = "WorldContextObj"))
		static 	ClueGameState GetCurrentGameState(UObject* WorldContextObj)
	{
		TArray<FPlayerSetupStaticData> PlayerStaticSetupData;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return ClueGameState::PreGaming;

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return ClueGameState::PreGaming;

		return GameState->GetGameState();


	}
	
	/**
	 * @brief for both client and server
	*/
	UFUNCTION(BlueprintCallable, Category = "GamePlay API", meta = (WorldContext = "WorldContextObj"))
	static TArray<FPlayerSetupStaticData> GetPlayerStaticSetupData(UObject* WorldContextObj)
	{
		TArray<FPlayerSetupStaticData> PlayerStaticSetupData;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return PlayerStaticSetupData;

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState != nullptr)		
			PlayerStaticSetupData = GameState->GetPlayerSetupStaticData();
		
		return PlayerStaticSetupData;
	}

	/**
	* @brief For both client and server
	*/
	UFUNCTION(BlueprintCallable, Category = "GamePlay API", meta = (WorldContext = "WorldContextObj"))
		static TArray<FCardEntityData> GetCurrentPlayerCards(ACharacter* CurrentCharacter, UObject* WorldContextObj)
	{
		TArray<FCardEntityData> ActivePlayerCards;

		if (CurrentCharacter->IsLocallyControlled())
		{
			ActivePlayerCards = ((AClueless_PlayerState*)CurrentCharacter->GetPlayerState())->GetCardsInHand();
		}

		return ActivePlayerCards;
	}

	/*
	* @brief for both client and server
	*/
	UFUNCTION(BlueprintCallable, Category = "GamePlay API", meta = (WorldContext = "WorldContextObj"))
		static TArray<FCardEntityData> GetLeftOverCards(UObject* WorldContextObj)
	{
		TArray<FCardEntityData> ActivePlayerCards;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return ActivePlayerCards;

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return ActivePlayerCards;


		return GameState->GetLeftoverCards(); 
	}

	/**
	* @brief For both client and server
	*/
	UFUNCTION(BlueprintCallable, Category = "GamePlay API", meta = (WorldContext = "WorldContextObj"))
	static FPlayerSetupStaticData GetCurrentTurnCharacterInfo(int CurrentTurnIndex , UObject* WorldContextObj)
	{
		FPlayerSetupStaticData PlayerSetupData;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return PlayerSetupData;


		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return PlayerSetupData;

		TArray<FPlayerCharacterRelationEntry> PlayerRelationMapping = GameState->GetCharatersRelationMapping();

		if (PlayerRelationMapping.Num() <= 0)
			return PlayerSetupData;

		TArray<FPlayerSetupStaticData> PlayerStaticSetupData = GetPlayerStaticSetupData(World);


		for (int i = 0; i < PlayerRelationMapping.Num(); i++)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Relationship index: %d , Name %s"), PlayerRelationMapping[i].Index, *(PlayerRelationMapping[i].Character->GetName()));

			if (i == CurrentTurnIndex)
			{
				int index = PlayerRelationMapping[i].Index;
				PlayerSetupData = PlayerStaticSetupData[index];				
				break;
			}
		}

		return PlayerSetupData;
	}

	/* Get Player State from turn index, player states must have the role ID corresponding to Current Turn Index*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static AClueless_PlayerState* GetPlayerStateFromTurnIndex(int CurrentTurnIndex, UObject* WorldContextObj)
	{
		AClueless_PlayerState* CPlayerState = nullptr;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return CPlayerState;


		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return CPlayerState;

		TArray<FPlayerCharacterRelationEntry> PlayerRelationMapping = GameState->GetCharatersRelationMapping();

		for (int i = 0; i < PlayerRelationMapping.Num(); i++)
		{
			if (i == CurrentTurnIndex)
			{
				if (PlayerRelationMapping[i].Character)
				{
					CPlayerState = PlayerRelationMapping[i].Character->GetPlayerState<AClueless_PlayerState>();
					break;
				}
			}

		}

		return CPlayerState;

	}

	/* This may return empty role ID*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static AClueless_PlayerState* GetPlayerStateFromRoleID(int RoleID, UObject* WorldContextObj)
	{
		AClueless_PlayerState* FoundPlayerState = nullptr;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		TArray<AClueless_PlayerState*> CActivePlayerStates;

		if (World)
			CActivePlayerStates = UGameplayAPI::GetActivePlayerStates(World);
			
		for (int i = 0; i < CActivePlayerStates.Num(); i++)
		{
			if (CActivePlayerStates[i]->GetRoleID() == RoleID)
			{
				FoundPlayerState = CActivePlayerStates[i];
			}
		}

		return FoundPlayerState;
	}

	/* Get Card Information from CardID*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static FCardEntityData GetCardFromCardID(FString CardID, UObject* WorldContextObj)
	{
		FCardEntityData FoundCard;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return FoundCard;

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return FoundCard;	

		TArray<FCardEntityData> CardEntityData = GameState->GetCardsSetupData();

		for (int i = 0; i < CardEntityData.Num(); i++)
		{
			if (CardEntityData[i].CardID == CardID)
			{
				FoundCard = CardEntityData[i];
			}
		}

		return FoundCard;
	}

	/* Get Current Player Turn Information*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static FPlayerTurnInfo GetCurrentTurnInfo(UObject* WorldContextObj)
	{
		FPlayerTurnInfo PlayerTurnInfo;
		PlayerTurnInfo.PlayerGamingAction = 0;
		PlayerTurnInfo.PlayerTurnIndex = -1;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return PlayerTurnInfo;

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return PlayerTurnInfo;

		return GameState->GetCurrentTurnInfo();
	}

	/* Check if certain action is allowed*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static bool IsAllowedAction(EPlayerGameAction Action, UObject* WorldContextObj)
	{
		FPlayerTurnInfo TurnInfo = UGameplayAPI::GetCurrentTurnInfo(WorldContextObj);
	
		return TurnInfo.IsActionAllowed(Action);
	}	

	/**
	* @brief For both client and server
	*/
	UFUNCTION(BlueprintCallable, Category = "GamePlay API", meta = (WorldContext = "WorldContextObj"))
		static int GetCurrentTurn(UObject* WorldContextObj)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return 0;

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return 0;

		return GameState->GetCurrentTurn();
	}

	/**
	* @brief Get Role Location client and server
	*/
	UFUNCTION(BlueprintCallable, Category = "GamePlay API", meta = (WorldContext = "WorldContextObj"))
		static ACharacter* GetCharacterFromRoleID(UObject* WorldContextObj, int RoleID)
	{
		FPlayerSetupStaticData RoleEntry;
		ACharacter* RoleCharacter = nullptr;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return RoleCharacter;

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		TArray<FPlayerCharacterRelationEntry> PlayerRelationMapping = GameState->GetCharatersRelationMapping();

		if (PlayerRelationMapping.Num() <= 0)
			return RoleCharacter;

		for (auto& Entry : PlayerRelationMapping)
		{
			if (Entry.Index == RoleID)
			{
				RoleCharacter = Entry.Character;
			}
		}

		return RoleCharacter;		
	}

	/**
	* @brief For both client and server
	*/
	UFUNCTION(BlueprintCallable, Category = "GamePlay API", meta = (WorldContext = "WorldContextObj"))
		static TArray<FPlayerRelationMappingEntry> GetCurrentData(ACharacter* CurrentCharacter, UObject* WorldContextObj)
	{
		TArray<FPlayerRelationMappingEntry> DynamicRelationMapping;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return DynamicRelationMapping;


		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return DynamicRelationMapping;

		TArray<FPlayerCharacterRelationEntry> PlayerRelationMapping = GameState->GetCharatersRelationMapping();

		if (PlayerRelationMapping.Num() <= 0)
			return DynamicRelationMapping;

		TArray<FPlayerSetupStaticData> PlayerStaticSetupData = GetPlayerStaticSetupData(World);

		for (int i = 0; i < PlayerRelationMapping.Num(); i++)
		{
			FPlayerRelationMappingEntry PlayerRelationEntry;

			int index = PlayerRelationMapping[i].Index;
			index = FMath::Clamp(index, 0, PlayerStaticSetupData.Num()-1);

			PlayerRelationEntry.PlayerStaticData = PlayerStaticSetupData[index];
			PlayerRelationEntry.IsYou = (CurrentCharacter == PlayerRelationMapping[i].Character);

			DynamicRelationMapping.Add(PlayerRelationEntry);
		}

		return DynamicRelationMapping;
	}

	/*
	* Get all currently active player states, works on both client and server.
	*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static TArray<AClueless_PlayerState*> GetActivePlayerStates(UObject* WorldContextObj)
	{
		TArray<AClueless_PlayerState*> ActivePlayerStates;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return ActivePlayerStates;

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return ActivePlayerStates;

		TArray<FPlayerCharacterRelationEntry> PlayerRelationMapping = GameState->GetCharatersRelationMapping();

		for (auto& Entry : PlayerRelationMapping)
		{
			APlayerState* PlayerState = Entry.Character->GetPlayerState();
			if (PlayerState != nullptr)
			{
				AClueless_PlayerState* CPlayerState = (AClueless_PlayerState*)PlayerState;
				if (CPlayerState)
					ActivePlayerStates.Add(CPlayerState);
			}
		}

		return ActivePlayerStates;		
	}

#pragma endregion Server and Client calls


#pragma region Server Calls
	/**
	* Update player character into Mapping.
	*/
	UFUNCTION(BlueprintCallable, Category = "GamePlay API", meta = (WorldContext = "WorldContextObj"))
		static void UpdatePlayerControllerWithCharacter_Server(ACharacter* CurrentCharacter, APlayerController* PlayerController, UObject* WorldContextObj)
	{
		TArray<FPlayerSetupStaticData> PlayerStaticSetupData;

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (!World)
			return;

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return;

		GameState->UpdatePlayerControllerWithCharacterOnServer(PlayerController, CurrentCharacter);
	}

	/* This will change the game state of the current game.
	* should be called on server
	*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static void ChangeClueGameState_Server(ClueGameState State, UObject* WorldContextObj)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (World == nullptr)
			return;

		ACGameStateBase* GameState = World->GetGameState<ACGameStateBase>();

		if (GameState == nullptr)
			return;

		GameState->ChangeGameState(State);
	}

	/* This will teleport the role ID's to its necessary position*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static void MakeTeleport_Server(UObject* WorldContextObj, int RoleID, int BlockID)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (World == nullptr)
			return;

		AGameModeBase* GM = World->GetAuthGameMode<AGameModeBase>();

		if (!GM) {
			UE_LOG(LogTemp, Warning, TEXT("GM from Client is not Valid"));
		}
		else
		{
			UCluelessMovementComponent* GameMovementModeComp = (UCluelessMovementComponent*)GM->GetComponentByClass(UCluelessMovementComponent::StaticClass());

			if (GameMovementModeComp != nullptr)
			{
				GameMovementModeComp->RoleMakeTeleport(BlockID, RoleID);
			}
		}
	}

	UFUNCTION(Blueprintcallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static void MakeAccusation_Server(UObject* WorldContextObj, int RoleID, FString CWeaponID, FString CRoleID, FString CRoomID)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (World == nullptr)
			return;

		AGameModeBase* GM = World->GetAuthGameMode<AGameModeBase>();

		if (!GM) {
			UE_LOG(LogTemp, Warning, TEXT("GM from Client is not Valid"));
		}
		else
		{
			UClueGameTurnBasedComponent* TurnBasedGameModeComp = (UClueGameTurnBasedComponent*)GM->GetComponentByClass(UClueGameTurnBasedComponent::StaticClass());

			if (TurnBasedGameModeComp)
			{
				TurnBasedGameModeComp->OnPlayerMakeAccusation(RoleID, CWeaponID, CRoleID, CRoomID);
			}
		}

	}

	UFUNCTION(BlueprintCallable, Category = "Gamplay API", meta = (WorldContext = "WorldContextObj"))
		static void MakeMovement_Server(UObject* WorldContextObj, int BlockID, int CurrentRoleID)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (World == nullptr)
			return;

		AGameModeBase* GM = World->GetAuthGameMode<AGameModeBase>();

		if (!GM) {
			UE_LOG(LogTemp, Warning, TEXT("GM from Client is not Valid"));
		}
		else
		{
			UCluelessMovementComponent* GameMovementModeComp = (UCluelessMovementComponent*)GM->GetComponentByClass(UCluelessMovementComponent::StaticClass());

			if (GameMovementModeComp != nullptr)
			{
				GameMovementModeComp->OnPlayerMakeMovement(BlockID, CurrentRoleID);
			}
		}

	}

	UFUNCTION(BlueprintCallable, Category = "Gamplay API", meta = (WorldContext = "WorldContextObj"))
		static void MakeSuggestion_Server(UObject* WorldContextObj, int RoleID, FString CWeaponID, FString CRoleID)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (World == nullptr)
			return;

		AGameModeBase* GM = World->GetAuthGameMode<AGameModeBase>();

		if (!GM) {
			UE_LOG(LogTemp, Warning, TEXT("GM from Client is not Valid"));
		}
		else
		{
			UClueGameTurnBasedComponent* TurnBasedGameModeComp = (UClueGameTurnBasedComponent*)GM->GetComponentByClass(UClueGameTurnBasedComponent::StaticClass());

			if (TurnBasedGameModeComp)
			{
				TurnBasedGameModeComp->OnPlayerMakeSuggestion(RoleID, CWeaponID, CRoleID);
			}
		}

	}

	UFUNCTION(BlueprintCallable, Category = "Gamplay API", meta = (WorldContext = "WorldContextObj"))
		static void EndPlayerTurn_Server(UObject* WorldContextObj)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (World == nullptr)
			return;

		AGameModeBase* GM = World->GetAuthGameMode<AGameModeBase>();

		if (!GM) {
			UE_LOG(LogTemp, Warning, TEXT("GM from Client is not Valid"));
		}
		else
		{
			UClueGameTurnBasedComponent* TurnBasedGameModeComp = (UClueGameTurnBasedComponent*)GM->GetComponentByClass(UClueGameTurnBasedComponent::StaticClass());

			if (TurnBasedGameModeComp)
			{
				TurnBasedGameModeComp->OnPlayerEndTurn();
			}
		}

	}

	UFUNCTION(BlueprintCallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static void SkipShowCard_Server(UObject* WorldContextObj, int RoleID)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (World == nullptr)
			return;

		AGameModeBase* GM = World->GetAuthGameMode<AGameModeBase>();

		if (!GM) {
			UE_LOG(LogTemp, Warning, TEXT("GM from Client is not Valid"));
		}
		else
		{
			UClueGameTurnBasedComponent* TurnBasedGameModeComp = (UClueGameTurnBasedComponent*)GM->GetComponentByClass(UClueGameTurnBasedComponent::StaticClass());

			if (TurnBasedGameModeComp)
			{
				TurnBasedGameModeComp->OnPlayerSkipShowCard(RoleID);
			}
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Gameplay API", meta = (WorldContext = "WorldContextObj"))
		static void ShowCard_Server(UObject* WorldContextObj, int RoleID, FString CardID)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObj, EGetWorldErrorMode::LogAndReturnNull);

		if (World == nullptr)
			return;

		AGameModeBase* GM = World->GetAuthGameMode<AGameModeBase>();

		if (!GM) {
			UE_LOG(LogTemp, Warning, TEXT("GM from Client is not Valid"));
		}
		else
		{
			UClueGameTurnBasedComponent* TurnBasedGameModeComp = (UClueGameTurnBasedComponent*)GM->GetComponentByClass(UClueGameTurnBasedComponent::StaticClass());

			if (TurnBasedGameModeComp)
			{
				TurnBasedGameModeComp->OnPlayerShowCard(RoleID, CardID);
			}
		}
		
	}

#pragma endregion Server Calls

};