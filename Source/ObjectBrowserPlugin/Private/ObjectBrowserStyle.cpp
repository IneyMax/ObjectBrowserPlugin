

#include "ObjectBrowserStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

const FName FObjectBrowserStyle::StyleName("ObjectBrowserStyle");
TSharedPtr<FObjectBrowserStyle> FObjectBrowserStyle::StyleInstance;

void FObjectBrowserStyle::Register()
{
	check(!StyleInstance.IsValid());
	StyleInstance = MakeShared<FObjectBrowserStyle>();

	FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
}

void FObjectBrowserStyle::UnRegister()
{
	check(StyleInstance.IsValid());

	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);

	StyleInstance.Reset();
}

FObjectBrowserStyle& FObjectBrowserStyle::Get()
{
	check(StyleInstance.IsValid());
	return *StyleInstance;
}

FObjectBrowserStyle::FObjectBrowserStyle() : FSlateStyleSet(StyleName)
{
	const FVector2D Icon16x16(16.f, 16.f);
	const FVector2D Icon64x64(64.f, 64.f);

	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("ObjectBrowserPlugin"));
	check(Plugin.IsValid());
	FSlateStyleSet::SetContentRoot(FPaths::Combine(Plugin->GetBaseDir(), TEXT("Resources")));
	FSlateStyleSet::SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	// register styles here
	// Set("ObjectBrowser.TabIcon", new IMAGE_BRUSH("Icons/ObjectBrowser_16x", Icon16x16));
}

const ISlateStyle& FStyleHelper::Get()
{
#if UE_VERSION_OLDER_THAN(5,1,0)
	return FEditorStyle::Get();
#else
	return FAppStyle::Get();
#endif
}

const FSlateBrush* FStyleHelper::GetBrush(const FName& InName)
{
#if UE_VERSION_OLDER_THAN(5,1,0)
	return FEditorStyle::GetBrush(InName);
#else
	return FAppStyle::GetBrush(InName);
#endif
}

FSlateFontInfo FStyleHelper::GetFontStyle(const FName& InName)
{
#if UE_VERSION_OLDER_THAN(5,1,0)
	return FEditorStyle::GetFontStyle(InName);
#else
	return FAppStyle::GetFontStyle(InName);
#endif
}

FSlateIcon FStyleHelper::GetSlateIcon(const FName& InIcon)
{
#if UE_VERSION_OLDER_THAN(5,1,0)
	return FSlateIcon( FEditorStyle::GetStyleSetName(), InIcon );
#else
	return FSlateIcon(  FAppStyle::GetAppStyleSetName(), InIcon);
#endif
}

FSlateColor FStyleHelper::GetSlateColor(const FName& InName)
{
#if UE_VERSION_OLDER_THAN(5,1,0)
	return FEditorStyle::GetSlateColor(InName);
#else
	return FAppStyle::GetSlateColor(InName);
#endif
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
