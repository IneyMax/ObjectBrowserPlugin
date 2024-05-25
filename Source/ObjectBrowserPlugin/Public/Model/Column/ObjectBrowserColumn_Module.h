

#pragma once

#include "Model/ObjectBrowserColumn.h"


/*
 * "Module" column implementation
 */
struct FObjectDynamicColumn_Module : public FObjectDynamicTextColumn
{
	using Super = FObjectDynamicTextColumn;

	FObjectDynamicColumn_Module();

	virtual bool IsVisibleByDefault() const override { return true; }

	virtual FText ExtractText(TSharedRef<const IObjectTreeItem> Item) const override;
	virtual FText ExtractTooltipText(TSharedRef<const IObjectTreeItem> Item) const override { return FText::GetEmpty(); }
	virtual void PopulateSearchStrings(const IObjectTreeItem& Item, TArray<FString>& OutSearchStrings) const override;

};