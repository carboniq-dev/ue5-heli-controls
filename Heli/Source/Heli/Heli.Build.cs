// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Heli : ModuleRules
{
	public Heli(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Niagara", "Landscape", "EnhancedInput" });
	}
}
