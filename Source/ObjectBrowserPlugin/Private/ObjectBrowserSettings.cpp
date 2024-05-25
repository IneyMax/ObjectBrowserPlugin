

#include "ObjectBrowserSettings.h"
#include "ObjectBrowserModule.h"
#include "Model/ObjectBrowserModel.h"

UObjectBrowserSettings::FSettingChangedEvent UObjectBrowserSettings::SettingChangedEvent;

const FName FObjectBrowserConfigMeta::MD_ConfigAffectsView(TEXT("ConfigAffectsView"));
const FName FObjectBrowserConfigMeta::MD_ConfigAffectsColumns(TEXT("ConfigAffectsColumns"));
const FName FObjectBrowserConfigMeta::MD_ConfigAffectsDetails(TEXT("ConfigAffectsDetails"));

UObjectBrowserSettings::UObjectBrowserSettings()
{
}

void UObjectBrowserSettings::OnSettingsSelected()
{
	UE_LOG(LogObjectBrowser, Log, TEXT("Browser settings being selected"));

	SyncCategorySettings();
	SyncColumnSettings();
}

bool UObjectBrowserSettings::OnSettingsModified()
{
	UE_LOG(LogObjectBrowser, Log, TEXT("Browser settings being modified"));
	return true;
}

bool UObjectBrowserSettings::OnSettingsReset()
{
	UE_LOG(LogObjectBrowser, Log, TEXT("Browser settings being reset"));
	return true;
}

// Sync category settings with categories we have in module
void UObjectBrowserSettings::SyncCategorySettings()
{
	TMap<FName, bool> CurrentSettings;
	LoadDataFromConfig(CategoryVisibilityState, CurrentSettings);
	// clear from possible stale/removed categories
	CategoryVisibilityState.Empty();

	for (const auto& Category : FObjectBrowserModule::Get().GetCategories())
	{
		if (!CurrentSettings.Contains(Category->Name))
		{
			CurrentSettings.Emplace(Category->Name, Category->IsVisibleByDefault());
		}
	}

	StoreDataToConfig(CurrentSettings, CategoryVisibilityState);
}

void UObjectBrowserSettings::LoadCategoryStates(TMap<FName, bool>& States)
{
	SyncCategorySettings();
	LoadDataFromConfig(CategoryVisibilityState, States);
}

void UObjectBrowserSettings::SetCategoryStates(const TMap<FName, bool>& States)
{
	StoreDataToConfig(States, CategoryVisibilityState);
	NotifyPropertyChange(GET_MEMBER_NAME_CHECKED(ThisClass, CategoryVisibilityState));
}

void UObjectBrowserSettings::SetCategoryState(FName Category, bool State)
{
	SetConfigFlag(CategoryVisibilityState, Category, State);
	NotifyPropertyChange(GET_MEMBER_NAME_CHECKED(ThisClass, CategoryVisibilityState));
}

void UObjectBrowserSettings::LoadTreeExpansionStates(TMap<FName, bool>& States)
{
	LoadDataFromConfig(TreeExpansionState, States);
}

void UObjectBrowserSettings::SetTreeExpansionStates(TMap<FName, bool> const& States)
{
	StoreDataToConfig(States, TreeExpansionState);
	NotifyPropertyChange(GET_MEMBER_NAME_CHECKED(ThisClass, TreeExpansionState));
}

void UObjectBrowserSettings::SetSeparatorLocation(float NewValue)
{
	HorizontalSeparatorLocation = NewValue;
	NotifyPropertyChange(GET_MEMBER_NAME_CHECKED(ThisClass, HorizontalSeparatorLocation));
}

void UObjectBrowserSettings::SetColoringEnabled(bool bNewValue)
{
	bEnableColoring = bNewValue;
	NotifyPropertyChange(GET_MEMBER_NAME_CHECKED(ThisClass, bEnableColoring));
}

void UObjectBrowserSettings::SetShowHiddenProperties(bool bNewValue)
{
	bShowHiddenProperties = bNewValue;
	NotifyPropertyChange(GET_MEMBER_NAME_CHECKED(ThisClass, bShowHiddenProperties));
}

void UObjectBrowserSettings::SetShouldShowOnlyGame(bool bNewValue)
{
	bShowOnlyGameModules = bNewValue;
	NotifyPropertyChange(GET_MEMBER_NAME_CHECKED(ThisClass, bShowOnlyGameModules));
}

void UObjectBrowserSettings::SetShouldShowOnlyPlugins(bool bNewValue)
{
	bShowOnlyPluginModules = bNewValue;
	NotifyPropertyChange(GET_MEMBER_NAME_CHECKED(ThisClass, bShowOnlyPluginModules));
}

void UObjectBrowserSettings::SyncColumnSettings()
{
	TMap<FName, bool> CurrentSettings;
	LoadDataFromConfig(TableColumnVisibilityState, CurrentSettings);
	// clear from possible stale/removed categories
	TableColumnVisibilityState.Empty();

	for (const auto& DynamicColumn : FObjectBrowserModule::Get().GetDynamicColumns())
	{
		if (!CurrentSettings.Contains(DynamicColumn->Name))
		{
			CurrentSettings.Emplace(DynamicColumn->Name, DynamicColumn->IsVisibleByDefault());
		}
	}

	StoreDataToConfig(CurrentSettings, TableColumnVisibilityState);
}

bool UObjectBrowserSettings::GetTableColumnState(FName Column) const
{
	const FObjectBrowserConfigItem* bFoundState = TableColumnVisibilityState.FindByKey(Column);
	return bFoundState ? bFoundState->bValue : true;
}

void UObjectBrowserSettings::SetTableColumnState(FName Column, bool State)
{
	SetConfigFlag(TableColumnVisibilityState, Column, State);
	NotifyPropertyChange(GET_MEMBER_NAME_CHECKED(ThisClass, TableColumnVisibilityState));
}

void UObjectBrowserSettings::NotifyPropertyChange(FName PropertyName)
{
	UE_LOG(LogObjectBrowser, Log, TEXT("Property %s changed and called save"), *PropertyName.ToString());
	SaveConfig();

	SettingChangedEvent.Broadcast(PropertyName);
}

void UObjectBrowserSettings::PostLoad()
{
	UE_LOG(LogObjectBrowser, Log, TEXT("Browser settings being loaded"));

	Super::PostLoad();
}

void UObjectBrowserSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Take the class member property name instead of struct member
	FName PropertyName = (PropertyChangedEvent.MemberProperty ? PropertyChangedEvent.MemberProperty->GetFName() : PropertyChangedEvent.GetPropertyName());

	NotifyPropertyChange(PropertyName);
}
