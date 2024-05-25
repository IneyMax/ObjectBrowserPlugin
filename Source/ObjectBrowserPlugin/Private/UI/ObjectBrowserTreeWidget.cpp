

#include "UI/ObjectBrowserTreeWidget.h"
#include "Model/ObjectBrowserModel.h"
#include "UI/SObjectBrowser.h"

#define LOCTEXT_NAMESPACE "ObjectBrowser"

void SObjectTreeWidget::Construct(const FArguments& InArgs, const TSharedPtr<FObjectModel>& InModel, const TSharedPtr<SObjectBrowser>& InBrowser)
{
	STreeView<SubsystemTreeItemPtr>::Construct(InArgs);

	Model = InModel;
	Browser = InBrowser;
}

#undef LOCTEXT_NAMESPACE
