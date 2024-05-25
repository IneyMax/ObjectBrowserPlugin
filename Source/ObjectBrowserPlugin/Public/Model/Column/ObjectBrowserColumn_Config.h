

#pragma once

#include "Model/ObjectBrowserColumn.h"

/**
 * "Config" column implementation
 */
struct FObjectDynamicColumn_Config : public FObjectDynamicTextColumn
{
	using Super = FObjectDynamicTextColumn;

	FObjectDynamicColumn_Config();

	virtual bool IsVisibleByDefault() const override { return false; }

	virtual FText ExtractText(TSharedRef<const ISubsystemTreeItem> Item) const override;
	virtual FText ExtractTooltipText(TSharedRef<const ISubsystemTreeItem> Item) const override { return FText::GetEmpty(); }
	virtual void PopulateSearchStrings(const ISubsystemTreeItem& Item, TArray<FString>& OutSearchStrings) const override;
};