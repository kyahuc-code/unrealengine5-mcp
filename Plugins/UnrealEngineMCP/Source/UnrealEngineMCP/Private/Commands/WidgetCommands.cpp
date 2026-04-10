#include "Commands/WidgetCommands.h"
#include "Commands/CommonUtils.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/ProgressBar.h"
#include "Components/Slider.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/GridPanel.h"
#include "Components/WrapBox.h"
#include "Components/ScaleBox.h"
#include "Components/ListView.h"
#include "Components/ComboBoxString.h"
#include "Components/RichTextBlock.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/BackgroundBlur.h"
#include "Components/Throbber.h"
#include "Components/ExpandableArea.h"
#include "Components/RetainerBox.h"
#include "Components/InvalidationBox.h"
#include "Components/SafeZone.h"
#include "Components/NamedSlot.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorAssetLibrary.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UObject/SavePackage.h"
#include "K2Node_ComponentBoundEvent.h"
#include "K2Node_CallFunction.h"
#include "EdGraphSchema_K2.h"
#include "ScopedTransaction.h"
#include "Animation/WidgetAnimation.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_MacroInstance.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_VariableGet.h"
#include "K2Node_FunctionEntry.h"
#include "EdGraphNode_Comment.h"
#include "K2Node_Event.h"
#include "K2Node_MakeStruct.h"
#include "K2Node_BreakStruct.h"
#include "K2Node_SwitchEnum.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateColor.h"
#include "Fonts/SlateFontInfo.h"

FWidgetCommands::FWidgetCommands()
{
}

FWidgetCommands::~FWidgetCommands()
{
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleCommand(const FString& CommandType, const TSharedPtr<FJsonObject>& Params)
{
	if (CommandType == TEXT("create_widget_blueprint"))
	{
		return HandleCreateWidgetBlueprint(Params);
	}
	else if (CommandType == TEXT("analyze_widget_blueprint"))
	{
		return HandleAnalyzeWidgetBlueprint(Params);
	}
	else if (CommandType == TEXT("add_widget"))
	{
		return HandleAddWidget(Params);
	}
	else if (CommandType == TEXT("remove_widget"))
	{
		return HandleRemoveWidget(Params);
	}
	else if (CommandType == TEXT("set_widget_property"))
	{
		return HandleSetWidgetProperty(Params);
	}
	else if (CommandType == TEXT("set_widget_slot"))
	{
		return HandleSetWidgetSlot(Params);
	}
	else if (CommandType == TEXT("list_widget_children"))
	{
		return HandleListWidgetChildren(Params);
	}
	// Tier 1: Event Binding
	else if (CommandType == TEXT("bind_widget_event"))
	{
		return HandleBindWidgetEvent(Params);
	}
	else if (CommandType == TEXT("unbind_widget_event"))
	{
		return HandleUnbindWidgetEvent(Params);
	}
	else if (CommandType == TEXT("list_widget_events"))
	{
		return HandleListWidgetEvents(Params);
	}
	else if (CommandType == TEXT("add_widget_function_node"))
	{
		return HandleAddWidgetFunctionNode(Params);
	}
	// Tier 2: Content & Styling
	else if (CommandType == TEXT("set_widget_text"))
	{
		return HandleSetWidgetText(Params);
	}
	else if (CommandType == TEXT("set_widget_color"))
	{
		return HandleSetWidgetColor(Params);
	}
	else if (CommandType == TEXT("set_widget_brush"))
	{
		return HandleSetWidgetBrush(Params);
	}
	else if (CommandType == TEXT("set_widget_font"))
	{
		return HandleSetWidgetFont(Params);
	}
	else if (CommandType == TEXT("set_widget_padding"))
	{
		return HandleSetWidgetPadding(Params);
	}
	// Tier 3: Animation
	else if (CommandType == TEXT("create_widget_animation"))
	{
		return HandleCreateWidgetAnimation(Params);
	}
	else if (CommandType == TEXT("play_animation_node"))
	{
		return HandlePlayAnimationNode(Params);
	}
	else if (CommandType == TEXT("list_widget_animations"))
	{
		return HandleListWidgetAnimations(Params);
	}
	// Tier 4: Atomic Batch
	else if (CommandType == TEXT("build_widget_tree"))
	{
		return HandleBuildWidgetTree(Params);
	}
	else if (CommandType == TEXT("clone_widget_subtree"))
	{
		return HandleCloneWidgetSubtree(Params);
	}
	// Tier 5: Introspection
	else if (CommandType == TEXT("analyze_widget_hierarchy"))
	{
		return HandleAnalyzeWidgetHierarchy(Params);
	}
	else if (CommandType == TEXT("get_widget_type_info"))
	{
		return HandleGetWidgetTypeInfo(Params);
	}
	else if (CommandType == TEXT("search_widgets"))
	{
		return HandleSearchWidgets(Params);
	}
	// Tier 6: State & Layout
	else if (CommandType == TEXT("set_widget_visibility"))
	{
		return HandleSetWidgetVisibility(Params);
	}
	else if (CommandType == TEXT("set_widget_enabled"))
	{
		return HandleSetWidgetEnabled(Params);
	}
	else if (CommandType == TEXT("set_box_slot"))
	{
		return HandleSetBoxSlot(Params);
	}
	else if (CommandType == TEXT("set_grid_slot"))
	{
		return HandleSetGridSlot(Params);
	}
	else if (CommandType == TEXT("set_widget_transform"))
	{
		return HandleSetWidgetTransform(Params);
	}
	// Tier 7: Advanced
	else if (CommandType == TEXT("set_widget_tooltip"))
	{
		return HandleSetWidgetTooltip(Params);
	}
	// Tier 8: Variable Management
	else if (CommandType == TEXT("add_widget_variable"))
	{
		return HandleAddWidgetVariable(Params);
	}
	else if (CommandType == TEXT("delete_widget_variable"))
	{
		return HandleDeleteWidgetVariable(Params);
	}
	else if (CommandType == TEXT("get_widget_variables"))
	{
		return HandleGetWidgetVariables(Params);
	}
	// Tier 9: Node Connection/Deletion
	else if (CommandType == TEXT("connect_widget_nodes"))
	{
		return HandleConnectWidgetNodes(Params);
	}
	else if (CommandType == TEXT("disconnect_widget_nodes"))
	{
		return HandleDisconnectWidgetNodes(Params);
	}
	else if (CommandType == TEXT("delete_widget_node"))
	{
		return HandleDeleteWidgetNode(Params);
	}
	// Tier 10: Flow Control & Custom Events
	else if (CommandType == TEXT("add_widget_flow_control"))
	{
		return HandleAddWidgetFlowControl(Params);
	}
	else if (CommandType == TEXT("add_widget_custom_event"))
	{
		return HandleAddWidgetCustomEvent(Params);
	}
	else if (CommandType == TEXT("add_widget_generic_node"))
	{
		return HandleAddWidgetGenericNode(Params);
	}
	// Tier 11: Pin Value Management
	else if (CommandType == TEXT("set_widget_pin_default"))
	{
		return HandleSetWidgetPinDefault(Params);
	}
	else if (CommandType == TEXT("get_widget_pin_value"))
	{
		return HandleGetWidgetPinValue(Params);
	}
	// Tier 12: Graph Introspection
	else if (CommandType == TEXT("list_widget_graph_nodes"))
	{
		return HandleListWidgetGraphNodes(Params);
	}
	else if (CommandType == TEXT("list_widget_graphs"))
	{
		return HandleListWidgetGraphs(Params);
	}
	else if (CommandType == TEXT("compile_widget_blueprint"))
	{
		return HandleCompileWidgetBlueprint(Params);
	}
	// Tier 13: Auxiliary
	else if (CommandType == TEXT("add_widget_comment_box"))
	{
		return HandleAddWidgetCommentBox(Params);
	}
	else if (CommandType == TEXT("add_widget_function_override"))
	{
		return HandleAddWidgetFunctionOverride(Params);
	}

	return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Unknown widget command: %s"), *CommandType));
}

// =========================================================================
// Widget Blueprint Asset Commands
// =========================================================================

TSharedPtr<FJsonObject> FWidgetCommands::HandleCreateWidgetBlueprint(const TSharedPtr<FJsonObject>& Params)
{
	FString Name = Params->GetStringField(TEXT("name"));
	FString Path = Params->GetStringField(TEXT("path"));
	FString ParentClass = Params->HasField(TEXT("parent_class"))
		? Params->GetStringField(TEXT("parent_class"))
		: TEXT("UserWidget");

	if (Name.IsEmpty())
	{
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'name' parameter"));
	}
	if (Path.IsEmpty())
	{
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'path' parameter"));
	}

	// Find parent class
	UClass* ParentUClass = nullptr;
	if (ParentClass == TEXT("UserWidget") || ParentClass == TEXT("UUserWidget"))
	{
		ParentUClass = UUserWidget::StaticClass();
	}
	else
	{
		ParentUClass = FCommonUtils::FindClassByName(ParentClass);
		if (!ParentUClass || !ParentUClass->IsChildOf(UUserWidget::StaticClass()))
		{
			return FCommonUtils::CreateErrorResponse(
				FString::Printf(TEXT("Parent class '%s' not found or not a UUserWidget subclass"), *ParentClass));
		}
	}

	// Create package
	FString PackagePath = Path / Name;
	UPackage* Package = CreatePackage(*PackagePath);
	if (!Package)
	{
		return FCommonUtils::CreateErrorResponse(
			FString::Printf(TEXT("Failed to create package at '%s'"), *PackagePath));
	}

	// Create Widget Blueprint
	UWidgetBlueprint* NewWBP = CastChecked<UWidgetBlueprint>(
		FKismetEditorUtilities::CreateBlueprint(
			ParentUClass,
			Package,
			*Name,
			BPTYPE_Normal,
			UWidgetBlueprint::StaticClass(),
			UBlueprintGeneratedClass::StaticClass()
		)
	);

	if (!NewWBP)
	{
		return FCommonUtils::CreateErrorResponse(TEXT("Failed to create Widget Blueprint"));
	}

	// Add default root widget (CanvasPanel) if none exists
	if (NewWBP->WidgetTree && !NewWBP->WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = NewWBP->WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		NewWBP->WidgetTree->RootWidget = RootCanvas;
	}

	// Compile and save
	FKismetEditorUtilities::CompileBlueprint(NewWBP);
	FAssetRegistryModule::AssetCreated(NewWBP);
	Package->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("name"), Name);
	Result->SetStringField(TEXT("path"), PackagePath);
	Result->SetStringField(TEXT("parent_class"), ParentUClass->GetName());
	Result->SetStringField(TEXT("root_widget"), TEXT("RootCanvas (CanvasPanel)"));
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleAnalyzeWidgetBlueprint(const TSharedPtr<FJsonObject>& Params)
{
	FString Name = Params->GetStringField(TEXT("name"));
	FString Path = Params->HasField(TEXT("path")) ? Params->GetStringField(TEXT("path")) : TEXT("/Game/");

	UWidgetBlueprint* WBP = FindWidgetBlueprint(Name, Path);
	if (!WBP)
	{
		return FCommonUtils::CreateErrorResponse(
			FString::Printf(TEXT("Widget Blueprint '%s' not found"), *Name));
	}

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("name"), Name);
	Result->SetStringField(TEXT("parent_class"), WBP->ParentClass ? WBP->ParentClass->GetName() : TEXT("Unknown"));

	// Widget tree
	if (WBP->WidgetTree && WBP->WidgetTree->RootWidget)
	{
		Result->SetObjectField(TEXT("widget_tree"), WidgetTreeToJson(WBP->WidgetTree->RootWidget));
	}

	return Result;
}

// =========================================================================
// Widget Manipulation Commands
// =========================================================================

