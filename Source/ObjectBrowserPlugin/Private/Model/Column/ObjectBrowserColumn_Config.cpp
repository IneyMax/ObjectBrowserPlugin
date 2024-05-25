

#include "Model/Column/ObjectBrowserColumn_Config.h"

#include "Item/ObjectTreeObjectItem.h"

#define LOCTEXT_NAMESPACE "ObjectBrowser"


FObjectDynamicColumn_Config::FObjectDynamicColumn_Config()
{
	Name = TEXT("Config");
	TableLabel = LOCTEXT("ObjectBrowser_Column_Config", "Config");
	ConfigLabel = LOCTEXT("ObjectBrowser_Column_Config", "Config");
	PreferredWidthRatio = 0.15f;
}

FText FObjectDynamicColumn_Config::ExtractText(TSharedRef<const IObjectTreeItem> Item) const
{
	if (const FObjectTreeObjectItem* ObjectItem = Item->GetAsObjectDescriptor())
	{
		return Item->IsConfigExportable() ? FText::FromName(ObjectItem->ConfigName) : FText::GetEmpty();
	}

	return FText::GetEmpty();
}

void FObjectDynamicColumn_Config::PopulateSearchStrings(const IObjectTreeItem& Item, TArray<FString>& OutSearchStrings) const
{
	if (const FObjectTreeObjectItem* ObjectItem = Item.GetAsObjectDescriptor())
	{
		if (ObjectItem->IsConfigExportable())
		{
			OutSearchStrings.Add(ObjectItem->ConfigName.ToString());
		}
	}
}

#undef LOCTEXT_NAMESPACE