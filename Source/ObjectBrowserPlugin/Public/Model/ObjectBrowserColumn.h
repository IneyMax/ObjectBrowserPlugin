

#pragma once
#include "ObjectBrowserFwd.h"

struct IObjectTreeItem;


/* Represents a configurable column */
struct OBJECTBROWSERPLUGIN_API FObjectDynamicColumn : public TSharedFromThis<FObjectDynamicColumn>
{
	/* Column config identifier */
	FName Name;
	/* Column display name for table header */
	FText TableLabel;
	/* Column display name for menu */
	FText ConfigLabel;
	/* Column preferred width ratio (from 0 to 1) */
	float PreferredWidthRatio = 0.1f;
	/* Column sort order */
	int32 SortOrder = 0;

	FObjectDynamicColumn();
	virtual ~FObjectDynamicColumn() = default;

	const FName& GetID() const { return Name; }
	int32 GetSortOrder() const { return SortOrder; }

	/**
	 * Generate visual representation of column in header row
	 */
	virtual SHeaderRow::FColumn::FArguments GenerateHeaderColumnWidget() const;

	/**
	 * Generate visual representation of column in table row
	 */
	virtual TSharedPtr<SWidget> GenerateColumnWidget(TSharedRef<const IObjectTreeItem> Item, TSharedRef<class SObjectBrowserTableItem> TableRow) const = 0;

	/**
	 * Gather searchable strings for column
	 */
	virtual void PopulateSearchStrings(const IObjectTreeItem& Item, TArray<FString>& OutSearchStrings) const {}

	/**
	 * Does this column support sorting? WIP
	 */
	virtual bool SupportsSorting() const { return false; }

	/**
	 * Perform sorting of table items. WIP
	 */
	virtual void SortItems(TArray<ObjectTreeItemPtr>& RootItems, const EColumnSortMode::Type SortMode) const {}

	/**
	 * Test if custom column name is valid (not None or permanent column)
	 */
	static bool IsValidColumnName(FName InName);

	/**
	 * Get default column visibility state
	 */
	virtual bool IsVisibleByDefault() const { return false; }
};


/**
 * A prefab type for simple columns that have text representation
 */
struct OBJECTBROWSERPLUGIN_API FObjectDynamicTextColumn : public FObjectDynamicColumn
{
	using Super = FObjectDynamicTextColumn;

	virtual TSharedPtr<SWidget> GenerateColumnWidget(TSharedRef<const IObjectTreeItem> Item, TSharedRef<class SObjectBrowserTableItem> TableRow) const override;
	virtual void PopulateSearchStrings(const IObjectTreeItem& Item, TArray<FString>& OutSearchStrings) const override { }

protected:
	/* get text to display for specified item */
	virtual FText ExtractText(TSharedRef<const IObjectTreeItem> Item) const = 0;
	
	/* get tooltip text to display for specified item */
	virtual FText ExtractTooltipText(TSharedRef<const IObjectTreeItem> Item) const;
	
	/* get color and opacity of text for specified item */
	virtual FSlateColor ExtractColor(TSharedRef<const IObjectTreeItem> Item) const;
	
	/* internal */
	FSlateColor ExtractColorIfEnabled(TSharedRef<const IObjectTreeItem> Item) const;
	
	/* get font of text for specified item */
	virtual FSlateFontInfo ExtractFont(TSharedRef<const IObjectTreeItem> Item) const;
	
	/* text columns support sorting by default */
	virtual bool SupportsSorting() const override { return true; }
	
	virtual void SortItems(TArray<ObjectTreeItemPtr>& RootItems, const EColumnSortMode::Type SortMode) const override;
};
