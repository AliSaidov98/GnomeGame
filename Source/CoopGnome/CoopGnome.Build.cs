// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CoopGnome : ModuleRules
{
	public CoopGnome(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "GameplayTasks", "AnimGraphRuntime", "OnlineSubsystem" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Weapons", "Health", "Inventory", "ActionSystem", "MultiplayerSessions" });
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
	}
}
