using UnrealBuildTool;

public class ANS_SoundbankManagerFMOD : ModuleRules
{
    public ANS_SoundbankManagerFMOD(ReadOnlyTargetRules target) : base(target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange([]);
        PrivateIncludePaths.AddRange([]);
		
        PublicDependencyModuleNames.AddRange(["Core", "FMODStudio"]);

        PrivateDependencyModuleNames.AddRange([
                "CoreUObject",
                "Engine",
                "FMODStudio"
            ]
        );
    }
}