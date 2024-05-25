

#pragma once

#include "CoreMinimal.h"

/**
 * Object Browser data provider delegate
 */
DECLARE_DELEGATE_TwoParams(FEnumObjectBrowserDelegate, UWorld* /* InContext */, TArray<UObject*>& /* OutData */);

/**
 * Represents a type of subsystem that will be shown in browser
 */
struct OBJECTBROWSERPLUGIN_API FObjectCategory : public TSharedFromThis<FObjectCategory>
{
	/* Category config identifier */
	FName Name;
	/* Category display title */
	FText Label;
	/* Sort weight for the category (with 0 being topmost, 1000 bottom last) */
	int32 SortOrder = 0;

	FObjectCategory() = default;
	FObjectCategory(const FName& Name, const FText& Label, int32 SortOrder);
	virtual ~FObjectCategory() = default;

	const FName& GetID() const { return Name; }
	const FText& GetDisplayName() const { return Label; }
	int32 GetSortOrder() const { return SortOrder; }

	virtual bool IsVisibleByDefault() const { return true; }

	/* Select subsystems for the respected category */
	virtual void Select(UWorld* InContext, TArray<UObject*>& OutData) const = 0;
};

using ObjectCategoryPtr = TSharedPtr<FObjectCategory>;

/**
 * Basic implementation of category that take in delegate selector
 */
struct OBJECTBROWSERPLUGIN_API FSimpleSubsystemCategory : public FObjectCategory
{
	/* Data supplier function */
	FEnumObjectBrowserDelegate Selector;

	FSimpleSubsystemCategory() = default;
	FSimpleSubsystemCategory(const FName& Name, const FText& Label, const FEnumObjectBrowserDelegate& Selector, int32 SortOrder);

	virtual void Select(UWorld* InContext, TArray<UObject*>& OutData) const override;
};