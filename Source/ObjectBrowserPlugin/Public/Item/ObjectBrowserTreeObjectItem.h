

#pragma once

#include "IObjectTreeItem.h"
#include "ObjectBrowserFwd.h"


/**
 * Object node
 */
struct OBJECTBROWSERPLUGIN_API FObjectBrowserTreeObjectItem final : public IObjectTreeItem
{
	TWeakObjectPtr<UObject>			Object;
	TWeakObjectPtr<UClass>			Class;

	FText							DisplayName;
	FName							ClassName;
	FString							Package;
	FString							LongPackage;
	FString							ShortPackage;
	FName							ConfigName;

	FString							OwnerName;

	TArray<FString>					SourceFilePaths;

	FString							PluginName;
	FString							PluginDisplayName;
	
	FClassPropertyCounts			PropertyStats;

	bool							bConfigExportable = false;
	bool							bIsDefaultConfig = false;
	bool							bIsGameModuleClass = false;
	bool							bIsPluginClass = false;

	FObjectBrowserTreeObjectItem();
	FObjectBrowserTreeObjectItem(TSharedRef<FObjectModel> InModel, TSharedPtr<IObjectTreeItem> InParent, UObject* Instance);

	virtual FObjectTreeItemID GetID() const override { return ClassName; }
	virtual bool IsSelected() const override;

	virtual FText GetDisplayName() const override;

	virtual FObjectBrowserTreeObjectItem* GetAsObjectDescriptor() const override {  return const_cast<FObjectBrowserTreeObjectItem*>(this); }
	virtual UObject* GetObjectForDetails() const override { return Object.Get(); }
	virtual bool IsStale() const override { return Object.IsStale() || Class.IsStale(); }
	virtual bool IsConfigExportable() const override { return bConfigExportable; }
	bool IsDefaultConfig() const { return bIsDefaultConfig; }
	virtual bool IsGameModule() const override { return bIsGameModuleClass; }
	virtual bool IsPluginModule() const override { return bIsPluginClass; }

	virtual void GenerateTooltip(class FObjectBrowserTableItemTooltipBuilder& TooltipBuilder) const override;
	virtual void GenerateContextMenu(class UToolMenu* MenuBuilder) const override;
};