TSharedPtr<FJsonObject> FWidgetCommands::HandleAddWidget(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetType = Params->GetStringField(TEXT("widget_type"));
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));
	FString ParentName = Params->HasField(TEXT("parent_name")) ? Params->GetStringField(TEXT("parent_name")) : TEXT("");

	if (BlueprintName.IsEmpty() || WidgetType.IsEmpty() || WidgetName.IsEmpty())
	{
		return FCommonUtils::CreateErrorResponse(TEXT("Missing required parameters: blueprint_name, widget_type, widget_name"));
	}

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP || !WBP->WidgetTree)
	{
		return FCommonUtils::CreateErrorResponse(
			FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));
	}

	// Map widget type string to UClass
	static TMap<FString, UClass*> WidgetTypeMap;
	if (WidgetTypeMap.Num() == 0)
	{
		WidgetTypeMap.Add(TEXT("CanvasPanel"), UCanvasPanel::StaticClass());
		WidgetTypeMap.Add(TEXT("VerticalBox"), UVerticalBox::StaticClass());
		WidgetTypeMap.Add(TEXT("HorizontalBox"), UHorizontalBox::StaticClass());
		WidgetTypeMap.Add(TEXT("Button"), UButton::StaticClass());
		WidgetTypeMap.Add(TEXT("TextBlock"), UTextBlock::StaticClass());
		WidgetTypeMap.Add(TEXT("Image"), UImage::StaticClass());
		WidgetTypeMap.Add(TEXT("Border"), UBorder::StaticClass());
		WidgetTypeMap.Add(TEXT("Overlay"), UOverlay::StaticClass());
		WidgetTypeMap.Add(TEXT("ScrollBox"), UScrollBox::StaticClass());
		WidgetTypeMap.Add(TEXT("SizeBox"), USizeBox::StaticClass());
		WidgetTypeMap.Add(TEXT("Spacer"), USpacer::StaticClass());
		WidgetTypeMap.Add(TEXT("ProgressBar"), UProgressBar::StaticClass());
		WidgetTypeMap.Add(TEXT("Slider"), USlider::StaticClass());
		WidgetTypeMap.Add(TEXT("CheckBox"), UCheckBox::StaticClass());
		WidgetTypeMap.Add(TEXT("EditableTextBox"), UEditableTextBox::StaticClass());
		WidgetTypeMap.Add(TEXT("GridPanel"), UGridPanel::StaticClass());
		WidgetTypeMap.Add(TEXT("WrapBox"), UWrapBox::StaticClass());
		WidgetTypeMap.Add(TEXT("ScaleBox"), UScaleBox::StaticClass());
		WidgetTypeMap.Add(TEXT("ComboBoxString"), UComboBoxString::StaticClass());
		WidgetTypeMap.Add(TEXT("RichTextBlock"), URichTextBlock::StaticClass());
		WidgetTypeMap.Add(TEXT("MultiLineEditableTextBox"), UMultiLineEditableTextBox::StaticClass());
		WidgetTypeMap.Add(TEXT("WidgetSwitcher"), UWidgetSwitcher::StaticClass());
		WidgetTypeMap.Add(TEXT("UniformGridPanel"), UUniformGridPanel::StaticClass());
		WidgetTypeMap.Add(TEXT("BackgroundBlur"), UBackgroundBlur::StaticClass());
		WidgetTypeMap.Add(TEXT("Throbber"), UThrobber::StaticClass());
		WidgetTypeMap.Add(TEXT("CircularThrobber"), UCircularThrobber::StaticClass());
		WidgetTypeMap.Add(TEXT("ExpandableArea"), UExpandableArea::StaticClass());
		WidgetTypeMap.Add(TEXT("RetainerBox"), URetainerBox::StaticClass());
		WidgetTypeMap.Add(TEXT("InvalidationBox"), UInvalidationBox::StaticClass());
		WidgetTypeMap.Add(TEXT("SafeZone"), USafeZone::StaticClass());
		WidgetTypeMap.Add(TEXT("NamedSlot"), UNamedSlot::StaticClass());
	}

	UClass* WidgetClass = nullptr;
	if (UClass** FoundClass = WidgetTypeMap.Find(WidgetType))
	{
		WidgetClass = *FoundClass;
	}
	else
	{
		// Try finding by class name
		WidgetClass = FCommonUtils::FindClassByName(WidgetType);
		if (!WidgetClass || !WidgetClass->IsChildOf(UWidget::StaticClass()))
		{
			FString SupportedTypes;
			for (auto& Pair : WidgetTypeMap)
			{
				if (!SupportedTypes.IsEmpty()) SupportedTypes += TEXT(", ");
				SupportedTypes += Pair.Key;
			}
			return FCommonUtils::CreateErrorResponse(
				FString::Printf(TEXT("Unknown widget type: '%s'. Supported: %s"), *WidgetType, *SupportedTypes));
		}
	}

	// Create widget
	UWidget* NewWidget = WBP->WidgetTree->ConstructWidget<UWidget>(WidgetClass, *WidgetName);
	if (!NewWidget)
	{
		return FCommonUtils::CreateErrorResponse(
			FString::Printf(TEXT("Failed to create widget of type '%s'"), *WidgetType));
	}

	// Find parent and add
	if (ParentName.IsEmpty())
	{
		// Add to root
		UPanelWidget* RootPanel = Cast<UPanelWidget>(WBP->WidgetTree->RootWidget);
		if (RootPanel)
		{
			RootPanel->AddChild(NewWidget);
		}
		else if (!WBP->WidgetTree->RootWidget)
		{
			WBP->WidgetTree->RootWidget = NewWidget;
		}
		else
		{
			return FCommonUtils::CreateErrorResponse(TEXT("Root widget is not a panel, cannot add children. Specify parent_name."));
		}
	}
	else
	{
		UWidget* ParentWidget = FindWidgetByName(WBP, ParentName);
		if (!ParentWidget)
		{
			return FCommonUtils::CreateErrorResponse(
				FString::Printf(TEXT("Parent widget '%s' not found"), *ParentName));
		}
		UPanelWidget* ParentPanel = Cast<UPanelWidget>(ParentWidget);
		if (!ParentPanel)
		{
			return FCommonUtils::CreateErrorResponse(
				FString::Printf(TEXT("Parent widget '%s' is not a panel widget, cannot add children"), *ParentName));
		}
		ParentPanel->AddChild(NewWidget);
	}

	// Compile
	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget_name"), WidgetName);
	Result->SetStringField(TEXT("widget_type"), WidgetType);
	Result->SetStringField(TEXT("parent"), ParentName.IsEmpty() ? TEXT("root") : ParentName);
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleRemoveWidget(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP || !WBP->WidgetTree)
	{
		return FCommonUtils::CreateErrorResponse(
			FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));
	}

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget)
	{
		return FCommonUtils::CreateErrorResponse(
			FString::Printf(TEXT("Widget '%s' not found"), *WidgetName));
	}

	// Remove from parent
	Widget->RemoveFromParent();
	WBP->WidgetTree->RemoveWidget(Widget);

	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("removed"), WidgetName);
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleSetWidgetProperty(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));
	FString PropertyName = Params->GetStringField(TEXT("property_name"));
	FString Value = Params->GetStringField(TEXT("value"));

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP)
	{
		return FCommonUtils::CreateErrorResponse(
			FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));
	}

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget)
	{
		return FCommonUtils::CreateErrorResponse(
			FString::Printf(TEXT("Widget '%s' not found"), *WidgetName));
	}

	// Use SetObjectProperty from CommonUtils
	if (!FCommonUtils::SetObjectProperty(Widget, PropertyName, Value))
	{
		return FCommonUtils::CreateErrorResponse(
			FString::Printf(TEXT("Failed to set property '%s' on widget '%s'"), *PropertyName, *WidgetName));
	}

	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetStringField(TEXT("property"), PropertyName);
	Result->SetStringField(TEXT("value"), Value);
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleSetWidgetSlot(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP)
	{
		return FCommonUtils::CreateErrorResponse(
			FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));
	}

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget || !Widget->Slot)
	{
		return FCommonUtils::CreateErrorResponse(
			FString::Printf(TEXT("Widget '%s' not found or has no slot"), *WidgetName));
	}

	// Canvas Panel Slot properties
	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
	{
		if (Params->HasField(TEXT("position")))
		{
			FVector2D Pos = FCommonUtils::GetVector2DFromJson(Params, TEXT("position"));
			CanvasSlot->SetPosition(Pos);
		}
		if (Params->HasField(TEXT("size")))
		{
			FVector2D Size = FCommonUtils::GetVector2DFromJson(Params, TEXT("size"));
			CanvasSlot->SetSize(Size);
		}
		if (Params->HasField(TEXT("anchors")))
		{
			const TSharedPtr<FJsonObject>* AnchorsObj;
			if (Params->TryGetObjectField(TEXT("anchors"), AnchorsObj))
			{
				FAnchors Anchors;
				Anchors.Minimum.X = (*AnchorsObj)->GetNumberField(TEXT("min_x"));
				Anchors.Minimum.Y = (*AnchorsObj)->GetNumberField(TEXT("min_y"));
				Anchors.Maximum.X = (*AnchorsObj)->HasField(TEXT("max_x")) ? (*AnchorsObj)->GetNumberField(TEXT("max_x")) : Anchors.Minimum.X;
				Anchors.Maximum.Y = (*AnchorsObj)->HasField(TEXT("max_y")) ? (*AnchorsObj)->GetNumberField(TEXT("max_y")) : Anchors.Minimum.Y;
				CanvasSlot->SetAnchors(Anchors);
			}
		}
		if (Params->HasField(TEXT("alignment")))
		{
			FVector2D Alignment = FCommonUtils::GetVector2DFromJson(Params, TEXT("alignment"));
			CanvasSlot->SetAlignment(Alignment);
		}
		if (Params->HasField(TEXT("z_order")))
		{
			CanvasSlot->SetZOrder(Params->GetIntegerField(TEXT("z_order")));
		}
		if (Params->HasField(TEXT("auto_size")))
		{
			CanvasSlot->SetAutoSize(Params->GetBoolField(TEXT("auto_size")));
		}
	}

	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetStringField(TEXT("slot_type"), Widget->Slot ? Widget->Slot->GetClass()->GetName() : TEXT("None"));
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleListWidgetChildren(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString ParentName = Params->HasField(TEXT("parent_name")) ? Params->GetStringField(TEXT("parent_name")) : TEXT("");

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP || !WBP->WidgetTree)
	{
		return FCommonUtils::CreateErrorResponse(
			FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));
	}

	UWidget* TargetWidget = nullptr;
	if (ParentName.IsEmpty())
	{
		TargetWidget = WBP->WidgetTree->RootWidget;
	}
	else
	{
		TargetWidget = FindWidgetByName(WBP, ParentName);
	}

	if (!TargetWidget)
	{
		return FCommonUtils::CreateErrorResponse(TEXT("Target widget not found"));
	}

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);

	TArray<TSharedPtr<FJsonValue>> ChildrenArray;
	UPanelWidget* Panel = Cast<UPanelWidget>(TargetWidget);
	if (Panel)
	{
		for (int32 i = 0; i < Panel->GetChildrenCount(); i++)
		{
			UWidget* Child = Panel->GetChildAt(i);
			if (Child)
			{
				ChildrenArray.Add(MakeShared<FJsonValueObject>(WidgetToJson(Child)));
			}
		}
	}

	Result->SetArrayField(TEXT("children"), ChildrenArray);
	Result->SetNumberField(TEXT("count"), ChildrenArray.Num());
	return Result;
}

// =========================================================================
// Helper Methods
// =========================================================================

UWidgetBlueprint* FWidgetCommands::FindWidgetBlueprint(const FString& Name, const FString& Path)
{
	// Try direct path first
	FString FullPath = Path;
	if (!FullPath.EndsWith(TEXT("/")))
	{
		FullPath += TEXT("/");
	}
	FullPath += Name + TEXT(".") + Name;

	UObject* LoadedObject = UEditorAssetLibrary::LoadAsset(FullPath);
	if (UWidgetBlueprint* WBP = Cast<UWidgetBlueprint>(LoadedObject))
	{
		return WBP;
	}

	// Search via AssetRegistry
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.ClassPaths.Add(FTopLevelAssetPath(TEXT("/Script/UMGEditor"), TEXT("WidgetBlueprint")));
	Filter.PackagePaths.Add(*Path);
	Filter.bRecursivePaths = true;

	TArray<FAssetData> Assets;
	AssetRegistry.GetAssets(Filter, Assets);

	for (const FAssetData& Asset : Assets)
	{
		if (Asset.AssetName.ToString() == Name)
		{
			UObject* Obj = Asset.GetAsset();
			if (UWidgetBlueprint* WBP = Cast<UWidgetBlueprint>(Obj))
			{
				return WBP;
			}
		}
	}

	return nullptr;
}

UWidget* FWidgetCommands::FindWidgetByName(UWidgetBlueprint* WBP, const FString& WidgetName)
{
	if (!WBP || !WBP->WidgetTree)
	{
		return nullptr;
	}

	UWidget* FoundWidget = nullptr;
	WBP->WidgetTree->ForEachWidget([&](UWidget* Widget)
	{
		if (Widget && Widget->GetName() == WidgetName)
		{
			FoundWidget = Widget;
		}
	});
	return FoundWidget;
}

TSharedPtr<FJsonObject> FWidgetCommands::WidgetToJson(UWidget* Widget)
{
	TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
	if (!Widget) return Obj;

	Obj->SetStringField(TEXT("name"), Widget->GetName());
	Obj->SetStringField(TEXT("class"), Widget->GetClass()->GetName());
	Obj->SetBoolField(TEXT("is_panel"), Widget->IsA<UPanelWidget>());
	Obj->SetBoolField(TEXT("is_visible"), Widget->IsVisible());

	if (UPanelWidget* Panel = Cast<UPanelWidget>(Widget))
	{
		Obj->SetNumberField(TEXT("child_count"), Panel->GetChildrenCount());
	}

	return Obj;
}

TSharedPtr<FJsonObject> FWidgetCommands::WidgetTreeToJson(UWidget* Widget)
{
	TSharedPtr<FJsonObject> Obj = WidgetToJson(Widget);
	if (!Widget) return Obj;

	if (UPanelWidget* Panel = Cast<UPanelWidget>(Widget))
	{
		TArray<TSharedPtr<FJsonValue>> Children;
		for (int32 i = 0; i < Panel->GetChildrenCount(); i++)
		{
			UWidget* Child = Panel->GetChildAt(i);
			if (Child)
			{
				Children.Add(MakeShared<FJsonValueObject>(WidgetTreeToJson(Child)));
			}
		}
		Obj->SetArrayField(TEXT("children"), Children);
	}

	return Obj;
}

UClass* FWidgetCommands::ResolveWidgetClass(const FString& WidgetType)
{
	static TMap<FString, UClass*> WidgetTypeMap;
	if (WidgetTypeMap.Num() == 0)
	{
		WidgetTypeMap.Add(TEXT("CanvasPanel"), UCanvasPanel::StaticClass());
		WidgetTypeMap.Add(TEXT("VerticalBox"), UVerticalBox::StaticClass());
		WidgetTypeMap.Add(TEXT("HorizontalBox"), UHorizontalBox::StaticClass());
		WidgetTypeMap.Add(TEXT("Button"), UButton::StaticClass());
		WidgetTypeMap.Add(TEXT("TextBlock"), UTextBlock::StaticClass());
		WidgetTypeMap.Add(TEXT("Image"), UImage::StaticClass());
		WidgetTypeMap.Add(TEXT("Border"), UBorder::StaticClass());
		WidgetTypeMap.Add(TEXT("Overlay"), UOverlay::StaticClass());
		WidgetTypeMap.Add(TEXT("ScrollBox"), UScrollBox::StaticClass());
		WidgetTypeMap.Add(TEXT("SizeBox"), USizeBox::StaticClass());
		WidgetTypeMap.Add(TEXT("Spacer"), USpacer::StaticClass());
		WidgetTypeMap.Add(TEXT("ProgressBar"), UProgressBar::StaticClass());
		WidgetTypeMap.Add(TEXT("Slider"), USlider::StaticClass());
		WidgetTypeMap.Add(TEXT("CheckBox"), UCheckBox::StaticClass());
		WidgetTypeMap.Add(TEXT("EditableTextBox"), UEditableTextBox::StaticClass());
		WidgetTypeMap.Add(TEXT("GridPanel"), UGridPanel::StaticClass());
		WidgetTypeMap.Add(TEXT("WrapBox"), UWrapBox::StaticClass());
		WidgetTypeMap.Add(TEXT("ScaleBox"), UScaleBox::StaticClass());
		WidgetTypeMap.Add(TEXT("ComboBoxString"), UComboBoxString::StaticClass());
		WidgetTypeMap.Add(TEXT("RichTextBlock"), URichTextBlock::StaticClass());
		WidgetTypeMap.Add(TEXT("MultiLineEditableTextBox"), UMultiLineEditableTextBox::StaticClass());
		WidgetTypeMap.Add(TEXT("WidgetSwitcher"), UWidgetSwitcher::StaticClass());
		WidgetTypeMap.Add(TEXT("UniformGridPanel"), UUniformGridPanel::StaticClass());
		WidgetTypeMap.Add(TEXT("BackgroundBlur"), UBackgroundBlur::StaticClass());
		WidgetTypeMap.Add(TEXT("Throbber"), UThrobber::StaticClass());
		WidgetTypeMap.Add(TEXT("CircularThrobber"), UCircularThrobber::StaticClass());
		WidgetTypeMap.Add(TEXT("ExpandableArea"), UExpandableArea::StaticClass());
		WidgetTypeMap.Add(TEXT("RetainerBox"), URetainerBox::StaticClass());
		WidgetTypeMap.Add(TEXT("InvalidationBox"), UInvalidationBox::StaticClass());
		WidgetTypeMap.Add(TEXT("SafeZone"), USafeZone::StaticClass());
		WidgetTypeMap.Add(TEXT("NamedSlot"), UNamedSlot::StaticClass());
	}
	if (UClass** Found = WidgetTypeMap.Find(WidgetType))
	{
		return *Found;
	}
	UClass* FoundClass = FCommonUtils::FindClassByName(WidgetType);
	if (FoundClass && FoundClass->IsChildOf(UWidget::StaticClass()))
	{
		return FoundClass;
	}
	return nullptr;
}

// =========================================================================
// Tier 1: Event Binding
// =========================================================================

