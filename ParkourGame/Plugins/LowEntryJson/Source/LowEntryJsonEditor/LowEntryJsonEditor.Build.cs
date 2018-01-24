namespace UnrealBuildTool.Rules
{
	public class LowEntryJsonEditor : ModuleRules
	{
		public LowEntryJsonEditor(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(
				new string[] {
					"LowEntryJsonEditor/Public/Module",
					"LowEntryJsonEditor/Public/Classes",
				}
			);

			PrivateIncludePaths.AddRange(
				new string[] {
					"LowEntryJsonEditor/Private/Module",
					"LowEntryJsonEditor/Private/Classes",
				}
			);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Engine",
					"Core",
					"CoreUObject",
					"InputCore",
					"Slate",
					"EditorStyle",
					"AIModule",
					"BlueprintGraph",
					"LowEntryJson",
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"EditorStyle",
					"KismetCompiler",
					"UnrealEd",
					"GraphEditor",
					"SlateCore",
                    "Kismet",
                    "KismetWidgets",
                    "PropertyEditor",
                }
			);
		}
	}
}
