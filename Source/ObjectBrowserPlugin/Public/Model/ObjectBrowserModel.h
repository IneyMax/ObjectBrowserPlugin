

#pragma once

#include "ObjectBrowserFwd.h"

class ObjectTextFilter;
class FObjectCategoryFilter;
struct IObjectTreeItem;


/* Object list data model */
class FObjectModel : public TSharedFromThis<FObjectModel>
{
public:
	FObjectModel();

	TWeakObjectPtr<UWorld> GetCurrentWorld() const;
	void SetCurrentWorld(TWeakObjectPtr<UWorld> InWorld);

	bool IsObjectFilterActive() const;

	int32 GetNumCategories() const;
	const TArray<FObjectTreeItemPtr>& GetAllCategories() const;
	void GetFilteredCategories(TArray<FObjectTreeItemPtr>& OutCategories) const;

	const TArray<FObjectTreeItemPtr>& GetAllObjects() const;
	void GetAllObjectsInCategory(FObjectTreeItemConstPtr Category, TArray<FObjectTreeItemPtr>& OutChildren) const;

	void GetFilteredObjects(FObjectTreeItemConstPtr Category, TArray<FObjectTreeItemPtr>& OutChildren) const;

	/* get total number of objects in visible categories */
	int32 GetNumObjectsFromVisibleCategories() const;

	/* find a permanent or dynamic column by its name */
	FObjectColumnPtr FindTableColumn(const FName& ColumnName) const;
	
	/* returns all visible permanent and dynamic columns in sorted order */
	TArray<FObjectColumnPtr> GetSelectedTableColumns() const;

	/* returns all dynamic columns in sorted order */
	TArray<FObjectColumnPtr> GetDynamicTableColumns() const;
	
	/* return a total number of dynamic columns registered */
	int32 GetNumDynamicColumns() const;
	
	/* check if dynamic column is enabled by settings */
	bool ShouldShowColumn(FObjectColumnPtr Column) const;

	bool IsItemSelected(TSharedRef<const IObjectTreeItem> Item);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemSelectionChange, TSharedPtr<IObjectTreeItem> /* Item */);
	/* delegate that is triggered when tree selection changed */
	FOnItemSelectionChange OnSelectionChanged;

	void NotifySelected(TSharedPtr<IObjectTreeItem> Item);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemDataChanged, TSharedRef<IObjectTreeItem> /* Item */);
	/* delegate that is triggered when one of objects in this model is changed and needs possible update */
	FOnItemDataChanged OnDataChanged;

private:
	void EmptyModel();
	void PopulateCategories();
	void PopulateObjects();

	/* Global list of all categories */
	TArray<FObjectTreeItemPtr> AllCategories;
	
	/* Global list of all objects */
	TArray<FObjectTreeItemPtr> AllObjects;
	
	/* Global list of all objects by category */
	TMap<FName, TArray<FObjectTreeItemPtr>> AllObjectsByCategory;
	
	/* List of permanent columns */
	TArray<FObjectColumnPtr> PermanentColumns;

	/* Pointer to currently browsing world */
	TWeakObjectPtr<UWorld> CurrentWorld;
	
	/* Weak pointer to last selected item */
	TWeakPtr<IObjectTreeItem> LastSelectedItem;
	
public:
	TSharedPtr<FObjectCategoryFilter> CategoryFilter;
	TSharedPtr<ObjectTextFilter> ObjectTextFilter;
};
