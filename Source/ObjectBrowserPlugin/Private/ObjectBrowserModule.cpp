


#include "ObjectBrowserModule.h"

#include "ObjectBrowserUtils.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "LevelEditor.h"
#include "ObjectBrowserSettings.h"
#include "ObjectBrowserStyle.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Model/Category/ObjectBrowserCategory_Default.h"
#include "Model/Column/ObjectBrowserColumn_Config.h"
#include "Model/Column/ObjectBrowserColumn_Module.h"
#include "Model/Column/ObjectBrowserColumn_Name.h"
#include "Model/Column/ObjectBrowserColumn_Plugin.h"
#include "UI/SObjectBrowser.h"


IMPLEMENT_MODULE(FObjectBrowserModule, ObjectBrowser);

DEFINE_LOG_CATEGORY(LogObjectBrowser);

#define LOCTEXT_NAMESPACE "ObjectBrowser"

const FName FObjectBrowserModule::ObjectBrowserTabName = TEXT("ObjectBrowserTab");
const FName FObjectBrowserModule::ObjectBrowserContextMenuName = TEXT("ObjectBrowser.ContextMenu");

FObjectBrowserModule::FOnGetObjectOwnerName FObjectBrowserModule::OnGetObjectOwnerName;
FObjectBrowserModule::FOnGenerateTooltip FObjectBrowserModule::OnGenerateTooltip;
FObjectBrowserModule::FOnGenerateMenu FObjectBrowserModule::OnGenerateContextMenu;

static const FName PanelIconName(TEXT("Icons.Settings"));


void FObjectBrowserModule::StartupModule()
{
	if (GIsEditor && !IsRunningCommandlet())
	{
		FObjectBrowserStyle::Register();

		UObjectBrowserSettings* SettingsObject = UObjectBrowserSettings::Get();

		ISettingsModule& SettingsModule = FModuleManager::GetModuleChecked<ISettingsModule>("Settings");
		SettingsSection = SettingsModule.RegisterSettings(
			TEXT("Editor"), TEXT("Plugins"), TEXT("ObjectBrowser"),
			LOCTEXT("ObjectBrowserSettingsName", "Object Browser"),
			LOCTEXT("ObjectBrowserSettingsDescription", "Settings for Object Browser Plugin"),
			SettingsObject
		);
		SettingsSection->OnSelect().BindUObject(SettingsObject, &UObjectBrowserSettings::OnSettingsSelected);
		//SettingsSection->OnResetDefaults().BindUObject(SettingsObject, &UObjectBrowserSettings::OnSettingsReset);

		FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
		LevelEditorModule.OnTabManagerChanged().AddLambda([ &LevelEditorModule ]()
		{
			TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();
			if (LevelEditorTabManager.IsValid())
			{
				LevelEditorTabManager->RegisterTabSpawner(ObjectBrowserTabName, FOnSpawnTab::CreateStatic(&FObjectBrowserModule::HandleTabManagerSpawnTab))
					.SetDisplayName(LOCTEXT("ObjectBrowserTitle", "Object Browser"))
					.SetTooltipText(LOCTEXT("ObjectBrowserTooltip", "Open the Object Browser tab."))
					.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory())
					.SetIcon( FStyleHelper::GetSlateIcon(PanelIconName) );
			}
		});
		
		UToolMenus::Get()->RegisterMenu(ObjectBrowserContextMenuName);
		
		RegisterDynamicColumn(MakeShared<FObjectDynamicColumn_Module>());
		RegisterDynamicColumn(MakeShared<FObjectDynamicColumn_Config>());
		RegisterDynamicColumn(MakeShared<FObjectDynamicColumn_Plugin>());
		
		RegisterCategory<FObjectCategory_Default>();
	}
}

void FObjectBrowserModule::ShutdownModule()
{
	if (GIsEditor && !IsRunningCommandlet())
	{
		FLevelEditorModule* LevelEditorModule = FModuleManager::GetModulePtr<FLevelEditorModule>(TEXT("LevelEditor"));
		if (LevelEditorModule)
		{
			LevelEditorModule->GetLevelEditorTabManager()->UnregisterTabSpawner(ObjectBrowserTabName);
		}

		FObjectBrowserStyle::UnRegister();
	}
}

TSharedRef<SDockTab> FObjectBrowserModule::HandleTabManagerSpawnTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(LOCTEXT("ObjectBrowserTitle", "Object Browser"))
	[
		SNew(SBorder)
		//.Padding(4)
		.BorderImage( FStyleHelper::GetBrush("ToolPanel.GroupBorder") )
		[
			CreateObjectBrowserWidget(Args)
		]
	];
}

TSharedRef<SWidget> FObjectBrowserModule::CreateObjectBrowserWidget(const FSpawnTabArgs& Args)
{
	UWorld* EditorWorld = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	return SNew(SObjectBrowser).InWorld(EditorWorld);
}

void FObjectBrowserModule::SummonObjectTab()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	TSharedPtr<ILevelEditor> LevelEditorInstance = LevelEditorModule.GetLevelEditorInstance().Pin();
	FGlobalTabmanager::Get()->TryInvokeTab(ObjectBrowserTabName);
}

void FObjectBrowserModule::SummonPluginSettingsTab()
{
	ISettingsModule& Module = FModuleManager::GetModuleChecked<ISettingsModule>("Settings");
	Module.ShowViewer(TEXT("Editor"), TEXT("Plugins"), TEXT("ObjectBrowser"));
}

const TArray<FObjectCategoryPtr>& FObjectBrowserModule::GetCategories() const
{
	return Categories;
}

const TArray<FObjectColumnPtr>& FObjectBrowserModule::GetDynamicColumns() const
{
	return DynamicColumns;
}

void FObjectBrowserModule::AddPermanentColumns(TArray<FObjectColumnPtr>& Columns)
{
	Columns.Add(MakeShared<FObjectDynamicColumn_Name>());
}

void FObjectBrowserModule::RegisterCategory(TSharedRef<FObjectCategoryBase> InCategory)
{
	if (InCategory->Name.IsNone())
	{
		UE_LOG(LogObjectBrowser, Error, TEXT("Invalid category being registered"));
		return;
	}

	for (const FObjectCategoryPtr& Category : Categories)
	{
		if (Category->Name == InCategory->Name)
		{
			UE_LOG(LogObjectBrowser, Error, TEXT("Duplicating category with name %s."), *Category->Name.ToString());
			return;
		}
	}

	Categories.Add(InCategory);
}

void FObjectBrowserModule::RemoveCategory(FName CategoryName)
{
	for (auto It = Categories.CreateIterator(); It; ++It)
	{
		if ((*It)->Name == CategoryName)
		{
			It.RemoveCurrent();
		}
	}
}

void FObjectBrowserModule::RegisterDynamicColumn(TSharedRef<FObjectDynamicColumn> InColumn)
{
	if (!FObjectDynamicColumn::IsValidColumnName(InColumn->Name))
	{
		UE_LOG(LogObjectBrowser, Error, TEXT("Invalid column being registered"));
		return;
	}

	for (FObjectColumnPtr& Column : DynamicColumns)
	{
		if (Column->Name == InColumn->Name)
		{
			UE_LOG(LogObjectBrowser, Error, TEXT("Duplicating column with name %s."), *Column->Name.ToString());
			return;
		}
	}

	DynamicColumns.Add(InColumn);

	// Sort columns by order
	DynamicColumns.StableSort(FObjectColumnSorter());
}

#undef LOCTEXT_NAMESPACE
