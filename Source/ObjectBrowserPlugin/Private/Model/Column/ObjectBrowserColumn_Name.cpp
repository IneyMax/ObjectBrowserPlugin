﻿

#include "Model/Column/ObjectBrowserColumn_Name.h"

#include "UI/ObjectBrowserTableItem.h"

#define LOCTEXT_NAMESPACE "ObjectBrowser"

FObjectDynamicColumn_Name::FObjectDynamicColumn_Name()
{
	Name = TEXT("Name");
	ConfigLabel = LOCTEXT("ObjectBrowser_Column_Name", "Name");
	TableLabel = LOCTEXT("ObjectBrowser_Column_Name", "Name");
	SortOrder = -1;
	PreferredWidthRatio = 0.6f;
}

TSharedPtr<SWidget> FObjectDynamicColumn_Name::GenerateColumnWidget(TSharedRef<const ISubsystemTreeItem> Item, TSharedRef<SObjectBrowserTableItem> TableRow) const
{
	return SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SExpanderArrow, TableRow)
			]

			+SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
		    .Padding(1, 0, 0, 0)
			.AutoWidth()
			[
				SNew(SBox)
				.VAlign(VAlign_Center)
				.HeightOverride(22)
				.WidthOverride(Item->CanHaveChildren() ? 16.f : 7.f)
				[
					SNew(SImage)
					.Image(TableRow, &SObjectBrowserTableItem::GetItemIconBrush)
				]
			]

			+SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.Padding(4, 3, 0, 3)
			.AutoWidth()
			[
				SNew(STextBlock)
					.Font(this, &FObjectDynamicColumn_Name::ExtractFont, Item)
					.ColorAndOpacity(this, &FObjectDynamicColumn_Name::ExtractColor, Item)
					.Text(this, &FObjectDynamicColumn_Name::ExtractText, Item)
					.ToolTipText(this, &FObjectDynamicColumn_Name::ExtractTooltipText, Item)
					.HighlightText(TableRow->HighlightText)
			];
}

FText FObjectDynamicColumn_Name::ExtractText(TSharedRef<const ISubsystemTreeItem> Item) const
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("DisplayText"), Item->GetDisplayName());
	Args.Add(TEXT("Stale"), (Item->IsStale() ? LOCTEXT("SubsystemItem_Stale", " (Stale)") : FText::GetEmpty()));
	return FText::Format(LOCTEXT("SubsystemItem_Name", "{DisplayText}{Stale}"), Args);
}

FSlateColor FObjectDynamicColumn_Name::ExtractColor(TSharedRef<const ISubsystemTreeItem> Item) const
{
	if (Item->IsStale())
	{
		return FSlateColor::UseSubduedForeground();
	}
	return Super::ExtractColor(Item);
}

void FObjectDynamicColumn_Name::PopulateSearchStrings(const ISubsystemTreeItem& Item, TArray<FString>& OutSearchStrings) const
{
	OutSearchStrings.Add(Item.GetDisplayName().ToString());
}


#undef LOCTEXT_NAMESPACE
