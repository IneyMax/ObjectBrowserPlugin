

#include "UI/SObjectBrowser.h"

#include "ClassViewerModule.h"
#include "ObjectBrowserFlags.h"
#include "ObjectBrowserModule.h"
#include "ObjectBrowserSettings.h"
#include "ObjectBrowserStyle.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Views/STableRow.h"
#include "SlateOptMacros.h"
#include "ToolMenus.h"
#include "Editor.h"
#include "EditorFontGlyphs.h"
#include "Engine/MemberReference.h"
#include "IDetailsView.h"
#include "Item/ObjectBrowserTreeCategoryItem.h"
#include "Kismet2/SClassPickerDialog.h"
#include "Model/ObjectBrowserFilters.h"
#include "UI/SObjectBrowserHeaderRow.h"
#include "UI/SObjectBrowserTableItem.h"
#include "UI/SObjectBrowserTreeView.h"


#define LOCTEXT_NAMESPACE "ObjectBrowser"

SObjectBrowser::SObjectBrowser()
{
}

SObjectBrowser::~SObjectBrowser()
{
	FEditorDelegates::PostPIEStarted.RemoveAll(this);
	FEditorDelegates::PrePIEEnded.RemoveAll(this);
}

FText SObjectBrowser::GetFilterClassText() const
{
	if (FilterClass)
	{
		return FilterClass->GetDisplayNameText();
	}

	return LOCTEXT("ClassFilter", "Class Filter");
}

