

#pragma once

#include "Model/ObjectBrowserColumn.h"


/**
 * "Name" column implementation
 */
struct FObjectDynamicColumn_Name : public FObjectDynamicTextColumn
{
	using Super = FObjectDynamicTextColumn;

	FObjectDynamicColumn_Name();

	virtual bool IsVisibleByDefault() const override { return true; }
	virtual TSharedPtr<SWidget> GenerateColumnWidget(TSharedRef<const IObjectTreeItem> Item, TSharedRef<class SObjectBrowserTableItem> TableRow) const override;
	virtual void PopulateSearchStrings(const IObjectTreeItem& Item, TArray<FString>& OutSearchStrings) const override;
	virtual FText ExtractText(TSharedRef<const IObjectTreeItem> Item) const override;
	virtual FText ExtractTooltipText(TSharedRef<const IObjectTreeItem> Item) const override { return FText::GetEmpty(); }
	virtual FSlateColor ExtractColor(TSharedRef<const IObjectTreeItem> Item) const override;
};
