// Fill out your copyright notice in the Description page of Project Settings.


#include "HyperLinkRichTextBlockDecorator.h"

#include "Components/RichTextBlockDecorator.h"
#include "Widgets/Input/SRichTextHyperLink.h"

FRichHyperLinkData::FRichHyperLinkData(UHyperLinkRichTextBlockDecorator* decorator, TMap<FString, FString> metaData)
{
	// m_Decorator = decorator;
	// m_MetaData = metaData;
	m_LinkDelegate.BindLambda([=]() {
		if (decorator)
		{
			decorator->ClickFun(metaData);
		}
		});
}


FRichInlineHyperLinkDecorator::FRichInlineHyperLinkDecorator(URichTextBlock* InOwner, UHyperLinkRichTextBlockDecorator* decorator)
	:FRichTextDecorator(InOwner)
{
	m_Decorator = decorator;
	m_Linkstyle = decorator->LinkStyle;
	m_DefaultFont = m_Linkstyle.TextStyle.Font;
	m_DefaultColor = m_Linkstyle.TextStyle.ColorAndOpacity;
}

bool FRichInlineHyperLinkDecorator::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const
{

	if (RunParseResult.Name == TEXT("a"))
	{
		/*ExplodeTextStyleInfo(RunParseResult.MetaData)*/

		TMap<FString, FString> metaData;
		TArray<FString> keys;
		RunParseResult.MetaData.GetKeys(keys);
		for (int i = 0; i < keys.Num(); i++)
		{
			const FTextRange& curRange = RunParseResult.MetaData[keys[i]];
			FString valueString = Text.Mid(curRange.BeginIndex, curRange.EndIndex - curRange.BeginIndex);
			metaData.Add(keys[i], valueString);
		}

		if (metaData.Contains("font") || metaData.Contains("size") || metaData.Contains("style") || metaData.Contains("color"))
		{
			FSlateFontInfo OutFont;
			FLinearColor OutColor;
			ExplodeTextStyleInfo(metaData, OutFont, OutColor);
			m_Linkstyle.TextStyle.SetFont(OutFont);
			FSlateColor color = FSlateColor(OutColor);
			m_Linkstyle.TextStyle.ColorAndOpacity = color;
		}
		else
		{
			m_Linkstyle.TextStyle.SetFont(m_DefaultFont);
			m_Linkstyle.TextStyle.ColorAndOpacity = m_DefaultColor;
		}

		m_LinkData.Add(FRichHyperLinkData(m_Decorator, metaData));
		m_curLinkIndex = m_LinkData.Num() - 1;
		m_Content = Text.Mid(
			RunParseResult.ContentRange.BeginIndex, RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);
		return true;
	}

	return false;

 	
}

TSharedPtr<SWidget> FRichInlineHyperLinkDecorator::CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const
{

	const bool bWarnIfMissing = true;

	TSharedPtr<FSlateHyperlinkRun::FWidgetViewModel> model = MakeShareable(new FSlateHyperlinkRun::FWidgetViewModel);

	TSharedPtr<SRichTextHyperlink> link = SNew(SRichTextHyperlink, model.ToSharedRef())
		.Text(FText::FromString(m_Content))
		.Style(&m_Linkstyle)
		.OnNavigate(m_LinkData[m_curLinkIndex].m_LinkDelegate);

	return link;
	
}

void FRichInlineHyperLinkDecorator::ExplodeTextStyleInfo(TMap<FString, FString>& MetaData, FSlateFontInfo& OutFont, FLinearColor& OutFontColor) const
{
	OutFont = m_DefaultFont;

	const FString* const FontFamilyString = MetaData.Find(TEXT("font"));
	const FString* const FontSizeString = MetaData.Find(TEXT("size"));
	const FString* const FontStyleString = MetaData.Find(TEXT("style"));
	const FString* const FontColorString = MetaData.Find(TEXT("color"));

	if (FontFamilyString)
	{
		FSoftObjectPath Font(**FontFamilyString);
		if (UObject* FontAsset = Font.TryLoad())
		{
			OutFont.FontObject = FontAsset;
		}
		MetaData.Remove("font");
	}

	if (FontSizeString)
	{
		OutFont.Size = static_cast<uint8>(FPlatformString::Atoi(**FontSizeString));
		MetaData.Remove("size");
	}

	if (FontStyleString)
	{
		OutFont.TypefaceFontName = FName(**FontStyleString);
		MetaData.Remove("style");
	}

	OutFontColor = m_DefaultColor.GetSpecifiedColor();
	if (FontColorString)
	{
		const FString& FontColorStringRef = *FontColorString;

		// Is Hex color?
		if (!FontColorStringRef.IsEmpty() && FontColorStringRef[0] == TCHAR('#'))
		{
			OutFontColor = FLinearColor(FColor::FromHex(FontColorStringRef));
		}
		else if (OutFontColor.InitFromString(*FontColorString))
		{
			// Initialized
		}
		else
		{
			OutFontColor = m_DefaultColor.GetSpecifiedColor();
		}
		MetaData.Remove("color");
	}

}


UHyperLinkRichTextBlockDecorator::UHyperLinkRichTextBlockDecorator(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

TSharedPtr<ITextDecorator> UHyperLinkRichTextBlockDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return MakeShareable(new FRichInlineHyperLinkDecorator(InOwner, this));
}

void UHyperLinkRichTextBlockDecorator::ClickFun_Implementation(const TMap<FString, FString>& metaData)
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Red, "click link");
	}
}

