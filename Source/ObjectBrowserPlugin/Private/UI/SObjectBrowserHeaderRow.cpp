

#include "Model/ObjectBrowserModel.h"
#include "UI/SObjectBrowser.h"
#include "UI/SObjectBrowserHeaderRow.h"


#define LOCTEXT_NAMESPACE "ObjectBrowser"

void SObjectBrowserHeaderRow::Construct(const FArguments& InArgs, const TSharedPtr<FObjectModel>& InModel, const TSharedPtr<SObjectBrowser>& InBrowser)
{
	Model = InModel;
	Browser = InBrowser;

	SHeaderRow::Construct(InArgs);

	RebuildColumns();
}

void SObjectBrowserHeaderRow::RebuildColumns()
{
	Browser.Pin()->SetupColumns(*this);
}

#undef LOCTEXT_NAMESPACE
