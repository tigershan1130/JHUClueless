// Copyright 2021 Justin Kiesskalt, All Rights Reserved.


#include "OSLSettings.h"
#include "OSLWidget.h"

#define LOCTEXT_NAMESPACE "OSL"

UOSLSettings::UOSLSettings(const FObjectInitializer& OI) : Super(OI)
{

}

bool UOSLSettings::GetIsDisabled() const
{
	#if UE_BUILD_SHIPPING
	return bDisableInShipping;
	#else
	return false;
	#endif
}

FName UOSLSettings::GetContainerName() const
{
	return TEXT("Project");
}

FName UOSLSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

FName UOSLSettings::GetSectionName() const
{
	return TEXT("On Screen Logging");
}

#if WITH_EDITOR
FText UOSLSettings::GetSectionText() const
{
	return NSLOCTEXT("OSLPlugin", "OSLSectionText", "On Screen Logging");
}

FText UOSLSettings::GetSectionDescription() const
{
	return NSLOCTEXT("OSLPlugin", "OSLSectionDescription", "General Configuration for the On Screen Logging Plugin");
}
#endif

#undef LOCTEXT_NAMESPACE