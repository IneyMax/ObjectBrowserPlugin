

#include "Model/Column/ObjectBrowserColumn_Plugin.h"

#include "Item/IObjectTreeItem.h"
#include "Item/ObjectTreeObjectItem.h"

#define LOCTEXT_NAMESPACE "ObjectBrowser"


FObjectDynamicColumn_Plugin::FObjectDynamicColumn_Plugin()
{
	Name = TEXT("Plugin");
	TableLabel = LOCTEXT("ObjectBrowser_Column_Plugin", "Plugin");
	ConfigLabel = LOCTEXT("ObjectBrowser_Column_Plugin", "Plugin");
	PreferredWidthRatio = 0.25f;
}

FText FObjectDynamicColumn_Plugin::ExtractText(TSharedRef<const IObjectTreeItem> Item) const
{
	if (const FObjectTreeObjectItem* ObjectItem = Item->GetAsObjectDescriptor())
	{
		return FText::FromString(ObjectItem->PluginDisplayName);
	}

	return FText::GetEmpty();
}

void FObjectDynamicColumn_Plugin::PopulateSearchStrings(const IObjectTreeItem& Item, TArray<FString>& OutSearchStrings) const
{
	if (const FObjectTreeObjectItem* ObjectItem = Item.GetAsObjectDescriptor())
	{
		OutSearchStrings.Add(ObjectItem->PluginDisplayName);
	}
}

#undef LOCTEXT_NAMESPACE
