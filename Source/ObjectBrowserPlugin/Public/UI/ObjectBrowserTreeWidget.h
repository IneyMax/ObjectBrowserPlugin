

#pragma once

#include "Widgets/Views/STreeView.h"
#include "Model/ObjectBrowserDescriptor.h"

class FObjectModel;
class SObjectBrowser;

class SObjectTreeWidget : public STreeView<SubsystemTreeItemPtr>
{
public:
	void Construct(const FArguments& InArgs, const TSharedPtr<FObjectModel>& InModel, const TSharedPtr<SObjectBrowser>& InBrowser);

private:
	TSharedPtr<FObjectModel> Model;
	TWeakPtr<SObjectBrowser> Browser;
};
