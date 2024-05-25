

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class SObjectBrowser;

DECLARE_LOG_CATEGORY_EXTERN(LogObjectBrowserPlugin, Log, All);

class FObjectBrowserPluginModule : public IModuleInterface
{
public:
	static const FName ObjectBrowserTabName;
	static const FName ObjectBrowserContextMenuName;
	static const FName PanelIconName;

public:
	static FObjectBrowserPluginModule& Get()
	{
		return FModuleManager::GetModuleChecked<FObjectBrowserPluginModule>(TEXT("ObjectBrowserPlugin"));
	}
	
	/** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

	virtual bool SupportsDynamicReloading() override { return false; }
	
	/**
	 * Register a new subsystem category
	 */
	template<typename TCategory, typename... TArgs>
	void RegisterCategory(TArgs&&... InArgs);
	
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
	 * Open editor settings tab with plugin settings pre-selected
	 */
	void SummonPluginSettingsTab();

	/**
	 * Open ObjectBrowser tab
	 */
	void SummonObjectTab();
};

template <typename TCategory, typename... TArgs>
void FObjectBrowserPluginModule::RegisterCategory(TArgs&&... InArgs)
{
	RegisterCategory(MakeShared<TCategory>(Forward<TArgs>(InArgs)...));
}

template <typename TColumn, typename... TArgs>
void FObjectBrowserPluginModule::RegisterDynamicColumn(TArgs&&... InArgs)
{
	RegisterDynamicColumn(MakeShared<TColumn>(Forward<TArgs>(InArgs)...));
}