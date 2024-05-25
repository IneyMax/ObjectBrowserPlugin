

#pragma once

#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Model/ObjectBrowserModel.h"

struct FObjectTreeObjectItem;
class SObjectTreeWidget;
class IDetailsView;


/**
 * Object Browser tab content widget
 */
class SObjectBrowser : public SCompoundWidget
{
	friend class SObjectHeaderRow;
public:
	SLATE_BEGIN_ARGS(SObjectBrowser)
		:_InWorld(nullptr)
		{}
		SLATE_ARGUMENT(UWorld*, InWorld)
	SLATE_END_ARGS()

	SObjectBrowser();
	virtual ~SObjectBrowser();

	FText GetFilterClassText() const;
	TSharedRef<SWidget> MakeFilterMenu();
	void AddBoolFilter(FMenuBuilder& MenuBuilder, FText Text, FText InToolTip, bool* BoolOption);
	void Construct(const FArguments& InArgs);
	bool IsItemSelected(ObjectTreeItemPtr Item);

protected:
	virtual void Tick( const FGeometry& AllotedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

	void Populate();
	void EmptyTreeItems();

	void RefreshView() { bNeedsRefresh = true; }
	void RefreshDetails() { bNeedRefreshDetails = true; }
	void RefreshColumns() { bNeedsRefresh = true; bNeedListRebuild = true; bNeedsColumnRefresh = true; }
	void RequestSort() { bSortDirty = true; }
	void FullRefresh();

	// Search bar

	void TransformItemToString(const IObjectTreeItem& Level, TArray<FString>& OutSearchStrings) const;
	void SetFilterText(const FText& InFilterText);
	FText GetSearchBoxText() const;
	FText GetFilterStatusText() const;
	FSlateColor GetFilterStatusTextColor() const;

	// View options panel

	void BrowserSplitterFinishedResizing();

	FSlateColor GetViewOptionsButtonForegroundColor() const;
	TSharedRef<SWidget> GetViewOptionsButtonContent();
	void BuildColumnPickerContent(FMenuBuilder& MenuBuilder);
	void BuildCategoryPickerContent(FMenuBuilder& MenuBuilder);

	void ToggleDisplayCategory(FObjectTreeItemID InCategory);
	bool GetCategoryDisplayStatus(FObjectTreeItemID InCategory);

	// Tree view
	void SetupColumns(SHeaderRow& HeaderRow);
	TSharedRef<class ITableRow> GenerateTreeRow(ObjectTreeItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable);
	void GetChildrenForTree(ObjectTreeItemPtr Item, TArray<ObjectTreeItemPtr>& OutChildren);
	void OnExpansionChanged(ObjectTreeItemPtr Item, bool bIsItemExpanded);
	void OnSelectionChanged(const ObjectTreeItemPtr Item, ESelectInfo::Type SelectInfo);
	void OnTreeViewMouseButtonDoubleClick(ObjectTreeItemPtr Item);

	EColumnSortMode::Type GetColumnSortMode(FName ColumnId) const;
	void OnColumnSortModeChanged( const EColumnSortPriority::Type SortPriority, const FName& ColumnId, const EColumnSortMode::Type InSortMode );
	void SortItems(TArray<ObjectTreeItemPtr>& Items) const;

	void ToggleDisplayColumn(FName ColumnName);
	void ToggleTableColoring();
	void ToggleShowHiddenProperties();
	void ToggleShouldShowOnlyGame();
	void ToggleShouldShowOnlyPlugins();

	void ShowPluginSettingsTab() const;

	// Selection and Expansion
	FReply OnClassSelectionClicked();

	TMap<FObjectTreeItemID, bool> GetParentsExpansionState() const;
	void SetParentsExpansionState(const TMap<FObjectTreeItemID, bool>& ExpansionInfo);

	ObjectTreeItemPtr GetFirstSelectedItem() const;
	const FObjectTreeObjectItem* GetFirstSelectedObject() const;
	FObjectTreeItemID GetFirstSelectedItemId() const;

	// World picker

	const FSlateBrush* GetWorldsMenuBrush() const;
	FText GetCurrentWorldText() const;
	FText GetWorldDescription(UWorld* World) const;
	void OnSelectWorld(TWeakObjectPtr<UWorld> InWorld);
	bool IsWorldChecked(TWeakObjectPtr<UWorld> InWorld);
	TSharedRef<SWidget> GetWorldsButtonContent();

	void HandlePIEStart(const bool bIsSimulating);
	void HandlePIEEnd(const bool bIsSimulating);

	// Details

	TSharedRef<IDetailsView> CreateDetails();
	void RecreateDetails();
	void SetSelectedObject(ObjectTreeItemPtr Item);
	void ResetSelectedObject();

	bool IsDetailsPropertyEditingEnabled();
	bool IsDetailsPropertyReadOnly(const FPropertyAndParent& InProperty);
	bool IsDetailsPropertyVisible(const FPropertyAndParent& InProperty);

	// Item context menu

	TSharedPtr<SWidget> ConstructObjectContextMenu();
	bool HasSelectedObject() const;

	// Settings

	void OnSettingsChanged(FName InPropertyName);

	// Data tracking

	void OnModulesChanged(FName ModuleThatChanged, EModuleChangeReason ReasonForChange);
	void OnObjectDataChanged(TSharedRef<IObjectTreeItem> Item);

private:
	TSharedPtr<FObjectModel> ObjectModel;

	TSharedPtr<IDetailsView>	DetailsView;
	TSharedPtr<SVerticalBox>	DetailsViewBox;
	TSharedPtr<SSplitter>		BrowserSplitter;

	TSharedPtr<SComboButton>    ViewOptionsComboButton;

	TSharedPtr<ObjectTextFilter> SearchBoxObjectFilter;
	TSharedPtr<ObjectCategoryFilter> CategoryFilter;
	int32 FilteredObjectsCount = 0;

	TSharedPtr<SObjectHeaderRow> HeaderRowWidget;

	TArray<ObjectColumnPtr> DynamicColumnSlots;

	/** Root items for the tree widget */
	TArray<ObjectTreeItemPtr> RootTreeItems;

	/** All items that are currently displayed in the tree widget */
	TMap<FObjectTreeItemID, ObjectTreeItemPtr> TreeItemMap;

	TSharedPtr<SObjectTreeWidget> TreeWidget;

	UClass* FilterClass = UObject::StaticClass();
	bool bShouldIncludeDefaultObjects = false;
	bool bOnlyListDefaultObjects = false;
	bool bOnlyListRootObjects = false;
	bool bOnlyListGCObjects = false;
	bool bIncludeTransient = false;

	bool bIsReentrant = false;
	bool bFullRefresh = true;
	bool bNeedsRefresh = true; // needs initial update
	bool bNeedRefreshDetails = false;
	bool bUpdatingSelection = false;
	bool bLoadedExpansionSettings = false;
	bool bNeedListRebuild = true; // needs initial header update to upply config
	bool bNeedsColumnRefresh = false; // refresh header widgets?
	
	FName SortByColumn;
	EColumnSortMode::Type SortMode = EColumnSortMode::None;
	bool bSortDirty = false;
};
