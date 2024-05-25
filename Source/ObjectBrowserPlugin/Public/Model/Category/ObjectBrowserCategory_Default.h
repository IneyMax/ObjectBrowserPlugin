

#pragma once

#include "Model/ObjectBrowserCategory.h"

struct FObjectCategory_Default : public FObjectCategory
{
	FObjectCategory_Default();
	virtual void Select(UWorld* InContext, TArray<UObject*>& OutData) const override;
};