TSharedPtr<FJsonObject> FWidgetCommands::HandleBindWidgetEvent(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));
	FString EventName = Params->GetStringField(TEXT("event_name"));
	FVector2D NodePosition = Params->HasField(TEXT("node_position"))
		? FCommonUtils::GetVector2DFromJson(Params, TEXT("node_position")) : FVector2D(300, 0);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' not found"), *WidgetName));

	// Find the multicast delegate property on the widget
	FMulticastDelegateProperty* DelegateProp = nullptr;
	for (TFieldIterator<FMulticastDelegateProperty> It(Widget->GetClass()); It; ++It)
	{
		if (It->GetName() == EventName)
		{
			DelegateProp = *It;
			break;
		}
	}
	if (!DelegateProp)
	{
		return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Event '%s' not found on widget '%s'"), *EventName, *WidgetName));
	}

	// Find or create event graph
	UEdGraph* EventGraph = FCommonUtils::FindOrCreateEventGraph(WBP);
	if (!EventGraph) return FCommonUtils::CreateErrorResponse(TEXT("Failed to get EventGraph"));

	// Create K2Node_ComponentBoundEvent
	UK2Node_ComponentBoundEvent* EventNode = NewObject<UK2Node_ComponentBoundEvent>(EventGraph);
	EventNode->CreateNewGuid();
	EventNode->NodePosX = NodePosition.X;
	EventNode->NodePosY = NodePosition.Y;
	EventNode->InitializeComponentBoundEventParams(nullptr, DelegateProp);
	EventNode->ComponentPropertyName = FName(*WidgetName);
	EventGraph->AddNode(EventNode, true);
	EventNode->PostPlacedNewNode();
	EventNode->AllocateDefaultPins();

	FBlueprintEditorUtils::MarkBlueprintAsModified(WBP);
	FKismetEditorUtilities::CompileBlueprint(WBP);

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetStringField(TEXT("event"), EventName);
	Result->SetStringField(TEXT("node_id"), EventNode->NodeGuid.ToString());
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleUnbindWidgetEvent(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));
	FString EventName = Params->GetStringField(TEXT("event_name"));

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UEdGraph* EventGraph = FCommonUtils::FindOrCreateEventGraph(WBP);
	if (!EventGraph) return FCommonUtils::CreateErrorResponse(TEXT("EventGraph not found"));

	bool bRemoved = false;
	TArray<UEdGraphNode*> NodesToRemove;
	for (UEdGraphNode* Node : EventGraph->Nodes)
	{
		UK2Node_ComponentBoundEvent* BoundEvent = Cast<UK2Node_ComponentBoundEvent>(Node);
		if (BoundEvent && BoundEvent->ComponentPropertyName == FName(*WidgetName)
			&& BoundEvent->DelegatePropertyName == FName(*EventName))
		{
			NodesToRemove.Add(Node);
			bRemoved = true;
		}
	}
	for (UEdGraphNode* Node : NodesToRemove)
	{
		EventGraph->RemoveNode(Node);
	}

	if (!bRemoved) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("No bound event '%s' found on widget '%s'"), *EventName, *WidgetName));

	FBlueprintEditorUtils::MarkBlueprintAsModified(WBP);
	FKismetEditorUtilities::CompileBlueprint(WBP);

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetStringField(TEXT("event"), EventName);
	Result->SetNumberField(TEXT("removed_count"), NodesToRemove.Num());
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleListWidgetEvents(const TSharedPtr<FJsonObject>& Params)
{
	FString WidgetType = Params->GetStringField(TEXT("widget_type"));
	if (WidgetType.IsEmpty()) return FCommonUtils::CreateErrorResponse(TEXT("Missing 'widget_type' parameter"));

	UClass* WidgetClass = ResolveWidgetClass(WidgetType);
	if (!WidgetClass) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget type '%s' not found"), *WidgetType));

	TArray<TSharedPtr<FJsonValue>> Events;
	for (TFieldIterator<FMulticastDelegateProperty> It(WidgetClass); It; ++It)
	{
		TSharedPtr<FJsonObject> EventObj = MakeShared<FJsonObject>();
		EventObj->SetStringField(TEXT("name"), It->GetName());
		EventObj->SetStringField(TEXT("class"), It->GetOwnerClass()->GetName());

		// Get delegate signature parameters
		if (UFunction* SignatureFunc = It->SignatureFunction)
		{
			TArray<TSharedPtr<FJsonValue>> ParamsArray;
			for (TFieldIterator<FProperty> ParamIt(SignatureFunc); ParamIt; ++ParamIt)
			{
				if (!(ParamIt->PropertyFlags & CPF_ReturnParm))
				{
					TSharedPtr<FJsonObject> ParamObj = MakeShared<FJsonObject>();
					ParamObj->SetStringField(TEXT("name"), ParamIt->GetName());
					ParamObj->SetStringField(TEXT("type"), ParamIt->GetCPPType());
					ParamsArray.Add(MakeShared<FJsonValueObject>(ParamObj));
				}
			}
			EventObj->SetArrayField(TEXT("parameters"), ParamsArray);
		}
		Events.Add(MakeShared<FJsonValueObject>(EventObj));
	}

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget_type"), WidgetType);
	Result->SetArrayField(TEXT("events"), Events);
	Result->SetNumberField(TEXT("count"), Events.Num());
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleAddWidgetFunctionNode(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));
	FString FunctionName = Params->GetStringField(TEXT("function_name"));
	FString GraphName = Params->HasField(TEXT("graph_name")) ? Params->GetStringField(TEXT("graph_name")) : TEXT("");
	FVector2D NodePosition = Params->HasField(TEXT("node_position"))
		? FCommonUtils::GetVector2DFromJson(Params, TEXT("node_position")) : FVector2D(500, 0);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' not found"), *WidgetName));

	// Find function on widget class
	UFunction* Func = Widget->GetClass()->FindFunctionByName(FName(*FunctionName));
	if (!Func) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Function '%s' not found on '%s'"), *FunctionName, *WidgetName));

	UEdGraph* TargetGraph = GraphName.IsEmpty()
		? FCommonUtils::FindOrCreateEventGraph(WBP)
		: FCommonUtils::FindGraphByName(WBP, GraphName);
	if (!TargetGraph) return FCommonUtils::CreateErrorResponse(TEXT("Target graph not found"));

	UK2Node_CallFunction* FuncNode = FCommonUtils::CreateFunctionCallNode(TargetGraph, Func, NodePosition);
	if (!FuncNode) return FCommonUtils::CreateErrorResponse(TEXT("Failed to create function node"));

	FBlueprintEditorUtils::MarkBlueprintAsModified(WBP);
	FKismetEditorUtilities::CompileBlueprint(WBP);

	TSharedPtr<FJsonObject> Result = FCommonUtils::CreateNodeResponse(FuncNode, true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetStringField(TEXT("function"), FunctionName);
	return Result;
}

// =========================================================================
// Tier 2: Content & Styling
// =========================================================================

TSharedPtr<FJsonObject> FWidgetCommands::HandleSetWidgetText(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));
	FString Text = Params->GetStringField(TEXT("text"));

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' not found"), *WidgetName));

	// Handle TextBlock
	if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
	{
		TextBlock->SetText(FText::FromString(Text));
	}
	// Handle EditableTextBox
	else if (UEditableTextBox* EditText = Cast<UEditableTextBox>(Widget))
	{
		EditText->SetText(FText::FromString(Text));
	}
	else
	{
		// Try generic Text property
		FProperty* TextProp = Widget->GetClass()->FindPropertyByName(TEXT("Text"));
		if (TextProp)
		{
			FTextProperty* FTextProp = CastField<FTextProperty>(TextProp);
			if (FTextProp)
			{
				FTextProp->SetPropertyValue_InContainer(Widget, FText::FromString(Text));
			}
		}
		else
		{
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' has no Text property"), *WidgetName));
		}
	}

	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetStringField(TEXT("text"), Text);
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleSetWidgetColor(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));
	FString PropertyName = Params->HasField(TEXT("property_name")) ? Params->GetStringField(TEXT("property_name")) : TEXT("ColorAndOpacity");

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' not found"), *WidgetName));

	FLinearColor Color = FLinearColor::White;

	// Parse hex color
	if (Params->HasField(TEXT("hex")))
	{
		FString Hex = Params->GetStringField(TEXT("hex"));
		Color = FColor::FromHex(Hex);
	}
	// Parse RGBA array
	else if (Params->HasField(TEXT("color")))
	{
		const TArray<TSharedPtr<FJsonValue>>* ColorArray;
		if (Params->TryGetArrayField(TEXT("color"), ColorArray) && ColorArray->Num() >= 3)
		{
			Color.R = (*ColorArray)[0]->AsNumber();
			Color.G = (*ColorArray)[1]->AsNumber();
			Color.B = (*ColorArray)[2]->AsNumber();
			Color.A = ColorArray->Num() >= 4 ? (*ColorArray)[3]->AsNumber() : 1.0f;
		}
	}

	// Set via property reflection
	FProperty* Prop = Widget->GetClass()->FindPropertyByName(FName(*PropertyName));
	if (Prop)
	{
		FStructProperty* StructProp = CastField<FStructProperty>(Prop);
		if (StructProp)
		{
			void* ValuePtr = StructProp->ContainerPtrToValuePtr<void>(Widget);
			if (StructProp->Struct == TBaseStructure<FLinearColor>::Get())
			{
				*static_cast<FLinearColor*>(ValuePtr) = Color;
			}
			else if (StructProp->Struct == TBaseStructure<FSlateColor>::Get())
			{
				*static_cast<FSlateColor*>(ValuePtr) = FSlateColor(Color);
			}
			else
			{
				return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Property '%s' is not a color type"), *PropertyName));
			}
		}
	}
	else
	{
		return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Property '%s' not found"), *PropertyName));
	}

	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetStringField(TEXT("property"), PropertyName);
	TArray<TSharedPtr<FJsonValue>> ColorArr;
	ColorArr.Add(MakeShared<FJsonValueNumber>(Color.R));
	ColorArr.Add(MakeShared<FJsonValueNumber>(Color.G));
	ColorArr.Add(MakeShared<FJsonValueNumber>(Color.B));
	ColorArr.Add(MakeShared<FJsonValueNumber>(Color.A));
	Result->SetArrayField(TEXT("color"), ColorArr);
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleSetWidgetBrush(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));
	FString PropertyName = Params->HasField(TEXT("property_name")) ? Params->GetStringField(TEXT("property_name")) : TEXT("Brush");
	FString ResourcePath = Params->HasField(TEXT("resource_path")) ? Params->GetStringField(TEXT("resource_path")) : TEXT("");

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' not found"), *WidgetName));

	FProperty* Prop = Widget->GetClass()->FindPropertyByName(FName(*PropertyName));
	FStructProperty* StructProp = Prop ? CastField<FStructProperty>(Prop) : nullptr;
	if (!StructProp || StructProp->Struct->GetName() != TEXT("SlateBrush"))
	{
		return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Property '%s' is not a SlateBrush"), *PropertyName));
	}

	FSlateBrush* BrushPtr = StructProp->ContainerPtrToValuePtr<FSlateBrush>(Widget);
	if (!BrushPtr) return FCommonUtils::CreateErrorResponse(TEXT("Failed to get brush pointer"));

	if (!ResourcePath.IsEmpty())
	{
		UObject* Resource = UEditorAssetLibrary::LoadAsset(ResourcePath);
		if (Resource)
		{
			BrushPtr->SetResourceObject(Resource);
		}
		else
		{
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Resource '%s' not found"), *ResourcePath));
		}
	}

	if (Params->HasField(TEXT("image_size")))
	{
		FVector2D ImageSize = FCommonUtils::GetVector2DFromJson(Params, TEXT("image_size"));
		BrushPtr->ImageSize = ImageSize;
	}

	if (Params->HasField(TEXT("tint")))
	{
		const TArray<TSharedPtr<FJsonValue>>* TintArr;
		if (Params->TryGetArrayField(TEXT("tint"), TintArr) && TintArr->Num() >= 3)
		{
			BrushPtr->TintColor = FSlateColor(FLinearColor(
				(*TintArr)[0]->AsNumber(), (*TintArr)[1]->AsNumber(), (*TintArr)[2]->AsNumber(),
				TintArr->Num() >= 4 ? (*TintArr)[3]->AsNumber() : 1.0f));
		}
	}

	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetStringField(TEXT("property"), PropertyName);
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleSetWidgetFont(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));
	int32 Size = Params->HasField(TEXT("size")) ? Params->GetIntegerField(TEXT("size")) : 0;
	FString FontStyle = Params->HasField(TEXT("font_style")) ? Params->GetStringField(TEXT("font_style")) : TEXT("");

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' not found"), *WidgetName));

	// Find Font property
	FProperty* Prop = Widget->GetClass()->FindPropertyByName(TEXT("Font"));
	FStructProperty* StructProp = Prop ? CastField<FStructProperty>(Prop) : nullptr;
	if (!StructProp)
	{
		return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' has no Font property"), *WidgetName));
	}

	FSlateFontInfo* FontPtr = StructProp->ContainerPtrToValuePtr<FSlateFontInfo>(Widget);
	if (!FontPtr) return FCommonUtils::CreateErrorResponse(TEXT("Failed to get font pointer"));

	if (Size > 0)
	{
		FontPtr->Size = Size;
	}
	if (!FontStyle.IsEmpty())
	{
		FontPtr->TypefaceFontName = FName(*FontStyle);
	}
	if (Params->HasField(TEXT("font_path")))
	{
		FString FontPath = Params->GetStringField(TEXT("font_path"));
		UObject* FontObj = UEditorAssetLibrary::LoadAsset(FontPath);
		if (FontObj)
		{
			FontPtr->FontObject = FontObj;
		}
	}
	if (Params->HasField(TEXT("letter_spacing")))
	{
		FontPtr->LetterSpacing = Params->GetIntegerField(TEXT("letter_spacing"));
	}

	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetNumberField(TEXT("size"), FontPtr->Size);
	Result->SetStringField(TEXT("style"), FontPtr->TypefaceFontName.ToString());
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleSetWidgetPadding(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));
	FString PropertyName = Params->HasField(TEXT("property_name")) ? Params->GetStringField(TEXT("property_name")) : TEXT("Padding");

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' not found"), *WidgetName));

	FProperty* Prop = Widget->GetClass()->FindPropertyByName(FName(*PropertyName));
	FStructProperty* StructProp = Prop ? CastField<FStructProperty>(Prop) : nullptr;
	if (!StructProp)
	{
		return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Property '%s' is not a Margin type"), *PropertyName));
	}

	FMargin* MarginPtr = StructProp->ContainerPtrToValuePtr<FMargin>(Widget);
	if (!MarginPtr) return FCommonUtils::CreateErrorResponse(TEXT("Failed to get margin pointer"));

	if (Params->HasField(TEXT("left"))) MarginPtr->Left = Params->GetNumberField(TEXT("left"));
	if (Params->HasField(TEXT("top"))) MarginPtr->Top = Params->GetNumberField(TEXT("top"));
	if (Params->HasField(TEXT("right"))) MarginPtr->Right = Params->GetNumberField(TEXT("right"));
	if (Params->HasField(TEXT("bottom"))) MarginPtr->Bottom = Params->GetNumberField(TEXT("bottom"));

	// Uniform padding shortcut
	if (Params->HasField(TEXT("uniform")))
	{
		float Uniform = Params->GetNumberField(TEXT("uniform"));
		*MarginPtr = FMargin(Uniform);
	}

	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetStringField(TEXT("property"), PropertyName);
	Result->SetNumberField(TEXT("left"), MarginPtr->Left);
	Result->SetNumberField(TEXT("top"), MarginPtr->Top);
	Result->SetNumberField(TEXT("right"), MarginPtr->Right);
	Result->SetNumberField(TEXT("bottom"), MarginPtr->Bottom);
	return Result;
}

// =========================================================================
// Tier 3: Animation
// =========================================================================

