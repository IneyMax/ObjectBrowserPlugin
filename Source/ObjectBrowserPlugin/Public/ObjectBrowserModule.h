

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Model/ObjectBrowserCategory.h" // [no-fwd]
#include "Model/ObjectBrowserColumn.h" // [no-fwd]

class UObjectBrowserSettings;
class UToolMenu;
struct IObjectTreeItem;

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
	OBJECTBROWSERPLUGIN_API const TArray<ObjectCategoryPtr>& GetCategories() const;
	/**
	 * Register default Object categories
	 */
	void RegisterDefaultCategories();
	/**
	 * Register a new Object category
	 */
	template<typename TCategory, typename... TArgs>
	void RegisterCategory(TArgs&&... InArgs);
	/**
	 * Register a new Object category to show
	 */
	OBJECTBROWSERPLUGIN_API void RegisterCategory(TSharedRef<FObjectCategory> InCategory);
	/**
	 * Remove a category by its name
	 */
	OBJECTBROWSERPLUGIN_API void RemoveCategory(FName CategoryName);

	/**
	 * Get a list of all custom dynamic columns
	 */
	const TArray<ObjectColumnPtr>& GetDynamicColumns() const;
	/**
	 *
	 */
	void RegisterDefaultDynamicColumns();
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
	static void AddPermanentColumns(TArray<ObjectColumnPtr>& Columns);

	/**
	 * Open editor settings tab with plugin settings pre-selected
	 */
	void SummonPluginSettingsTab();

	/**
	 * Open subsystems tab
	 */
	void SummonSubsystemTab();

	/**
	 * Callback that is called whenever an owner name is needed to be obtained for the subsystem
	 */
	DECLARE_DELEGATE_RetVal_OneParam(FString, FOnGetSubsystemOwnerName, UObject*);
	static OBJECTBROWSERPLUGIN_API FOnGetSubsystemOwnerName OnGetSubsystemOwnerName;

	/**
	 * Callback that is called whenever a tooltip for item needs to be generated
	 * Used to add custom data to tooltips.
	 */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGenerateTooltip, TSharedRef<const ISubsystemTreeItem>, class FObjectBrowserTableItemTooltipBuilder&);
	static OBJECTBROWSERPLUGIN_API FOnGenerateTooltip OnGenerateTooltip;

	/**
	 * Callback that is called whenever a menu for item needs to be generated.
	 * Used to add custom menu actions.
	 */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGenerateMenu, TSharedRef<const ISubsystemTreeItem>, UToolMenu*);
	static OBJECTBROWSERPLUGIN_API FOnGenerateMenu OnGenerateContextMenu;

private:
	static TSharedRef<class SDockTab> HandleTabManagerSpawnTab(const FSpawnTabArgs& Args);
	static TSharedRef<class SWidget> CreateObjectBrowserWidget(const FSpawnTabArgs& Args);

	// Saved instance of Settings section
	TSharedPtr<class ISettingsSection> SettingsSection;
	// Instances of subsystem categories
	TArray<ObjectCategoryPtr> Categories;
	// Instances of dynamic subsystem columns
	TArray<ObjectColumnPtr> DynamicColumns;
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
