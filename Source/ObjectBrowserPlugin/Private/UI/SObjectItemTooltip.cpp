

#include "UI/SObjectItemTooltip.h"
#include "ObjectBrowserFlags.h"
#include "ObjectBrowserModule.h"
#include "ObjectBrowserStyle.h"
#include "Item/IObjectTreeItem.h"


#define LOCTEXT_NAMESPACE "ObjectBrowser"

void SObjectItemTooltip::Construct(const FArguments& InArgs)
{
	ObjectBrowserTableItem = InArgs._ObjectBrowserTableItem;

	Super::Construct(
		SToolTip::FArguments()
		.TextMargin(1.0f)
		.BorderImage(FStyleHelper::GetBrush("ContentBrowser.TileViewTooltip.ToolTipBorder"))
	);
}

void SObjectItemTooltip::OnOpening()
{
	TSharedPtr<SObjectBrowserTableItem> TableItem = ObjectBrowserTableItem.Pin();
	if (TableItem.IsValid())
	{
		SetContentWidget(CreateToolTipWidget(TableItem.ToSharedRef()));
	}
}

void SObjectItemTooltip::OnClosed()
{
#if UE_VERSION_OLDER_THAN(5,0,0)
	SetContentWidget(SNullWidget::NullWidget);
#else
	ResetContentWidget();
#endif
}

TSharedRef<SWidget> SObjectItemTooltip::CreateToolTipWidget(TSharedRef<SObjectBrowserTableItem> TableItem) const
{
	if (TableItem->Item.IsValid())
	{
		FObjectBrowserTableItemTooltipBuilder TooltipBuilder(TableItem);
		TableItem->Item->GenerateTooltip(TooltipBuilder);

		// Apply external customizations for tooltips
		FObjectBrowserModule::OnGenerateTooltip.Broadcast(TableItem->Item.ToSharedRef(), TooltipBuilder);

		TSharedRef<SVerticalBox> OverallTooltipVBox = SNew(SVerticalBox);

		// Top section (asset name, type, is checked out)
		OverallTooltipVBox->AddSlot()
						  .AutoHeight()
						  .Padding(0, 0, 0, 4)
		[
			SNew(SBorder)
					.Padding(6)
					.BorderImage(FStyleHelper::GetBrush("ContentBrowser.TileViewTooltip.ContentBorder"))
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					  .AutoWidth()
					  .VAlign(VAlign_Center)
					  .Padding(0, 0, 4, 0)
					[
						SNew(STextBlock)
								.Text(TableItem->Item->GetDisplayName())
								.Font(FStyleHelper::GetFontStyle("ContentBrowser.TileViewTooltip.NameFont"))
					]
				]

			]
		];

		if (TooltipBuilder.Primary.IsValid())
		{
			OverallTooltipVBox->AddSlot()
							  .AutoHeight()
			[
				SNew(SBorder)
					.Padding(6)
					.BorderImage(FStyleHelper::GetBrush("ContentBrowser.TileViewTooltip.ContentBorder"))
				[
					TooltipBuilder.Primary.ToSharedRef()
				]
			];
		}

		if (TooltipBuilder.Secondary.IsValid())
		{
			OverallTooltipVBox->AddSlot()
							  .AutoHeight()
			[
				SNew(SBorder)
					.Padding(6)
					.BorderImage(FStyleHelper::GetBrush("ContentBrowser.TileViewTooltip.ContentBorder"))
				[
					TooltipBuilder.Secondary.ToSharedRef()
				]
			];
		}

		return SNew(SBorder)
				.Padding(6)
				.BorderImage( FStyleHelper::GetBrush("ContentBrowser.TileViewTooltip.NonContentBorder") )
				[
					OverallTooltipVBox
				];
	}

	return SNullWidget::NullWidget;
}

void FObjectBrowserTableItemTooltipBuilder::AddPrimary(const FText& Key, const FText& Value, uint32 DisplayFlags)
{
	if (!Primary.IsValid())
	{
		SAssignNew(Primary, SVerticalBox);
	}
	AddBox(Primary.ToSharedRef(), Key, Value, DisplayFlags);
}

void FObjectBrowserTableItemTooltipBuilder::AddSecondary(const FText& Key, const FText& Value, uint32 DisplayFlags)
{
	if (!Secondary.IsValid())
	{
		SAssignNew(Secondary, SVerticalBox);
	}
	AddBox(Secondary.ToSharedRef(), Key, Value, DisplayFlags);
}

void FObjectBrowserTableItemTooltipBuilder::AddBox(TSharedRef<SVerticalBox> Target, const FText& Key, const FText& Value, uint32 DisplayFlags)
{
	const bool bImportant = (DisplayFlags & DF_IMPORTANT) != 0;
	const bool bHighlightable = (DisplayFlags & DF_WITH_HIGHLIGHT) != 0;

	FWidgetStyle ImportantStyle;
	ImportantStyle.SetForegroundColor(FLinearColor(1, 0.5, 0, 1));

	Target->AddSlot()
		   .AutoHeight()
		   .Padding(0, 1)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		  .AutoWidth()
		  .Padding(0, 0, 4, 0)
		[
			SNew(STextBlock).Text(FText::Format(LOCTEXT("ObjectItemTooltipFormat", "{0}:"), Key))
							.ColorAndOpacity( bImportant ? ImportantStyle.GetSubduedForegroundColor() : FSlateColor::UseSubduedForeground())
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock).Text(Value)
							.ColorAndOpacity(bImportant ? ImportantStyle.GetForegroundColor() : FSlateColor::UseForeground())
							.HighlightText(bHighlightable ? Item->HighlightText : FText())
							.WrapTextAt(700.0f)
		]
	];
}

#undef LOCTEXT_NAMESPACE
