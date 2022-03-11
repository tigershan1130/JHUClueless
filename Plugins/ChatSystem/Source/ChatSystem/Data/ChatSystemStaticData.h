// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "ChatSystemStaticData.generated.h"


UENUM(BlueprintType)
enum class  EChannelReceiveTarget :uint8
{
	Custom,			//自定义

	ALl,			//所有对象 

	AllAlive,		//存活对象
	
	AllDead,		//死亡对象
	
	BadGuy,			//狼人
	
	GoodGuy,		//好人
};

/**
 * 
 */
UCLASS()
class CHATSYSTEM_API UChatSystemStaticData : public UDataAsset
{
	GENERATED_BODY()
	
};



/*
聊天Channel数据结构
*/
USTRUCT(BlueprintType)
struct FChatSystemChannelData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

public:
	/*频道名称*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ChannelName;

	/*频道的接收对象*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EChannelReceiveTarget ChannelReceiveTarget;
};




/*
预设富文本超链接 数据
*/
USTRUCT(BlueprintType)
struct FHyperLinkChatMessageData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

public:
	/*预设名称*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName MessageName;

	/*预设描述*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Description;

	/*预设文本*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString MessageContant;


	/*附带TagTag*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTag SelfGamePlayTag;
};