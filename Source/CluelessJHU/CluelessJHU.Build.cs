// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class CluelessJHU : ModuleRules
{
	public CluelessJHU(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "StaticDataTableManager", "JsonUtilities", "Json", "UMG", "NetCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {"StaticDataTableManager" , "JsonUtilities"});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