FReply SObjectBrowser::OnClassSelectionClicked()
{
	const FText TitleText = LOCTEXT("PickClass", "Pick Class");
	
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;

	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UObject::StaticClass());
	if ( bPressedOk )
	{
		FilterClass = ChosenClass;
		FullRefresh();
	}

	return FReply::Handled();
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SObjectBrowser::Construct(const FArguments& InArgs)
{
	// Automatically switch to pie world and back
	FEditorDelegates::PostPIEStarted.AddSP(this, &SObjectBrowser::HandlePIEStart);
	FEditorDelegates::PrePIEEnded.AddSP(this, &SObjectBrowser::HandlePIEEnd);

	// Update initial settings to apply custom category registrations
	UObjectBrowserSettings* Settings = UObjectBrowserSettings::Get();
	Settings->SyncCategorySettings();
	Settings->SyncColumnSettings();

	// Automatically handle settings change
	UObjectBrowserSettings::OnSettingChanged().AddSP(this, &SObjectBrowser::OnSettingsChanged);
	FModuleManager::Get().OnModulesChanged().AddSP(this, &SObjectBrowser::OnModulesChanged);

	ObjectModel = MakeShared<FObjectModel>();
	ObjectModel->SetCurrentWorld(InArgs._InWorld);
	ObjectModel->OnDataChanged.AddSP(this, &SObjectBrowser::OnObjectDataChanged);

	// Generate search box
	SearchBoxObjectFilter = MakeShared<ObjectTextFilter>(
		ObjectTextFilter::FItemToStringArray::CreateSP(this, &SObjectBrowser::TransformItemToString)
	);
	SearchBoxObjectFilter->OnChanged().AddSP(this, &SObjectBrowser::FullRefresh);

	// Generate category selector
	CategoryFilter = MakeShared<FObjectCategoryFilter>();
	CategoryFilter->OnChanged().AddSP(this, &SObjectBrowser::FullRefresh);

	// Assign filters to model
	ObjectModel->CategoryFilter = CategoryFilter;
	ObjectModel->ObjectTextFilter = SearchBoxObjectFilter;

	// Generate tree view header.
	HeaderRowWidget = SNew(SObjectBrowserHeaderRow, ObjectModel, SharedThis(this));

	// Build the details viewer
	DetailsView = CreateDetails();
	check(DetailsView.IsValid());

	// Context menu

	FOnContextMenuOpening ContextMenuEvent = FOnContextMenuOpening::CreateSP(this, &SObjectBrowser::ConstructObjectContextMenu);

	// Build the actual Object Browser view panel
	ChildSlot
	[
		SNew(SVerticalBox)

		// World Selection
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FStyleHelper::GetBrush(TEXT("ToolPanel.GroupBorder")))
			[
				SNew(SHorizontalBox)

				// Toolbar Button
				+SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				[
					// Levels menu
					SNew( SComboButton )
					.ComboButtonStyle(FStyleHelper::GetWidgetStylePtr<FComboButtonStyle>("ToolbarComboButton"))
					.ForegroundColor(FLinearColor::White)
					.ContentPadding(0)
					.OnGetMenuContent(this, &SObjectBrowser::GetWorldsButtonContent)
					.ButtonContent()
					[
						SNew(SHorizontalBox)

						// Icon
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SImage)
							.Image(this, &SObjectBrowser::GetWorldsMenuBrush)
						]

						// Text
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						.Padding(1,2,1,2)
						[
							SNew(STextBlock)
							.TextStyle(FStyleHelper::GetWidgetStylePtr<FTextBlockStyle>("ContentBrowser.TopBar.Font"))
							.Text(this, &SObjectBrowser::GetCurrentWorldText)
						]
					]
				]

				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
				.FillWidth(1.0f)
				[
					SNew(SButton)
					.OnClicked(this, &SObjectBrowser::OnClassSelectionClicked)
					[
						SNew(STextBlock)
						.Text(this, &SObjectBrowser::GetFilterClassText)
					]
				]
			]
		]

		// Search by name panel
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FStyleHelper::GetBrush(TEXT("ToolPanel.GroupBorder")))
			[
				SNew(SHorizontalBox)
				
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SSearchBox)
						.ToolTipText(LOCTEXT("FilterSearchToolTip", "Type here to search Objects"))
						.HintText(LOCTEXT("FilterSearchHint", "Search Objects"))
						.OnTextChanged(this, &SObjectBrowser::SetFilterText)
				]
			]
		]

		// Objects Content
		+SVerticalBox::Slot()
		.FillHeight(1.f)
		.Padding(0,4,0,0)
		[
			SNew(SBorder)
			.BorderImage(FStyleHelper::GetBrush(TEXT("ToolPanel.GroupBorder")))
			[
				SAssignNew(BrowserSplitter, SSplitter)
				.MinimumSlotHeight(100.f)
				.Orientation(Orient_Vertical)
#if UE_VERSION_OLDER_THAN(5, 0, 0)
				.Style(FStyleHelper::GetWidgetStylePtr<FSplitterStyle>("Splitter"))
#else
				.Style(FStyleHelper::GetWidgetStylePtr<FSplitterStyle>("SplitterDark"))
#endif
				.PhysicalSplitterHandleSize(4.0f)
				.HitDetectionSplitterHandleSize(6.0f)
				.OnSplitterFinishedResizing(this, &SObjectBrowser::BrowserSplitterFinishedResizing)
				+ SSplitter::Slot()
#if !UE_VERSION_OLDER_THAN(5, 0, 0)
				.MinSize(120.f)
#endif
				.Value(Settings->GetSeparatorLocation())
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.FillHeight(1.f)
					.Padding(0, 0, 0, 2)
					[
						SAssignNew(TreeWidget, SObjectBrowserTreeView, ObjectModel, SharedThis(this))
							.TreeItemsSource(&RootTreeItems)
							.SelectionMode(ESelectionMode::Single)
							.OnGenerateRow(this, &SObjectBrowser::GenerateTreeRow)
							.OnGetChildren(this, &SObjectBrowser::GetChildrenForTree)
							.OnSelectionChanged(this, &SObjectBrowser::OnSelectionChanged)
							.OnExpansionChanged(this, &SObjectBrowser::OnExpansionChanged)
							.OnMouseButtonDoubleClick(this, &SObjectBrowser::OnTreeViewMouseButtonDoubleClick)
							.OnContextMenuOpening(ContextMenuEvent)
							.HighlightParentNodesForSelection(true)
							.ClearSelectionOnClick(true)
							.HeaderRow(HeaderRowWidget.ToSharedRef())
					]

					// View options
					+SVerticalBox::Slot()
					.Padding(0, 0, 0, 2)
					.AutoHeight()
					[
						SNew(SHorizontalBox)
						.Visibility(EVisibility::Visible)

						// Asset count
						+SHorizontalBox::Slot()
						.FillWidth(1.f)
						.VAlign(VAlign_Center)
						.Padding(8, 0)
						[
							SNew( STextBlock )
							.Text( this, &SObjectBrowser::GetFilterStatusText )
							.ColorAndOpacity( this, &SObjectBrowser::GetFilterStatusTextColor )
						]

						// View mode combo button
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SAssignNew( ViewOptionsComboButton, SComboButton )
							.ContentPadding(0)
							.ForegroundColor( this, &SObjectBrowser::GetViewOptionsButtonForegroundColor )
							.ButtonStyle( FStyleHelper::GetWidgetStylePtr<FButtonStyle>("ToggleButton") )
							.OnGetMenuContent( this, &SObjectBrowser::GetViewOptionsButtonContent )
							.ButtonContent()
							[
								SNew(SHorizontalBox)

								+SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								[
									SNew(SImage).Image( FStyleHelper::GetBrush("GenericViewButton") )
								]

								+SHorizontalBox::Slot()
								.AutoWidth()
								.Padding(2, 0, 0, 0)
								.VAlign(VAlign_Center)
								[
									SNew(STextBlock).Text( LOCTEXT("ViewButton", "View Options") )
								]
							]
						]
					]

					// Separator
					+SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0, 0, 0, 1)
					[
						SNew(SSeparator)
						.Visibility(EVisibility::Visible)
					]
				]
				+ SSplitter::Slot()
				[
					SAssignNew( DetailsViewBox, SVerticalBox )
					+SVerticalBox::Slot()
					.Padding(0, 4, 0, 2)
					[
						DetailsView.ToSharedRef()
					]
				]
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SObjectBrowser::Tick(const FGeometry& AllotedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllotedGeometry, InCurrentTime, InDeltaTime);

	if (bNeedsRefresh)
	{
		if (!bIsReentrant)
		{
			Populate();
		}
	}

	if (bSortDirty)
	{
		// SortItems(RootTreeItems);
		for (const auto& Pair : TreeItemMap)
		{
			Pair.Value->bChildrenRequireSort = true;
		}

		TreeWidget->RequestTreeRefresh();

		bSortDirty = false;
	}

	if (bNeedsColumnRefresh)
	{
		bNeedsColumnRefresh = false;
		HeaderRowWidget->RefreshColumns();
	}

	if (bNeedRefreshDetails)
	{
		if (DetailsView.IsValid())
		{
			DetailsView->ForceRefresh();
		}
		bNeedRefreshDetails = false;
	}
}

