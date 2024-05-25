

#include "Item/ObjectTreeCategoryItem.h"

#include "Model/ObjectBrowserCategory.h"
#include "Model/ObjectBrowserModel.h"
#include "UI/SObjectBrowserTableItemTooltip.h"


#define LOCTEXT_NAMESPACE "ObjectBrowser"

FObjectTreeCategoryItem::FObjectTreeCategoryItem(TSharedRef<FObjectModel> InModel, TSharedRef<FObjectCategoryBase> InCategory)
	: Data(InCategory)
{
	Model = InModel;
}

FObjectTreeItemID FObjectTreeCategoryItem::GetID() const
{
	return Data->Name;
}

int32 FObjectTreeCategoryItem::GetSortOrder() const
{
	return Data->SortOrder;
}

FText FObjectTreeCategoryItem::GetDisplayName() const
{
	return Data->Label;
}

bool FObjectTreeCategoryItem::CanHaveChildren() const
{
	return true;
}

TArray<UObject*> FObjectTreeCategoryItem::Select(UWorld* InContext) const
{
	TArray<UObject*> Result;
	Data->Select(InContext,Result);
	return Result;
}

void FObjectTreeCategoryItem::GenerateTooltip(FObjectBrowserTableItemTooltipBuilder& TooltipBuilder) const
{
	TArray<ObjectTreeItemPtr> Objects;
	Model->GetAllObjectsInCategory(SharedThis(this), Objects);
	TooltipBuilder.AddPrimary(LOCTEXT("ObjectTooltipItem_NumSub", "Num Objects"), FText::AsNumber(Objects.Num()));
}

#undef LOCTEXT_NAMESPACE