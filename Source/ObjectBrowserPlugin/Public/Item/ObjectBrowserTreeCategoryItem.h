

#pragma once
#include "IObjectTreeItem.h"
#include "ObjectBrowserFwd.h"


/**
 * Category node
 */
struct OBJECTBROWSERPLUGIN_API FObjectBrowserTreeCategoryItem final : public IObjectTreeItem
{
	TSharedPtr<FObjectCategoryBase> Data;
	
	FObjectBrowserTreeCategoryItem() = default;
	FObjectBrowserTreeCategoryItem(TSharedRef<FObjectModel> InModel, TSharedRef<FObjectCategoryBase> InCategory);

	virtual FObjectTreeItemID GetID() const override;
	virtual int32 GetSortOrder() const override;
	virtual FText GetDisplayName() const override;
	virtual bool CanHaveChildren() const override;
	virtual FObjectBrowserTreeCategoryItem* GetAsCategoryDescriptor() const override { return const_cast<FObjectBrowserTreeCategoryItem*>(this); }
	virtual void GenerateTooltip(class FObjectBrowserTableItemTooltipBuilder& TooltipBuilder) const override;
	
	TArray<UObject*> Select(UWorld* InContext) const;
};
