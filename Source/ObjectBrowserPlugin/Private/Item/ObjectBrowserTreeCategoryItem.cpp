

#include "Item/ObjectBrowserTreeCategoryItem.h"

#include "Model/ObjectBrowserCategory.h"
#include "Model/ObjectBrowserModel.h"
#include "UI/SObjectItemTooltip.h"


#define LOCTEXT_NAMESPACE "ObjectBrowser"

FObjectBrowserTreeCategoryItem::FObjectBrowserTreeCategoryItem(TSharedRef<FObjectModel> InModel, TSharedRef<FObjectCategoryBase> InCategory)
	: Data(InCategory)
{
	Model = InModel;
}

FObjectTreeItemID FObjectBrowserTreeCategoryItem::GetID() const
{
	return Data->Name;
}

int32 FObjectBrowserTreeCategoryItem::GetSortOrder() const
{
	return Data->SortOrder;
}

FText FObjectBrowserTreeCategoryItem::GetDisplayName() const
{
	return Data->Label;
}

bool FObjectBrowserTreeCategoryItem::CanHaveChildren() const
{
	return true;
}

TArray<UObject*> FObjectBrowserTreeCategoryItem::Select(UWorld* InContext) const
{
	TArray<UObject*> Result;
	Data->Select(InContext,Result);
	return Result;
}

void FObjectBrowserTreeCategoryItem::GenerateTooltip(FObjectBrowserTableItemTooltipBuilder& TooltipBuilder) const
{
	TArray<FObjectTreeItemPtr> Objects;
	Model->GetAllObjectsInCategory(SharedThis(this), Objects);
	TooltipBuilder.AddPrimary(LOCTEXT("ObjectTooltipItem_NumSub", "Num Objects"), FText::AsNumber(Objects.Num()));
}

#undef LOCTEXT_NAMESPACE