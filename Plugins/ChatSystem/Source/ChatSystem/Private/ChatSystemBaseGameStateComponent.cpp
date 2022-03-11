// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatSystemBaseGameStateComponent.h"
#include "Net/UnrealNetwork.h"




DEFINE_LOG_CATEGORY(ChatSystem);

// Sets default values for this component's properties
UChatSystemBaseGameStateComponent::UChatSystemBaseGameStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	

	// ...
}

void UChatSystemBaseGameStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;
	SharedParams.Condition = COND_None;

	DOREPLIFETIME_WITH_PARAMS_FAST(UChatSystemBaseGameStateComponent, ChatMesaageContainer, SharedParams);
	
}


// Called when the game starts
void UChatSystemBaseGameStateComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	//FastArray×¢²áChatSysGameStateConponent
	ChatMesaageContainer.SetChatSysGameStateComponent(this);
}

void UChatSystemBaseGameStateComponent::ServerReceiveChatMessage(FSingleChatMessage ChatMessage)
{
	ChatMessage.MessageID = MessageID++;
	FSingleChatMessageDynamic DynamicMessage;
	DynamicMessage.SingleMessage = ChatMessage;
	ChatMesaageContainer.Messages.Add(DynamicMessage);
	ChatMesaageContainer.MarkItemDirty(DynamicMessage);

	if (GetNetMode() == ENetMode::NM_ListenServer)
	{
		OnClientReceiveChatMessageAdd(DynamicMessage.SingleMessage);
	}
}

void UChatSystemBaseGameStateComponent::OnClientReceiveChatMessageAdd_Implementation(const FSingleChatMessage& ChatMessage)
{

	UE_LOG(ChatSystem, Log, TEXT("ChatMessageAddOnClient,MessageIndex= %D"), ChatMessage.MessageID);
}