TSharedPtr<FJsonObject> FWidgetCommands::HandleCreateWidgetAnimation(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString AnimationName = Params->GetStringField(TEXT("animation_name"));
	float Duration = Params->HasField(TEXT("duration")) ? Params->GetNumberField(TEXT("duration")) : 1.0f;

	if (AnimationName.IsEmpty()) return FCommonUtils::CreateErrorResponse(TEXT("Missing 'animation_name' parameter"));

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	// Check if animation already exists
	for (UWidgetAnimation* Existing : WBP->Animations)
	{
		if (Existing && Existing->GetName() == AnimationName)
		{
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Animation '%s' already exists"), *AnimationName));
		}
	}

	// Create animation
	UWidgetAnimation* NewAnim = NewObject<UWidgetAnimation>(WBP, FName(*AnimationName), RF_Transactional);
	if (!NewAnim) return FCommonUtils::CreateErrorResponse(TEXT("Failed to create animation"));

	// Set up MovieScene with duration
	UMovieScene* MovieScene = NewAnim->GetMovieScene();
	if (MovieScene)
	{
		FFrameRate FrameRate = MovieScene->GetTickResolution();
		FFrameNumber EndFrame = (Duration * FrameRate).FloorToFrame();
		MovieScene->SetPlaybackRange(FFrameNumber(0), EndFrame.Value);
	}

	WBP->Animations.Add(NewAnim);

	FBlueprintEditorUtils::MarkBlueprintAsModified(WBP);
	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("animation_name"), AnimationName);
	Result->SetNumberField(TEXT("duration"), Duration);
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandlePlayAnimationNode(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString AnimationName = Params->GetStringField(TEXT("animation_name"));
	FString Action = Params->HasField(TEXT("action")) ? Params->GetStringField(TEXT("action")) : TEXT("PlayForward");
	FVector2D NodePosition = Params->HasField(TEXT("node_position"))
		? FCommonUtils::GetVector2DFromJson(Params, TEXT("node_position")) : FVector2D(500, 0);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	// Map action to function name
	FString FuncName;
	if (Action == TEXT("PlayForward") || Action == TEXT("Play")) FuncName = TEXT("PlayAnimation");
	else if (Action == TEXT("PlayReverse") || Action == TEXT("Reverse")) FuncName = TEXT("PlayAnimationReverse");
	else if (Action == TEXT("Stop")) FuncName = TEXT("StopAnimation");
	else if (Action == TEXT("Pause")) FuncName = TEXT("PauseAnimation");
	else FuncName = TEXT("PlayAnimation");

	UFunction* Func = UUserWidget::StaticClass()->FindFunctionByName(FName(*FuncName));
	if (!Func) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Function '%s' not found"), *FuncName));

	UEdGraph* EventGraph = FCommonUtils::FindOrCreateEventGraph(WBP);
	if (!EventGraph) return FCommonUtils::CreateErrorResponse(TEXT("EventGraph not found"));

	UK2Node_CallFunction* FuncNode = FCommonUtils::CreateFunctionCallNode(EventGraph, Func, NodePosition);
	if (!FuncNode) return FCommonUtils::CreateErrorResponse(TEXT("Failed to create play animation node"));

	FBlueprintEditorUtils::MarkBlueprintAsModified(WBP);
	FKismetEditorUtilities::CompileBlueprint(WBP);

	TSharedPtr<FJsonObject> Result = FCommonUtils::CreateNodeResponse(FuncNode, true);
	Result->SetStringField(TEXT("animation"), AnimationName);
	Result->SetStringField(TEXT("action"), Action);
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleListWidgetAnimations(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	TArray<TSharedPtr<FJsonValue>> AnimArray;
	for (UWidgetAnimation* Anim : WBP->Animations)
	{
		if (!Anim) continue;
		TSharedPtr<FJsonObject> AnimObj = MakeShared<FJsonObject>();
		AnimObj->SetStringField(TEXT("name"), Anim->GetName());

		UMovieScene* MovieScene = Anim->GetMovieScene();
		if (MovieScene)
		{
			FFrameRate FrameRate = MovieScene->GetTickResolution();
			TRange<FFrameNumber> Range = MovieScene->GetPlaybackRange();
			float DurationSec = FrameRate.AsSeconds(FFrameTime(Range.Size<FFrameNumber>()));
			AnimObj->SetNumberField(TEXT("duration"), DurationSec);
			AnimObj->SetNumberField(TEXT("track_count"), MovieScene->GetMasterTracks().Num() + MovieScene->GetObjectBindings().Num());
		}
		AnimArray.Add(MakeShared<FJsonValueObject>(AnimObj));
	}

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetArrayField(TEXT("animations"), AnimArray);
	Result->SetNumberField(TEXT("count"), AnimArray.Num());
	return Result;
}

// =========================================================================
// Tier 4: Atomic Batch Construction
// =========================================================================

UWidget* FWidgetCommands::BuildWidgetRecursive(UWidgetBlueprint* WBP, const TSharedPtr<FJsonObject>& WidgetSpec,
	UPanelWidget* Parent, TMap<FString, UWidget*>& Registry, TArray<FString>& Errors)
{
	FString Name = WidgetSpec->GetStringField(TEXT("name"));
	FString Type = WidgetSpec->GetStringField(TEXT("type"));

	if (Name.IsEmpty() || Type.IsEmpty())
	{
		Errors.Add(TEXT("Widget spec missing 'name' or 'type'"));
		return nullptr;
	}

	UClass* WidgetClass = ResolveWidgetClass(Type);
	if (!WidgetClass)
	{
		Errors.Add(FString::Printf(TEXT("Unknown widget type: '%s'"), *Type));
		return nullptr;
	}

	UWidget* NewWidget = WBP->WidgetTree->ConstructWidget<UWidget>(WidgetClass, *Name);
	if (!NewWidget)
	{
		Errors.Add(FString::Printf(TEXT("Failed to create widget '%s' of type '%s'"), *Name, *Type));
		return nullptr;
	}

	// Add to parent
	if (Parent)
	{
		Parent->AddChild(NewWidget);
	}

	Registry.Add(Name, NewWidget);

	// Helper lambdas for alignment parsing
	auto ParseHAlign = [](const FString& Str) -> EHorizontalAlignment {
		if (Str == TEXT("Left")) return HAlign_Left;
		if (Str == TEXT("Center")) return HAlign_Center;
		if (Str == TEXT("Right")) return HAlign_Right;
		return HAlign_Fill;
	};
	auto ParseVAlign = [](const FString& Str) -> EVerticalAlignment {
		if (Str == TEXT("Top")) return VAlign_Top;
		if (Str == TEXT("Center")) return VAlign_Center;
		if (Str == TEXT("Bottom")) return VAlign_Bottom;
		return VAlign_Fill;
	};
	auto ParsePadding = [](const TSharedPtr<FJsonObject>& Obj, const FString& Key) -> FMargin {
		FMargin M(0);
		if (!Obj->HasField(Key)) return M;
		const TSharedPtr<FJsonValue>& Val = Obj->Values[Key];
		if (Val->Type == EJson::Number) { M = FMargin(Val->AsNumber()); }
		else if (Val->Type == EJson::Object) {
			TSharedPtr<FJsonObject> PO = Val->AsObject();
			M.Left = PO->HasField(TEXT("left")) ? PO->GetNumberField(TEXT("left")) : 0;
			M.Top = PO->HasField(TEXT("top")) ? PO->GetNumberField(TEXT("top")) : 0;
			M.Right = PO->HasField(TEXT("right")) ? PO->GetNumberField(TEXT("right")) : 0;
			M.Bottom = PO->HasField(TEXT("bottom")) ? PO->GetNumberField(TEXT("bottom")) : 0;
		}
		else if (Val->Type == EJson::Array) {
			const TArray<TSharedPtr<FJsonValue>>& A = Val->AsArray();
			if (A.Num() >= 4) { M.Left = A[0]->AsNumber(); M.Top = A[1]->AsNumber(); M.Right = A[2]->AsNumber(); M.Bottom = A[3]->AsNumber(); }
		}
		return M;
	};

	// Apply slot properties
	if (WidgetSpec->HasField(TEXT("slot")) && NewWidget->Slot)
	{
		const TSharedPtr<FJsonObject>* SlotObj;
		if (WidgetSpec->TryGetObjectField(TEXT("slot"), SlotObj))
		{
			// Canvas Panel Slot
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(NewWidget->Slot))
			{
				if ((*SlotObj)->HasField(TEXT("position")))
					CanvasSlot->SetPosition(FCommonUtils::GetVector2DFromJson(*SlotObj, TEXT("position")));
				if ((*SlotObj)->HasField(TEXT("size")))
					CanvasSlot->SetSize(FCommonUtils::GetVector2DFromJson(*SlotObj, TEXT("size")));
				if ((*SlotObj)->HasField(TEXT("z_order")))
					CanvasSlot->SetZOrder((*SlotObj)->GetIntegerField(TEXT("z_order")));
				if ((*SlotObj)->HasField(TEXT("anchors")))
				{
					const TSharedPtr<FJsonObject>* AnchObj;
					if ((*SlotObj)->TryGetObjectField(TEXT("anchors"), AnchObj))
					{
						FAnchors Anchors;
						Anchors.Minimum.X = (*AnchObj)->HasField(TEXT("min_x")) ? (*AnchObj)->GetNumberField(TEXT("min_x")) : 0;
						Anchors.Minimum.Y = (*AnchObj)->HasField(TEXT("min_y")) ? (*AnchObj)->GetNumberField(TEXT("min_y")) : 0;
						Anchors.Maximum.X = (*AnchObj)->HasField(TEXT("max_x")) ? (*AnchObj)->GetNumberField(TEXT("max_x")) : Anchors.Minimum.X;
						Anchors.Maximum.Y = (*AnchObj)->HasField(TEXT("max_y")) ? (*AnchObj)->GetNumberField(TEXT("max_y")) : Anchors.Minimum.Y;
						CanvasSlot->SetAnchors(Anchors);
					}
				}
				if ((*SlotObj)->HasField(TEXT("alignment")))
					CanvasSlot->SetAlignment(FCommonUtils::GetVector2DFromJson(*SlotObj, TEXT("alignment")));
				if ((*SlotObj)->HasField(TEXT("auto_size")))
					CanvasSlot->SetAutoSize((*SlotObj)->GetBoolField(TEXT("auto_size")));
			}
			// VerticalBox Slot
			else if (UVerticalBoxSlot* VSlot = Cast<UVerticalBoxSlot>(NewWidget->Slot))
			{
				if ((*SlotObj)->HasField(TEXT("h_align")))
					VSlot->SetHorizontalAlignment(ParseHAlign((*SlotObj)->GetStringField(TEXT("h_align"))));
				if ((*SlotObj)->HasField(TEXT("v_align")))
					VSlot->SetVerticalAlignment(ParseVAlign((*SlotObj)->GetStringField(TEXT("v_align"))));
				if ((*SlotObj)->HasField(TEXT("padding")))
					VSlot->SetPadding(ParsePadding(*SlotObj, TEXT("padding")));
				if ((*SlotObj)->HasField(TEXT("size_rule")))
				{
					FSlateChildSize Size;
					FString SizeRule = (*SlotObj)->GetStringField(TEXT("size_rule"));
					if (SizeRule == TEXT("Auto")) Size.SizeRule = ESlateSizeRule::Automatic;
					else { Size.SizeRule = ESlateSizeRule::Fill; Size.Value = (*SlotObj)->HasField(TEXT("fill_weight")) ? (*SlotObj)->GetNumberField(TEXT("fill_weight")) : 1.0f; }
					VSlot->SetSize(Size);
				}
			}
			// HorizontalBox Slot
			else if (UHorizontalBoxSlot* HSlot = Cast<UHorizontalBoxSlot>(NewWidget->Slot))
			{
				if ((*SlotObj)->HasField(TEXT("h_align")))
					HSlot->SetHorizontalAlignment(ParseHAlign((*SlotObj)->GetStringField(TEXT("h_align"))));
				if ((*SlotObj)->HasField(TEXT("v_align")))
					HSlot->SetVerticalAlignment(ParseVAlign((*SlotObj)->GetStringField(TEXT("v_align"))));
				if ((*SlotObj)->HasField(TEXT("padding")))
					HSlot->SetPadding(ParsePadding(*SlotObj, TEXT("padding")));
				if ((*SlotObj)->HasField(TEXT("size_rule")))
				{
					FSlateChildSize Size;
					FString SizeRule = (*SlotObj)->GetStringField(TEXT("size_rule"));
					if (SizeRule == TEXT("Auto")) Size.SizeRule = ESlateSizeRule::Automatic;
					else { Size.SizeRule = ESlateSizeRule::Fill; Size.Value = (*SlotObj)->HasField(TEXT("fill_weight")) ? (*SlotObj)->GetNumberField(TEXT("fill_weight")) : 1.0f; }
					HSlot->SetSize(Size);
				}
			}
			// Overlay Slot
			else if (UOverlaySlot* OSlot = Cast<UOverlaySlot>(NewWidget->Slot))
			{
				if ((*SlotObj)->HasField(TEXT("h_align")))
					OSlot->SetHorizontalAlignment(ParseHAlign((*SlotObj)->GetStringField(TEXT("h_align"))));
				if ((*SlotObj)->HasField(TEXT("v_align")))
					OSlot->SetVerticalAlignment(ParseVAlign((*SlotObj)->GetStringField(TEXT("v_align"))));
				if ((*SlotObj)->HasField(TEXT("padding")))
					OSlot->SetPadding(ParsePadding(*SlotObj, TEXT("padding")));
			}
			// UniformGridPanel Slot
			else if (UUniformGridSlot* GridSlot = Cast<UUniformGridSlot>(NewWidget->Slot))
			{
				if ((*SlotObj)->HasField(TEXT("row")))
					GridSlot->SetRow((*SlotObj)->GetIntegerField(TEXT("row")));
				if ((*SlotObj)->HasField(TEXT("column")))
					GridSlot->SetColumn((*SlotObj)->GetIntegerField(TEXT("column")));
				if ((*SlotObj)->HasField(TEXT("h_align")))
					GridSlot->SetHorizontalAlignment(ParseHAlign((*SlotObj)->GetStringField(TEXT("h_align"))));
				if ((*SlotObj)->HasField(TEXT("v_align")))
					GridSlot->SetVerticalAlignment(ParseVAlign((*SlotObj)->GetStringField(TEXT("v_align"))));
			}
		}
	}

	// Apply visibility
	if (WidgetSpec->HasField(TEXT("visibility")))
	{
		FString Vis = WidgetSpec->GetStringField(TEXT("visibility"));
		if (Vis == TEXT("Collapsed")) NewWidget->SetVisibility(ESlateVisibility::Collapsed);
		else if (Vis == TEXT("Hidden")) NewWidget->SetVisibility(ESlateVisibility::Hidden);
		else if (Vis == TEXT("HitTestInvisible")) NewWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		else if (Vis == TEXT("SelfHitTestInvisible")) NewWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		else NewWidget->SetVisibility(ESlateVisibility::Visible);
	}

	// Apply enabled state
	if (WidgetSpec->HasField(TEXT("enabled")))
	{
		NewWidget->SetIsEnabled(WidgetSpec->GetBoolField(TEXT("enabled")));
	}

	// Apply render opacity
	if (WidgetSpec->HasField(TEXT("render_opacity")))
	{
		NewWidget->SetRenderOpacity(WidgetSpec->GetNumberField(TEXT("render_opacity")));
	}

	// Apply tooltip
	if (WidgetSpec->HasField(TEXT("tooltip")))
	{
		NewWidget->SetToolTipText(FText::FromString(WidgetSpec->GetStringField(TEXT("tooltip"))));
	}

	// Apply render transform
	if (WidgetSpec->HasField(TEXT("transform")))
	{
		const TSharedPtr<FJsonObject>* TransObj;
		if (WidgetSpec->TryGetObjectField(TEXT("transform"), TransObj))
		{
			FWidgetTransform Transform;
			if ((*TransObj)->HasField(TEXT("translation")))
				Transform.Translation = FCommonUtils::GetVector2DFromJson(*TransObj, TEXT("translation"));
			if ((*TransObj)->HasField(TEXT("scale")))
				Transform.Scale = FCommonUtils::GetVector2DFromJson(*TransObj, TEXT("scale"));
			else
				Transform.Scale = FVector2D(1.0f, 1.0f);
			if ((*TransObj)->HasField(TEXT("shear")))
				Transform.Shear = FCommonUtils::GetVector2DFromJson(*TransObj, TEXT("shear"));
			if ((*TransObj)->HasField(TEXT("angle")))
				Transform.Angle = (*TransObj)->GetNumberField(TEXT("angle"));
			NewWidget->SetRenderTransform(Transform);
		}
	}

	// Apply color shortcut (for any widget with ColorAndOpacity)
	if (WidgetSpec->HasField(TEXT("color")))
	{
		FLinearColor Color = FLinearColor::White;
		const TSharedPtr<FJsonValue>& ColorVal = WidgetSpec->Values[TEXT("color")];
		if (ColorVal->Type == EJson::String)
		{
			Color = FColor::FromHex(ColorVal->AsString());
		}
		else if (ColorVal->Type == EJson::Array)
		{
			const TArray<TSharedPtr<FJsonValue>>& CA = ColorVal->AsArray();
			if (CA.Num() >= 3)
			{
				Color.R = CA[0]->AsNumber(); Color.G = CA[1]->AsNumber(); Color.B = CA[2]->AsNumber();
				Color.A = CA.Num() >= 4 ? CA[3]->AsNumber() : 1.0f;
			}
		}
		FProperty* ColorProp = NewWidget->GetClass()->FindPropertyByName(TEXT("ColorAndOpacity"));
		if (ColorProp)
		{
			FStructProperty* SP = CastField<FStructProperty>(ColorProp);
			if (SP)
			{
				void* VP = SP->ContainerPtrToValuePtr<void>(NewWidget);
				if (SP->Struct == TBaseStructure<FLinearColor>::Get())
					*static_cast<FLinearColor*>(VP) = Color;
				else if (SP->Struct == TBaseStructure<FSlateColor>::Get())
					*static_cast<FSlateColor*>(VP) = FSlateColor(Color);
			}
		}
	}

	// Apply font_size shortcut
	if (WidgetSpec->HasField(TEXT("font_size")))
	{
		FProperty* FontProp = NewWidget->GetClass()->FindPropertyByName(TEXT("Font"));
		if (FontProp)
		{
			FSlateFontInfo* FontPtr = CastField<FStructProperty>(FontProp)->ContainerPtrToValuePtr<FSlateFontInfo>(NewWidget);
			if (FontPtr) FontPtr->Size = WidgetSpec->GetIntegerField(TEXT("font_size"));
		}
	}

	// Apply properties (generic)
	if (WidgetSpec->HasField(TEXT("properties")))
	{
		const TSharedPtr<FJsonObject>* PropsObj;
		if (WidgetSpec->TryGetObjectField(TEXT("properties"), PropsObj))
		{
			for (auto& Pair : (*PropsObj)->Values)
			{
				FString PropError;
					FCommonUtils::SetObjectProperty(NewWidget, Pair.Key, Pair.Value, PropError);
			}
		}
	}

	// Apply text shortcut
	if (WidgetSpec->HasField(TEXT("text")))
	{
		FString TextStr = WidgetSpec->GetStringField(TEXT("text"));
		if (UTextBlock* TextBlock = Cast<UTextBlock>(NewWidget))
		{
			TextBlock->SetText(FText::FromString(TextStr));
		}
		else if (UEditableTextBox* EditText = Cast<UEditableTextBox>(NewWidget))
		{
			EditText->SetText(FText::FromString(TextStr));
		}
		else if (URichTextBlock* RichText = Cast<URichTextBlock>(NewWidget))
		{
			RichText->SetText(FText::FromString(TextStr));
		}
		else
		{
			// Try generic Text property
			FProperty* TextProp = NewWidget->GetClass()->FindPropertyByName(TEXT("Text"));
			if (TextProp)
			{
				FTextProperty* FTP = CastField<FTextProperty>(TextProp);
				if (FTP) FTP->SetPropertyValue_InContainer(NewWidget, FText::FromString(TextStr));
			}
		}
	}

	// Recurse into children
	if (WidgetSpec->HasField(TEXT("children")))
	{
		UPanelWidget* AsPanel = Cast<UPanelWidget>(NewWidget);
		if (!AsPanel)
		{
			Errors.Add(FString::Printf(TEXT("Widget '%s' is not a panel but has children"), *Name));
		}
		else
		{
			const TArray<TSharedPtr<FJsonValue>>* ChildrenArr;
			if (WidgetSpec->TryGetArrayField(TEXT("children"), ChildrenArr))
			{
				for (const auto& ChildVal : *ChildrenArr)
				{
					TSharedPtr<FJsonObject> ChildObj = ChildVal->AsObject();
					if (ChildObj.IsValid())
					{
						BuildWidgetRecursive(WBP, ChildObj, AsPanel, Registry, Errors);
					}
				}
			}
		}
	}

	return NewWidget;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleBuildWidgetTree(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	bool bReplaceRoot = Params->HasField(TEXT("replace_root")) ? Params->GetBoolField(TEXT("replace_root")) : false;

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP || !WBP->WidgetTree)
		return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	const TArray<TSharedPtr<FJsonValue>>* WidgetsArr;
	if (!Params->TryGetArrayField(TEXT("widgets"), WidgetsArr) || WidgetsArr->Num() == 0)
		return FCommonUtils::CreateErrorResponse(TEXT("Missing or empty 'widgets' array"));

	FScopedTransaction Transaction(FText::FromString(TEXT("MCP: Build Widget Tree")));

	TMap<FString, UWidget*> Registry;
	TArray<FString> Errors;

	// Determine parent
	UPanelWidget* RootParent = nullptr;
	if (bReplaceRoot)
	{
		// Clear existing tree
		if (WBP->WidgetTree->RootWidget)
		{
			WBP->WidgetTree->RemoveWidget(WBP->WidgetTree->RootWidget);
			WBP->WidgetTree->RootWidget = nullptr;
		}
	}
	else
	{
		RootParent = Cast<UPanelWidget>(WBP->WidgetTree->RootWidget);
	}

	for (const auto& WidgetVal : *WidgetsArr)
	{
		TSharedPtr<FJsonObject> WidgetObj = WidgetVal->AsObject();
		if (!WidgetObj.IsValid()) continue;

		UWidget* Built = BuildWidgetRecursive(WBP, WidgetObj, RootParent, Registry, Errors);

		// If replacing root and this is the first widget, set as root
		if (bReplaceRoot && !WBP->WidgetTree->RootWidget && Built)
		{
			WBP->WidgetTree->RootWidget = Built;
			RootParent = Cast<UPanelWidget>(Built);
		}
	}

	if (Errors.Num() > 0)
	{
		TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
		Result->SetBoolField(TEXT("success"), false);
		TArray<TSharedPtr<FJsonValue>> ErrArr;
		for (const FString& Err : Errors)
		{
			ErrArr.Add(MakeShared<FJsonValueString>(Err));
		}
		Result->SetArrayField(TEXT("errors"), ErrArr);
		Result->SetNumberField(TEXT("widgets_created"), Registry.Num());
		return Result;
	}

	FBlueprintEditorUtils::MarkBlueprintAsModified(WBP);
	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetNumberField(TEXT("widgets_created"), Registry.Num());

	TArray<TSharedPtr<FJsonValue>> CreatedArr;
	for (auto& Pair : Registry)
	{
		TSharedPtr<FJsonObject> W = MakeShared<FJsonObject>();
		W->SetStringField(TEXT("name"), Pair.Key);
		W->SetStringField(TEXT("class"), Pair.Value ? Pair.Value->GetClass()->GetName() : TEXT("None"));
		CreatedArr.Add(MakeShared<FJsonValueObject>(W));
	}
	Result->SetArrayField(TEXT("widgets"), CreatedArr);
	return Result;
}

