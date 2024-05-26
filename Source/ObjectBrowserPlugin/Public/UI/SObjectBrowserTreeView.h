

#pragma once

#include "ObjectBrowserFwd.h"
#include "Widgets/Views/STreeView.h"


class SObjectBrowserTreeView : public STreeView<FObjectTreeItemPtr>
{
public:
	void Construct(const FArguments& InArgs, const TSharedPtr<FObjectModel>& InModel, const TSharedPtr<SObjectBrowser>& InBrowser);

private:
	TSharedPtr<FObjectModel> Model;
	TWeakPtr<SObjectBrowser> Browser;
};
