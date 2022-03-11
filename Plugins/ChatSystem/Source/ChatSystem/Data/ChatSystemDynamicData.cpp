#include "ChatSystemDynamicData.h"
#include "ChatSystem/Public/ChatSysBasePlayerStateComponent.h"
#include "ChatSystem/Public/ChatSystemBaseGameStateComponent.h"

void FChatMessageContainer::SetChatSysGameStateComponent(UChatSystemBaseGameStateComponent* ChatSystemComponent)
{
	ChatGameStateComponent = ChatSystemComponent;
}

void FSingleChatMessageDynamic::PostReplicatedAdd(const FChatMessageContainer& ArraySerializer)
{
	if (ArraySerializer.ChatGameStateComponent)
	{
		ArraySerializer.ChatGameStateComponent->OnClientReceiveChatMessageAdd(SingleMessage);
	}
}