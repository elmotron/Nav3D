using UnrealBuildTool;

public class Nav3DEditor : ModuleRules
{
	public Nav3DEditor(ReadOnlyTargetRules Target) : base(Target)
	{
	    PublicDependencyModuleNames.AddRange(["Core", "CoreUObject", "Engine",  "Nav3D", "InputCore"]);
	    PrivateDependencyModuleNames.AddRange([
		    "Slate", 
		    "SlateCore", "PropertyEditor", "EditorStyle", "UnrealEd", "GraphEditor", "BlueprintGraph","DetailCustomizations"
	    ]);
	    PrivateIncludePaths.AddRange(["Nav3DEditor/Private"]);
	    PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
    }
};