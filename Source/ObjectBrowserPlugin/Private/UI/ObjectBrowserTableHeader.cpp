

#include "UI/ObjectBrowserTableHeader.h"
#include "UI/SObjectBrowser.h"
#include "Model/ObjectBrowserModel.h"

#define LOCTEXT_NAMESPACE "ObjectBrowser"

void SObjectHeaderRow::Construct(const FArguments& InArgs, const TSharedPtr<FObjectModel>& InModel, const TSharedPtr<SObjectBrowser>& InBrowser)
{
	Model = InModel;
	Browser = InBrowser;

	SHeaderRow::Construct(InArgs);

	RebuildColumns();
}

void SObjectHeaderRow::RebuildColumns()
{
	Browser.Pin()->SetupColumns(*this);
}

#undef LOCTEXT_NAMESPACE
