// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/NetSerialization.h"

#include "Engine/DataTable.h"
#include "UObject/SoftObjectPtr.h"
#include "ChatSystemDynamicData.generated.h"


class UChatSystemBaseGameStateComponent;


/*单条聊天信息*/
USTRUCT(BlueprintType)
struct  FSingleChatMessage
{
	GENERATED_USTRUCT_BODY();

public:
	/*消息ID*/
	UPROPERTY(BlueprintReadOnly)
		int64 MessageID;

	/*发送者ID*/
	UPROPERTY(BlueprintReadOnly)
		int32 SenderIndex;


	/*接收者ID*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ReceiverIndex = -1;

	/*消息时间戳*/
	UPROPERTY(BlueprintReadOnly)
		int64 TimeStamp;


	/*消息所在频道*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName MessageChannel;

	/*消息主体内容*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ChatContent;


	


	bool operator==(const FSingleChatMessage& lhs) const
	{
		return (MessageID == lhs.MessageID);
	}

};



/**/
USTRUCT()
struct FSingleChatMessageDynamic : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()
	/*	FSingleChatMessageDynamic(FSingleChatMessage SingleMessage)
	{};*/

		UPROPERTY()
		FSingleChatMessage SingleMessage;

	/** Optional functions you can implement for client side notification of changes to items */
	void PreReplicatedRemove(const struct FChatMessageContainer& ArraySerializer) {};
	void PostReplicatedAdd(const struct FChatMessageContainer& ArraySerializer);
	void PostReplicatedChange(const struct FChatMessageContainer& ArraySerializer) {};

	bool operator==(const FSingleChatMessageDynamic& lhs) const
	{
		return (SingleMessage == lhs.SingleMessage);
	}
};


USTRUCT()
struct FChatMessageContainer : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()
		FChatMessageContainer()
	{};

	UPROPERTY()
		TArray<FSingleChatMessageDynamic> Messages;

	UPROPERTY()
		UChatSystemBaseGameStateComponent* ChatGameStateComponent;

	void SetChatSysGameStateComponent(UChatSystemBaseGameStateComponent* ChatSystemComponent);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSingleChatMessageDynamic>(Messages, DeltaParms, *this);
	}
};



template<>
struct TStructOpsTypeTraits<FChatMessageContainer> : public TStructOpsTypeTraitsBase2<FChatMessageContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};







USTRUCT(BlueprintType)
struct FSingleTypeChatMessage
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FSingleChatMessage> ChatMessages;


};


USTRUCT(BlueprintType)
struct FChatMessages
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FSingleTypeChatMessage> MessageS;

};