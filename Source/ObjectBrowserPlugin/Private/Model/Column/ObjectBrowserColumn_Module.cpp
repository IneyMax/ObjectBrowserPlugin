

#include "Model/Column/ObjectBrowserColumn_Module.h"

#include "Item/ObjectBrowserTreeObjectItem.h"

#define LOCTEXT_NAMESPACE "ObjectBrowser"


FObjectDynamicColumn_Module::FObjectDynamicColumn_Module()
{
	Name = TEXT("Module");
	TableLabel = LOCTEXT("ObjectBrowser_Column_Module", "Module");
	ConfigLabel = LOCTEXT("ObjectBrowser_Column_Module", "Module");
	PreferredWidthRatio = 0.25f;
}

FText FObjectDynamicColumn_Module::ExtractText(TSharedRef<const IObjectTreeItem> Item) const
{
	if (const FObjectBrowserTreeObjectItem* ObjectItem = Item->GetAsObjectDescriptor())
	{
		return FText::FromString(ObjectItem->ShortPackage);
	}

	return FText::GetEmpty();
}

void FObjectDynamicColumn_Module::PopulateSearchStrings(const IObjectTreeItem& Item, TArray<FString>& OutSearchStrings) const
{
	if (const FObjectBrowserTreeObjectItem* ObjectItem = Item.GetAsObjectDescriptor())
	{
		OutSearchStrings.Add(ObjectItem->ShortPackage);
	}
}

#undef LOCTEXT_NAMESPACE
