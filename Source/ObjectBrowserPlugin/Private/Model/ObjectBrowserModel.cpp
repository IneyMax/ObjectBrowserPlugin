

#include "Model/ObjectBrowserModel.h"

#include "ObjectBrowserModule.h"
#include "ObjectBrowserSettings.h"
#include "ObjectBrowserUtils.h"
#include "Item/ObjectBrowserTreeCategoryItem.h"
#include "Item/ObjectBrowserTreeObjectItem.h"
#include "Model/ObjectBrowserFilters.h"

#define LOCTEXT_NAMESPACE "ObjectBrowser"

FObjectModel::FObjectModel()
{
	FObjectBrowserModule::AddPermanentColumns(PermanentColumns);
}

TWeakObjectPtr<UWorld> FObjectModel::GetCurrentWorld() const
{
	return CurrentWorld;
}

void FObjectModel::SetCurrentWorld(TWeakObjectPtr<UWorld> InWorld)
{
	CurrentWorld = InWorld;

	EmptyModel();

	PopulateCategories();
	PopulateObjects();
}

bool FObjectModel::IsObjectFilterActive() const
{
	return ObjectTextFilter.IsValid() && ObjectTextFilter->HasText();
}

int32 FObjectModel::GetNumCategories() const
{
	return AllCategories.Num();
}

const TArray<FObjectTreeItemPtr>& FObjectModel::GetAllCategories() const
{
	return AllCategories;
}

void FObjectModel::GetFilteredCategories(TArray<FObjectTreeItemPtr>& OutCategories) const
{
	OutCategories.Empty();

	for (const FObjectTreeItemPtr& Item : GetAllCategories())
	{
		check(Item->GetAsCategoryDescriptor());
		if (!CategoryFilter.IsValid() || CategoryFilter->PassesFilter(*Item))
		{
			OutCategories.Add(Item);
		}
	}
}

const TArray<FObjectTreeItemPtr>& FObjectModel::GetAllObjects() const
{
	return AllObjects;
}

void FObjectModel::GetAllObjectsInCategory(FObjectTreeItemConstPtr Category, TArray<FObjectTreeItemPtr>& OutChildren) const
{
	check(Category->GetAsCategoryDescriptor());

	OutChildren.Empty();
	if (AllObjectsByCategory.Contains(Category->GetID()))
	{
		for (const FObjectTreeItemPtr& Item : AllObjectsByCategory.FindChecked(Category->GetID()))
		{
			OutChildren.Add(Item);
		}
	}
}

void FObjectModel::GetFilteredObjects(FObjectTreeItemConstPtr Category, TArray<FObjectTreeItemPtr>& OutChildren) const
{
	FObjectBrowserTreeCategoryItem* AsCategory = Category->GetAsCategoryDescriptor();
	check(AsCategory);

	OutChildren.Empty();

	const UObjectBrowserSettings* Settings = UObjectBrowserSettings::Get();

	if (AllObjectsByCategory.Contains(AsCategory->GetID()))
	{
		for (const FObjectTreeItemPtr& Item : AllObjectsByCategory.FindChecked(AsCategory->GetID()))
		{
			if (Settings->ShouldShowOnlyGame() && !Item->IsGameModule())
				continue;
			if (Settings->ShouldShowOnlyPlugins() && !Item->IsPluginModule())
				continue;

			if (!ObjectTextFilter.IsValid() || ObjectTextFilter->PassesFilter(*Item))
			{
				OutChildren.Add(Item);
			}
		}
	}
}


int32 FObjectModel::GetNumObjectsFromVisibleCategories() const
{
	int32 Count = 0;

	TArray<FObjectTreeItemPtr> VisibleCategories;
	GetFilteredCategories(VisibleCategories);

	TArray<FObjectTreeItemPtr> Objects;

	for (const FObjectTreeItemPtr& Category : VisibleCategories)
	{
		GetAllObjectsInCategory(Category, Objects);

		Count += Objects.Num();
	}

	return Count;
}

int32 FObjectModel::GetNumDynamicColumns() const
{
	return FObjectBrowserModule::Get().GetDynamicColumns().Num();
}

bool FObjectModel::ShouldShowColumn(FObjectColumnPtr Column) const
{
	if (PermanentColumns.Contains(Column))
		return true;

	return UObjectBrowserSettings::Get()->GetTableColumnState(Column->Name);
}

bool FObjectModel::IsItemSelected(TSharedRef<const IObjectTreeItem> Item)
{
	return LastSelectedItem == Item;
}

void FObjectModel::NotifySelected(TSharedPtr<IObjectTreeItem> Item)
{
	if (Item.IsValid())
	{
		LastSelectedItem = Item;
	}

	OnSelectionChanged.Broadcast(Item);
}

TArray<FObjectColumnPtr> FObjectModel::GetSelectedTableColumns() const
{
	const UObjectBrowserSettings* Settings = UObjectBrowserSettings::Get();

	TArray<FObjectColumnPtr> Result;
	Result.Append(PermanentColumns);

	for (const FObjectColumnPtr& Column : FObjectBrowserModule::Get().GetDynamicColumns())
	{
		if (Settings->GetTableColumnState(Column->Name))
		{
			Result.Add(Column);
		}
	}

	Result.StableSort(FObjectColumnSorter());
	return Result;
}

TArray<FObjectColumnPtr> FObjectModel::GetDynamicTableColumns() const
{
	TArray<FObjectColumnPtr> Result;
	Result.Append(FObjectBrowserModule::Get().GetDynamicColumns());
	Result.StableSort(FObjectColumnSorter());
	return Result;
}

FObjectColumnPtr FObjectModel::FindTableColumn(const FName& ColumnName) const
{
	TArray<FObjectColumnPtr, TInlineAllocator<8>> Result;
	Result.Append(PermanentColumns);
	Result.Append(FObjectBrowserModule::Get().GetDynamicColumns());

	for (const FObjectColumnPtr& Column : Result)
	{
		if (Column->Name == ColumnName)
		{
			return Column;
		}
	}
	return nullptr;
}

void FObjectModel::EmptyModel()
{
	for (const FObjectTreeItemPtr& Category : AllCategories)
	{
		Category->RemoveAllChildren();
	}
	AllCategories.Empty();

	AllObjects.Empty();
	AllObjectsByCategory.Empty();

	LastSelectedItem.Reset();
}

void FObjectModel::PopulateCategories()
{
	FObjectBrowserModule& BrowserModule = FObjectBrowserModule::Get();
	for (auto& ObjectCategory : BrowserModule.GetCategories())
	{
		auto Category = MakeShared<FObjectBrowserTreeCategoryItem>(SharedThis(this), ObjectCategory.ToSharedRef());

		AllCategories.Add(MoveTemp(Category));
	}

	// sort categories after populating them
	AllCategories.StableSort(FObjectCategorySorter());
}

void FObjectModel::PopulateObjects()
{
	check(!AllObjects.Num());
	check(!AllObjectsByCategory.Num());

	UWorld* const LocalWorld = CurrentWorld.Get();

	for (const auto & Category : AllCategories)
	{
		const FObjectBrowserTreeCategoryItem* AsCategory = Category->GetAsCategoryDescriptor();

		for (UObject* Impl : AsCategory->Select(LocalWorld))
		{
			auto TreeObjectItem = MakeShared<FObjectBrowserTreeObjectItem>(SharedThis(this), Category, Impl);

			AllObjects.Add(TreeObjectItem);
			AllObjectsByCategory.FindOrAdd(AsCategory->GetID()).Add(TreeObjectItem);
		}
	}
}

#undef LOCTEXT_NAMESPACE
