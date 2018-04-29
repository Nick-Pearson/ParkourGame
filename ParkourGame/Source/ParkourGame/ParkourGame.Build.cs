// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ParkourGame : ModuleRules
{
	public ParkourGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "PhysX",
            "APEX"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "HTTP",
            "LowEntryJson",
            "NetworkReplayStreaming"
        });
	}
}
