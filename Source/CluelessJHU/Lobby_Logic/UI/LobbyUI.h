// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUI.generated.h"

/**
 * 
 */
UCLASS(Config=Game)
class CLUELESSJHU_API ULobbyUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, BlueprintReadOnly)
		FString IPAddress;
	
};
