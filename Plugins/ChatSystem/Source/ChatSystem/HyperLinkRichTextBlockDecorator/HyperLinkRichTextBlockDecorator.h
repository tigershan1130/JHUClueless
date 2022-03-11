// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "HyperLinkRichTextBlockDecorator.generated.h"


class FRichHyperLinkData
{
public:
	FRichHyperLinkData(UHyperLinkRichTextBlockDecorator* decorator, TMap<FString, FString> metaData);
	FSimpleDelegate m_LinkDelegate;
};


class FRichInlineHyperLinkDecorator : public FRichTextDecorator
{
public:
	FRichInlineHyperLinkDecorator(URichTextBlock* InOwner, UHyperLinkRichTextBlockDecorator* decorator);
	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override;
protected:
	virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const override;

	void ExplodeTextStyleInfo(TMap<FString, FString>& MetaData, FSlateFontInfo& OutFont, FLinearColor& OutFontColor) const;
	UHyperLinkRichTextBlockDecorator* m_Decorator;
	mutable FHyperlinkStyle m_Linkstyle;
	mutable FSlateFontInfo m_DefaultFont;
	mutable FSlateColor m_DefaultColor;
	mutable TArray<FRichHyperLinkData> m_LinkData;
	mutable uint8 m_curLinkIndex;
	mutable FString m_Content;
};
/**
 * 
 */
UCLASS()
class CHATSYSTEM_API UHyperLinkRichTextBlockDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:
	UHyperLinkRichTextBlockDecorator(const FObjectInitializer& ObjectInitializer);

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	UPROPERTY(EditAnywhere, Category = Appearance)
		FHyperlinkStyle LinkStyle;

	UFUNCTION(BlueprintNativeEvent)
		void ClickFun(const TMap<FString, FString>& metaData);
	
};