UWidget* FWidgetCommands::CloneWidgetRecursive(UWidgetBlueprint* WBP, UWidget* Source, const FString& NamePrefix, UPanelWidget* NewParent)
{
	if (!Source || !WBP || !WBP->WidgetTree) return nullptr;

	FString NewName = NamePrefix + TEXT("_") + Source->GetName();
	UWidget* Clone = WBP->WidgetTree->ConstructWidget<UWidget>(Source->GetClass(), *NewName);
	if (!Clone) return nullptr;

	// Copy properties via UEngine::CopyPropertiesForUnrelatedObjects
	UEngine::CopyPropertiesForUnrelatedObjects(Source, Clone);

	if (NewParent)
	{
		NewParent->AddChild(Clone);
	}

	// Recurse children
	if (UPanelWidget* SourcePanel = Cast<UPanelWidget>(Source))
	{
		UPanelWidget* ClonePanel = Cast<UPanelWidget>(Clone);
		if (ClonePanel)
		{
			for (int32 i = 0; i < SourcePanel->GetChildrenCount(); i++)
			{
				UWidget* Child = SourcePanel->GetChildAt(i);
				if (Child)
				{
					CloneWidgetRecursive(WBP, Child, NamePrefix, ClonePanel);
				}
			}
		}
	}

	return Clone;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleCloneWidgetSubtree(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString SourceName = Params->GetStringField(TEXT("source_widget"));
	FString NewPrefix = Params->GetStringField(TEXT("new_name"));
	FString TargetParentName = Params->HasField(TEXT("target_parent")) ? Params->GetStringField(TEXT("target_parent")) : TEXT("");

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UWidget* Source = FindWidgetByName(WBP, SourceName);
	if (!Source) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Source widget '%s' not found"), *SourceName));

	UPanelWidget* TargetParent = nullptr;
	if (!TargetParentName.IsEmpty())
	{
		TargetParent = Cast<UPanelWidget>(FindWidgetByName(WBP, TargetParentName));
		if (!TargetParent) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Target parent '%s' not found or not a panel"), *TargetParentName));
	}
	else
	{
		TargetParent = Cast<UPanelWidget>(WBP->WidgetTree->RootWidget);
	}

	FScopedTransaction Transaction(FText::FromString(TEXT("MCP: Clone Widget Subtree")));
	UWidget* Cloned = CloneWidgetRecursive(WBP, Source, NewPrefix, TargetParent);
	if (!Cloned) return FCommonUtils::CreateErrorResponse(TEXT("Failed to clone widget subtree"));

	FBlueprintEditorUtils::MarkBlueprintAsModified(WBP);
	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("source"), SourceName);
	Result->SetStringField(TEXT("cloned_root"), Cloned->GetName());
	return Result;
}

// =========================================================================
// Tier 5: Introspection
// =========================================================================

TSharedPtr<FJsonObject> FWidgetCommands::WidgetToDetailedJson(UWidget* Widget, bool bIncludeProperties, bool bIncludeEvents)
{
	TSharedPtr<FJsonObject> Obj = WidgetToJson(Widget);
	if (!Widget) return Obj;

	if (bIncludeProperties)
	{
		TArray<TSharedPtr<FJsonValue>> PropsArr;
		for (TFieldIterator<FProperty> It(Widget->GetClass()); It; ++It)
		{
			if (It->HasAnyPropertyFlags(CPF_Edit | CPF_BlueprintVisible))
			{
				TSharedPtr<FJsonObject> PropObj = MakeShared<FJsonObject>();
				PropObj->SetStringField(TEXT("name"), It->GetName());
				PropObj->SetStringField(TEXT("type"), It->GetCPPType());
				PropObj->SetStringField(TEXT("category"), It->GetMetaData(TEXT("Category")));
				PropsArr.Add(MakeShared<FJsonValueObject>(PropObj));
			}
		}
		Obj->SetArrayField(TEXT("properties"), PropsArr);
	}

	if (bIncludeEvents)
	{
		TArray<TSharedPtr<FJsonValue>> EventsArr;
		for (TFieldIterator<FMulticastDelegateProperty> It(Widget->GetClass()); It; ++It)
		{
			TSharedPtr<FJsonObject> EventObj = MakeShared<FJsonObject>();
			EventObj->SetStringField(TEXT("name"), It->GetName());
			EventsArr.Add(MakeShared<FJsonValueObject>(EventObj));
		}
		Obj->SetArrayField(TEXT("events"), EventsArr);
	}

	return Obj;
}

TSharedPtr<FJsonObject> FWidgetCommands::WidgetTreeToDetailedJson(UWidget* Widget, bool bIncludeProperties, bool bIncludeEvents)
{
	TSharedPtr<FJsonObject> Obj = WidgetToDetailedJson(Widget, bIncludeProperties, bIncludeEvents);
	if (!Widget) return Obj;

	if (UPanelWidget* Panel = Cast<UPanelWidget>(Widget))
	{
		TArray<TSharedPtr<FJsonValue>> Children;
		for (int32 i = 0; i < Panel->GetChildrenCount(); i++)
		{
			UWidget* Child = Panel->GetChildAt(i);
			if (Child)
			{
				Children.Add(MakeShared<FJsonValueObject>(WidgetTreeToDetailedJson(Child, bIncludeProperties, bIncludeEvents)));
			}
		}
		Obj->SetArrayField(TEXT("children"), Children);
	}

	return Obj;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleAnalyzeWidgetHierarchy(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	bool bIncludeProperties = Params->HasField(TEXT("include_properties")) ? Params->GetBoolField(TEXT("include_properties")) : true;
	bool bIncludeEvents = Params->HasField(TEXT("include_events")) ? Params->GetBoolField(TEXT("include_events")) : true;
	bool bIncludeAnimations = Params->HasField(TEXT("include_animations")) ? Params->GetBoolField(TEXT("include_animations")) : true;

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("name"), BlueprintName);
	Result->SetStringField(TEXT("parent_class"), WBP->ParentClass ? WBP->ParentClass->GetName() : TEXT("Unknown"));

	// Widget tree
	if (WBP->WidgetTree && WBP->WidgetTree->RootWidget)
	{
		Result->SetObjectField(TEXT("widget_tree"), WidgetTreeToDetailedJson(WBP->WidgetTree->RootWidget, bIncludeProperties, bIncludeEvents));
	}

	// Animations
	if (bIncludeAnimations)
	{
		TArray<TSharedPtr<FJsonValue>> AnimArr;
		for (UWidgetAnimation* Anim : WBP->Animations)
		{
			if (!Anim) continue;
			TSharedPtr<FJsonObject> AnimObj = MakeShared<FJsonObject>();
			AnimObj->SetStringField(TEXT("name"), Anim->GetName());
			UMovieScene* MS = Anim->GetMovieScene();
			if (MS)
			{
				FFrameRate FR = MS->GetTickResolution();
				TRange<FFrameNumber> R = MS->GetPlaybackRange();
				AnimObj->SetNumberField(TEXT("duration"), FR.AsSeconds(FFrameTime(R.Size<FFrameNumber>())));
			}
			AnimArr.Add(MakeShared<FJsonValueObject>(AnimObj));
		}
		Result->SetArrayField(TEXT("animations"), AnimArr);
	}

	// Count total widgets
	int32 WidgetCount = 0;
	if (WBP->WidgetTree)
	{
		WBP->WidgetTree->ForEachWidget([&](UWidget*) { WidgetCount++; });
	}
	Result->SetNumberField(TEXT("total_widgets"), WidgetCount);

	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleGetWidgetTypeInfo(const TSharedPtr<FJsonObject>& Params)
{
	FString WidgetType = Params->GetStringField(TEXT("widget_type"));
	if (WidgetType.IsEmpty()) return FCommonUtils::CreateErrorResponse(TEXT("Missing 'widget_type' parameter"));

	UClass* WidgetClass = ResolveWidgetClass(WidgetType);
	if (!WidgetClass) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget type '%s' not found"), *WidgetType));

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("type"), WidgetType);
	Result->SetStringField(TEXT("class_name"), WidgetClass->GetName());
	Result->SetStringField(TEXT("parent_class"), WidgetClass->GetSuperClass() ? WidgetClass->GetSuperClass()->GetName() : TEXT("None"));
	Result->SetBoolField(TEXT("is_panel"), WidgetClass->IsChildOf(UPanelWidget::StaticClass()));

	// Properties
	TArray<TSharedPtr<FJsonValue>> PropsArr;
	for (TFieldIterator<FProperty> It(WidgetClass); It; ++It)
	{
		if (It->HasAnyPropertyFlags(CPF_Edit | CPF_BlueprintVisible))
		{
			TSharedPtr<FJsonObject> PropObj = MakeShared<FJsonObject>();
			PropObj->SetStringField(TEXT("name"), It->GetName());
			PropObj->SetStringField(TEXT("type"), It->GetCPPType());
			PropObj->SetBoolField(TEXT("editable"), It->HasAnyPropertyFlags(CPF_Edit));
			PropsArr.Add(MakeShared<FJsonValueObject>(PropObj));
		}
	}
	Result->SetArrayField(TEXT("properties"), PropsArr);

	// Events
	TArray<TSharedPtr<FJsonValue>> EventsArr;
	for (TFieldIterator<FMulticastDelegateProperty> It(WidgetClass); It; ++It)
	{
		TSharedPtr<FJsonObject> EventObj = MakeShared<FJsonObject>();
		EventObj->SetStringField(TEXT("name"), It->GetName());
		EventsArr.Add(MakeShared<FJsonValueObject>(EventObj));
	}
	Result->SetArrayField(TEXT("events"), EventsArr);

	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleSearchWidgets(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString ClassFilter = Params->HasField(TEXT("class_filter")) ? Params->GetStringField(TEXT("class_filter")) : TEXT("");
	FString NamePattern = Params->HasField(TEXT("name_pattern")) ? Params->GetStringField(TEXT("name_pattern")) : TEXT("");

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP || !WBP->WidgetTree)
		return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UClass* FilterClass = nullptr;
	if (!ClassFilter.IsEmpty())
	{
		FilterClass = ResolveWidgetClass(ClassFilter);
	}

	TArray<TSharedPtr<FJsonValue>> MatchesArr;
	WBP->WidgetTree->ForEachWidget([&](UWidget* Widget)
	{
		if (!Widget) return;

		bool bMatch = true;

		if (FilterClass && !Widget->IsA(FilterClass))
			bMatch = false;

		if (bMatch && !NamePattern.IsEmpty())
		{
			if (!Widget->GetName().Contains(NamePattern))
				bMatch = false;
		}

		if (bMatch)
		{
			MatchesArr.Add(MakeShared<FJsonValueObject>(WidgetToJson(Widget)));
		}
	});

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetArrayField(TEXT("widgets"), MatchesArr);
	Result->SetNumberField(TEXT("count"), MatchesArr.Num());
	return Result;
}

// =========================================================================
// Tier 6: State & Layout
// =========================================================================

