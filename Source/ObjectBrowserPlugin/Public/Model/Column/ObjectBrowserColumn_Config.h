

#pragma once

#include "Model/ObjectBrowserColumn.h"


/**
 * "Config" column implementation
 */
struct FObjectDynamicColumn_Config : public FObjectDynamicTextColumn
{
	FObjectDynamicColumn_Config();

	virtual bool IsVisibleByDefault() const override { return false; }

	virtual FText ExtractText(TSharedRef<const IObjectTreeItem> Item) const override;
	virtual FText ExtractTooltipText(TSharedRef<const IObjectTreeItem> Item) const override { return FText::GetEmpty(); }
	virtual void PopulateSearchStrings(const IObjectTreeItem& Item, TArray<FString>& OutSearchStrings) const override;
};