void SObjectBrowser::Populate()
{
	TGuardValue<bool> ReentrantGuard(bIsReentrant, true);

	TMap<FObjectTreeItemID, bool> ExpansionStateInfo = GetParentsExpansionState();
	if (!bLoadedExpansionSettings)
	{// load settings only on first populate
		UObjectBrowserSettings::Get()->LoadTreeExpansionStates(ExpansionStateInfo);
		bLoadedExpansionSettings = true;
	}

	const FObjectTreeItemID SelectedItem = GetFirstSelectedItemId();

	if (bFullRefresh)
	{
		FilteredObjectsCount = 0;
		EmptyTreeItems();
		ResetSelectedObject();

		ObjectModel->GetFilteredCategories(RootTreeItems);
		for (FObjectTreeItemPtr Category : RootTreeItems)
		{
			TreeItemMap.Add(Category->GetID(), Category);

			ObjectModel->GetFilteredObjects(Category, Category->Children);
			for (FObjectTreeItemPtr  Child : Category->GetChildren())
			{
				TreeItemMap.Add(Child->GetID(), Child);

				FilteredObjectsCount ++;
			}
		}

		bFullRefresh = false;
	}

	SetParentsExpansionState(ExpansionStateInfo);

	if (FObjectTreeItemPtr LastSelected = TreeItemMap.FindRef(SelectedItem))
	{
		SetSelectedObject(LastSelected);
	}

	if (bNeedListRebuild)
	{
		HeaderRowWidget->RebuildColumns();
		TreeWidget->RebuildList();

		bNeedListRebuild = false;
	}

	TreeWidget->RequestTreeRefresh();

	bNeedsRefresh = false;
}

void SObjectBrowser::EmptyTreeItems()
{
	for (auto& Pair : TreeItemMap)
	{
		Pair.Value->RemoveAllChildren();
	}

	RootTreeItems.Empty();
	TreeItemMap.Reset();
}

void SObjectBrowser::FullRefresh()
{
	bFullRefresh = true;

	RefreshView();
	RefreshDetails();
	RequestSort();
}

void SObjectBrowser::TransformItemToString(const IObjectTreeItem&  Item, TArray<FString>& OutSearchStrings) const
{
	if (Item.GetAsObjectDescriptor())
	{
		for (auto& Column : ObjectModel->GetSelectedTableColumns())
		{
			Column->PopulateSearchStrings(Item, OutSearchStrings);
		}
	}
}

void SObjectBrowser::SetFilterText(const FText& InFilterText)
{
	SearchBoxObjectFilter->SetRawFilterText(InFilterText);
}

FText SObjectBrowser::GetSearchBoxText() const
{
	return SearchBoxObjectFilter->GetRawFilterText();
}

FText SObjectBrowser::GetFilterStatusText() const
{
	const int32 ObjectTotalCount = ObjectModel->GetNumObjectsFromVisibleCategories();

	if (!ObjectModel->IsObjectFilterActive())
	{
		return FText::Format( LOCTEXT("ShowObjectsCounterFmt", "{0} objects"), FText::AsNumber( FilteredObjectsCount ) );
	}
	else
	{
		if ( FilteredObjectsCount == 0)
		{   // all objects were filtered out
			return FText::Format( LOCTEXT("ShowObjectsCounterFmt", "No matching objects out of {0} total"), FText::AsNumber( ObjectTotalCount ) );
		}
		else
		{   // got something to display
			return FText::Format( LOCTEXT("ShowingOnlySomeActorsFmt", "Showing {0} of {1} objects"), FText::AsNumber( FilteredObjectsCount ), FText::AsNumber( ObjectTotalCount ) );
		}
	}
}

FSlateColor SObjectBrowser::GetFilterStatusTextColor() const
{
	if (!ObjectModel->IsObjectFilterActive())
	{
		// White = no text filter
		return FLinearColor(1.0f, 1.0f, 1.0f);
	}
	else if (FilteredObjectsCount == 0)
	{
		// Red = no matching actors
		return FLinearColor(1.0f, 0.4f, 0.4f);
	}
	else
	{
		// Green = found at least one match!
		return FLinearColor(0.4f, 1.0f, 0.4f);
	}
}

