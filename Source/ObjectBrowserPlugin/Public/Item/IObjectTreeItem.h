

#pragma once
#include "ObjectBrowserFwd.h"


/*
 * Abstract object tree item node
 */
struct OBJECTBROWSERPLUGIN_API IObjectTreeItem : public TSharedFromThis<IObjectTreeItem>
{
	virtual ~IObjectTreeItem()  = default;

	virtual FObjectTreeItemID GetID() const = 0;
	virtual int32 GetSortOrder() const { return 0; }

	virtual bool CanHaveChildren() const { return false; }
	virtual TArray<FObjectTreeItemPtr> GetChildren() const { return Children; }
	virtual int32 GetNumChildren() const { return Children.Num(); }
	virtual void RemoveAllChildren() { Children.Empty(); }
	virtual bool IsSelected() const { return false; }

	virtual UObject* GetObjectForDetails() const { return nullptr; }
	virtual bool IsStale() const { return false; }
	virtual bool IsConfigExportable() const { return false; }
	virtual bool IsGameModule() const { return false; }
	virtual bool IsPluginModule() const { return false; }

	virtual FText GetDisplayName() const = 0;

	virtual FObjectBrowserTreeObjectItem* GetAsObjectDescriptor() const { return nullptr; }
	virtual FObjectBrowserTreeCategoryItem* GetAsCategoryDescriptor() const { return nullptr; }

	virtual void GenerateTooltip(class FObjectBrowserTableItemTooltipBuilder& TooltipBuilder) const {}
	virtual void GenerateContextMenu(class UToolMenu* MenuBuilder) const { }

	TSharedPtr<FObjectModel> GetModel() const { return Model; }
	FObjectTreeItemPtr GetParent() const { return Parent; }
	
	TSharedPtr<FObjectModel> Model;
	mutable FObjectTreeItemPtr Parent;
	mutable TArray<FObjectTreeItemPtr> Children;

	bool bExpanded = true;
	bool bVisible = true;
	bool bNeedsRefresh = true;
	bool bChildrenRequireSort = false;
};
