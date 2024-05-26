


#include "ObjectBrowserSettings.h"
#include "Model/ObjectBrowserFilters.h"


FObjectCategoryFilter::FObjectCategoryFilter()
{
	// load initial state from config
	UObjectBrowserSettings::Get()->LoadCategoryStates(FilterState);
}

bool FObjectCategoryFilter::PassesFilter(const IObjectTreeItem& InItem) const
{
	return IsCategoryVisible(InItem.GetID());
}

void FObjectCategoryFilter::ShowCategory(FObjectTreeItemID InCategory)
{
	FilterState.Add(InCategory, true);
	UObjectBrowserSettings::Get()->SetCategoryState(InCategory, true);
	OnChangedInternal.Broadcast();
}

void FObjectCategoryFilter::HideCategory(FObjectTreeItemID InCategory)
{
	FilterState.Add(InCategory, false);
	UObjectBrowserSettings::Get()->SetCategoryState(InCategory, false);
	OnChangedInternal.Broadcast();
}

bool FObjectCategoryFilter::IsCategoryVisible(FObjectTreeItemID InCategory) const
{
	return !FilterState.Contains(InCategory) ? true : FilterState.FindChecked(InCategory);
}