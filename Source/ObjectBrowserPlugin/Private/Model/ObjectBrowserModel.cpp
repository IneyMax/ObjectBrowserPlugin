

#include "Model/ObjectBrowserModel.h"

#include "ObjectBrowserModule.h"
#include "ObjectBrowserSettings.h"

#define LOCTEXT_NAMESPACE "ObjectBrowser"

ObjectCategoryFilter::ObjectCategoryFilter()
{
	// load initial state from config
	UObjectBrowserSettings::Get()->LoadCategoryStates(FilterState);
}

bool ObjectCategoryFilter::PassesFilter(const ISubsystemTreeItem& InItem) const
{
	return IsCategoryVisible(InItem.GetID());
}

void ObjectCategoryFilter::ShowCategory(FObjectTreeItemID InCategory)
{
	FilterState.Add(InCategory, true);
	UObjectBrowserSettings::Get()->SetCategoryState(InCategory, true);
	OnChangedInternal.Broadcast();
}

void ObjectCategoryFilter::HideCategory(FObjectTreeItemID InCategory)
{
	FilterState.Add(InCategory, false);
	UObjectBrowserSettings::Get()->SetCategoryState(InCategory, false);
	OnChangedInternal.Broadcast();
}

bool ObjectCategoryFilter::IsCategoryVisible(FObjectTreeItemID InCategory) const
{
	return !FilterState.Contains(InCategory) ? true : FilterState.FindChecked(InCategory);
}

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

bool FObjectModel::IsSubsystemFilterActive() const
{
	return ObjectTextFilter.IsValid() && ObjectTextFilter->HasText();
}

int32 FObjectModel::GetNumCategories() const
{
	return AllCategories.Num();
}

const TArray<SubsystemTreeItemPtr>& FObjectModel::GetAllCategories() const
{
	return AllCategories;
}

void FObjectModel::GetFilteredCategories(TArray<SubsystemTreeItemPtr>& OutCategories) const
{
	OutCategories.Empty();

	for (const SubsystemTreeItemPtr& Item : GetAllCategories())
	{
		check(Item->GetAsCategoryDescriptor());
		if (!CategoryFilter.IsValid() || CategoryFilter->PassesFilter(*Item))
		{
			OutCategories.Add(Item);
		}
	}
}

const TArray<SubsystemTreeItemPtr>& FObjectModel::GetAllObjects() const
{
	return AllObjects;
}

void FObjectModel::GetAllObjectsInCategory(SubsystemTreeItemConstPtr Category, TArray<SubsystemTreeItemPtr>& OutChildren) const
{
	check(Category->GetAsCategoryDescriptor());

	OutChildren.Empty();
	if (AllObjectsByCategory.Contains(Category->GetID()))
	{
		for (const SubsystemTreeItemPtr& Item : AllObjectsByCategory.FindChecked(Category->GetID()))
		{
			OutChildren.Add(Item);
		}
	}
}

void FObjectModel::GetFilteredSubsystems(SubsystemTreeItemConstPtr Category, TArray<SubsystemTreeItemPtr>& OutChildren) const
{
	FObjectTreeCategoryItem* AsCategory = Category->GetAsCategoryDescriptor();
	check(AsCategory);

	OutChildren.Empty();

	const UObjectBrowserSettings* Settings = UObjectBrowserSettings::Get();

	if (AllObjectsByCategory.Contains(AsCategory->GetID()))
	{
		for (const SubsystemTreeItemPtr& Item : AllObjectsByCategory.FindChecked(AsCategory->GetID()))
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


int32 FObjectModel::GetNumSubsystemsFromVisibleCategories() const
{
	int32 Count = 0;

	TArray<SubsystemTreeItemPtr> VisibleCategories;
	GetFilteredCategories(VisibleCategories);

	TArray<SubsystemTreeItemPtr> Subsystems;

	for (const SubsystemTreeItemPtr& Category : VisibleCategories)
	{
		GetAllObjectsInCategory(Category, Subsystems);

		Count += Subsystems.Num();
	}

	return Count;
}

int32 FObjectModel::GetNumDynamicColumns() const
{
	return FObjectBrowserModule::Get().GetDynamicColumns().Num();
}

bool FObjectModel::ShouldShowColumn(ObjectColumnPtr Column) const
{
	if (PermanentColumns.Contains(Column))
		return true;

	return UObjectBrowserSettings::Get()->GetTableColumnState(Column->Name);
}

bool FObjectModel::IsItemSelected(TSharedRef<const ISubsystemTreeItem> Item)
{
	return LastSelectedItem == Item;
}

void FObjectModel::NotifySelected(TSharedPtr<ISubsystemTreeItem> Item)
{
	if (Item.IsValid())
	{
		LastSelectedItem = Item;
	}

	OnSelectionChanged.Broadcast(Item);
}

TArray<ObjectColumnPtr> FObjectModel::GetSelectedTableColumns() const
{
	const UObjectBrowserSettings* Settings = UObjectBrowserSettings::Get();

	TArray<ObjectColumnPtr> Result;
	Result.Append(PermanentColumns);

	for (const ObjectColumnPtr& Column : FObjectBrowserModule::Get().GetDynamicColumns())
	{
		if (Settings->GetTableColumnState(Column->Name))
		{
			Result.Add(Column);
		}
	}

	Result.StableSort(SubsystemColumnSorter());
	return Result;
}

TArray<ObjectColumnPtr> FObjectModel::GetDynamicTableColumns() const
{
	TArray<ObjectColumnPtr> Result;
	Result.Append(FObjectBrowserModule::Get().GetDynamicColumns());
	Result.StableSort(SubsystemColumnSorter());
	return Result;
}

ObjectColumnPtr FObjectModel::FindTableColumn(const FName& ColumnName) const
{
	TArray<ObjectColumnPtr, TInlineAllocator<8>> Result;
	Result.Append(PermanentColumns);
	Result.Append(FObjectBrowserModule::Get().GetDynamicColumns());

	for (const ObjectColumnPtr& Column : Result)
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
	for (const SubsystemTreeItemPtr& Category : AllCategories)
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
	for (auto& SubsystemCategory : BrowserModule.GetCategories())
	{
		auto Category = MakeShared<FObjectTreeCategoryItem>(SharedThis(this), SubsystemCategory.ToSharedRef());

		AllCategories.Add(MoveTemp(Category));
	}

	// sort categories after populating them
	AllCategories.StableSort(SubsystemCategorySorter());
}

void FObjectModel::PopulateObjects()
{
	check(!AllObjects.Num());
	check(!AllObjectsByCategory.Num());

	UWorld* const LocalWorld = CurrentWorld.Get();

	for (const auto & Category : AllCategories)
	{
		const FObjectTreeCategoryItem* AsCategory = Category->GetAsCategoryDescriptor();

		for (UObject* Impl : AsCategory->Select(LocalWorld))
		{
			auto Descriptor = MakeShared<FObjectTreeSubsystemItem>(SharedThis(this), Category, Impl);

			AllObjects.Add(Descriptor);
			AllObjectsByCategory.FindOrAdd(AsCategory->GetID()).Add(Descriptor);
		}
	}
}

#undef LOCTEXT_NAMESPACE
