

#pragma once
#include "ObjectBrowserTypes.h"

struct IObjectTreeItem;

struct FObjectBrowserTreeObjectItem;

struct FObjectDynamicColumn;

struct FObjectCategoryBase;
struct FObjectBrowserTreeCategoryItem;

using FObjectTreeItemID = FName;
using FObjectTreeItemConstPtr = TSharedPtr<const IObjectTreeItem>;
using FObjectTreeItemPtr = TSharedPtr<IObjectTreeItem>;
using FObjectCategoryPtr = TSharedPtr<FObjectCategoryBase>;
using FClassPropertyCounts = FClassFieldStats;
using FObjectColumnPtr = TSharedPtr<FObjectDynamicColumn>;

class FObjectModel;
class SObjectBrowser;

class SObjectBrowserTreeView;
class IDetailsView;

