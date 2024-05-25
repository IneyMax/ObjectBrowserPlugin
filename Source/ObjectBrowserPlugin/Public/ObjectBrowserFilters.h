

#pragma once

#include "Item/IObjectTreeItem.h"
#include "Misc/TextFilter.h"


/* Object text filter */
class ObjectTextFilter : public TTextFilter<const IObjectTreeItem&>
{
	using Super = TTextFilter<const IObjectTreeItem&>;
	
public:
	ObjectTextFilter(const FItemToStringArray& InTransformDelegate) : Super(InTransformDelegate) { }

	bool HasText() const { return !GetRawFilterText().IsEmpty(); }
};


/* Object category filter */
class ObjectCategoryFilter : public IFilter<const IObjectTreeItem&>
{
public:
	ObjectCategoryFilter();

	virtual FChangedEvent& OnChanged() override { return OnChangedInternal; }
	virtual bool PassesFilter(const IObjectTreeItem& InItem) const override;
	
	void ShowCategory(FObjectTreeItemID InCategory);
	void HideCategory(FObjectTreeItemID InCategory);
	bool IsCategoryVisible(FObjectTreeItemID InCategory) const;
	
private:
	TMap<FObjectTreeItemID, bool> FilterState;
	FChangedEvent OnChangedInternal;
};