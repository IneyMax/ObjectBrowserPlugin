

#pragma once

#include "Model/ObjectBrowserDescriptor.h"
#include "Model/ObjectBrowserColumn.h"
#include "Misc/TextFilter.h"

/* Subsystem text filter */
class ObjectTextFilter : public TTextFilter<const ISubsystemTreeItem&>
{
	using Super = TTextFilter<const ISubsystemTreeItem&>;
public:
	ObjectTextFilter(const FItemToStringArray& InTransformDelegate) : Super(InTransformDelegate) { }

	bool HasText() const { return !GetRawFilterText().IsEmpty(); }
};

/* Subsystem category filter */
class ObjectCategoryFilter : public IFilter<const ISubsystemTreeItem&>
{
public:
	ObjectCategoryFilter();

	virtual FChangedEvent& OnChanged() override { return OnChangedInternal; }
	virtual bool PassesFilter(const ISubsystemTreeItem& InItem) const override;
	void ShowCategory(FObjectTreeItemID InCategory);
	void HideCategory(FObjectTreeItemID InCategory);
	bool IsCategoryVisible(FObjectTreeItemID InCategory) const;
private:
	TMap<FObjectTreeItemID, bool>	FilterState;
	FChangedEvent						OnChangedInternal;
};

struct SubsystemCategorySorter
{
	bool operator()(const SubsystemTreeItemPtr& A, const SubsystemTreeItemPtr& B) const
	{
		return A->GetSortOrder() < B->GetSortOrder();
	}
};

/* Subsystem list data model */
class FObjectModel : public TSharedFromThis<FObjectModel>
{
public:
	FObjectModel();

	TWeakObjectPtr<UWorld> GetCurrentWorld() const;
	void SetCurrentWorld(TWeakObjectPtr<UWorld> InWorld);

	bool IsSubsystemFilterActive() const;

	int32 GetNumCategories() const;
	const TArray<SubsystemTreeItemPtr>& GetAllCategories() const;
	void GetFilteredCategories(TArray<SubsystemTreeItemPtr>& OutCategories) const;

	const TArray<SubsystemTreeItemPtr>& GetAllObjects() const;
	void GetAllObjectsInCategory(SubsystemTreeItemConstPtr Category, TArray<SubsystemTreeItemPtr>& OutChildren) const;

	void GetFilteredSubsystems(SubsystemTreeItemConstPtr Category, TArray<SubsystemTreeItemPtr>& OutChildren) const;

	/* get total number of subsystems in visible categories */
	int32 GetNumSubsystemsFromVisibleCategories() const;

	/* find a permanent or dynamic column by its name */
	ObjectColumnPtr FindTableColumn(const FName& ColumnName) const;
	/* returns all visible permanent and dynamic columns in sorted order */
	TArray<ObjectColumnPtr> GetSelectedTableColumns() const;

	/* returns all dynamic columns in sorted order */
	TArray<ObjectColumnPtr> GetDynamicTableColumns() const;
	/* return a total number of dynamic columns registered */
	int32 GetNumDynamicColumns() const;
	/* check if dynamic column is enabled by settings */
	bool ShouldShowColumn(ObjectColumnPtr Column) const;

	bool IsItemSelected(TSharedRef<const ISubsystemTreeItem> Item);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemSelectionChange, TSharedPtr<ISubsystemTreeItem> /* Item */);
	/* delegate that is triggered when tree selection changed */
	FOnItemSelectionChange OnSelectionChanged;

	void NotifySelected(TSharedPtr<ISubsystemTreeItem> Item);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemDataChanged, TSharedRef<ISubsystemTreeItem> /* Item */);
	/* delegate that is triggered when one of subsystems in this model is changed and needs possible update */
	FOnItemDataChanged OnDataChanged;

private:
	void EmptyModel();
	void PopulateCategories();
	void PopulateObjects();

	/* Global list of all categories */
	TArray<SubsystemTreeItemPtr> AllCategories;
	/* Global list of all subsystems */
	TArray<SubsystemTreeItemPtr> AllObjects;
	/* Global list of all subsystems by category */
	TMap<FName, TArray<SubsystemTreeItemPtr>> AllObjectsByCategory;
	/* List of permanent columns */
	TArray<ObjectColumnPtr> PermanentColumns;

	/* Pointer to currently browsing world */
	TWeakObjectPtr<UWorld> CurrentWorld;
	/* Weak pointer to last selected item */
	TWeakPtr<ISubsystemTreeItem> LastSelectedItem;
	
public:
	TSharedPtr<ObjectCategoryFilter> CategoryFilter;
	TSharedPtr<ObjectTextFilter> ObjectTextFilter;
};
