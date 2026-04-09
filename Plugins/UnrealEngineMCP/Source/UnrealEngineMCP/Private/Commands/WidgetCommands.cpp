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
#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorAssetLibrary.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UObject/SavePackage.h"

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
