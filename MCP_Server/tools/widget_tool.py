"""
Widget Tools - Widget Blueprint (UMG) creation and manipulation.

Creates and modifies Widget Blueprints for UI development.
For Actor Blueprints, use blueprint_tool instead.
"""
from typing import List, Dict, Any, Optional, Literal
from mcp.server.fastmcp import FastMCP
from ..unreal_client import get_unreal_client
from ..utils import (
    log_info, log_error,
    create_error_response,
    validate_name, validate_path
)


def register_widget_tools(mcp: FastMCP):
    """Register Widget Blueprint manipulation tools with MCP server"""

    # =========================================================================
    # Widget Blueprint Asset Tools
    # =========================================================================

    @mcp.tool()
    def create_widget_blueprint(
        name: str,
        path: str = "/Game/UI/",
        parent_class: str = "UserWidget"
    ) -> Dict[str, Any]:
        """Create a Widget Blueprint (UMG) asset. Adds a CanvasPanel as root widget by default."""
        if error := validate_name(name, "name"):
            log_error(f"create_widget_blueprint validation failed: {error}")
            return create_error_response(error)
        if error := validate_path(path, "path"):
            log_error(f"create_widget_blueprint validation failed: {error}")
            return create_error_response(error)

        return get_unreal_client().execute_command("create_widget_blueprint", {
            "name": name,
            "path": path,
            "parent_class": parent_class
        })

    @mcp.tool()
    def analyze_widget_blueprint(
        name: str,
        path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Analyze Widget Blueprint structure. Returns widget tree hierarchy."""
        if error := validate_name(name, "name"):
            log_error(f"analyze_widget_blueprint validation failed: {error}")
            return create_error_response(error)

        return get_unreal_client().execute_command("analyze_widget_blueprint", {
            "name": name,
            "path": path
        })

    # =========================================================================
    # Widget Manipulation Tools
    # =========================================================================

    @mcp.tool()
    def add_widget(
        blueprint_name: str,
        widget_type: str,
        widget_name: str,
        parent_name: str = "",
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Add a widget to a Widget Blueprint.

        Supported widget_type: CanvasPanel, VerticalBox, HorizontalBox, Button,
        TextBlock, Image, Border, Overlay, ScrollBox, SizeBox, Spacer,
        ProgressBar, Slider, CheckBox, EditableTextBox, GridPanel, WrapBox, ScaleBox.

        If parent_name is empty, adds to root widget."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_type, "widget_type"), (widget_name, "widget_name")]:
            if error := validate_name(val, param_name):
                log_error(f"add_widget validation failed: {error}")
                return create_error_response(error)

        params = {
            "blueprint_name": blueprint_name,
            "widget_type": widget_type,
            "widget_name": widget_name,
            "blueprint_path": blueprint_path
        }
        if parent_name:
            params["parent_name"] = parent_name

        return get_unreal_client().execute_command("add_widget", params)

    @mcp.tool()
    def remove_widget(
        blueprint_name: str,
        widget_name: str,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Remove a widget from a Widget Blueprint."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name")]:
            if error := validate_name(val, param_name):
                log_error(f"remove_widget validation failed: {error}")
                return create_error_response(error)

        return get_unreal_client().execute_command("remove_widget", {
            "blueprint_name": blueprint_name,
            "widget_name": widget_name,
            "blueprint_path": blueprint_path
        })

    @mcp.tool()
    def set_widget_property(
        blueprint_name: str,
        widget_name: str,
        property_name: str,
        value: str,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Set a property on a widget (Visibility, ColorAndOpacity, Font, etc.)."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name"), (property_name, "property_name")]:
            if error := validate_name(val, param_name):
                log_error(f"set_widget_property validation failed: {error}")
                return create_error_response(error)

        return get_unreal_client().execute_command("set_widget_property", {
            "blueprint_name": blueprint_name,
            "widget_name": widget_name,
            "property_name": property_name,
            "value": value,
            "blueprint_path": blueprint_path
        })

    @mcp.tool()
    def set_widget_slot(
        blueprint_name: str,
        widget_name: str,
        position: Optional[List[float]] = None,
        size: Optional[List[float]] = None,
        anchors: Optional[Dict[str, float]] = None,
        alignment: Optional[List[float]] = None,
        z_order: Optional[int] = None,
        auto_size: Optional[bool] = None,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Set slot properties for a widget in a CanvasPanel.

        Args:
            position: [X, Y] position in canvas
            size: [Width, Height] size
            anchors: {"min_x": 0, "min_y": 0, "max_x": 0, "max_y": 0} (0-1 range)
            alignment: [X, Y] pivot point (0-1 range)
            z_order: Draw order (higher = on top)
            auto_size: Whether to auto-size to content
        """
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name")]:
            if error := validate_name(val, param_name):
                log_error(f"set_widget_slot validation failed: {error}")
                return create_error_response(error)

        params: Dict[str, Any] = {
            "blueprint_name": blueprint_name,
            "widget_name": widget_name,
            "blueprint_path": blueprint_path
        }
        if position is not None:
            params["position"] = position
        if size is not None:
            params["size"] = size
        if anchors is not None:
            params["anchors"] = anchors
        if alignment is not None:
            params["alignment"] = alignment
        if z_order is not None:
            params["z_order"] = z_order
        if auto_size is not None:
            params["auto_size"] = auto_size

        return get_unreal_client().execute_command("set_widget_slot", params)

    @mcp.tool()
    def list_widget_children(
        blueprint_name: str,
        parent_name: str = "",
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """List children of a widget. If parent_name is empty, lists root widget's children."""
        if error := validate_name(blueprint_name, "blueprint_name"):
            log_error(f"list_widget_children validation failed: {error}")
            return create_error_response(error)

        params = {
            "blueprint_name": blueprint_name,
            "blueprint_path": blueprint_path
        }
        if parent_name:
            params["parent_name"] = parent_name

        return get_unreal_client().execute_command("list_widget_children", params)

    # =========================================================================
    # Tier 1: Event Binding
    # =========================================================================

    @mcp.tool()
    def bind_widget_event(
        blueprint_name: str,
        widget_name: str,
        event_name: str,
        node_position: Optional[List[float]] = None,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Bind a widget event (OnClicked, OnTextChanged, etc.) to a Blueprint event node.
        Creates a K2Node_ComponentBoundEvent in the EventGraph."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name"), (event_name, "event_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        params: Dict[str, Any] = {
            "blueprint_name": blueprint_name, "widget_name": widget_name,
            "event_name": event_name, "blueprint_path": blueprint_path
        }
        if node_position:
            params["node_position"] = node_position
        return get_unreal_client().execute_command("bind_widget_event", params)

    @mcp.tool()
    def unbind_widget_event(
        blueprint_name: str,
        widget_name: str,
        event_name: str,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Remove a widget event binding from the Blueprint graph."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name"), (event_name, "event_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        return get_unreal_client().execute_command("unbind_widget_event", {
            "blueprint_name": blueprint_name, "widget_name": widget_name,
            "event_name": event_name, "blueprint_path": blueprint_path
        })

    @mcp.tool()
    def list_widget_events(
        widget_type: str
    ) -> Dict[str, Any]:
        """List all available events for a widget type (e.g., Button → OnClicked, OnPressed, etc.)."""
        if error := validate_name(widget_type, "widget_type"):
            return create_error_response(error)
        return get_unreal_client().execute_command("list_widget_events", {"widget_type": widget_type})

    @mcp.tool()
    def add_widget_function_node(
        blueprint_name: str,
        widget_name: str,
        function_name: str,
        node_position: Optional[List[float]] = None,
        graph_name: str = "",
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Add a function call node for a widget (e.g., SetText, SetColorAndOpacity)."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name"), (function_name, "function_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        params: Dict[str, Any] = {
            "blueprint_name": blueprint_name, "widget_name": widget_name,
            "function_name": function_name, "blueprint_path": blueprint_path
        }
        if node_position:
            params["node_position"] = node_position
        if graph_name:
            params["graph_name"] = graph_name
        return get_unreal_client().execute_command("add_widget_function_node", params)

    # =========================================================================
    # Tier 2: Content & Styling
    # =========================================================================

    @mcp.tool()
    def set_widget_text(
        blueprint_name: str,
        widget_name: str,
        text: str,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Set text content on TextBlock, EditableTextBox, or any widget with a Text property."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        return get_unreal_client().execute_command("set_widget_text", {
            "blueprint_name": blueprint_name, "widget_name": widget_name,
            "text": text, "blueprint_path": blueprint_path
        })

    @mcp.tool()
    def set_widget_color(
        blueprint_name: str,
        widget_name: str,
        color: Optional[List[float]] = None,
        hex: Optional[str] = None,
        property_name: str = "ColorAndOpacity",
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Set color on a widget. Use color=[R,G,B,A] (0-1) or hex='#RRGGBB'."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        params: Dict[str, Any] = {
            "blueprint_name": blueprint_name, "widget_name": widget_name,
            "property_name": property_name, "blueprint_path": blueprint_path
        }
        if color:
            params["color"] = color
        if hex:
            params["hex"] = hex
        return get_unreal_client().execute_command("set_widget_color", params)

    @mcp.tool()
    def set_widget_brush(
        blueprint_name: str,
        widget_name: str,
        resource_path: str = "",
        image_size: Optional[List[float]] = None,
        tint: Optional[List[float]] = None,
        property_name: str = "Brush",
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Set FSlateBrush on a widget (texture, icon). resource_path is the UE asset path."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        params: Dict[str, Any] = {
            "blueprint_name": blueprint_name, "widget_name": widget_name,
            "property_name": property_name, "blueprint_path": blueprint_path
        }
        if resource_path:
            params["resource_path"] = resource_path
        if image_size:
            params["image_size"] = image_size
        if tint:
            params["tint"] = tint
        return get_unreal_client().execute_command("set_widget_brush", params)

    @mcp.tool()
    def set_widget_font(
        blueprint_name: str,
        widget_name: str,
        size: int = 0,
        font_style: str = "",
        font_path: str = "",
        letter_spacing: Optional[int] = None,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Set font properties on a text widget. size=font size, font_style='Bold'/'Italic'/etc."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        params: Dict[str, Any] = {
            "blueprint_name": blueprint_name, "widget_name": widget_name,
            "blueprint_path": blueprint_path
        }
        if size > 0:
            params["size"] = size
        if font_style:
            params["font_style"] = font_style
        if font_path:
            params["font_path"] = font_path
        if letter_spacing is not None:
            params["letter_spacing"] = letter_spacing
        return get_unreal_client().execute_command("set_widget_font", params)

    @mcp.tool()
    def set_widget_padding(
        blueprint_name: str,
        widget_name: str,
        left: Optional[float] = None,
        top: Optional[float] = None,
        right: Optional[float] = None,
        bottom: Optional[float] = None,
        uniform: Optional[float] = None,
        property_name: str = "Padding",
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Set padding/margin on a widget. Use uniform for equal padding on all sides."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        params: Dict[str, Any] = {
            "blueprint_name": blueprint_name, "widget_name": widget_name,
            "property_name": property_name, "blueprint_path": blueprint_path
        }
        if uniform is not None:
            params["uniform"] = uniform
        else:
            if left is not None: params["left"] = left
            if top is not None: params["top"] = top
            if right is not None: params["right"] = right
            if bottom is not None: params["bottom"] = bottom
        return get_unreal_client().execute_command("set_widget_padding", params)

    # =========================================================================
    # Tier 3: Animation
    # =========================================================================

    @mcp.tool()
    def create_widget_animation(
        blueprint_name: str,
        animation_name: str,
        duration: float = 1.0,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Create a UWidgetAnimation in a Widget Blueprint."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (animation_name, "animation_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        return get_unreal_client().execute_command("create_widget_animation", {
            "blueprint_name": blueprint_name, "animation_name": animation_name,
            "duration": duration, "blueprint_path": blueprint_path
        })

    @mcp.tool()
    def play_animation_node(
        blueprint_name: str,
        animation_name: str,
        action: str = "Play",
        node_position: Optional[List[float]] = None,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Add a PlayAnimation/StopAnimation/PauseAnimation function node. action: Play, Reverse, Stop, Pause."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (animation_name, "animation_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        params: Dict[str, Any] = {
            "blueprint_name": blueprint_name, "animation_name": animation_name,
            "action": action, "blueprint_path": blueprint_path
        }
        if node_position:
            params["node_position"] = node_position
        return get_unreal_client().execute_command("play_animation_node", params)

    @mcp.tool()
    def list_widget_animations(
        blueprint_name: str,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """List all animations in a Widget Blueprint."""
        if error := validate_name(blueprint_name, "blueprint_name"):
            return create_error_response(error)
        return get_unreal_client().execute_command("list_widget_animations", {
            "blueprint_name": blueprint_name, "blueprint_path": blueprint_path
        })

    # =========================================================================
    # Tier 4: Atomic Batch Construction
    # =========================================================================

    @mcp.tool()
    def build_widget_tree(
        blueprint_name: str,
        widgets: List[Dict[str, Any]],
        replace_root: bool = False,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Build entire widget tree atomically in one call. Supports Ctrl+Z undo.

        Each widget: {"name": "MyButton", "type": "Button", "children": [...],
                      "slot": {"position": [x,y], "size": [w,h]},
                      "properties": {"bIsEnabled": true},
                      "text": "Click Me"}

        If replace_root=True, clears existing tree and uses first widget as root."""
        if error := validate_name(blueprint_name, "blueprint_name"):
            return create_error_response(error)
        if not widgets:
            return create_error_response("widgets list cannot be empty")
        return get_unreal_client().execute_command("build_widget_tree", {
            "blueprint_name": blueprint_name, "widgets": widgets,
            "replace_root": replace_root, "blueprint_path": blueprint_path
        })

    @mcp.tool()
    def clone_widget_subtree(
        blueprint_name: str,
        source_widget: str,
        new_name: str,
        target_parent: str = "",
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Clone a widget and all its children with a new name prefix."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (source_widget, "source_widget"), (new_name, "new_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        params: Dict[str, Any] = {
            "blueprint_name": blueprint_name, "source_widget": source_widget,
            "new_name": new_name, "blueprint_path": blueprint_path
        }
        if target_parent:
            params["target_parent"] = target_parent
        return get_unreal_client().execute_command("clone_widget_subtree", params)

    # =========================================================================
    # Tier 5: Introspection
    # =========================================================================

    @mcp.tool()
    def analyze_widget_hierarchy(
        blueprint_name: str,
        include_properties: bool = True,
        include_events: bool = True,
        include_animations: bool = True,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Deep analysis of Widget Blueprint: widget tree with properties, events, and animations."""
        if error := validate_name(blueprint_name, "blueprint_name"):
            return create_error_response(error)
        return get_unreal_client().execute_command("analyze_widget_hierarchy", {
            "blueprint_name": blueprint_name, "include_properties": include_properties,
            "include_events": include_events, "include_animations": include_animations,
            "blueprint_path": blueprint_path
        })

    @mcp.tool()
    def get_widget_type_info(
        widget_type: str
    ) -> Dict[str, Any]:
        """Get all properties and events for a widget type (e.g., Button, TextBlock)."""
        if error := validate_name(widget_type, "widget_type"):
            return create_error_response(error)
        return get_unreal_client().execute_command("get_widget_type_info", {"widget_type": widget_type})

    @mcp.tool()
    def search_widgets(
        blueprint_name: str,
        class_filter: str = "",
        name_pattern: str = "",
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Search widgets by class type or name pattern in a Widget Blueprint."""
        if error := validate_name(blueprint_name, "blueprint_name"):
            return create_error_response(error)
        params: Dict[str, Any] = {
            "blueprint_name": blueprint_name, "blueprint_path": blueprint_path
        }
        if class_filter:
            params["class_filter"] = class_filter
        if name_pattern:
            params["name_pattern"] = name_pattern
        return get_unreal_client().execute_command("search_widgets", params)

    log_info("Widget tools registered successfully")