TSharedPtr<FJsonObject> FWidgetCommands::HandleSetWidgetVisibility(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));
	FString Visibility = Params->GetStringField(TEXT("visibility"));

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' not found"), *WidgetName));

	ESlateVisibility NewVis;
	if (Visibility == TEXT("Visible")) NewVis = ESlateVisibility::Visible;
	else if (Visibility == TEXT("Collapsed")) NewVis = ESlateVisibility::Collapsed;
	else if (Visibility == TEXT("Hidden")) NewVis = ESlateVisibility::Hidden;
	else if (Visibility == TEXT("HitTestInvisible")) NewVis = ESlateVisibility::HitTestInvisible;
	else if (Visibility == TEXT("SelfHitTestInvisible")) NewVis = ESlateVisibility::SelfHitTestInvisible;
	else return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Unknown visibility: '%s'. Use: Visible, Collapsed, Hidden, HitTestInvisible, SelfHitTestInvisible"), *Visibility));

	Widget->SetVisibility(NewVis);

	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetStringField(TEXT("visibility"), Visibility);
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleSetWidgetEnabled(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));
	bool bEnabled = Params->GetBoolField(TEXT("enabled"));

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' not found"), *WidgetName));

	Widget->SetIsEnabled(bEnabled);

	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetBoolField(TEXT("enabled"), bEnabled);
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleSetBoxSlot(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget || !Widget->Slot) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' not found or has no slot"), *WidgetName));

	auto ParseHAlign = [](const FString& S) -> EHorizontalAlignment {
		if (S == TEXT("Left")) return HAlign_Left;
		if (S == TEXT("Center")) return HAlign_Center;
		if (S == TEXT("Right")) return HAlign_Right;
		return HAlign_Fill;
	};
	auto ParseVAlign = [](const FString& S) -> EVerticalAlignment {
		if (S == TEXT("Top")) return VAlign_Top;
		if (S == TEXT("Center")) return VAlign_Center;
		if (S == TEXT("Bottom")) return VAlign_Bottom;
		return VAlign_Fill;
	};

	FString SlotType;

	// VerticalBox Slot
	if (UVerticalBoxSlot* VSlot = Cast<UVerticalBoxSlot>(Widget->Slot))
	{
		SlotType = TEXT("VerticalBoxSlot");
		if (Params->HasField(TEXT("h_align")))
			VSlot->SetHorizontalAlignment(ParseHAlign(Params->GetStringField(TEXT("h_align"))));
		if (Params->HasField(TEXT("v_align")))
			VSlot->SetVerticalAlignment(ParseVAlign(Params->GetStringField(TEXT("v_align"))));
		if (Params->HasField(TEXT("padding")))
		{
			float Pad = Params->GetNumberField(TEXT("padding"));
			VSlot->SetPadding(FMargin(Pad));
		}
		if (Params->HasField(TEXT("padding_left")) || Params->HasField(TEXT("padding_top")))
		{
			FMargin M;
			M.Left = Params->HasField(TEXT("padding_left")) ? Params->GetNumberField(TEXT("padding_left")) : 0;
			M.Top = Params->HasField(TEXT("padding_top")) ? Params->GetNumberField(TEXT("padding_top")) : 0;
			M.Right = Params->HasField(TEXT("padding_right")) ? Params->GetNumberField(TEXT("padding_right")) : 0;
			M.Bottom = Params->HasField(TEXT("padding_bottom")) ? Params->GetNumberField(TEXT("padding_bottom")) : 0;
			VSlot->SetPadding(M);
		}
		if (Params->HasField(TEXT("size_rule")))
		{
			FSlateChildSize Size;
			FString Rule = Params->GetStringField(TEXT("size_rule"));
			if (Rule == TEXT("Auto"))
				Size.SizeRule = ESlateSizeRule::Automatic;
			else
			{
				Size.SizeRule = ESlateSizeRule::Fill;
				Size.Value = Params->HasField(TEXT("fill_weight")) ? Params->GetNumberField(TEXT("fill_weight")) : 1.0f;
			}
			VSlot->SetSize(Size);
		}
	}
	// HorizontalBox Slot
	else if (UHorizontalBoxSlot* HSlot = Cast<UHorizontalBoxSlot>(Widget->Slot))
	{
		SlotType = TEXT("HorizontalBoxSlot");
		if (Params->HasField(TEXT("h_align")))
			HSlot->SetHorizontalAlignment(ParseHAlign(Params->GetStringField(TEXT("h_align"))));
		if (Params->HasField(TEXT("v_align")))
			HSlot->SetVerticalAlignment(ParseVAlign(Params->GetStringField(TEXT("v_align"))));
		if (Params->HasField(TEXT("padding")))
		{
			float Pad = Params->GetNumberField(TEXT("padding"));
			HSlot->SetPadding(FMargin(Pad));
		}
		if (Params->HasField(TEXT("padding_left")) || Params->HasField(TEXT("padding_top")))
		{
			FMargin M;
			M.Left = Params->HasField(TEXT("padding_left")) ? Params->GetNumberField(TEXT("padding_left")) : 0;
			M.Top = Params->HasField(TEXT("padding_top")) ? Params->GetNumberField(TEXT("padding_top")) : 0;
			M.Right = Params->HasField(TEXT("padding_right")) ? Params->GetNumberField(TEXT("padding_right")) : 0;
			M.Bottom = Params->HasField(TEXT("padding_bottom")) ? Params->GetNumberField(TEXT("padding_bottom")) : 0;
			HSlot->SetPadding(M);
		}
		if (Params->HasField(TEXT("size_rule")))
		{
			FSlateChildSize Size;
			FString Rule = Params->GetStringField(TEXT("size_rule"));
			if (Rule == TEXT("Auto"))
				Size.SizeRule = ESlateSizeRule::Automatic;
			else
			{
				Size.SizeRule = ESlateSizeRule::Fill;
				Size.Value = Params->HasField(TEXT("fill_weight")) ? Params->GetNumberField(TEXT("fill_weight")) : 1.0f;
			}
			HSlot->SetSize(Size);
		}
	}
	// Overlay Slot
	else if (UOverlaySlot* OSlot = Cast<UOverlaySlot>(Widget->Slot))
	{
		SlotType = TEXT("OverlaySlot");
		if (Params->HasField(TEXT("h_align")))
			OSlot->SetHorizontalAlignment(ParseHAlign(Params->GetStringField(TEXT("h_align"))));
		if (Params->HasField(TEXT("v_align")))
			OSlot->SetVerticalAlignment(ParseVAlign(Params->GetStringField(TEXT("v_align"))));
		if (Params->HasField(TEXT("padding")))
		{
			float Pad = Params->GetNumberField(TEXT("padding"));
			OSlot->SetPadding(FMargin(Pad));
		}
	}
	else
	{
		return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' is not in a Box/Overlay slot. Use set_widget_slot for Canvas, set_grid_slot for Grid."), *WidgetName));
	}

	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetStringField(TEXT("slot_type"), SlotType);
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleSetGridSlot(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget || !Widget->Slot) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' not found or has no slot"), *WidgetName));

	UUniformGridSlot* GridSlot = Cast<UUniformGridSlot>(Widget->Slot);
	if (!GridSlot) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' is not in a UniformGridPanel"), *WidgetName));

	if (Params->HasField(TEXT("row")))
		GridSlot->SetRow(Params->GetIntegerField(TEXT("row")));
	if (Params->HasField(TEXT("column")))
		GridSlot->SetColumn(Params->GetIntegerField(TEXT("column")));
	if (Params->HasField(TEXT("h_align")))
	{
		FString HA = Params->GetStringField(TEXT("h_align"));
		if (HA == TEXT("Left")) GridSlot->SetHorizontalAlignment(HAlign_Left);
		else if (HA == TEXT("Center")) GridSlot->SetHorizontalAlignment(HAlign_Center);
		else if (HA == TEXT("Right")) GridSlot->SetHorizontalAlignment(HAlign_Right);
		else GridSlot->SetHorizontalAlignment(HAlign_Fill);
	}
	if (Params->HasField(TEXT("v_align")))
	{
		FString VA = Params->GetStringField(TEXT("v_align"));
		if (VA == TEXT("Top")) GridSlot->SetVerticalAlignment(VAlign_Top);
		else if (VA == TEXT("Center")) GridSlot->SetVerticalAlignment(VAlign_Center);
		else if (VA == TEXT("Bottom")) GridSlot->SetVerticalAlignment(VAlign_Bottom);
		else GridSlot->SetVerticalAlignment(VAlign_Fill);
	}

	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetStringField(TEXT("slot_type"), TEXT("UniformGridSlot"));
	return Result;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleSetWidgetTransform(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' not found"), *WidgetName));

	FWidgetTransform Transform = Widget->GetRenderTransform();

	if (Params->HasField(TEXT("translation")))
		Transform.Translation = FCommonUtils::GetVector2DFromJson(Params, TEXT("translation"));
	if (Params->HasField(TEXT("scale")))
		Transform.Scale = FCommonUtils::GetVector2DFromJson(Params, TEXT("scale"));
	if (Params->HasField(TEXT("shear")))
		Transform.Shear = FCommonUtils::GetVector2DFromJson(Params, TEXT("shear"));
	if (Params->HasField(TEXT("angle")))
		Transform.Angle = Params->GetNumberField(TEXT("angle"));

	Widget->SetRenderTransform(Transform);

	if (Params->HasField(TEXT("pivot")))
	{
		FVector2D Pivot = FCommonUtils::GetVector2DFromJson(Params, TEXT("pivot"));
		Widget->SetRenderTransformPivot(Pivot);
	}

	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetNumberField(TEXT("angle"), Transform.Angle);
	TArray<TSharedPtr<FJsonValue>> TransArr;
	TransArr.Add(MakeShared<FJsonValueNumber>(Transform.Translation.X));
	TransArr.Add(MakeShared<FJsonValueNumber>(Transform.Translation.Y));
	Result->SetArrayField(TEXT("translation"), TransArr);
	TArray<TSharedPtr<FJsonValue>> ScaleArr;
	ScaleArr.Add(MakeShared<FJsonValueNumber>(Transform.Scale.X));
	ScaleArr.Add(MakeShared<FJsonValueNumber>(Transform.Scale.Y));
	Result->SetArrayField(TEXT("scale"), ScaleArr);
	return Result;
}

// =========================================================================
// Tier 7: Advanced
// =========================================================================

TSharedPtr<FJsonObject> FWidgetCommands::HandleSetWidgetTooltip(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName = Params->GetStringField(TEXT("blueprint_name"));
	FString BlueprintPath = Params->HasField(TEXT("blueprint_path")) ? Params->GetStringField(TEXT("blueprint_path")) : TEXT("/Game/");
	FString WidgetName = Params->GetStringField(TEXT("widget_name"));
	FString TooltipText = Params->GetStringField(TEXT("tooltip_text"));

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UWidget* Widget = FindWidgetByName(WBP, WidgetName);
	if (!Widget) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget '%s' not found"), *WidgetName));

	Widget->SetToolTipText(FText::FromString(TooltipText));

	FKismetEditorUtilities::CompileBlueprint(WBP);
	WBP->GetPackage()->MarkPackageDirty();

	TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("widget"), WidgetName);
	Result->SetStringField(TEXT("tooltip"), TooltipText);
	return Result;
}

// ============================================================================
// Tier 8: Variable Management
// ============================================================================

