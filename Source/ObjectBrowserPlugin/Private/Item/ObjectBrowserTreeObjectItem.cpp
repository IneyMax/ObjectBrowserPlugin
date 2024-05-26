

#include "Item/ObjectBrowserTreeObjectItem.h"

#include "ObjectBrowserModule.h"
#include "ObjectBrowserStyle.h"
#include "ObjectBrowserUtils.h"
#include "SourceCodeNavigation.h"
#include "Interfaces/IPluginManager.h"
#include "Model/ObjectBrowserModel.h"
#include "UI/SObjectItemTooltip.h"

#define LOCTEXT_NAMESPACE "ObjectBrowser"


FObjectBrowserTreeObjectItem::FObjectBrowserTreeObjectItem()
{
}

FObjectBrowserTreeObjectItem::FObjectBrowserTreeObjectItem(TSharedRef<FObjectModel> InModel, TSharedPtr<IObjectTreeItem> InParent, UObject* Instance)
{
	Model = InModel;
	Parent = InParent;

	check(Instance);
	Object = Instance;

	UClass* const InClass = Instance->GetClass();
	Class = InClass;

	// save data needed to display so hotreload or other things won't crash editor or disaster happen
	// maybe let each column allocate some kind of struct to hold it for own usage?
	DisplayName = InClass->GetDisplayNameText();
	ClassName = InClass->GetFName();
	Package = InClass->GetOuterUPackage()->GetName();
	ShortPackage = FPackageName::GetShortName(Package);
	LongPackage = FString::Printf(TEXT("%s.%s"), *Package, *ClassName.ToString());

	if (InClass->HasAnyClassFlags(CLASS_Config) && !InClass->ClassConfigName.IsNone())
	{
		bConfigExportable = true;
		bIsDefaultConfig = InClass->HasAnyClassFlags(CLASS_DefaultConfig);
		ConfigName = InClass->ClassConfigName;
	}

	bIsGameModuleClass = FObjectBrowserUtils::IsGameModuleClass(InClass);

	FObjectBrowserUtils::CollectSourceFiles(InClass, SourceFilePaths);

	if (FObjectBrowserModule::OnGetObjectOwnerName.IsBound())
	{
		OwnerName = FObjectBrowserModule::OnGetObjectOwnerName.Execute(Instance);
	}
	else
	{
		OwnerName = FObjectBrowserUtils::GetDefaultObjectOwnerName(Instance);
	}

	TSharedPtr<IPlugin> Plugin = FObjectBrowserUtils::GetPluginForClass(InClass);
	if (Plugin.IsValid())
	{
		bIsPluginClass = true;
		PluginName = Plugin->GetName();
		PluginDisplayName = Plugin->GetFriendlyName();
	}

	PropertyStats = FObjectBrowserUtils::GetClassFieldStats(InClass);
}

bool FObjectBrowserTreeObjectItem::IsSelected() const
{
	return Model.IsValid() && Model->IsItemSelected(SharedThis(this));
}

FText FObjectBrowserTreeObjectItem::GetDisplayName() const
{
	return DisplayName;
}

void FObjectBrowserTreeObjectItem::GenerateTooltip(FObjectBrowserTableItemTooltipBuilder& TooltipBuilder) const
{
	//TooltipBuilder.AddPrimary(LOCTEXT("ObjectTooltipItem_Path", "Path"), FText::FromString(LongPackage));
	TooltipBuilder.AddPrimary(LOCTEXT("ObjectTooltipItem_Class", "Class"), FText::FromName(ClassName));
	TooltipBuilder.AddPrimary(LOCTEXT("ObjectTooltipItem_Module", "Module"), FText::FromString(ShortPackage));
	if (IsPluginModule())
	{
		TooltipBuilder.AddPrimary(LOCTEXT("ObjectTooltipItem_Plugin", "Plugin"), FText::FromString(PluginDisplayName));
	}
	if (IsConfigExportable())
	{
		TooltipBuilder.AddPrimary(LOCTEXT("ObjectTooltipItem_Config", "Config"), FText::FromName(ConfigName));
	}
	if (!OwnerName.IsEmpty())
	{
		TooltipBuilder.AddPrimary(LOCTEXT("ObjectTooltipItem_Owner", "Owned by"), FText::FromString(OwnerName));
	}

	TooltipBuilder.AddPrimary(LOCTEXT("ObjectTooltipItem_Props", "Num Properties"), FText::AsNumber(PropertyStats.NumProperties));
	TooltipBuilder.AddPrimary(LOCTEXT("ObjectTooltipItem_PropsEditable", "Num Editable Properties"), FText::AsNumber(PropertyStats.NumEditable));
	TooltipBuilder.AddPrimary(LOCTEXT("ObjectTooltipItem_PropsConfig", "Num Config Properties"), FText::AsNumber(PropertyStats.NumConfig));
}

