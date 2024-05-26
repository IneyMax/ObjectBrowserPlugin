

#pragma once

#include "ObjectBrowserFwd.h"
#include "Widgets/Views/STableRow.h"


class SObjectBrowserTableItem : public SMultiColumnTableRow<FObjectTreeItemPtr>
{
	using Super = SMultiColumnTableRow<FObjectTreeItemPtr>;
public:
	SLATE_BEGIN_ARGS( SObjectBrowserTableItem )
		: _IsItemExpanded( false )
	{}
		/** Data for the world */
		SLATE_ARGUMENT(TSharedPtr<FObjectModel>, InModel)

		/** Item model this widget represents */
		SLATE_ARGUMENT(FObjectTreeItemPtr, InItemModel)

		/** The hierarchy that this item belongs to */
		SLATE_ARGUMENT(TSharedPtr<SObjectBrowser>, InBrowser)

		/** True when this item has children and is expanded */
		SLATE_ATTRIBUTE(bool, IsItemExpanded)

		/** The string in the title to highlight */
		SLATE_ATTRIBUTE(FText, HighlightText)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, TSharedRef<STableViewBase> OwnerTableView);

	virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& ColumnName ) override;

	const FSlateBrush* GetItemIconBrush() const;

	bool IsColoringEnabled() const;

public:
	TSharedPtr<FObjectModel>			Model;
	FObjectTreeItemPtr				Item;
	TSharedPtr<SObjectBrowser>	Browser;

	TAttribute<FText>					HighlightText;
	TAttribute<bool>					IsItemExpanded;
};
