

#pragma once

#include "ObjectBrowserUtils.h"
#include "Model/ObjectBrowserCategory.h"

class FObjectModel;
struct ISubsystemTreeItem;
struct FObjectTreeSubsystemItem;
struct FObjectTreeCategoryItem;

using FObjectTreeItemID = FName;
using SubsystemTreeItemPtr = TSharedPtr<ISubsystemTreeItem>;
using SubsystemTreeItemConstPtr = TSharedPtr<const ISubsystemTreeItem>;

/*
 * Abstract subsystem tree item node
 */
struct OBJECTBROWSERPLUGIN_API ISubsystemTreeItem : public TSharedFromThis<ISubsystemTreeItem>
{
	virtual ~ISubsystemTreeItem()  = default;

	virtual FObjectTreeItemID GetID() const = 0;
	virtual int32 GetSortOrder() const { return 0; }

	TSharedPtr<FObjectModel> GetModel() const { return Model; }
	SubsystemTreeItemPtr GetParent() const { return Parent; }

	virtual bool CanHaveChildren() const { return false; }
	virtual TArray<SubsystemTreeItemPtr> GetChildren() const { return Children; }
	virtual int32 GetNumChildren() const { return Children.Num(); }
	virtual void RemoveAllChildren() { Children.Empty(); }
	virtual bool IsSelected() const { return false; }

	virtual UObject* GetObjectForDetails() const { return nullptr; }
	virtual bool IsStale() const { return false; }
	virtual bool IsConfigExportable() const { return false; }
	virtual bool IsGameModule() const { return false; }
	virtual bool IsPluginModule() const { return false; }

	virtual FText GetDisplayName() const = 0;

	virtual FObjectTreeSubsystemItem* GetAsSubsystemDescriptor() const { return nullptr; }
	virtual FObjectTreeCategoryItem* GetAsCategoryDescriptor() const { return nullptr; }

	virtual void GenerateTooltip(class FObjectBrowserTableItemTooltipBuilder& TooltipBuilder) const {}
	virtual void GenerateContextMenu(class UToolMenu* MenuBuilder) const { }

	bool bExpanded = true;
	bool bVisible = true;
	bool bNeedsRefresh = true;
	bool bChildrenRequireSort = false;

	TSharedPtr<FObjectModel> Model;
	mutable SubsystemTreeItemPtr Parent;
	mutable TArray<SubsystemTreeItemPtr> Children;
};

/**
 * Category node
 */
struct OBJECTBROWSERPLUGIN_API FObjectTreeCategoryItem final : public ISubsystemTreeItem
{
	TSharedPtr<FObjectCategory> Data;

	FObjectTreeCategoryItem() = default;
	FObjectTreeCategoryItem(TSharedRef<FObjectModel> InModel, TSharedRef<FObjectCategory> InCategory);

	virtual FObjectTreeItemID GetID() const override { return Data->Name; }
	virtual int32 GetSortOrder() const override { return Data->SortOrder; }
	virtual FText GetDisplayName() const override { return Data->Label; }
	virtual bool CanHaveChildren() const override { return true; }
	virtual FObjectTreeCategoryItem* GetAsCategoryDescriptor() const override { return const_cast<FObjectTreeCategoryItem*>(this); }

	TArray<UObject*> Select(UWorld* InContext) const;

	virtual void GenerateTooltip(class FObjectBrowserTableItemTooltipBuilder& TooltipBuilder) const override;
};

/**
 * Subsystem node
 */
struct OBJECTBROWSERPLUGIN_API FObjectTreeSubsystemItem final : public ISubsystemTreeItem
{
	TWeakObjectPtr<UObject>			Subsystem;
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

	using FClassPropertyCounts = FObjectBrowserUtils::FClassFieldStats;
	FClassPropertyCounts			PropertyStats;

	bool							bConfigExportable = false;
	bool							bIsDefaultConfig = false;
	bool							bIsGameModuleClass = false;
	bool							bIsPluginClass = false;

	FObjectTreeSubsystemItem();
	FObjectTreeSubsystemItem(TSharedRef<FObjectModel> InModel, TSharedPtr<ISubsystemTreeItem> InParent, UObject* Instance);

	virtual FObjectTreeItemID GetID() const override { return ClassName; }
	virtual bool IsSelected() const override;

	virtual FText GetDisplayName() const override;

	virtual FObjectTreeSubsystemItem* GetAsSubsystemDescriptor() const override {  return const_cast<FObjectTreeSubsystemItem*>(this); }
	virtual UObject* GetObjectForDetails() const override { return Subsystem.Get(); }
	virtual bool IsStale() const override { return Subsystem.IsStale() || Class.IsStale(); }
	virtual bool IsConfigExportable() const override { return bConfigExportable; }
	bool IsDefaultConfig() const { return bIsDefaultConfig; }
	virtual bool IsGameModule() const override { return bIsGameModuleClass; }
	virtual bool IsPluginModule() const override { return bIsPluginClass; }

	virtual void GenerateTooltip(class FObjectBrowserTableItemTooltipBuilder& TooltipBuilder) const override;
	virtual void GenerateContextMenu(class UToolMenu* MenuBuilder) const override;

protected:

};
