using UnrealBuildTool;

public class ANS_EditorAudioToolsUE : ModuleRules
{
	public ANS_EditorAudioToolsUE(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange([]);
		PrivateIncludePaths.AddRange([]);
		
		PublicDependencyModuleNames.AddRange(["Core"]);
		PrivateDependencyModuleNames.AddRange([
				"AssetManagerEditor",
				"AssetRegistry",
				"Blutility",
				"CoreUObject",
				"EditorScriptingUtilities",
				"Engine",
				"Slate",
				"SlateCore",
				"UMG",
				"UnrealEd"
			]
		);
		
		DynamicallyLoadedModuleNames.AddRange([]);
	}
}
