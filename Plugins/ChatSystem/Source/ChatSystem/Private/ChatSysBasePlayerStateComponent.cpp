// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatSysBasePlayerStateComponent.h"
#include "Net/UnrealNetwork.h"
#include "ChatSystemBaseGameStateComponent.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"

// Sets default values for this component's properties
UChatSysBasePlayerStateComponent::UChatSysBasePlayerStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UChatSysBasePlayerStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;
	SharedParams.Condition = COND_ReplayOrOwner;
}

// Called when the game starts
void UChatSysBasePlayerStateComponent::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	if (World->IsServer())
	{
		AGameStateBase* GameStae = World->GetGameState();
		if (GameStae == nullptr)
		{
			return;
		}
		UChatSystemBaseGameStateComponent* ChatSysGameStateComponent = Cast<UChatSystemBaseGameStateComponent>(GameStae->GetComponentByClass(UChatSystemBaseGameStateComponent::StaticClass()));

		if (ChatSysGameStateComponent == nullptr)
		{
			return;
		}
		ChatSysGameStateComponent->RegeisterToChatSystem(this,Index);
	}
	// ...
}

//void UChatSysBasePlayerStateComponent::OnReceiveChatMessageOnServer(FSingleChatMessage SingleChatMessage)
//{
//
//	FSingleChatMessageDynamic DynamicMessage;
//	DynamicMessage.SingleMessage = SingleChatMessage;
//	ReceivedMessages.Messages.Add(DynamicMessage);
//	ReceivedMessages.MarkItemDirty(DynamicMessage);
//
//
//}

//void UChatSysBasePlayerStateComponent::OnChatMessageAdd(FSingleChatMessage SingleChatMessage)
//{
//	if (ChatMessageMap.MessageS.Contains(SingleChatMessage.MessageType))
//	{
//		ChatMessageMap.MessageS[SingleChatMessage.MessageType].ChatMessages.Add(SingleChatMessage);
//	}
//	else
//	{
//		FSingleTypeChatMessage SingleTypeChatMessage;
//		SingleTypeChatMessage.ChatMessages.Add(SingleChatMessage);
//		ChatMessageMap.MessageS.Add(SingleChatMessage.MessageType, SingleTypeChatMessage);
//	}
//	OnReceiveMessageOnClient(SingleChatMessage);
//}






void UChatSysBasePlayerStateComponent::SendChatMessageToServer(FSingleChatMessage ChatMessage)
{

	if (CanSendMessage(ChatMessage))
	{
		ServerSendChatMessage(ChatMessage);
	}
}

bool UChatSysBasePlayerStateComponent::CanSendMessage_Implementation(FSingleChatMessage ChatMessage)
{
	return true;
}

void UChatSysBasePlayerStateComponent::ServerSendChatMessage_Implementation(FSingleChatMessage ChatMessage)
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	AGameStateBase* GameStae = World->GetGameState();
	if (GameStae == nullptr)
	{
		return;
	}
	UChatSystemBaseGameStateComponent* ChatSysGameStateComponent = Cast<UChatSystemBaseGameStateComponent>(GameStae->GetComponentByClass(UChatSystemBaseGameStateComponent::StaticClass()));

	if (ChatSysGameStateComponent == nullptr)
	{
		return;
	}

	ChatMessage.SenderIndex = Index;

	ChatSysGameStateComponent->ServerReceiveChatMessage(ChatMessage);
}

bool UChatSysBasePlayerStateComponent::ServerSendChatMessage_Validate(FSingleChatMessage ChatMessage)
{
	return true;
}




