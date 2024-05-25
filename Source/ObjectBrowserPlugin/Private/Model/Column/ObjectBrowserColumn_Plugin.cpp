

#include "Model/Column/ObjectBrowserColumn_Plugin.h"

#include "UI/ObjectBrowserTableItem.h"

#define LOCTEXT_NAMESPACE "ObjectBrowser"

FObjectDynamicColumn_Plugin::FObjectDynamicColumn_Plugin()
{
	Name = TEXT("Plugin");
	TableLabel = LOCTEXT("ObjectBrowser_Column_Plugin", "Plugin");
	ConfigLabel = LOCTEXT("ObjectBrowser_Column_Plugin", "Plugin");
	PreferredWidthRatio = 0.25f;
}

FText FObjectDynamicColumn_Plugin::ExtractText(TSharedRef<const ISubsystemTreeItem> Item) const
{
	if (const FObjectTreeSubsystemItem* SubsystemItem = Item->GetAsSubsystemDescriptor())
	{
		return FText::FromString(SubsystemItem->PluginDisplayName);
	}

	return FText::GetEmpty();
}

void FObjectDynamicColumn_Plugin::PopulateSearchStrings(const ISubsystemTreeItem& Item, TArray<FString>& OutSearchStrings) const
{
	if (const FObjectTreeSubsystemItem* SubsystemItem = Item.GetAsSubsystemDescriptor())
	{
		OutSearchStrings.Add(SubsystemItem->PluginDisplayName);
	}
}

#undef LOCTEXT_NAMESPACE
