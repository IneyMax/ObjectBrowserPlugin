

#pragma once

#include "CoreMinimal.h"
#include "Model/ObjectBrowserColumn.h"
#include "Modules/ModuleInterface.h"


class FObjectBrowserModule : public IModuleInterface
{
public:
	static const FName ObjectBrowserTabName;
	static const FName ObjectBrowserContextMenuName;
	
public:
	static FObjectBrowserModule& Get()
	{
		return FModuleManager::GetModuleChecked<FObjectBrowserModule>(TEXT("ObjectBrowserPlugin"));
	}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override { return false; }

	/**
	 * Get list of all registered Object categories
	 */
	OBJECTBROWSERPLUGIN_API const TArray<FObjectCategoryPtr>& GetCategories() const;
	
	/**
	 * Register a new category
	 */
	template<typename TCategory, typename... TArgs>
	void RegisterCategory(TArgs&&... InArgs);
	
	/**
	 * Register a new category to show
	 */
	OBJECTBROWSERPLUGIN_API void RegisterCategory(TSharedRef<FObjectCategoryBase> InCategory);
	
	/**
	 * Remove a category by its name
	 */
	OBJECTBROWSERPLUGIN_API void RemoveCategory(FName CategoryName);

	/**
	 * Get a list of all custom dynamic columns
	 */
	const TArray<FObjectColumnPtr>& GetDynamicColumns() const;
	
	/**
	 * Register a new custom dynamic column
	 */
	template<typename TColumn, typename... TArgs>
	void RegisterDynamicColumn(TArgs&&... InArgs);
	
	/**
	 * Register a new custom dynamic column
	 */
	OBJECTBROWSERPLUGIN_API void RegisterDynamicColumn(TSharedRef<FObjectDynamicColumn> InColumn);
	
	/**
	 * Populate permanent columns
	 */
	static void AddPermanentColumns(TArray<FObjectColumnPtr>& Columns);

	/**
	 * Open editor settings tab with plugin settings pre-selected
	 */
	void SummonPluginSettingsTab();

	/**
	 * Open objects tab
	 */
	void SummonObjectTab();

	/**
	 * Callback that is called whenever an owner name is needed to be obtained for the object
	 */
	DECLARE_DELEGATE_RetVal_OneParam(FString, FOnGetObjectOwnerName, UObject*);
	static OBJECTBROWSERPLUGIN_API FOnGetObjectOwnerName OnGetObjectOwnerName;

	/**
	 * Callback that is called whenever a tooltip for item needs to be generated
	 * Used to add custom data to tooltips.
	 */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGenerateTooltip, TSharedRef<const IObjectTreeItem>, class FObjectBrowserTableItemTooltipBuilder&);
	static OBJECTBROWSERPLUGIN_API FOnGenerateTooltip OnGenerateTooltip;

	/**
	 * Callback that is called whenever a menu for item needs to be generated.
	 * Used to add custom menu actions.
	 */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGenerateMenu, TSharedRef<const IObjectTreeItem>, UToolMenu*);
	static OBJECTBROWSERPLUGIN_API FOnGenerateMenu OnGenerateContextMenu;

private:
	static TSharedRef<class SDockTab> HandleTabManagerSpawnTab(const FSpawnTabArgs& Args);
	static TSharedRef<class SWidget> CreateObjectBrowserWidget(const FSpawnTabArgs& Args);

	// Saved instance of Settings section
	TSharedPtr<class ISettingsSection> SettingsSection;
	
	// Instances of object categories
	TArray<FObjectCategoryPtr> Categories;
	
	// Instances of dynamic object columns
	TArray<FObjectColumnPtr> DynamicColumns;
};

template <typename TCategory, typename... TArgs>
void FObjectBrowserModule::RegisterCategory(TArgs&&... InArgs)
{
	RegisterCategory(MakeShared<TCategory>(Forward<TArgs>(InArgs)...));
}

template <typename TColumn, typename... TArgs>
void FObjectBrowserModule::RegisterDynamicColumn(TArgs&&... InArgs)
{
	RegisterDynamicColumn(MakeShared<TColumn>(Forward<TArgs>(InArgs)...));
}

#if UE_BUILD_DEBUG
DECLARE_LOG_CATEGORY_EXTERN(LogObjectBrowser, Log, All);
#else
DECLARE_LOG_CATEGORY_EXTERN(LogObjectBrowser, Log, Warning);
#endif
