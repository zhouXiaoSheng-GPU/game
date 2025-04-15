// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BurstingCold : ModuleRules
{
	public BurstingCold(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "MotionTrajectory", "PoseSearch","UMG" });
	}
}
