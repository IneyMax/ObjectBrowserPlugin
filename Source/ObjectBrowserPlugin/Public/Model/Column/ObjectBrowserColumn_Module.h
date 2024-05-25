

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

	virtual FText ExtractText(TSharedRef<const ISubsystemTreeItem> Item) const override;
	virtual FText ExtractTooltipText(TSharedRef<const ISubsystemTreeItem> Item) const override { return FText::GetEmpty(); }
	virtual void PopulateSearchStrings(const ISubsystemTreeItem& Item, TArray<FString>& OutSearchStrings) const override;

};