TSharedPtr<FJsonObject> FWidgetCommands::HandleAddWidgetVariable(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString VariableName;
	if (!Params->TryGetStringField(TEXT("variable_name"), VariableName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'variable_name'"));

	FString VariableType;
	if (!Params->TryGetStringField(TEXT("variable_type"), VariableType))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'variable_type'"));

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);

	FString SubType;
	Params->TryGetStringField(TEXT("sub_type"), SubType);

	if (VariableType == TEXT("Actor") || VariableType == TEXT("Pawn") || VariableType == TEXT("Character") ||
		VariableType == TEXT("Controller") || VariableType == TEXT("PlayerController") || VariableType == TEXT("ActorComponent"))
	{
		SubType = VariableType;
		VariableType = TEXT("Object");
	}

	FEdGraphPinType PinType;

	if (VariableType == TEXT("Boolean"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
	}
	else if (VariableType == TEXT("Integer") || VariableType == TEXT("Int"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Int;
	}
	else if (VariableType == TEXT("Float") || VariableType == TEXT("Double"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Real;
		PinType.PinSubCategory = VariableType == TEXT("Double") ? TEXT("double") : TEXT("float");
	}
	else if (VariableType == TEXT("String"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_String;
	}
	else if (VariableType == TEXT("Name"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Name;
	}
	else if (VariableType == TEXT("Text"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Text;
	}
	else if (VariableType == TEXT("Vector"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		PinType.PinSubCategoryObject = TBaseStructure<FVector>::Get();
	}
	else if (VariableType == TEXT("Rotator"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		PinType.PinSubCategoryObject = TBaseStructure<FRotator>::Get();
	}
	else if (VariableType == TEXT("Transform"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		PinType.PinSubCategoryObject = TBaseStructure<FTransform>::Get();
	}
	else if (VariableType == TEXT("LinearColor") || VariableType == TEXT("Color"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		PinType.PinSubCategoryObject = TBaseStructure<FLinearColor>::Get();
	}
	else if (VariableType == TEXT("Object"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Object;
		if (!SubType.IsEmpty())
		{
			UClass* ObjClass = FCommonUtils::FindClassByName(SubType);
			if (!ObjClass) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Sub-type class not found: %s"), *SubType));
			PinType.PinSubCategoryObject = ObjClass;
		}
		else
		{
			PinType.PinSubCategoryObject = UObject::StaticClass();
		}
	}
	else if (VariableType == TEXT("Class"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Class;
		if (!SubType.IsEmpty())
		{
			UClass* MetaClass = FCommonUtils::FindClassByName(SubType);
			if (!MetaClass) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Class sub_type not found: %s"), *SubType));
			PinType.PinSubCategoryObject = MetaClass;
		}
		else
		{
			PinType.PinSubCategoryObject = UObject::StaticClass();
		}
	}
	else if (VariableType == TEXT("SoftObject"))
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_SoftObject;
		if (!SubType.IsEmpty())
		{
			UClass* ObjClass = FCommonUtils::FindClassByName(SubType);
			if (ObjClass) PinType.PinSubCategoryObject = ObjClass;
		}
	}
	else if (VariableType == TEXT("Struct"))
	{
		if (SubType.IsEmpty()) return FCommonUtils::CreateErrorResponse(TEXT("Struct type requires 'sub_type' parameter"));
		UScriptStruct* FoundStruct = FindFirstObject<UScriptStruct>(*SubType, EFindFirstObjectOptions::None);
		if (!FoundStruct) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Struct not found: %s"), *SubType));
		PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		PinType.PinSubCategoryObject = FoundStruct;
	}
	else
	{
		UScriptStruct* FoundStruct = FindFirstObject<UScriptStruct>(*VariableType, EFindFirstObjectOptions::None);
		if (FoundStruct)
		{
			PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
			PinType.PinSubCategoryObject = FoundStruct;
		}
		else
		{
			return FCommonUtils::CreateErrorResponse(FString::Printf(
				TEXT("Unsupported variable type: %s. Supported: Boolean, Int, Float, String, Text, Vector, Rotator, Transform, LinearColor, Object, Class, SoftObject, Struct"), *VariableType));
		}
	}

	bool bIsArray = false;
	Params->TryGetBoolField(TEXT("is_array"), bIsArray);
	if (bIsArray) PinType.ContainerType = EPinContainerType::Array;

	FBlueprintEditorUtils::AddMemberVariable(Blueprint, FName(*VariableName), PinType);
	FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

	TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
	ResultObj->SetBoolField(TEXT("success"), true);
	ResultObj->SetStringField(TEXT("variable_name"), VariableName);
	ResultObj->SetStringField(TEXT("variable_type"), VariableType);
	ResultObj->SetBoolField(TEXT("is_array"), bIsArray);
	return ResultObj;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleDeleteWidgetVariable(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString VariableName;
	if (!Params->TryGetStringField(TEXT("variable_name"), VariableName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'variable_name'"));

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);
	FName VarName(*VariableName);
	int32 VarIndex = FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, VarName);
	if (VarIndex == INDEX_NONE)
		return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Variable not found: %s"), *VariableName));

	FBlueprintEditorUtils::RemoveMemberVariable(Blueprint, VarName);
	FKismetEditorUtilities::CompileBlueprint(WBP);

	TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
	ResultObj->SetBoolField(TEXT("success"), true);
	ResultObj->SetStringField(TEXT("removed_variable"), VariableName);
	return ResultObj;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleGetWidgetVariables(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);
	TArray<TSharedPtr<FJsonValue>> Variables;

	for (const FBPVariableDescription& VarDesc : Blueprint->NewVariables)
	{
		TSharedPtr<FJsonObject> VarObj = MakeShared<FJsonObject>();
		VarObj->SetStringField(TEXT("name"), VarDesc.VarName.ToString());
		VarObj->SetStringField(TEXT("type"), VarDesc.VarType.PinCategory.ToString());
		if (VarDesc.VarType.PinSubCategoryObject.IsValid())
			VarObj->SetStringField(TEXT("object_type"), VarDesc.VarType.PinSubCategoryObject->GetName());
		VarObj->SetStringField(TEXT("category"), VarDesc.Category.ToString());
		VarObj->SetBoolField(TEXT("is_array"), VarDesc.VarType.IsArray());
		Variables.Add(MakeShared<FJsonValueObject>(VarObj));
	}

	TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
	ResultObj->SetBoolField(TEXT("success"), true);
	ResultObj->SetStringField(TEXT("blueprint"), BlueprintName);
	ResultObj->SetArrayField(TEXT("variables"), Variables);
	ResultObj->SetNumberField(TEXT("count"), Variables.Num());
	return ResultObj;
}

// ============================================================================
// Tier 9: Node Connection/Deletion
// ============================================================================

TSharedPtr<FJsonObject> FWidgetCommands::HandleConnectWidgetNodes(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString SourceNodeId;
	if (!Params->TryGetStringField(TEXT("source_node_id"), SourceNodeId))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'source_node_id'"));

	FString TargetNodeId;
	if (!Params->TryGetStringField(TEXT("target_node_id"), TargetNodeId))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'target_node_id'"));

	bool bConnectExec = true;
	bool bConnectData = false;
	Params->TryGetBoolField(TEXT("connect_exec"), bConnectExec);
	Params->TryGetBoolField(TEXT("connect_data"), bConnectData);

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);

	UEdGraphNode* SourceNode = FCommonUtils::FindNodeByGuidInBlueprint(Blueprint, SourceNodeId);
	if (!SourceNode) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Source node not found: %s"), *SourceNodeId));

	UEdGraphNode* TargetNode = FCommonUtils::FindNodeByGuidInBlueprint(Blueprint, TargetNodeId);
	if (!TargetNode) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Target node not found: %s"), *TargetNodeId));

	UEdGraph* Graph = SourceNode->GetGraph();
	if (!Graph) return FCommonUtils::CreateErrorResponse(TEXT("Failed to get graph from source node"));

	bool bConnected = FCommonUtils::TryAutoConnectNodes(Graph, SourceNode, TargetNode, bConnectExec, bConnectData);
	FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

	TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
	ResultObj->SetBoolField(TEXT("success"), true);
	ResultObj->SetBoolField(TEXT("connected"), bConnected);
	ResultObj->SetStringField(TEXT("source_node_id"), SourceNodeId);
	ResultObj->SetStringField(TEXT("target_node_id"), TargetNodeId);
	return ResultObj;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleDisconnectWidgetNodes(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString NodeId;
	if (!Params->TryGetStringField(TEXT("node_id"), NodeId))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'node_id'"));

	FString PinName;
	if (!Params->TryGetStringField(TEXT("pin_name"), PinName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'pin_name'"));

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);

	UEdGraphNode* TargetNode = FCommonUtils::FindNodeByGuidInBlueprint(Blueprint, NodeId);
	if (!TargetNode) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Node not found: %s"), *NodeId));

	UEdGraphPin* TargetPin = nullptr;
	for (UEdGraphPin* Pin : TargetNode->Pins)
	{
		if (Pin && Pin->PinName.ToString() == PinName)
		{
			TargetPin = Pin;
			break;
		}
	}
	if (!TargetPin) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Pin not found: %s"), *PinName));

	int32 DisconnectedCount = TargetPin->LinkedTo.Num();
	TargetPin->BreakAllPinLinks();
	FKismetEditorUtilities::CompileBlueprint(WBP);

	TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
	ResultObj->SetBoolField(TEXT("success"), true);
	ResultObj->SetStringField(TEXT("node_id"), NodeId);
	ResultObj->SetStringField(TEXT("pin_name"), PinName);
	ResultObj->SetNumberField(TEXT("disconnected_links"), DisconnectedCount);
	return ResultObj;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleDeleteWidgetNode(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString NodeId;
	if (!Params->TryGetStringField(TEXT("node_id"), NodeId))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'node_id'"));

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);

	UEdGraphNode* TargetNode = FCommonUtils::FindNodeByGuidInBlueprint(Blueprint, NodeId);
	if (!TargetNode) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Node not found: %s"), *NodeId));

	UEdGraph* OwningGraph = TargetNode->GetGraph();
	FString NodeTitle = TargetNode->GetNodeTitle(ENodeTitleType::ListView).ToString();
	OwningGraph->RemoveNode(TargetNode);
	FKismetEditorUtilities::CompileBlueprint(WBP);

	TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
	ResultObj->SetBoolField(TEXT("success"), true);
	ResultObj->SetStringField(TEXT("removed_node_id"), NodeId);
	ResultObj->SetStringField(TEXT("removed_node_title"), NodeTitle);
	return ResultObj;
}

// ============================================================================
// Tier 10: Flow Control & Custom Events
// ============================================================================

TSharedPtr<FJsonObject> FWidgetCommands::HandleAddWidgetFlowControl(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString ControlType;
	if (!Params->TryGetStringField(TEXT("control_type"), ControlType))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'control_type'"));

	FVector2D NodePosition(0.0f, 0.0f);
	if (Params->HasField(TEXT("node_position")))
		NodePosition = FCommonUtils::GetVector2DFromJson(Params, TEXT("node_position"));

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	FString GraphName;
	Params->TryGetStringField(TEXT("graph_name"), GraphName);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);

	UEdGraph* EventGraph = nullptr;
	if (!GraphName.IsEmpty())
		EventGraph = FCommonUtils::FindGraphByName(Blueprint, GraphName);
	if (!EventGraph)
		EventGraph = FCommonUtils::FindOrCreateEventGraph(Blueprint);
	if (!EventGraph)
		return FCommonUtils::CreateErrorResponse(TEXT("Failed to get target graph"));

	UEdGraphNode* NewNode = nullptr;

	FString ControlTypeLower = ControlType.ToLower();

	if (ControlTypeLower == TEXT("branch"))
	{
		UK2Node_IfThenElse* BranchNode = NewObject<UK2Node_IfThenElse>(EventGraph);
		if (BranchNode)
		{
			BranchNode->CreateNewGuid();
			BranchNode->NodePosX = NodePosition.X;
			BranchNode->NodePosY = NodePosition.Y;
			EventGraph->AddNode(BranchNode, true);
			BranchNode->PostPlacedNewNode();
			BranchNode->AllocateDefaultPins();
			NewNode = BranchNode;
		}
	}
	else if (ControlTypeLower == TEXT("sequence"))
	{
		UK2Node_ExecutionSequence* SeqNode = NewObject<UK2Node_ExecutionSequence>(EventGraph);
		if (SeqNode)
		{
			SeqNode->CreateNewGuid();
			SeqNode->NodePosX = NodePosition.X;
			SeqNode->NodePosY = NodePosition.Y;
			EventGraph->AddNode(SeqNode, true);
			SeqNode->PostPlacedNewNode();
			SeqNode->AllocateDefaultPins();
			NewNode = SeqNode;
		}
	}
	else
	{
		FString MacroPath;
		if (ControlTypeLower == TEXT("forloop") || ControlTypeLower == TEXT("for"))
			MacroPath = TEXT("/Engine/EditorBlueprintResources/StandardMacros.StandardMacros:ForLoop");
		else if (ControlTypeLower == TEXT("foreachloop") || ControlTypeLower == TEXT("foreach"))
			MacroPath = TEXT("/Engine/EditorBlueprintResources/StandardMacros.StandardMacros:ForEachLoop");
		else if (ControlTypeLower == TEXT("foreachloopwithbreak"))
			MacroPath = TEXT("/Engine/EditorBlueprintResources/StandardMacros.StandardMacros:ForEachLoopWithBreak");
		else if (ControlTypeLower == TEXT("whileloop") || ControlTypeLower == TEXT("while"))
			MacroPath = TEXT("/Engine/EditorBlueprintResources/StandardMacros.StandardMacros:WhileLoop");
		else if (ControlTypeLower == TEXT("doonce"))
			MacroPath = TEXT("/Engine/EditorBlueprintResources/StandardMacros.StandardMacros:DoOnce");
		else if (ControlTypeLower == TEXT("multigate"))
			MacroPath = TEXT("/Engine/EditorBlueprintResources/StandardMacros.StandardMacros:DoN");
		else if (ControlTypeLower == TEXT("flipflop"))
			MacroPath = TEXT("/Engine/EditorBlueprintResources/StandardMacros.StandardMacros:FlipFlop");
		else if (ControlTypeLower == TEXT("gate"))
			MacroPath = TEXT("/Engine/EditorBlueprintResources/StandardMacros.StandardMacros:Gate");

		if (!MacroPath.IsEmpty())
		{
			UEdGraph* MacroGraph = LoadObject<UEdGraph>(nullptr, *MacroPath);
			if (MacroGraph)
			{
				UK2Node_MacroInstance* MacroNode = NewObject<UK2Node_MacroInstance>(EventGraph);
				if (MacroNode)
				{
					MacroNode->CreateNewGuid();
					MacroNode->SetMacroGraph(MacroGraph);
					MacroNode->NodePosX = NodePosition.X;
					MacroNode->NodePosY = NodePosition.Y;
					EventGraph->AddNode(MacroNode, true);
					MacroNode->PostPlacedNewNode();
					MacroNode->AllocateDefaultPins();
					NewNode = MacroNode;
				}
			}
			else
			{
				return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Failed to load macro: %s"), *MacroPath));
			}
		}
		else
		{
			return FCommonUtils::CreateErrorResponse(FString::Printf(
				TEXT("Unknown control_type: %s. Supported: branch, sequence, forloop, foreachloop, whileloop, doonce, multigate, flipflop, gate"), *ControlType));
		}
	}

	if (!NewNode)
		return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Failed to create %s node"), *ControlType));

	FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

	TSharedPtr<FJsonObject> ResultObj = FCommonUtils::CreateNodeResponse(NewNode);
	ResultObj->SetStringField(TEXT("control_type"), ControlType);
	return ResultObj;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleAddWidgetCustomEvent(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString EventName;
	if (!Params->TryGetStringField(TEXT("event_name"), EventName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'event_name'"));

	FString Action;
	if (!Params->TryGetStringField(TEXT("action"), Action))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'action' (define or call)"));

	FVector2D NodePosition(0.0f, 0.0f);
	if (Params->HasField(TEXT("node_position")))
		NodePosition = FCommonUtils::GetVector2DFromJson(Params, TEXT("node_position"));

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	FString GraphName;
	Params->TryGetStringField(TEXT("graph_name"), GraphName);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);

	UEdGraph* TargetGraph = nullptr;
	if (!GraphName.IsEmpty())
		TargetGraph = FCommonUtils::FindGraphByName(Blueprint, GraphName);
	if (!TargetGraph)
		TargetGraph = FCommonUtils::FindOrCreateEventGraph(Blueprint);
	if (!TargetGraph)
		return FCommonUtils::CreateErrorResponse(TEXT("Failed to get target graph"));

	if (Action == TEXT("define"))
	{
		for (UEdGraphNode* Node : TargetGraph->Nodes)
		{
			UK2Node_CustomEvent* Existing = Cast<UK2Node_CustomEvent>(Node);
			if (Existing && Existing->CustomFunctionName == FName(*EventName))
			{
				TSharedPtr<FJsonObject> ResultObj = FCommonUtils::CreateNodeResponse(Existing);
				ResultObj->SetBoolField(TEXT("already_exists"), true);
				return ResultObj;
			}
		}

		UK2Node_CustomEvent* CustomEventNode = NewObject<UK2Node_CustomEvent>(TargetGraph);
		CustomEventNode->CreateNewGuid();
		CustomEventNode->CustomFunctionName = FName(*EventName);
		CustomEventNode->NodePosX = NodePosition.X;
		CustomEventNode->NodePosY = NodePosition.Y;
		TargetGraph->AddNode(CustomEventNode, true);
		CustomEventNode->PostPlacedNewNode();
		CustomEventNode->AllocateDefaultPins();

		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		return FCommonUtils::CreateNodeResponse(CustomEventNode);
	}
	else if (Action == TEXT("call"))
	{
		UK2Node_CustomEvent* FoundEvent = nullptr;
		for (UEdGraphNode* Node : TargetGraph->Nodes)
		{
			UK2Node_CustomEvent* CustomEvent = Cast<UK2Node_CustomEvent>(Node);
			if (CustomEvent && CustomEvent->CustomFunctionName == FName(*EventName))
			{
				FoundEvent = CustomEvent;
				break;
			}
		}
		if (!FoundEvent)
			return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Custom event '%s' not found"), *EventName));

		UK2Node_CallFunction* CallNode = NewObject<UK2Node_CallFunction>(TargetGraph);
		CallNode->CreateNewGuid();
		CallNode->FunctionReference.SetSelfMember(FName(*EventName));
		CallNode->NodePosX = NodePosition.X;
		CallNode->NodePosY = NodePosition.Y;
		TargetGraph->AddNode(CallNode, true);
		CallNode->PostPlacedNewNode();
		CallNode->AllocateDefaultPins();

		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		return FCommonUtils::CreateNodeResponse(CallNode);
	}

	return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Invalid action: %s (use 'define' or 'call')"), *Action));
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleAddWidgetGenericNode(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString NodeClassName;
	if (!Params->TryGetStringField(TEXT("node_class"), NodeClassName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'node_class'"));

	FVector2D NodePosition(0.0f, 0.0f);
	if (Params->HasField(TEXT("node_position")))
		NodePosition = FCommonUtils::GetVector2DFromJson(Params, TEXT("node_position"));

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	FString GraphName = TEXT("EventGraph");
	Params->TryGetStringField(TEXT("graph_name"), GraphName);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);

	UEdGraph* TargetGraph = FCommonUtils::FindGraphByName(Blueprint, GraphName);
	if (!TargetGraph) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Graph not found: %s"), *GraphName));

	UEdGraphNode* NewNode = FCommonUtils::CreateNodeByClassName(TargetGraph, NodeClassName, NodePosition);
	if (!NewNode) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Failed to create node: %s"), *NodeClassName));

	FCommonUtils::InitializeNodeFromParams(NewNode, Params);
	FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

	TSharedPtr<FJsonObject> ResultObj = FCommonUtils::CreateNodeResponse(NewNode);
	ResultObj->SetStringField(TEXT("graph_name"), TargetGraph->GetName());
	return ResultObj;
}

// ============================================================================
// Tier 11: Pin Value Management
// ============================================================================

TSharedPtr<FJsonObject> FWidgetCommands::HandleSetWidgetPinDefault(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString NodeId;
	if (!Params->TryGetStringField(TEXT("node_id"), NodeId))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'node_id'"));

	FString PinName;
	if (!Params->TryGetStringField(TEXT("pin_name"), PinName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'pin_name'"));

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);

	UEdGraphNode* TargetNode = FCommonUtils::FindNodeByGuidInBlueprint(Blueprint, NodeId);
	if (!TargetNode) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Node not found: %s"), *NodeId));

	UEdGraphPin* TargetPin = FCommonUtils::FindPin(TargetNode, PinName, EGPD_Input);
	if (!TargetPin) TargetPin = FCommonUtils::FindPin(TargetNode, PinName, EGPD_Output);
	if (!TargetPin) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Pin not found: %s"), *PinName));

	FString OriginalValue = TargetPin->DefaultValue;
	const FName& PinCategory = TargetPin->PinType.PinCategory;

	if (PinCategory == UEdGraphSchema_K2::PC_Wildcard)
		return FCommonUtils::CreateErrorResponse(TEXT("Pin is Wildcard type. Connect a typed pin first."));

	FString ValueStr;
	double ValueNum = 0;
	bool ValueBool = false;

	if (PinCategory == UEdGraphSchema_K2::PC_Boolean)
	{
		if (Params->TryGetBoolField(TEXT("value"), ValueBool))
			TargetPin->DefaultValue = ValueBool ? TEXT("true") : TEXT("false");
		else if (Params->TryGetStringField(TEXT("value"), ValueStr))
		{
			FString Lower = ValueStr.ToLower();
			if (Lower == TEXT("true") || Lower == TEXT("1")) TargetPin->DefaultValue = TEXT("true");
			else TargetPin->DefaultValue = TEXT("false");
		}
		else
			return FCommonUtils::CreateErrorResponse(TEXT("Missing 'value' for boolean pin"));
	}
	else if (Params->TryGetStringField(TEXT("value"), ValueStr))
	{
		if (PinCategory == UEdGraphSchema_K2::PC_Object || PinCategory == UEdGraphSchema_K2::PC_SoftObject)
		{
			UObject* FoundObject = LoadObject<UObject>(nullptr, *ValueStr);
			if (FoundObject) { TargetPin->DefaultObject = FoundObject; TargetPin->DefaultValue = FoundObject->GetPathName(); }
			else return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Object not found: %s"), *ValueStr));
		}
		else if (PinCategory == UEdGraphSchema_K2::PC_Class || PinCategory == UEdGraphSchema_K2::PC_SoftClass)
		{
			UClass* FoundClass = ValueStr.Contains(TEXT("/")) ? LoadClass<UObject>(nullptr, *ValueStr) : FCommonUtils::FindClassByName(ValueStr);
			if (FoundClass) { TargetPin->DefaultObject = FoundClass; TargetPin->DefaultValue = FoundClass->GetPathName(); }
			else return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Class not found: %s"), *ValueStr));
		}
		else if (PinCategory == UEdGraphSchema_K2::PC_Byte)
		{
			UEnum* EnumType = Cast<UEnum>(TargetPin->PinType.PinSubCategoryObject.Get());
			if (EnumType)
			{
				int64 EnumValue = EnumType->GetValueByNameString(ValueStr);
				if (EnumValue == INDEX_NONE)
				{
					FString FullName = FString::Printf(TEXT("%s::%s"), *EnumType->GetName(), *ValueStr);
					EnumValue = EnumType->GetValueByNameString(FullName);
				}
				TargetPin->DefaultValue = (EnumValue != INDEX_NONE) ? EnumType->GetNameStringByValue(EnumValue) : ValueStr;
			}
			else
				TargetPin->DefaultValue = ValueStr;
		}
		else
			TargetPin->DefaultValue = ValueStr;
	}
	else if (Params->TryGetNumberField(TEXT("value"), ValueNum))
	{
		if (PinCategory == UEdGraphSchema_K2::PC_Int)
			TargetPin->DefaultValue = FString::FromInt(FMath::RoundToInt(ValueNum));
		else
			TargetPin->DefaultValue = FString::SanitizeFloat(ValueNum);
	}
	else if (Params->TryGetBoolField(TEXT("value"), ValueBool))
	{
		TargetPin->DefaultValue = ValueBool ? TEXT("true") : TEXT("false");
	}
	else if (Params->HasField(TEXT("value")))
	{
		const TArray<TSharedPtr<FJsonValue>>* ArrayValue;
		if (Params->TryGetArrayField(TEXT("value"), ArrayValue))
		{
			if (ArrayValue->Num() == 2)
				TargetPin->DefaultValue = FString::Printf(TEXT("(X=%f,Y=%f)"), (*ArrayValue)[0]->AsNumber(), (*ArrayValue)[1]->AsNumber());
			else if (ArrayValue->Num() == 3)
				TargetPin->DefaultValue = FString::Printf(TEXT("(X=%f,Y=%f,Z=%f)"), (*ArrayValue)[0]->AsNumber(), (*ArrayValue)[1]->AsNumber(), (*ArrayValue)[2]->AsNumber());
			else if (ArrayValue->Num() == 4)
				TargetPin->DefaultValue = FString::Printf(TEXT("(R=%f,G=%f,B=%f,A=%f)"), (*ArrayValue)[0]->AsNumber(), (*ArrayValue)[1]->AsNumber(), (*ArrayValue)[2]->AsNumber(), (*ArrayValue)[3]->AsNumber());
			else
				return FCommonUtils::CreateErrorResponse(TEXT("Unsupported array size for value"));
		}
		else
			return FCommonUtils::CreateErrorResponse(TEXT("Unsupported value type"));
	}
	else
	{
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'value' parameter"));
	}

	bool bChanged = (TargetPin->DefaultValue != OriginalValue);
	FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

	TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
	ResultObj->SetBoolField(TEXT("success"), true);
	ResultObj->SetStringField(TEXT("node_id"), NodeId);
	ResultObj->SetStringField(TEXT("pin_name"), PinName);
	ResultObj->SetStringField(TEXT("pin_type"), PinCategory.ToString());
	ResultObj->SetStringField(TEXT("value_set"), TargetPin->DefaultValue);
	ResultObj->SetBoolField(TEXT("value_changed"), bChanged);
	return ResultObj;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleGetWidgetPinValue(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString NodeId;
	if (!Params->TryGetStringField(TEXT("node_id"), NodeId))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'node_id'"));

	FString PinName;
	if (!Params->TryGetStringField(TEXT("pin_name"), PinName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'pin_name'"));

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);

	UEdGraphNode* TargetNode = FCommonUtils::FindNodeByGuidInBlueprint(Blueprint, NodeId);
	if (!TargetNode) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Node not found: %s"), *NodeId));

	UEdGraphPin* TargetPin = FCommonUtils::FindPin(TargetNode, PinName, EGPD_Input);
	if (!TargetPin) TargetPin = FCommonUtils::FindPin(TargetNode, PinName, EGPD_Output);
	if (!TargetPin) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Pin not found: %s"), *PinName));

	TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
	ResultObj->SetBoolField(TEXT("success"), true);
	ResultObj->SetStringField(TEXT("node_id"), NodeId);
	ResultObj->SetStringField(TEXT("pin_name"), PinName);
	ResultObj->SetStringField(TEXT("pin_type"), TargetPin->PinType.PinCategory.ToString());
	ResultObj->SetStringField(TEXT("default_value"), TargetPin->DefaultValue);
	if (TargetPin->DefaultObject)
		ResultObj->SetStringField(TEXT("default_object"), TargetPin->DefaultObject->GetPathName());
	ResultObj->SetBoolField(TEXT("has_connection"), TargetPin->HasAnyConnections());
	return ResultObj;
}

// ============================================================================
// Tier 12: Graph Introspection
// ============================================================================

TSharedPtr<FJsonObject> FWidgetCommands::HandleListWidgetGraphNodes(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	FString GraphName;
	Params->TryGetStringField(TEXT("graph_name"), GraphName);

	FString NodeType;
	Params->TryGetStringField(TEXT("node_type"), NodeType);

	FString NodeTitle;
	Params->TryGetStringField(TEXT("node_title"), NodeTitle);

	int32 Limit = 50;
	Params->TryGetNumberField(TEXT("limit"), Limit);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);

	UEdGraph* TargetGraph = nullptr;
	if (GraphName.IsEmpty())
		TargetGraph = FCommonUtils::FindOrCreateEventGraph(Blueprint);
	else
		TargetGraph = FCommonUtils::FindGraphByName(Blueprint, GraphName);
	if (!TargetGraph) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Graph not found: %s"), *GraphName));

	TArray<UEdGraphNode*> MatchingNodes;
	for (UEdGraphNode* Node : TargetGraph->Nodes)
	{
		if (!Node) continue;
		bool bMatches = true;

		if (!NodeType.IsEmpty())
		{
			if (NodeType == TEXT("Event"))
			{
				if (!Cast<UK2Node_Event>(Node) && !Cast<UK2Node_CustomEvent>(Node)) bMatches = false;
			}
			else if (NodeType == TEXT("Function"))
			{
				if (!Cast<UK2Node_CallFunction>(Node)) bMatches = false;
			}
			else if (NodeType == TEXT("Variable"))
			{
				if (!Cast<UK2Node_VariableGet>(Node) && !Node->GetClass()->GetName().Contains(TEXT("VariableSet")))
					bMatches = false;
			}
			else if (NodeType == TEXT("FlowControl"))
			{
				if (!Cast<UK2Node_IfThenElse>(Node) && !Cast<UK2Node_ExecutionSequence>(Node) && !Cast<UK2Node_MacroInstance>(Node))
					bMatches = false;
			}
		}

		if (bMatches && !NodeTitle.IsEmpty())
		{
			FString Title = Node->GetNodeTitle(ENodeTitleType::ListView).ToString();
			if (!Title.Contains(NodeTitle, ESearchCase::IgnoreCase)) bMatches = false;
		}

		if (bMatches)
		{
			MatchingNodes.Add(Node);
			if (MatchingNodes.Num() >= Limit) break;
		}
	}

	TArray<TSharedPtr<FJsonValue>> NodesArray;
	for (UEdGraphNode* Node : MatchingNodes)
	{
		TSharedPtr<FJsonObject> NodeObj = MakeShared<FJsonObject>();
		NodeObj->SetStringField(TEXT("node_id"), Node->NodeGuid.ToString());
		NodeObj->SetStringField(TEXT("title"), Node->GetNodeTitle(ENodeTitleType::ListView).ToString());
		NodeObj->SetStringField(TEXT("class"), Node->GetClass()->GetName());
		NodeObj->SetNumberField(TEXT("pos_x"), Node->NodePosX);
		NodeObj->SetNumberField(TEXT("pos_y"), Node->NodePosY);
		NodeObj->SetArrayField(TEXT("pins"), FCommonUtils::NodePinsToJson(Node));
		NodesArray.Add(MakeShared<FJsonValueObject>(NodeObj));
	}

	TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
	ResultObj->SetBoolField(TEXT("success"), true);
	ResultObj->SetStringField(TEXT("graph_name"), TargetGraph->GetName());
	ResultObj->SetArrayField(TEXT("nodes"), NodesArray);
	ResultObj->SetNumberField(TEXT("count"), NodesArray.Num());
	return ResultObj;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleListWidgetGraphs(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);
	TArray<UEdGraph*> AllGraphs = FCommonUtils::GetAllGraphs(Blueprint);

	TArray<TSharedPtr<FJsonValue>> GraphsArray;
	for (UEdGraph* Graph : AllGraphs)
	{
		if (!Graph) continue;
		GraphsArray.Add(MakeShared<FJsonValueObject>(FCommonUtils::GraphToJson(Graph)));
	}

	TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
	ResultObj->SetBoolField(TEXT("success"), true);
	ResultObj->SetStringField(TEXT("blueprint"), BlueprintName);
	ResultObj->SetArrayField(TEXT("graphs"), GraphsArray);
	ResultObj->SetNumberField(TEXT("count"), GraphsArray.Num());
	return ResultObj;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleCompileWidgetBlueprint(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	bool bValidateOnly = false;
	Params->TryGetBoolField(TEXT("validate_only"), bValidateOnly);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);

	TArray<TSharedPtr<FJsonValue>> Issues = FCommonUtils::ValidateBlueprintGraphs(Blueprint);

	if (Issues.Num() > 0)
	{
		TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
		ResultObj->SetBoolField(TEXT("success"), false);
		ResultObj->SetStringField(TEXT("error"), TEXT("Validation failed. Fix issues before compiling."));
		ResultObj->SetArrayField(TEXT("validation_issues"), Issues);
		return ResultObj;
	}

	if (bValidateOnly)
	{
		TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
		ResultObj->SetBoolField(TEXT("success"), true);
		ResultObj->SetStringField(TEXT("message"), TEXT("Validation passed. Ready to compile."));
		return ResultObj;
	}

	FCompilerResultsLog ResultsLog;
	FKismetEditorUtilities::CompileBlueprint(WBP, EBlueprintCompileOptions::None, &ResultsLog);

	bool bHasErrors = WBP->Status == BS_Error;
	TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
	ResultObj->SetStringField(TEXT("blueprint"), BlueprintName);
	ResultObj->SetBoolField(TEXT("success"), !bHasErrors);

	if (bHasErrors)
	{
		TArray<TSharedPtr<FJsonValue>> CompileErrors;
		for (const TSharedRef<FTokenizedMessage>& Msg : ResultsLog.Messages)
		{
			TSharedPtr<FJsonObject> ErrObj = MakeShared<FJsonObject>();
			ErrObj->SetStringField(TEXT("message"), Msg->ToText().ToString());
			ErrObj->SetStringField(TEXT("severity"), Msg->GetSeverity() == EMessageSeverity::Error ? TEXT("error") : TEXT("warning"));
			CompileErrors.Add(MakeShared<FJsonValueObject>(ErrObj));
		}
		ResultObj->SetArrayField(TEXT("compile_errors"), CompileErrors);
	}

	return ResultObj;
}

// ============================================================================
// Tier 13: Auxiliary
// ============================================================================

TSharedPtr<FJsonObject> FWidgetCommands::HandleAddWidgetCommentBox(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString CommentText;
	if (!Params->TryGetStringField(TEXT("comment_text"), CommentText))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'comment_text'"));

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	FString GraphName;
	Params->TryGetStringField(TEXT("graph_name"), GraphName);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);

	UEdGraph* TargetGraph = nullptr;
	if (!GraphName.IsEmpty())
		TargetGraph = FCommonUtils::FindGraphByName(Blueprint, GraphName);
	if (!TargetGraph)
		TargetGraph = FCommonUtils::FindOrCreateEventGraph(Blueprint);
	if (!TargetGraph) return FCommonUtils::CreateErrorResponse(TEXT("Failed to get target graph"));

	FVector2D Position(0.0f, 0.0f);
	if (Params->HasField(TEXT("position")))
		Position = FCommonUtils::GetVector2DFromJson(Params, TEXT("position"));

	FVector2D Size(400.0f, 200.0f);
	if (Params->HasField(TEXT("size")))
		Size = FCommonUtils::GetVector2DFromJson(Params, TEXT("size"));

	UEdGraphNode_Comment* CommentNode = NewObject<UEdGraphNode_Comment>(TargetGraph);
	CommentNode->NodeComment = CommentText;
	CommentNode->NodePosX = Position.X;
	CommentNode->NodePosY = Position.Y;
	CommentNode->NodeWidth = Size.X;
	CommentNode->NodeHeight = Size.Y;

	TargetGraph->AddNode(CommentNode);
	CommentNode->CreateNewGuid();
	CommentNode->PostPlacedNewNode();

	FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

	TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
	ResultObj->SetBoolField(TEXT("success"), true);
	ResultObj->SetStringField(TEXT("node_id"), CommentNode->NodeGuid.ToString());
	ResultObj->SetStringField(TEXT("comment_text"), CommentText);
	return ResultObj;
}

TSharedPtr<FJsonObject> FWidgetCommands::HandleAddWidgetFunctionOverride(const TSharedPtr<FJsonObject>& Params)
{
	FString BlueprintName;
	if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name'"));

	FString FunctionName;
	if (!Params->TryGetStringField(TEXT("function_name"), FunctionName))
		return FCommonUtils::CreateErrorResponse(TEXT("Missing 'function_name'"));

	FString BlueprintPath = TEXT("/Game/UI/");
	Params->TryGetStringField(TEXT("blueprint_path"), BlueprintPath);

	UWidgetBlueprint* WBP = FindWidgetBlueprint(BlueprintName, BlueprintPath);
	if (!WBP) return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Widget Blueprint '%s' not found"), *BlueprintName));

	UBlueprint* Blueprint = Cast<UBlueprint>(WBP);

	UK2Node_FunctionEntry* FunctionEntry = nullptr;
	UEdGraph* OverrideGraph = FCommonUtils::CreateFunctionOverride(Blueprint, FunctionName, FunctionEntry);

	if (!OverrideGraph || !FunctionEntry)
		return FCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Failed to create override for function: %s"), *FunctionName));

	FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

	TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
	ResultObj->SetBoolField(TEXT("success"), true);
	ResultObj->SetStringField(TEXT("blueprint_name"), BlueprintName);
	ResultObj->SetStringField(TEXT("function_name"), FunctionName);
	ResultObj->SetStringField(TEXT("graph_name"), OverrideGraph->GetName());
	ResultObj->SetStringField(TEXT("entry_node_id"), FunctionEntry->NodeGuid.ToString());
	return ResultObj;
}
