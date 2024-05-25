

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateColor.h"
#include "Styling/SlateBrush.h"
#include "Textures/SlateIcon.h"
#include "Styling/AppStyle.h"


class FObjectBrowserStyle final : public FSlateStyleSet
{
public:
	static const FName StyleName;

	/** Register style set */
	static void Register();
	/** Unregister style set */
	static void UnRegister();

	/** Access the singleton instance for this style set */
	static FObjectBrowserStyle& Get();

	FObjectBrowserStyle();
	virtual ~FObjectBrowserStyle() = default;

private:

	static TSharedPtr<FObjectBrowserStyle> StyleInstance;
};


struct FStyleHelper
{
	static const ISlateStyle& Get();
	static const FSlateBrush* GetBrush(const FName& InName);
	static FSlateFontInfo GetFontStyle(const FName& InName);
	static FSlateIcon GetSlateIcon(const FName& InIcon);
	static FSlateColor GetSlateColor(const FName& Name);

	template<typename T>
	static const T& GetWidgetStyle(const FName& InName)
	{
		return FAppStyle::GetWidgetStyle<T>(InName);
	}

	template<typename T>
	static const T* GetWidgetStylePtr(const FName& InName)
	{
		return &GetWidgetStyle<T>(InName);
	}
};