void FObjectBrowserTreeObjectItem::GenerateContextMenu(UToolMenu* MenuBuilder) const
{
	TWeakPtr<const FObjectBrowserTreeObjectItem> Self = SharedThis(this);

	{
		FToolMenuSection& Section = MenuBuilder->AddSection("ObjectContextActions", LOCTEXT("ObjectContextActions", "Common"));
		Section.AddMenuEntry("OpenSourceFile",
			LOCTEXT("OpenSourceFile", "Open Source File"),
			FText::GetEmpty(),
			FStyleHelper::GetSlateIcon("SystemWideCommands.FindInContentBrowser"),
			FUIAction(
				FExecuteAction::CreateLambda([Self]()
				{
					if (Self.IsValid())
					{
						UObject* ViewedObject = Self.Pin()->GetObjectForDetails();
						if (!ViewedObject || !FSourceCodeNavigation::CanNavigateToClass(ViewedObject->GetClass()))
						{
							FObjectBrowserUtils::ShowBrowserInfoMessage(LOCTEXT("OpenSourceFile_Failed", "Failed to open source file."), SNotificationItem::CS_Fail);
						}
						else
						{
							FSourceCodeNavigation::NavigateToClass(ViewedObject->GetClass());
						}
					}
				})
			)
		);
	}

	{
		FToolMenuSection& Section = MenuBuilder->AddSection("ObjectReferenceActions", LOCTEXT("ObjectReferenceActions", "References"));
		Section.AddMenuEntry("CopyClassName",
			LOCTEXT("CopyClassName", "Copy Class Name"),
			FText::GetEmpty(),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([Self]()
				{
					if (Self.IsValid())
					{
						FObjectBrowserUtils::SetClipboardText(FString::Printf(TEXT("U%s"), *Self.Pin()->ClassName.ToString()));
					}
				})
			)
		);
		Section.AddMenuEntry("CopyPackageName",
			LOCTEXT("CopyPackageName", "Copy Module Name"),
			FText::GetEmpty(),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([Self]()
				{
					if (Self.IsValid())
					{
						FObjectBrowserUtils::SetClipboardText(Self.Pin()->ShortPackage);
					}
				})
			)
		);
		Section.AddMenuEntry("CopyScriptName",
			LOCTEXT("CopyScriptName", "Copy Script Name"),
			FText::GetEmpty(),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([Self]()
				{
					if (Self.IsValid())
					{
						FObjectBrowserUtils::SetClipboardText(Self.Pin()->LongPackage);
					}
				})
			)
		);
		Section.AddMenuEntry("CopyFilePath",
			LOCTEXT("CopyFilePath", "Copy File Path"),
			FText::GetEmpty(),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([Self]()
				{
					if (Self.IsValid())
					{
						const TArray<FString>& FilePaths = Self.Pin()->SourceFilePaths;

						FString ClipboardText;
						if (FilePaths.Num() > 0)
						{
							const FString* FoundHeader = FilePaths.FindByPredicate([](const FString& S)
							{
								FString Extension = FPaths::GetExtension(S);
								return Extension == TEXT("h") || Extension == TEXT("hpp");
							});

							if (!FoundHeader) FoundHeader = &FilePaths[0];

							ClipboardText = FPaths::ConvertRelativePathToFull(*FoundHeader);
						}

						FObjectBrowserUtils::SetClipboardText(ClipboardText);
					}
				})
			)
		);
	}

	if (IsConfigExportable())
	{
		FToolMenuSection& Section = MenuBuilder->AddSection("ObjectConfigActions", LOCTEXT("ObjectConfigActions", "Config"));

		Section.AddMenuEntry("ExportToDefaults",
			LOCTEXT("ExportToDefaults", "Export Default Config"),
			LOCTEXT("ExportToDefaultTooltip", "Export current values to DefaultConfig (only if class has DefaultConfig specifier)"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([Self]()
				{
					if (Self.IsValid())
					{
						if (FObjectBrowserUtils::TryUpdateDefaultConfigFile(Self.Pin()->GetObjectForDetails()))
						{
							FObjectBrowserUtils::ShowBrowserInfoMessage(LOCTEXT("ObjectBrowser", "Successfully updated defaults"), SNotificationItem::CS_Success);
						}
						else
						{
							FObjectBrowserUtils::ShowBrowserInfoMessage(LOCTEXT("ObjectBrowser", "Failed to update defaults"), SNotificationItem::CS_Fail);
						}
					}
				}),
				FCanExecuteAction::CreateSP(this, &FObjectBrowserTreeObjectItem::IsDefaultConfig)
			)
		);
		Section.AddMenuEntry("ExportModified",
			LOCTEXT("ExportModified", "Export Modified Properties"),
			LOCTEXT("ExportModifiedTooltip", "Export modified properties as an INI section and store it in clipboard"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([Self]()
				{
					if (Self.IsValid())
					{
						FString ClipboardText = FObjectBrowserUtils::GenerateConfigExport(Self.Pin().Get(), true);
						FObjectBrowserUtils::SetClipboardText(ClipboardText);
						FObjectBrowserUtils::ShowBrowserInfoMessage(LOCTEXT("ObjectBrowser", "Copied to clipboard"), SNotificationItem::CS_Success);
					}
				}),
				FCanExecuteAction::CreateSP(this, &FObjectBrowserTreeObjectItem::IsConfigExportable)
			)
		);
		Section.AddMenuEntry("ExportAll",
			LOCTEXT("ExportAll", "Export All Properties"),
			LOCTEXT("ExportAllTooltip", "Export all config properties as an INI section and store it in clipboard"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateLambda([Self]()
				{
					if (Self.IsValid())
					{
						FString ClipboardText = FObjectBrowserUtils::GenerateConfigExport(Self.Pin().Get(), false);
						FObjectBrowserUtils::SetClipboardText(ClipboardText);
						FObjectBrowserUtils::ShowBrowserInfoMessage(LOCTEXT("ObjectBrowser", "Copied to clipboard"), SNotificationItem::CS_Success);
					}
				}),
				FCanExecuteAction::CreateSP(this, &FObjectBrowserTreeObjectItem::IsConfigExportable)
			)
		);
	}
}

#undef LOCTEXT_NAMESPACE