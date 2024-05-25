

#include "Model/Column/ObjectBrowserColumn_Module.h"

#include "UI/ObjectBrowserTableItem.h"

#define LOCTEXT_NAMESPACE "ObjectBrowser"

FObjectDynamicColumn_Module::FObjectDynamicColumn_Module()
{
	Name = TEXT("Module");
	TableLabel = LOCTEXT("ObjectBrowser_Column_Module", "Module");
	ConfigLabel = LOCTEXT("ObjectBrowser_Column_Module", "Module");
	PreferredWidthRatio = 0.25f;
}

FText FObjectDynamicColumn_Module::ExtractText(TSharedRef<const ISubsystemTreeItem> Item) const
{
	if (const FObjectTreeSubsystemItem* SubsystemItem = Item->GetAsSubsystemDescriptor())
	{
		return FText::FromString(SubsystemItem->ShortPackage);
	}

	return FText::GetEmpty();
}

void FObjectDynamicColumn_Module::PopulateSearchStrings(const ISubsystemTreeItem& Item, TArray<FString>& OutSearchStrings) const
{
	if (const FObjectTreeSubsystemItem* SubsystemItem = Item.GetAsSubsystemDescriptor())
	{
		OutSearchStrings.Add(SubsystemItem->ShortPackage);
	}
}

#undef LOCTEXT_NAMESPACE
