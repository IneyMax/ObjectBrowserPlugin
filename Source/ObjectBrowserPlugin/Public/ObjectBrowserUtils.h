

#pragma once

#include "CoreMinimal.h"
#include "ObjectBrowserFwd.h"
#include "ObjectBrowserTypes.h"
#include "Item/IObjectTreeItem.h"
#include "Model/ObjectBrowserColumn.h"
#include "Widgets/Notifications/SNotificationList.h"


struct OBJECTBROWSERPLUGIN_API FObjectCategorySorter
{
	bool operator()(const FObjectTreeItemPtr& A, const FObjectTreeItemPtr& B) const
	{
		return A->GetSortOrder() < B->GetSortOrder();
	}
};


struct OBJECTBROWSERPLUGIN_API FObjectColumnSorter
{
	bool operator()(const FObjectColumnPtr& A, const FObjectColumnPtr& B) const
	{
		return A->SortOrder < B->SortOrder;
	}
};


struct OBJECTBROWSERPLUGIN_API FObjectBrowserUtils
{
	/**
	 * Get info about object "Owner"
	 */
	static FString GetDefaultObjectOwnerName(UObject* InObject);

	/**
	 * Finds the base directory for a given module.
	 */
	static FString GetModulePathForClass(UClass* InClass);
	
	/**
	 * Find fully qualified class module name
	 */
	static FString GetModuleNameForClass(UClass* InClass);

	/**
	 * Find plugin name that contains class
	 */
	static TSharedPtr<class IPlugin> GetPluginForClass(UClass* InClass);

	/**
	 * Test if class is belongs to a Game Module
	 */
	static bool IsGameModuleClass(UClass* InClass);

	/**
	 * Collect related source files belonging to specified class
	 */
	static void CollectSourceFiles(UClass* InClass, TArray<FString>& OutSourceFiles);

	/**
	 * Collect property display info for tooltip
	 */
	static FClassFieldStats GetClassFieldStats(UClass* InClass);

	/**
	 * Put text into clipboard
	 */
	static void SetClipboardText(const FString& ClipboardText);

	/**
	 * @brief
	 * @param InText
	 * @param InType
	 */
	static void ShowBrowserInfoMessage(FText InText, SNotificationItem::ECompletionState InType);

	/**
	 * @brief Generate config export string for specified item
	 * @param Item
	 * @param bModifiedOnly
	 * @return
	 */
	static FString GenerateConfigExport(const struct FObjectBrowserTreeObjectItem* Item, bool bModifiedOnly);

	/**
	 *
	 */
	static bool TryUpdateDefaultConfigFile(UObject* Object);

	/**
	 * Dump class flags to output
	 *
	 * Example: `SB.PrintClass /Script/ObjectBrowser.ObjectBrowserTestObject`
	 */
	static void PrintClassDetails(const TArray< FString >& InArgs, UWorld* InWorld, FOutputDevice& InLog);

	/**
	 * Dump property flags to output
	 *
	 * Example: `SB.PrintProperty /Script/ObjectBrowser.ObjectBrowserTestObject SingleDelegate`
	 */
	static void PrintPropertyDetails(const TArray< FString >& InArgs, UWorld* InWorld, FOutputDevice& InLog);
};
