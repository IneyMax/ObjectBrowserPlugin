

#include "Model/ObjectBrowserColumn.h"

#include "ObjectBrowserFlags.h"
#include "ObjectBrowserSettings.h"
#include "UI/SObjectBrowserTableItem.h"
#include "ObjectBrowserStyle.h"
#include "Item/IObjectTreeItem.h"
#include "Model/ObjectBrowserSorting.h"

#define LOCTEXT_NAMESPACE "ObjectBrowser"


FObjectDynamicColumn::FObjectDynamicColumn()
{
}

SHeaderRow::FColumn::FArguments FObjectDynamicColumn::GenerateHeaderColumnWidget() const
{
	return SHeaderRow::Column( Name )
			  .FillWidth(PreferredWidthRatio)
			  .HeaderContent()
			[
				SNew(SBox)
				.MinDesiredHeight(24)
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(TableLabel)
					]
				]
			];
}

bool FObjectDynamicColumn::IsValidColumnName(FName InName)
{
	static const FName ColumnID_Name("Name");
	return !InName.IsNone() && InName != ColumnID_Name;
}

TSharedPtr<SWidget> FObjectDynamicTextColumn::GenerateColumnWidget(TSharedRef<const IObjectTreeItem> Item, TSharedRef<SObjectBrowserTableItem> TableRow) const
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(1, 0, 0, 0)
		.AutoWidth()
		[
			SNew(STextBlock)
			.Font(this, &FObjectDynamicTextColumn::ExtractFont, Item)
			.ColorAndOpacity(this, &FObjectDynamicTextColumn::ExtractColorIfEnabled, Item)
			.Text(this, &FObjectDynamicTextColumn::ExtractText, Item)
			.ToolTipText(this, &FObjectDynamicTextColumn::ExtractTooltipText, Item)
			.HighlightText(TableRow->HighlightText)
		];
}

FText FObjectDynamicTextColumn::ExtractTooltipText(TSharedRef<const IObjectTreeItem> Item) const
{
	return ExtractText(Item);
}

FSlateColor FObjectDynamicTextColumn::ExtractColor(TSharedRef<const IObjectTreeItem> Item) const
{
	if (Item->IsStale())
	{
		return FSlateColor::UseSubduedForeground();
	}
	if (Item->IsGameModule() && !Item->IsSelected())
	{
		return FLinearColor(0.4f, 0.4f, 1.0f);
	}

	return FSlateColor::UseForeground();
}

FSlateColor FObjectDynamicTextColumn::ExtractColorIfEnabled(TSharedRef<const IObjectTreeItem> Item) const
{
	if (UObjectBrowserSettings::Get()->IsColoringEnabled())
	{
		return ExtractColor(Item);
	}
	return Item->IsStale() ? FSlateColor::UseSubduedForeground() : FSlateColor::UseForeground();
}

FSlateFontInfo FObjectDynamicTextColumn::ExtractFont(TSharedRef<const IObjectTreeItem> Item) const
{
	return FStyleHelper::GetFontStyle("WorldBrowser.LabelFont");
}

void FObjectDynamicTextColumn::SortItems(TArray<ObjectTreeItemPtr>& RootItems, const EColumnSortMode::Type SortMode) const
{
	ObjectBrowser::FSortHelper<ObjectTreeItemPtr, FString>()
		.Primary([this](TSharedPtr<IObjectTreeItem> Item) { return ExtractText(Item.ToSharedRef()).ToString(); }, SortMode)
		.Sort(RootItems);
}

#undef LOCTEXT_NAMESPACE

#if ENABLE_SUBSYSTEM_BROWSER_EXAMPLES && SINCE_UE_VERSION(4, 27, 0)

// 1. Create a new struct inheriting FObjectDynamicColumn

struct FObjectDynamicColumn_Tick : public FObjectDynamicColumn
{
	FObjectDynamicColumn_Tick()
	{
		// Configure name and header title
		Name = TEXT("IsTickable");
		TableLabel = INVTEXT("");
		ConfigLabel = INVTEXT("Tickable");
		PreferredWidthRatio = 0.05f;
	}

	virtual TSharedPtr<SWidget> GenerateColumnWidget(TSharedRef<const IObjectTreeItem> Item, TSharedRef<SObjectBrowserTableItem> TableRow) const override
	{
		// Build a widget to represent value
		return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor::UseForeground())
				.Image(FStyleHelper::GetBrush(TEXT("GraphEditor.Conduit_16x")))
				.DesiredSizeOverride(FVector2d{16,16})
				.Visibility(this, &FObjectDynamicColumn_Tick::ExtractIsTickable, Item)
			];
	}
private:
	EVisibility ExtractIsTickable(TSharedRef<const IObjectTreeItem> Item) const
	{
		auto* Object = Item->GetAsObjectDescriptor();
		return Object && Object->Class.IsValid() && Object->Class->IsChildOf(UTickableWorldObject::StaticClass())
			? EVisibility::Visible : EVisibility::Hidden;
	}
};

// 2. Call this during module initialization
void RegisterCustomColumns()
{
	// Get a reference to Object Browser module instance or load it
	FObjectBrowserModule& Module = FModuleManager::LoadModuleChecked<FObjectBrowserModule>(TEXT("ObjectBrowser"));
	// Construct and register new column
	Module.RegisterDynamicColumn(MakeShared<FObjectDynamicColumn_Tick>());
}

#endif