void SObjectBrowser::BrowserSplitterFinishedResizing()
{
#if SINCE_UE_VERSION(5, 0, 0)
	float NewValue = BrowserSplitter->SlotAt(0).GetSizeValue();
#else
	float NewValue = BrowserSplitter->SlotAt(0).SizeValue.Get();
#endif
	UObjectBrowserSettings::Get()->SetSeparatorLocation(NewValue);
}

FSlateColor SObjectBrowser::GetViewOptionsButtonForegroundColor() const
{
	static const FName InvertedForegroundName("InvertedForeground");
	static const FName DefaultForegroundName("DefaultForeground");

	return ViewOptionsComboButton->IsHovered()
		? FStyleHelper::GetSlateColor(InvertedForegroundName)
		: FStyleHelper::GetSlateColor(DefaultForegroundName);
}

TSharedRef<SWidget> SObjectBrowser::GetViewOptionsButtonContent()
{
	UObjectBrowserSettings* Settings = UObjectBrowserSettings::Get();

	FMenuBuilder MenuBuilder(true, nullptr);

	MenuBuilder.BeginSection(NAME_None, LOCTEXT("ViewColumnsGroup", "Columns"));
	{
		if (ObjectModel->GetNumDynamicColumns() > Settings->MaxColumnTogglesToShow)
		{
			MenuBuilder.AddSubMenu(
				LOCTEXT("ChooseColumnSubMenu", "Choose Columns"),
				LOCTEXT("ChooseColumnSubMenu_ToolTip", "Choose columns to display in browser."),
				FNewMenuDelegate::CreateSP(this, &SObjectBrowser::BuildColumnPickerContent)
			);
		}
		else
		{
			BuildColumnPickerContent(MenuBuilder);
		}
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection(NAME_None, LOCTEXT("ViewCategoryGroup", "Categories"));
	{
		if (ObjectModel->GetNumCategories() > Settings->MaxCategoryTogglesToShow)
		{
			MenuBuilder.AddSubMenu(
				LOCTEXT("ChooseCategorySubMenu", "Choose Category"),
				LOCTEXT("ChooseCategorySubMenu_ToolTip", "Choose categories to display in browser."),
				FNewMenuDelegate::CreateSP(this, &SObjectBrowser::BuildCategoryPickerContent)
			);
		}
		else
		{
			BuildCategoryPickerContent(MenuBuilder);
		}
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection(NAME_None, LOCTEXT("ViewOptionsGroup", "Options"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ToggleColoring", "Table Coloring"),
			LOCTEXT("ToggleColoring_Tooltip", "Toggles coloring in Object Browser tree."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &SObjectBrowser::ToggleTableColoring),
				FCanExecuteAction(),
				FIsActionChecked::CreateUObject(Settings, &UObjectBrowserSettings::IsColoringEnabled)
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ToggleGameOnly", "Only Game Modules"),
			LOCTEXT("ToggleGameOnly_Tooltip", "Show only objects that are within Game Modules."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &SObjectBrowser::ToggleShouldShowOnlyGame),
				FCanExecuteAction(),
				FIsActionChecked::CreateUObject(Settings, &UObjectBrowserSettings::ShouldShowOnlyGame)
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("TogglePluginOnly", "Only Plugin Modules"),
			LOCTEXT("TogglePluginOnly_Tooltip", "Show only objects that are within plugins."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &SObjectBrowser::ToggleShouldShowOnlyPlugins),
				FCanExecuteAction(),
				FIsActionChecked::CreateUObject(Settings, &UObjectBrowserSettings::ShouldShowOnlyPlugins)
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ToggleHiddenProps", "Show Hidden Properties"),
			LOCTEXT("ToggleHiddenProps_Tooltip", "Enforces display of all hidden object properties in details panel."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &SObjectBrowser::ToggleShowHiddenProperties),
				FCanExecuteAction(),
				FIsActionChecked::CreateUObject(Settings, &UObjectBrowserSettings::ShouldShowHiddenProperties)
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("OpenSettingsPanel", "All Options"),
			LOCTEXT("OpenSettingsPanel_Tooltip", "Open plugin settings panel."),
			FStyleHelper::GetSlateIcon("EditorPreferences.TabIcon"),
			FUIAction(
				FExecuteAction::CreateSP(this, &SObjectBrowser::ShowPluginSettingsTab)
			),
			NAME_None,
			EUserInterfaceActionType::Button
		);
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

void SObjectBrowser::BuildColumnPickerContent(FMenuBuilder& MenuBuilder)
{
	UObjectBrowserSettings* Settings = UObjectBrowserSettings::Get();

	for (const FObjectColumnPtr& Column : ObjectModel->GetDynamicTableColumns())
	{
		MenuBuilder.AddMenuEntry(
			Column->ConfigLabel,
			LOCTEXT("ToggleDisplayColumn_Tooltip", "Toggles display of Object Browser columns."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &SObjectBrowser::ToggleDisplayColumn, Column->Name),
				FCanExecuteAction(),
				FIsActionChecked::CreateUObject(Settings, &UObjectBrowserSettings::GetTableColumnState, Column->Name)
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);
	}
}

void SObjectBrowser::BuildCategoryPickerContent(FMenuBuilder& MenuBuilder)
{
	for (const FObjectTreeItemPtr& Category : ObjectModel->GetAllCategories())
	{
		check(Category->GetAsCategoryDescriptor());
		FObjectTreeItemID CategoryID = Category->GetID();

		MenuBuilder.AddMenuEntry(Category->GetDisplayName(),
			LOCTEXT("ToggleDisplayCategory_Tooltip", "Toggles display of objects for category."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &SObjectBrowser::ToggleDisplayCategory, CategoryID),
				FCanExecuteAction(),
				FIsActionChecked::CreateSP(this, &SObjectBrowser::GetCategoryDisplayStatus, CategoryID)
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);
	}
}

void SObjectBrowser::ToggleDisplayCategory(FObjectTreeItemID InCategory)
{
	if (CategoryFilter->IsCategoryVisible(InCategory))
	{
		CategoryFilter->HideCategory(InCategory);
	}
	else
	{
		CategoryFilter->ShowCategory(InCategory);
	}

	FullRefresh();
}

bool SObjectBrowser::GetCategoryDisplayStatus(FObjectTreeItemID InCategory)
{
	return CategoryFilter->IsCategoryVisible(InCategory);
}

void SObjectBrowser::SetupColumns(SHeaderRow& HeaderRow)
{
	HeaderRow.ClearColumns();

	for (const FObjectColumnPtr& Column : ObjectModel->GetSelectedTableColumns())
	{
		auto ColumnArgs = Column->GenerateHeaderColumnWidget();

		if (Column->SupportsSorting())
		{
			ColumnArgs
				.SortMode(this, &SObjectBrowser::GetColumnSortMode, Column->Name)
				.OnSort(this, &SObjectBrowser::OnColumnSortModeChanged);
		}
		else
		{
			ColumnArgs.SortMode(EColumnSortMode::None);
		}

		HeaderRow.AddColumn(ColumnArgs);
	}

	bNeedsColumnRefresh = true;
}


TSharedRef<ITableRow> SObjectBrowser::GenerateTreeRow(FObjectTreeItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	check(Item.IsValid());

	return SNew(SObjectBrowserTableItem, OwnerTable)
		.InModel(ObjectModel)
		.InBrowser(SharedThis(this))
		.InItemModel(Item)
		.IsItemExpanded(Item->bExpanded)
		.HighlightText(this, &SObjectBrowser::GetSearchBoxText)
		;
}

void SObjectBrowser::GetChildrenForTree(FObjectTreeItemPtr Item, TArray<FObjectTreeItemPtr>& OutChildren)
{
	OutChildren = Item->GetChildren();

	if (OutChildren.Num() && Item->bChildrenRequireSort)
	{
		// Sort the children we returned
		SortItems(OutChildren);

		// Empty out the children and repopulate them in the correct order
		Item->Children.Empty();
		for (auto& Child : OutChildren)
		{
			Item->Children.Emplace(Child);
		}

		// They no longer need sorting
		Item->bChildrenRequireSort = false;
	}
}

void SObjectBrowser::OnExpansionChanged(FObjectTreeItemPtr Item, bool bIsItemExpanded)
{
	Item->bExpanded = bIsItemExpanded;

	if (FObjectBrowserTreeCategoryItem* Folder = Item->GetAsCategoryDescriptor())
	{
		for (FObjectTreeItemPtr Child : Folder->GetChildren())
		{
			Child->bExpanded = bIsItemExpanded;
		}
	}

	// Save expansion states
	UObjectBrowserSettings::Get()->SetTreeExpansionStates(GetParentsExpansionState());

	RefreshView();
}

void SObjectBrowser::OnTreeViewMouseButtonDoubleClick(FObjectTreeItemPtr Item)
{
	if (Item.IsValid() && Item->GetAsCategoryDescriptor())
	{
		Item->bExpanded = ! Item->bExpanded;
		TreeWidget->SetItemExpansion(Item, Item->bExpanded);
	}
}

void SObjectBrowser::ToggleDisplayColumn(FName ColumnName)
{
	UObjectBrowserSettings* Settings = UObjectBrowserSettings::Get();

	bool OldState = Settings->GetTableColumnState(ColumnName);
	Settings->SetTableColumnState(ColumnName, !OldState);

	bNeedListRebuild = true;
	bNeedsRefresh = true;
}

void SObjectBrowser::ToggleTableColoring()
{
	UObjectBrowserSettings::Get()->ToggleColoringEnabled();

	RefreshView();
}

void SObjectBrowser::ToggleShowHiddenProperties()
{
	UObjectBrowserSettings::Get()->ToggleShouldShowHiddenProperties();

	RefreshView();
	RecreateDetails();
}

void SObjectBrowser::ToggleShouldShowOnlyGame()
{
	UObjectBrowserSettings::Get()->ToggleShouldShowOnlyGame();

	FullRefresh();
}

void SObjectBrowser::ToggleShouldShowOnlyPlugins()
{
	UObjectBrowserSettings::Get()->ToggleShouldShowOnlyPlugins();

	FullRefresh();
}

void SObjectBrowser::ShowPluginSettingsTab() const
{
	FObjectBrowserModule::Get().SummonPluginSettingsTab();
}

void SObjectBrowser::OnSelectionChanged(const FObjectTreeItemPtr Item, ESelectInfo::Type SelectInfo)
{
	if (SelectInfo == ESelectInfo::Direct)
	{
		return;
	}

	if (!bUpdatingSelection)
	{
		TGuardValue<bool> ScopeGuard(bUpdatingSelection, true);

		const TArray<FObjectTreeItemPtr>& SelectedItems = TreeWidget->GetSelectedItems();

		SetSelectedObject(SelectedItems.Num() ? SelectedItems[0] : nullptr);
	}
}

const FSlateBrush* SObjectBrowser::GetWorldsMenuBrush() const
{
	return FStyleHelper::GetBrush("WorldBrowser.LevelsMenuBrush");
}

FText SObjectBrowser::GetCurrentWorldText() const
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("World"), GetWorldDescription(ObjectModel->GetCurrentWorld().Get()));
	return FText::Format(LOCTEXT("WorldsSelectButton", "World: {World}"), Args);
}

FText SObjectBrowser::GetWorldDescription(UWorld* World) const
{
	FText Description;
	if(World)
	{
		FText PostFix;
		const FWorldContext* WorldContext = nullptr;
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			if(Context.World() == World)
			{
				WorldContext = &Context;
				break;
			}
		}

		if (World->WorldType == EWorldType::PIE)
		{
			switch(World->GetNetMode())
			{
				case NM_Client:
					if (WorldContext)
					{
						PostFix = FText::Format(LOCTEXT("ClientPostfixFormat", "(Client {0})"), FText::AsNumber(WorldContext->PIEInstance - 1));
					}
					else
					{
						PostFix = LOCTEXT("ClientPostfix", "(Client)");
					}
					break;
				case NM_DedicatedServer:
				case NM_ListenServer:
					PostFix = LOCTEXT("ServerPostfix", "(Server)");
					break;
				case NM_Standalone:
					PostFix = LOCTEXT("PlayInEditorPostfix", "(Play In Editor)");
					break;
				default:
					break;
			}
		}
		else if(World->WorldType == EWorldType::Editor)
		{
			PostFix = LOCTEXT("EditorPostfix", "(Editor)");
		}

		Description = FText::Format(LOCTEXT("WorldFormat", "{0} {1}"), FText::FromString(World->GetFName().GetPlainNameString()), PostFix);
	}

	return Description;
}

void SObjectBrowser::OnSelectWorld(TWeakObjectPtr<UWorld> InWorld)
{
	UE_LOG(LogObjectBrowser, Log, TEXT("Selected world %s"), *GetNameSafe(InWorld.Get()));

	ObjectModel->SetCurrentWorld(InWorld);
	FullRefresh();
}

bool SObjectBrowser::IsWorldChecked(TWeakObjectPtr<UWorld> InWorld)
{
	return ObjectModel->GetCurrentWorld() == InWorld;
}

TSharedRef<SWidget> SObjectBrowser::GetWorldsButtonContent()
{
	FMenuBuilder MenuBuilder(true, nullptr);

	MenuBuilder.BeginSection("Worlds", LOCTEXT("WorldsHeading", "Worlds"));

	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		UWorld* World = Context.World();
		if (World && (World->WorldType == EWorldType::PIE || Context.WorldType == EWorldType::Editor))
		{
			MenuBuilder.AddMenuEntry(
				GetWorldDescription(World),
				LOCTEXT("ChooseWorldToolTip", "Display objects for this world."),
				FSlateIcon(),
				FUIAction(
				FExecuteAction::CreateSP( this, &SObjectBrowser::OnSelectWorld, MakeWeakObjectPtr(World) ),
				FCanExecuteAction(),
				FIsActionChecked::CreateSP( this, &SObjectBrowser::IsWorldChecked, MakeWeakObjectPtr(World) )
				),
				NAME_None,
				EUserInterfaceActionType::RadioButton
			);
		}
	}

	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

void SObjectBrowser::HandlePIEStart(const bool bIsSimulating)
{
	UE_LOG(LogObjectBrowser, Log, TEXT("On PIE Start"));

	UWorld* PIEWorld = nullptr;
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		UWorld* World = Context.World();
		if (World && World->WorldType == EWorldType::PIE)
		{
			PIEWorld = World;
			break;
		}
	}
	OnSelectWorld(PIEWorld);
}

