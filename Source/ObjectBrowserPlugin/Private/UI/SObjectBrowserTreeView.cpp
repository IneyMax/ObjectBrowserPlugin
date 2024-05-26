

#include "UI/SObjectBrowserTreeView.h"
#include "Model/ObjectBrowserModel.h"
#include "UI/SObjectBrowser.h"


#define LOCTEXT_NAMESPACE "ObjectBrowser"

void SObjectBrowserTreeView::Construct(const FArguments& InArgs, const TSharedPtr<FObjectModel>& InModel, const TSharedPtr<SObjectBrowser>& InBrowser)
{
	STreeView<FObjectTreeItemPtr>::Construct(InArgs);

	Model = InModel;
	Browser = InBrowser;
}

#undef LOCTEXT_NAMESPACE
