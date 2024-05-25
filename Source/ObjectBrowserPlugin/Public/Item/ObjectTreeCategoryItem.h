

#pragma once
#include "IObjectTreeItem.h"
#include "ObjectBrowserFwd.h"

class FObjectModel;
struct FObjectCategoryBase;


/**
 * Category node
 */
struct OBJECTBROWSERPLUGIN_API FObjectTreeCategoryItem final : public IObjectTreeItem
{
	TSharedPtr<FObjectCategoryBase> Data;

	FObjectTreeCategoryItem() = default;
	FObjectTreeCategoryItem(TSharedRef<FObjectModel> InModel, TSharedRef<FObjectCategoryBase> InCategory);

	virtual FObjectTreeItemID GetID() const override;
	virtual int32 GetSortOrder() const override;
	virtual FText GetDisplayName() const override;
	virtual bool CanHaveChildren() const override;
	virtual FObjectTreeCategoryItem* GetAsCategoryDescriptor() const override { return const_cast<FObjectTreeCategoryItem*>(this); }

	TArray<UObject*> Select(UWorld* InContext) const;

	virtual void GenerateTooltip(class FObjectBrowserTableItemTooltipBuilder& TooltipBuilder) const override;
};