void SObjectBrowser::HandlePIEEnd(const bool bIsSimulating)
{
	UE_LOG(LogObjectBrowser, Log, TEXT("On PIE End"));

	UWorld* EditorWorld = nullptr;
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		UWorld* World = Context.World();
		if (World->WorldType == EWorldType::Editor)
		{
			EditorWorld = World;
			break;
		}
	}

	OnSelectWorld(EditorWorld);
}


TSharedRef<IDetailsView> SObjectBrowser::CreateDetails()
{
	bool bShowHidden = UObjectBrowserSettings::Get()->ShouldShowHiddenProperties();

	FDetailsViewArgs DetailsViewArgs;
	//DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::ObjectsUseNameArea,
	DetailsViewArgs.ViewIdentifier = TEXT("ObjectBrowserDetailsView");
	DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Show;
	DetailsViewArgs.bShowPropertyMatrixButton = false;
	DetailsViewArgs.bShowAnimatedPropertiesOption = false;
	DetailsViewArgs.bShowKeyablePropertiesOption = false;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.bAllowFavoriteSystem = false; // no favorites here
	// show All properties. possibly apply custom property filter or custom checkbox.
	// but there is no way to change its value via IDetailsView interface
	// so show all and filter visibility by IsPropertyVisible
	DetailsViewArgs.bForceHiddenPropertyVisibility = bShowHidden;

	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	TSharedRef<IDetailsView> DetailViewWidget = EditModule.CreateDetailView( DetailsViewArgs );

	DetailViewWidget->SetIsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateSP(this, &SObjectBrowser::IsDetailsPropertyEditingEnabled));
	DetailViewWidget->SetIsPropertyReadOnlyDelegate(FIsPropertyReadOnly::CreateSP(this, &SObjectBrowser::IsDetailsPropertyReadOnly));
	DetailViewWidget->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateSP(this, &SObjectBrowser::IsDetailsPropertyVisible));

	// DetailViewWidget->SetCustomFilterLabel(LOCTEXT("ShowAllParameters", "Show All Parameters"));
	// DetailViewWidget->SetCustomFilterDelegate(FSimpleDelegate::CreateSP(this, &SObjectBrowserPanel::ToggleShowingOnlyAllowedProperties));
	// DetailViewWidget->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateSP(this, &SObjectBrowserPanel::GetIsPropertyVisible));
	// DetailViewWidget->SetIsCustomRowVisibleDelegate(FIsCustomRowVisible::CreateSP(this, &SObjectBrowserPanel::GetIsRowVisible));

	return DetailViewWidget;
}

