// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChatSystem/Data/ChatSystemDynamicData.h"
#include "ChatSysBasePlayerStateComponent.generated.h"



UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom) )
class CHATSYSTEM_API UChatSysBasePlayerStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UChatSysBasePlayerStateComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;




protected:

	UPROPERTY(ReplicatedUsing = OnRep_Index)
		int32 Index;

	UFUNCTION()
		void OnRep_Index() {};



public:
	UFUNCTION(BlueprintCallable)
		void SetChatIndex(int32 ChatIndex)
	{
		Index = ChatIndex;
	};
#pragma region SendMessage

public:
	UFUNCTION(BlueprintCallable)
		void SendChatMessageToServer(FSingleChatMessage ChatMessage);

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category = "ChatSystem|CaneSendMessage")
		bool CanSendMessage(FSingleChatMessage ChatMessage);
	bool CanSendMessage_Implementation(FSingleChatMessage ChatMessage);

	UFUNCTION(Server, Reliable, WithValidation, Category = "ChatSystem|ServerSendMessage")
		void ServerSendChatMessage(FSingleChatMessage ChatMessage);

#pragma endregion



};
