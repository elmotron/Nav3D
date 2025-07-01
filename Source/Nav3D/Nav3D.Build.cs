
using UnrealBuildTool;
using System.IO;

public class Nav3D : ModuleRules
{
	public Nav3D(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			[
				Path.Combine(ModuleDirectory, "Public")
				// ... add public include paths required here ...
			]
		);

		PrivateIncludePaths.AddRange(
			[
				"Nav3D/Private"
			]
		);

		PublicDependencyModuleNames.AddRange(
			[
				"Core",
				"AIModule",
				"NavigationSystem",
				"GameplayTasks"
			]
		);

		if (Target.bBuildEditor)
		{
			PublicDependencyModuleNames.Add("UnrealEd");
		}

		PrivateDependencyModuleNames.AddRange(
			[
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore"
			]
		);

		DynamicallyLoadedModuleNames.AddRange(
			[]
		);
	}
}
