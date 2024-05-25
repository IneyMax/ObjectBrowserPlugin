

#pragma once

#include "Model/ObjectBrowserCategory.h"


struct FObjectCategory_Default : public FObjectCategoryBase
{
	FObjectCategory_Default();
	virtual void Select(UWorld* InContext, TArray<UObject*>& OutData) const override;
};
