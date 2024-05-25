// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ObjectBrowserPlugin : ModuleRules
{
	public ObjectBrowserPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ApplicationCore",
				"InputCore",
				"UnrealEd",
				"EditorStyle",
				"WorkspaceMenuStructure",
				"WorldBrowser",
				"LevelEditor",
				"ToolMenus",
				"AssetTools",
				"Projects",
				"EditorFramework",
				"EditorConfig",
				"UncontrolledChangelists",
				"EditorWidgets",
				"ToolWidgets",
				"UnsavedAssetsTracker",
				"AssetDefinition",
				"TypedElementFramework",
				"TypedElementRuntime",
			}
			);
	}
}
