

#pragma once

#include "ObjectBrowserFwd.h"
#include "Widgets/Views/STreeView.h"

class FObjectModel;
class SObjectBrowser;


class SObjectTreeWidget : public STreeView<ObjectTreeItemPtr>
{
public:
	void Construct(const FArguments& InArgs, const TSharedPtr<FObjectModel>& InModel, const TSharedPtr<SObjectBrowser>& InBrowser);

private:
	TSharedPtr<FObjectModel> Model;
	TWeakPtr<SObjectBrowser> Browser;
};
