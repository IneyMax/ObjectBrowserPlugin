

#include "Model/ObjectBrowserCategory.h"

#include "ObjectBrowserFlags.h"

#define LOCTEXT_NAMESPACE "ObjectBrowser"

FObjectCategoryBase::FObjectCategoryBase(const FName& Name, const FText& Label, int32 SortOrder)
	: Name(Name), Label(Label), SortOrder(SortOrder)
{
}

FSimpleObjectCategory::FSimpleObjectCategory(const FName& Name, const FText& Label, const FEnumObjectBrowserDelegate& Selector, int32 SortOrder)
	: FObjectCategoryBase(Name, Label, SortOrder), Selector(Selector)
{
	ensure(Selector.IsBound());
}

void FSimpleObjectCategory::Select(UWorld* InContext, TArray<UObject*>& OutData) const
{
	Selector.Execute(InContext, OutData);
}

#undef LOCTEXT_NAMESPACE

#if ENABLE_SUBSYSTEM_BROWSER_EXAMPLES

// 2. Call this in your editor modules StartupModule to register a new category
void RegisterCategoryExample()
{
	// Get a reference to Object Browser module instance or load it
	FObjectBrowserModule& Module = FModuleManager::LoadModuleChecked<FObjectBrowserModule>(TEXT("ObjectBrowser"));
	// Construct category
	auto SampleCategory = MakeShared<FSimpleObjectCategory>();
	SampleCategory->Name = TEXT("Sample");
	SampleCategory->Label = INVTEXT("Sample ObjectBrowser");
	SampleCategory->SortOrder = 50;
	SampleCategory->Selector = FEnumObjectBrowserDelegate::CreateLambda([](UWorld* InContext, TArray<UObject*>& OutData)
	{
		// Fill Data
	});
	// Register category in module
	Module.RegisterCategory(SampleCategory);
}

#endif

