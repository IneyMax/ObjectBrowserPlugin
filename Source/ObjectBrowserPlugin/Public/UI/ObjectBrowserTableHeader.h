

#pragma once

#include "Widgets/Views/SHeaderRow.h"

class FObjectModel;
class SObjectBrowser;

class SObjectHeaderRow : public SHeaderRow
{
public:
	void Construct(const FArguments& InArgs, const TSharedPtr<FObjectModel>& InModel, const TSharedPtr<SObjectBrowser>& InBrowser);
	void RebuildColumns();

private:
	TSharedPtr<FObjectModel> Model;
	TWeakPtr<SObjectBrowser> Browser;
};
