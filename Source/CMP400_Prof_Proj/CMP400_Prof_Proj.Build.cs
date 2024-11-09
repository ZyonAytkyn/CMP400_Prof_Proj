// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CMP400_Prof_Proj : ModuleRules
{
	public CMP400_Prof_Proj(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
