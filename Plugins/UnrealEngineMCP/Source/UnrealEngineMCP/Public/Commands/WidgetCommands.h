#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"

class UWidgetBlueprint;
class UWidget;
class UPanelWidget;

/**
 * Handles Widget Blueprint (UMG) commands (24 total)
 * Creates and manipulates Widget Blueprints for UI development
 */
class UNREALENGINEMCP_API FWidgetCommands
{
public:
	FWidgetCommands();
	~FWidgetCommands();

	TSharedPtr<FJsonObject> HandleCommand(const FString& CommandType, const TSharedPtr<FJsonObject>& Params);

private:
	// === Existing (7) ===
	TSharedPtr<FJsonObject> HandleCreateWidgetBlueprint(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleAnalyzeWidgetBlueprint(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleAddWidget(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleRemoveWidget(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleSetWidgetProperty(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleSetWidgetSlot(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleListWidgetChildren(const TSharedPtr<FJsonObject>& Params);

	// === Tier 1: Event Binding (4) ===
	TSharedPtr<FJsonObject> HandleBindWidgetEvent(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleUnbindWidgetEvent(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleListWidgetEvents(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleAddWidgetFunctionNode(const TSharedPtr<FJsonObject>& Params);

	// === Tier 2: Content & Styling (5) ===
	TSharedPtr<FJsonObject> HandleSetWidgetText(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleSetWidgetColor(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleSetWidgetBrush(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleSetWidgetFont(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleSetWidgetPadding(const TSharedPtr<FJsonObject>& Params);

	// === Tier 3: Animation (3) ===
	TSharedPtr<FJsonObject> HandleCreateWidgetAnimation(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandlePlayAnimationNode(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleListWidgetAnimations(const TSharedPtr<FJsonObject>& Params);

	// === Tier 4: Atomic Batch (2) ===
	TSharedPtr<FJsonObject> HandleBuildWidgetTree(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleCloneWidgetSubtree(const TSharedPtr<FJsonObject>& Params);

	// === Tier 5: Introspection (3) ===
	TSharedPtr<FJsonObject> HandleAnalyzeWidgetHierarchy(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleGetWidgetTypeInfo(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleSearchWidgets(const TSharedPtr<FJsonObject>& Params);

	// === Helpers ===
	UWidgetBlueprint* FindWidgetBlueprint(const FString& Name, const FString& Path);
	UWidget* FindWidgetByName(UWidgetBlueprint* WBP, const FString& WidgetName);
	UClass* ResolveWidgetClass(const FString& WidgetType);
	TSharedPtr<FJsonObject> WidgetToJson(UWidget* Widget);
	TSharedPtr<FJsonObject> WidgetToDetailedJson(UWidget* Widget, bool bIncludeProperties, bool bIncludeEvents);
	TSharedPtr<FJsonObject> WidgetTreeToJson(UWidget* Widget);
	TSharedPtr<FJsonObject> WidgetTreeToDetailedJson(UWidget* Widget, bool bIncludeProperties, bool bIncludeEvents);
	UWidget* BuildWidgetRecursive(UWidgetBlueprint* WBP, const TSharedPtr<FJsonObject>& WidgetSpec, UPanelWidget* Parent, TMap<FString, UWidget*>& Registry, TArray<FString>& Errors);
	UWidget* CloneWidgetRecursive(UWidgetBlueprint* WBP, UWidget* Source, const FString& NamePrefix, UPanelWidget* NewParent);
};
