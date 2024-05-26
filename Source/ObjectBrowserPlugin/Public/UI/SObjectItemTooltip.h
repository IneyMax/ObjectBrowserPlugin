

#pragma once

#include "UI/SObjectBrowserTableItem.h"
#include "Widgets/SToolTip.h"


class SObjectItemTooltip : public SToolTip
{
	using Super = SToolTip;

public:
	SLATE_BEGIN_ARGS(SObjectItemTooltip)
	    { }
		SLATE_ARGUMENT(TSharedPtr<SObjectBrowserTableItem>, ObjectBrowserTableItem)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// IToolTip interface
	virtual bool IsEmpty() const override
	{
		return !ObjectBrowserTableItem.IsValid();
	}

	virtual void OnOpening() override;

	virtual void OnClosed() override;

	/** Creates a tooltip widget for this item */
	TSharedRef<SWidget> CreateToolTipWidget(TSharedRef<SObjectBrowserTableItem> TableItem) const;

private:
	TWeakPtr<SObjectBrowserTableItem> ObjectBrowserTableItem;
};


class OBJECTBROWSERPLUGIN_API FObjectBrowserTableItemTooltipBuilder
{
	friend SObjectItemTooltip;

	TSharedRef<SObjectBrowserTableItem> Item;
	TSharedPtr<SVerticalBox>		Primary;
	TSharedPtr<SVerticalBox>		Secondary;
public:
	enum
	{
		DF_NONE				= 0,
		DF_IMPORTANT		= 1 << 0,
		DF_WITH_HIGHLIGHT	= 1 << 1,
	};

	FObjectBrowserTableItemTooltipBuilder(TSharedRef<SObjectBrowserTableItem> Item) : Item(Item) {}

	void AddPrimary(const FText& Key, const FText& Value, uint32 DisplayFlags = DF_NONE);
	void AddSecondary(const FText& Key, const FText& Value, uint32 DisplayFlags = DF_NONE);
	bool HasAnyData() const { return Primary.IsValid() || Secondary.IsValid(); }
private:
	void AddBox(TSharedRef<SVerticalBox> Target, const FText& Key, const FText& Value, uint32 DisplayFlags);
};