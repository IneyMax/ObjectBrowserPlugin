

#include "Model/Column/ObjectBrowserColumn_Config.h"

#include "UI/ObjectBrowserTableItem.h"

#define LOCTEXT_NAMESPACE "ObjectBrowser"

FObjectDynamicColumn_Config::FObjectDynamicColumn_Config()
{
	Name = TEXT("Config");
	TableLabel = LOCTEXT("ObjectBrowser_Column_Config", "Config");
	ConfigLabel = LOCTEXT("ObjectBrowser_Column_Config", "Config");
	PreferredWidthRatio = 0.15f;
}

FText FObjectDynamicColumn_Config::ExtractText(TSharedRef<const ISubsystemTreeItem> Item) const
{
	if (const FObjectTreeSubsystemItem* SubsystemItem = Item->GetAsSubsystemDescriptor())
	{
		return Item->IsConfigExportable() ? FText::FromName(SubsystemItem->ConfigName) : FText::GetEmpty();
	}

	return FText::GetEmpty();
}

void FObjectDynamicColumn_Config::PopulateSearchStrings(const ISubsystemTreeItem& Item, TArray<FString>& OutSearchStrings) const
{
	if (const FObjectTreeSubsystemItem* SubsystemItem = Item.GetAsSubsystemDescriptor())
	{
		if (SubsystemItem->IsConfigExportable())
		{
			OutSearchStrings.Add(SubsystemItem->ConfigName.ToString());
		}
	}
}

#undef LOCTEXT_NAMESPACE