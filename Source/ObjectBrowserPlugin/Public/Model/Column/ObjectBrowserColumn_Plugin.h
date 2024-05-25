

#pragma once

#include "Model/ObjectBrowserColumn.h"

/*
 * "Plugin" column implementation
 */
struct FObjectDynamicColumn_Plugin : public FObjectDynamicTextColumn
{
	using Super = FObjectDynamicTextColumn;

	FObjectDynamicColumn_Plugin();

	virtual bool IsVisibleByDefault() const override { return false; }

	virtual FText ExtractText(TSharedRef<const ISubsystemTreeItem> Item) const override;
	virtual FText ExtractTooltipText(TSharedRef<const ISubsystemTreeItem> Item) const override { return FText::GetEmpty(); }
	virtual void PopulateSearchStrings(const ISubsystemTreeItem& Item, TArray<FString>& OutSearchStrings) const override;
};