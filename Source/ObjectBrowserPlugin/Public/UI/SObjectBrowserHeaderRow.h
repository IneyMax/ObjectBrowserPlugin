

#pragma once

#include "Widgets/Views/SHeaderRow.h"


class SObjectBrowserHeaderRow : public SHeaderRow
{
public:
	void Construct(const FArguments& InArgs, const TSharedPtr<FObjectModel>& InModel, const TSharedPtr<SObjectBrowser>& InBrowser);
	void RebuildColumns();

private:
	TSharedPtr<FObjectModel> Model;
	TWeakPtr<SObjectBrowser> Browser;
};
