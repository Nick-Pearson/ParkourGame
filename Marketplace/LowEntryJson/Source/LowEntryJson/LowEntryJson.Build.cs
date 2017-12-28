namespace UnrealBuildTool.Rules
{
	public class LowEntryJson : ModuleRules
	{
		public LowEntryJson(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(
				new string[] {
					"LowEntryJson/Public/Module",
					"LowEntryJson/Public/Classes",
					"LowEntryJson/Public/Enums",
				}
			);

			PrivateIncludePaths.AddRange(
				new string[] {
					"LowEntryJson/Private/Module",
					"LowEntryJson/Private/Classes",
				}
			);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Engine",
					"Core",
					"CoreUObject",
					"Json",
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
				}
			);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
				}
			);
		}
	}
}
