// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChatSystem/Data/ChatSystemDynamicData.h"

#include "ChatSystemBaseGameStateComponent.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(ChatSystem, Log, All);

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom) )
class CHATSYSTEM_API UChatSystemBaseGameStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UChatSystemBaseGameStateComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;



public:
	

protected:
	UPROPERTY(ReplicatedUsing = OnRep_ChatMesaageContainer)
		FChatMessageContainer ChatMesaageContainer;


	UFUNCTION()
		void OnRep_ChatMesaageContainer() {};

	UPROPERTY(BlueprintReadOnly, Category = "ChatSystem")
		TArray<UChatSysBasePlayerStateComponent*> ChatPlayers;
	
private:
	UPROPERTY()
		int64 MessageID;



public:
	UFUNCTION()
		void RegeisterToChatSystem(UChatSysBasePlayerStateComponent* ChatPlayer, int32& Index)
	{
		Index = ChatPlayers.Num();
		ChatPlayers.Add(ChatPlayer);
	};

	UFUNCTION()
		void UnRegeisterToChatSystem(int32 Index)
	{};


public:
	UFUNCTION()
		void ServerReceiveChatMessage(FSingleChatMessage ChatMessage);



#pragma region ChatMessageData

public:
	UFUNCTION(BlueprintNativeEvent,Category = "ChatSystemMessageData")
		void OnClientReceiveChatMessageAdd(const FSingleChatMessage& ChatMessage);
	void OnClientReceiveChatMessageAdd_Implementation(const FSingleChatMessage& ChatMessage);

#pragma endregion
};
