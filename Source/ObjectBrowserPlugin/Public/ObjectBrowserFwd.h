

#pragma once
#include "ObjectBrowserTypes.h"


struct FObjectDynamicColumn;
struct FObjectCategoryBase;
struct IObjectTreeItem;

using FObjectTreeItemID = FName;
using ObjectTreeItemPtr = TSharedPtr<IObjectTreeItem>;
using ObjectTreeItemConstPtr = TSharedPtr<const IObjectTreeItem>;
using ObjectCategoryPtr = TSharedPtr<FObjectCategoryBase>;
using FClassPropertyCounts = FClassFieldStats;
using ObjectColumnPtr = TSharedPtr<FObjectDynamicColumn>;