void SObjectBrowser::RecreateDetails()
{
	TSharedPtr<IDetailsView> ExistingDetails = DetailsView;

	DetailsView = CreateDetails();

#if UE_VERSION_OLDER_THAN(5, 0, 0)
	DetailsViewBox->ClearChildren();
	DetailsViewBox->AddSlot().Padding(0, 4, 0, 2) [ DetailsView.ToSharedRef() ];
#else
	DetailsViewBox->GetSlot(0) [ DetailsView.ToSharedRef() ];
#endif

	// Copy other props from existing details ?
	ExistingDetails.Reset();
}

void SObjectBrowser::SetSelectedObject(FObjectTreeItemPtr Item)
{
	UObject* InObject = Item.IsValid() ? Item->GetObjectForDetails() : nullptr;
	UE_LOG(LogObjectBrowser, Log, TEXT("Selected object %s"), *GetNameSafe(InObject));

	ObjectModel->NotifySelected(Item);

	if (DetailsView.IsValid())
	{
		DetailsView->SetObject(InObject);
		RefreshDetails();
	}
}

void SObjectBrowser::ResetSelectedObject()
{
	UE_LOG(LogObjectBrowser, Log, TEXT("Reset selected object"));

	ObjectModel->NotifySelected(nullptr);

	if (DetailsView.IsValid())
	{
		DetailsView->SetObject(nullptr);
		RefreshDetails();
	}
}

