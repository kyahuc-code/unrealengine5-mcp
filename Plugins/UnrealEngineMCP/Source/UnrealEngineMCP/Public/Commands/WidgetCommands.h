#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"

class UWidgetBlueprint;
class UWidget;
class UPanelWidget;

/**
 * Handles Widget Blueprint (UMG) commands
 * Creates and manipulates Widget Blueprints for UI development
 */
class UNREALENGINEMCP_API FWidgetCommands
{
public:
	FWidgetCommands();
	~FWidgetCommands();

	/**
	 * Handle widget command
	 * Routes to specific handler based on command type
	 */
	TSharedPtr<FJsonObject> HandleCommand(const FString& CommandType, const TSharedPtr<FJsonObject>& Params);

private:
	// Widget Blueprint asset commands
	TSharedPtr<FJsonObject> HandleCreateWidgetBlueprint(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleAnalyzeWidgetBlueprint(const TSharedPtr<FJsonObject>& Params);

	// Widget manipulation commands
	TSharedPtr<FJsonObject> HandleAddWidget(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleRemoveWidget(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleSetWidgetProperty(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleSetWidgetSlot(const TSharedPtr<FJsonObject>& Params);
	TSharedPtr<FJsonObject> HandleListWidgetChildren(const TSharedPtr<FJsonObject>& Params);

	// Helper methods
	UWidgetBlueprint* FindWidgetBlueprint(const FString& Name, const FString& Path);
	UWidget* FindWidgetByName(UWidgetBlueprint* WBP, const FString& WidgetName);
	TSharedPtr<FJsonObject> WidgetToJson(UWidget* Widget);
	TSharedPtr<FJsonObject> WidgetTreeToJson(UWidget* Widget);
};
