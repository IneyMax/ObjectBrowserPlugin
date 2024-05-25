

#include "Model/Category/ObjectBrowserCategory_Default.h"

FObjectCategory_Default::FObjectCategory_Default()
{
	Name = TEXT("WorldObjectCategory");
	Label = NSLOCTEXT("ObjectBrowser", "ObjectBrowser_Default", "Default Objects");
	SortOrder = 400;
}

void FObjectCategory_Default::Select(UWorld* InContext, TArray<UObject*>& OutData) const
{
	for (FThreadSafeObjectIterator It; It; ++It )
	{
		OutData.Add(*It);
	}
	// if (IsValid(InContext))
	// {
	// 	OutData.Append(InContext->GetObjectArray<UWorldObject>());
	// }
}