bool SObjectBrowser::IsDetailsPropertyEditingEnabled()
{
	// Always allow editing
	return true;
}

bool SObjectBrowser::IsDetailsPropertyReadOnly(const FPropertyAndParent& InProperty)
{
	if (UObjectBrowserSettings::Get()->ShouldEditAnyProperties())
	{
		return false;
	}

	const FProperty* Property = InProperty.ParentProperties.Num() > 0 ? InProperty.ParentProperties.Last() : &InProperty.Property;

	bool bReadOnly = Property->HasAnyPropertyFlags(CPF_EditConst) || !Property->HasAnyPropertyFlags(CPF_Edit);

	if (Property->HasAnyPropertyFlags(CPF_Config))
	{ // config always editable
		return false;
	}



	return bReadOnly;
}

bool SObjectBrowser::IsDetailsPropertyVisible(const FPropertyAndParent& InProperty)
{
	if (UObjectBrowserSettings::Get()->ShouldShowHiddenProperties())
	{
		return true;
	}

	const FProperty* Property = InProperty.ParentProperties.Num() > 0 ? InProperty.ParentProperties.Last() : &InProperty.Property;

	if (CastField<FDelegateProperty>(Property) || CastField<FMulticastDelegateProperty>(Property))
	{ // hide blueprint delegate properties
		return false;
	}

	return true;
}

FObjectTreeItemPtr SObjectBrowser::GetFirstSelectedItem() const
{
	return TreeWidget->GetNumItemsSelected() ? TreeWidget->GetSelectedItems()[0] : FObjectTreeItemPtr();
}

