

#pragma once
#include "ObjectBrowserFwd.h"

struct FObjectTreeCategoryItem;
struct FObjectTreeObjectItem;
class FObjectModel;


/*
 * Abstract object tree item node
 */
struct OBJECTBROWSERPLUGIN_API IObjectTreeItem : public TSharedFromThis<IObjectTreeItem>
{
	virtual ~IObjectTreeItem()  = default;

	virtual FObjectTreeItemID GetID() const = 0;
	virtual int32 GetSortOrder() const { return 0; }

	TSharedPtr<FObjectModel> GetModel() const { return Model; }
	ObjectTreeItemPtr GetParent() const { return Parent; }

	virtual bool CanHaveChildren() const { return false; }
	virtual TArray<ObjectTreeItemPtr> GetChildren() const { return Children; }
	virtual int32 GetNumChildren() const { return Children.Num(); }
	virtual void RemoveAllChildren() { Children.Empty(); }
	virtual bool IsSelected() const { return false; }

	virtual UObject* GetObjectForDetails() const { return nullptr; }
	virtual bool IsStale() const { return false; }
	virtual bool IsConfigExportable() const { return false; }
	virtual bool IsGameModule() const { return false; }
	virtual bool IsPluginModule() const { return false; }

	virtual FText GetDisplayName() const = 0;

	virtual FObjectTreeObjectItem* GetAsObjectDescriptor() const { return nullptr; }
	virtual FObjectTreeCategoryItem* GetAsCategoryDescriptor() const { return nullptr; }

	virtual void GenerateTooltip(class FObjectBrowserTableItemTooltipBuilder& TooltipBuilder) const {}
	virtual void GenerateContextMenu(class UToolMenu* MenuBuilder) const { }

	bool bExpanded = true;
	bool bVisible = true;
	bool bNeedsRefresh = true;
	bool bChildrenRequireSort = false;

	TSharedPtr<FObjectModel> Model;
	mutable ObjectTreeItemPtr Parent;
	mutable TArray<ObjectTreeItemPtr> Children;
};
