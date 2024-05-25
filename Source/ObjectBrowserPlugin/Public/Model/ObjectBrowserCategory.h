

#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_TwoParams(FEnumObjectBrowserDelegate, UWorld* /* InContext */, TArray<UObject*>& /* OutData */);


/**
 * Represents a type of object that will be shown in browser
 */
struct OBJECTBROWSERPLUGIN_API FObjectCategoryBase : public TSharedFromThis<FObjectCategoryBase>
{
	/* Category config identifier */
	FName Name;
	/* Category display title */
	FText Label;
	/* Sort weight for the category (with 0 being topmost, 1000 bottom last) */
	int32 SortOrder = 0;

	FObjectCategoryBase() = default;
	FObjectCategoryBase(const FName& Name, const FText& Label, int32 SortOrder);
	virtual ~FObjectCategoryBase() = default;

	const FName& GetID() const { return Name; }
	const FText& GetDisplayName() const { return Label; }
	int32 GetSortOrder() const { return SortOrder; }

	virtual bool IsVisibleByDefault() const { return true; }

	/* Select objects for the respected category */
	virtual void Select(UWorld* InContext, TArray<UObject*>& OutData) const = 0;
};


/**
 * Basic implementation of category that take in delegate selector
 */
struct OBJECTBROWSERPLUGIN_API FSimpleObjectCategory : public FObjectCategoryBase
{
	/* Data supplier function */
	FEnumObjectBrowserDelegate Selector;

	FSimpleObjectCategory() = default;
	FSimpleObjectCategory(const FName& Name, const FText& Label, const FEnumObjectBrowserDelegate& Selector, int32 SortOrder);

	virtual void Select(UWorld* InContext, TArray<UObject*>& OutData) const override;
};