const FObjectBrowserTreeObjectItem* SObjectBrowser::GetFirstSelectedObject() const
{
	for(FObjectTreeItemPtr Selected : TreeWidget->GetSelectedItems())
	{
		if (Selected->GetAsObjectDescriptor())
		{
			return Selected->GetAsObjectDescriptor();
		}
	}

	return nullptr;
}

FObjectTreeItemID SObjectBrowser::GetFirstSelectedItemId() const
{
	return TreeWidget->GetNumItemsSelected() ? TreeWidget->GetSelectedItems()[0]->GetID() : FObjectTreeItemID();
}

bool SObjectBrowser::IsItemSelected(FObjectTreeItemPtr Item)
{
	return TreeWidget->IsItemSelected(Item);
}

TMap<FObjectTreeItemID, bool> SObjectBrowser::GetParentsExpansionState() const
{
	TMap<FObjectTreeItemID, bool> ExpansionStates;

	for (const auto& Pair : TreeItemMap)
	{
		if (Pair.Value->GetChildren().Num() > 0)
		{
			ExpansionStates.Add(Pair.Key, Pair.Value->bExpanded);
		}
	}

	return ExpansionStates;
}

void SObjectBrowser::SetParentsExpansionState(const TMap<FObjectTreeItemID, bool>& ExpansionInfo)
{
	for (const auto& Pair : TreeItemMap)
	{
		auto& Item = Pair.Value;
		if (Item->GetNumChildren() > 0)
		{
			const bool* bExpandedPtr = ExpansionInfo.Find(Pair.Key);
			bool bExpanded = bExpandedPtr != nullptr ? *bExpandedPtr : Item->bExpanded;

			TreeWidget->SetItemExpansion(Item, bExpanded);
		}
	}
}

TSharedPtr<SWidget> SObjectBrowser::ConstructObjectContextMenu()
{
	UToolMenus* ToolMenus = UToolMenus::Get();

	if (!ToolMenus->IsMenuRegistered(FObjectBrowserModule::ObjectBrowserContextMenuName))
	{
		ToolMenus->RegisterMenu(FObjectBrowserModule::ObjectBrowserContextMenuName);
		UE_LOG(LogObjectBrowser, Warning, TEXT("For some reason context menu did not register"));
	}

	FToolMenuContext Context;
	UToolMenu* Menu = ToolMenus->GenerateMenu(FObjectBrowserModule::ObjectBrowserContextMenuName, Context);

	FObjectTreeItemPtr Selected = GetFirstSelectedItem();
	if (Selected.IsValid())
	{
		Selected->GenerateContextMenu(Menu);

		// Apply customizations
		FObjectBrowserModule::OnGenerateContextMenu.Broadcast(Selected.ToSharedRef(), Menu);
	}

	return ToolMenus->GenerateWidget(Menu);
}

bool SObjectBrowser::HasSelectedObject() const
{
	auto SelectedObject = GetFirstSelectedItem();
	return SelectedObject.IsValid()  && SelectedObject->GetAsObjectDescriptor();
}

void SObjectBrowser::OnSettingsChanged(FName InPropertyName)
{
	if (FProperty* Property = UObjectBrowserSettings::StaticClass()->FindPropertyByName(InPropertyName))
	{
		if (Property->HasMetaData(FObjectBrowserConfigMeta::MD_ConfigAffectsView))
		{
			bFullRefresh = true;
			RefreshView();
		}
		if (Property->HasMetaData(FObjectBrowserConfigMeta::MD_ConfigAffectsColumns))
		{
			RefreshColumns();
		}
		if (Property->HasMetaData(FObjectBrowserConfigMeta::MD_ConfigAffectsDetails))
		{
			RecreateDetails();
		}
	}
}

void SObjectBrowser::OnModulesChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange)
{
	RefreshView();
}

void SObjectBrowser::OnObjectDataChanged(TSharedRef<IObjectTreeItem> Item)
{
	RefreshView();
}

EColumnSortMode::Type SObjectBrowser::GetColumnSortMode(FName ColumnId) const
{
	if (SortByColumn == ColumnId)
	{
		auto Column = ObjectModel->FindTableColumn(ColumnId);
		if (Column.IsValid() && Column->SupportsSorting())
		{
			return SortMode;
		}
	}
	return EColumnSortMode::None;
}

void SObjectBrowser::OnColumnSortModeChanged(const EColumnSortPriority::Type SortPriority, const FName& ColumnId,
	const EColumnSortMode::Type InSortMode)
{
	auto Column = ObjectModel->FindTableColumn(ColumnId);
	if (!Column.IsValid() || !Column->SupportsSorting())
	{
		return;
	}

	SortByColumn = ColumnId;
	SortMode = InSortMode;

	bSortDirty = true;
}

void SObjectBrowser::SortItems(TArray<FObjectTreeItemPtr>& Items) const
{
	auto Column = ObjectModel->FindTableColumn(SortByColumn);
	if (Column.IsValid() && Column->SupportsSorting())
	{
		Column->SortItems(Items, SortMode);
	}
}

#undef LOCTEXT_NAMESPACE
