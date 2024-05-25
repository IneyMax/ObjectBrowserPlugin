

#include "ObjectBrowserFilters.h"
#include "ObjectBrowserSettings.h"


ObjectCategoryFilter::ObjectCategoryFilter()
{
	// load initial state from config
	UObjectBrowserSettings::Get()->LoadCategoryStates(FilterState);
}

bool ObjectCategoryFilter::PassesFilter(const IObjectTreeItem& InItem) const
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