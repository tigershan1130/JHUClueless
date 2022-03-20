// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CluelessJHU/Data/Game_StaticData.h"
#include "Math/UnrealMathUtility.h"
#include "CluelessJHU/Game_Logic/CGameStateBase.h"
#include "GameplayAPI.generated.h"

/**
 * 
 */
UCLASS()
class CLUELESSJHU_API UGameplayAPI : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/**
	 * @brief 
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
	* @brief 
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


	/**
	* Update player character into Mapping.
	*/
	UFUNCTION(BlueprintCallable, Category = "GamePlay API", meta = (WorldContext = "WorldContextObj"))
	static 	void UpdatePlayerControllerWithCharacter_Server(ACharacter* CurrentCharacter, APlayerController* PlayerController, UObject* WorldContextObj)
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

	/*
	
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


	// This will change the game state of the current game.
	// should be called on server
	UFUNCTION(BlueprintCallable, Category = "Gamplay  API", meta = (WorldContext = "WorldContextObj"))
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



};