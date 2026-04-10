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
        ProgressBar, Slider, CheckBox, EditableTextBox, GridPanel, WrapBox, ScaleBox,
        ComboBoxString, RichTextBlock, MultiLineEditableTextBox, WidgetSwitcher,
        UniformGridPanel, BackgroundBlur, Throbber, CircularThrobber,
        ExpandableArea, RetainerBox, InvalidationBox, SafeZone, NamedSlot.

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

        Each widget spec supports:
          name, type (required), children (recursive)
          text: "Hello" (TextBlock/RichTextBlock/EditableTextBox shortcut)
          color: [R,G,B,A] or "#RRGGBB" (ColorAndOpacity shortcut)
          font_size: 24 (font size shortcut)
          visibility: "Visible"|"Collapsed"|"Hidden"|"HitTestInvisible"|"SelfHitTestInvisible"
          enabled: true/false
          render_opacity: 0.0-1.0
          tooltip: "Hover text"
          transform: {"translation": [x,y], "scale": [x,y], "angle": 45, "shear": [x,y]}
          properties: {"PropertyName": "value"} (generic, via reflection)
          slot: depends on parent type:
            CanvasPanel: {"position": [x,y], "size": [w,h], "anchors": {"min_x":0,"min_y":0,"max_x":1,"max_y":1}, "alignment": [0.5,0.5], "z_order": 1, "auto_size": true}
            VerticalBox/HorizontalBox: {"h_align": "Fill", "v_align": "Fill", "size_rule": "Fill", "fill_weight": 1.0, "padding": 8}
            Overlay: {"h_align": "Fill", "v_align": "Fill", "padding": 8}
            UniformGridPanel: {"row": 0, "column": 0, "h_align": "Fill", "v_align": "Fill"}

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

    # =========================================================================
    # Tier 6: State & Layout
    # =========================================================================

    @mcp.tool()
    def set_widget_visibility(
        blueprint_name: str,
        widget_name: str,
        visibility: Literal["Visible", "Collapsed", "Hidden", "HitTestInvisible", "SelfHitTestInvisible"],
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Set widget visibility state.

        - Visible: Fully visible and interactive
        - Collapsed: Hidden, takes no space in layout
        - Hidden: Hidden but still occupies layout space
        - HitTestInvisible: Visible but not interactive (click-through), children too
        - SelfHitTestInvisible: Visible but not interactive, children CAN receive input
        """
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        return get_unreal_client().execute_command("set_widget_visibility", {
            "blueprint_name": blueprint_name, "widget_name": widget_name,
            "visibility": visibility, "blueprint_path": blueprint_path
        })

    @mcp.tool()
    def set_widget_enabled(
        blueprint_name: str,
        widget_name: str,
        enabled: bool,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Enable or disable a widget. Disabled widgets cannot receive user input."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        return get_unreal_client().execute_command("set_widget_enabled", {
            "blueprint_name": blueprint_name, "widget_name": widget_name,
            "enabled": enabled, "blueprint_path": blueprint_path
        })

    @mcp.tool()
    def set_box_slot(
        blueprint_name: str,
        widget_name: str,
        h_align: Optional[Literal["Left", "Center", "Right", "Fill"]] = None,
        v_align: Optional[Literal["Top", "Center", "Bottom", "Fill"]] = None,
        size_rule: Optional[Literal["Auto", "Fill"]] = None,
        fill_weight: Optional[float] = None,
        padding: Optional[float] = None,
        padding_left: Optional[float] = None,
        padding_top: Optional[float] = None,
        padding_right: Optional[float] = None,
        padding_bottom: Optional[float] = None,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Set slot properties for a widget in a VerticalBox, HorizontalBox, or Overlay.

        Args:
            h_align: Horizontal alignment (Left, Center, Right, Fill)
            v_align: Vertical alignment (Top, Center, Bottom, Fill)
            size_rule: Auto (fit content) or Fill (expand to fill available space)
            fill_weight: Weight when size_rule=Fill (default 1.0)
            padding: Uniform padding on all sides
            padding_left/top/right/bottom: Per-side padding (overrides padding)
        """
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        params: Dict[str, Any] = {
            "blueprint_name": blueprint_name, "widget_name": widget_name,
            "blueprint_path": blueprint_path
        }
        if h_align is not None: params["h_align"] = h_align
        if v_align is not None: params["v_align"] = v_align
        if size_rule is not None: params["size_rule"] = size_rule
        if fill_weight is not None: params["fill_weight"] = fill_weight
        if padding is not None: params["padding"] = padding
        if padding_left is not None: params["padding_left"] = padding_left
        if padding_top is not None: params["padding_top"] = padding_top
        if padding_right is not None: params["padding_right"] = padding_right
        if padding_bottom is not None: params["padding_bottom"] = padding_bottom
        return get_unreal_client().execute_command("set_box_slot", params)

    @mcp.tool()
    def set_grid_slot(
        blueprint_name: str,
        widget_name: str,
        row: Optional[int] = None,
        column: Optional[int] = None,
        h_align: Optional[Literal["Left", "Center", "Right", "Fill"]] = None,
        v_align: Optional[Literal["Top", "Center", "Bottom", "Fill"]] = None,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Set slot properties for a widget in a UniformGridPanel.

        Args:
            row: Grid row index (0-based)
            column: Grid column index (0-based)
            h_align: Horizontal alignment within cell
            v_align: Vertical alignment within cell
        """
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        params: Dict[str, Any] = {
            "blueprint_name": blueprint_name, "widget_name": widget_name,
            "blueprint_path": blueprint_path
        }
        if row is not None: params["row"] = row
        if column is not None: params["column"] = column
        if h_align is not None: params["h_align"] = h_align
        if v_align is not None: params["v_align"] = v_align
        return get_unreal_client().execute_command("set_grid_slot", params)

    @mcp.tool()
    def set_widget_transform(
        blueprint_name: str,
        widget_name: str,
        translation: Optional[List[float]] = None,
        scale: Optional[List[float]] = None,
        shear: Optional[List[float]] = None,
        angle: Optional[float] = None,
        pivot: Optional[List[float]] = None,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Set render transform on a widget.

        Args:
            translation: [X, Y] offset in pixels
            scale: [X, Y] scale factors (1.0 = normal)
            shear: [X, Y] shear amounts
            angle: Rotation in degrees
            pivot: [X, Y] transform pivot point (0-1 range, default [0.5, 0.5])
        """
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        params: Dict[str, Any] = {
            "blueprint_name": blueprint_name, "widget_name": widget_name,
            "blueprint_path": blueprint_path
        }
        if translation is not None: params["translation"] = translation
        if scale is not None: params["scale"] = scale
        if shear is not None: params["shear"] = shear
        if angle is not None: params["angle"] = angle
        if pivot is not None: params["pivot"] = pivot
        return get_unreal_client().execute_command("set_widget_transform", params)

    @mcp.tool()
    def set_widget_tooltip(
        blueprint_name: str,
        widget_name: str,
        tooltip_text: str,
        blueprint_path: str = "/Game/"
    ) -> Dict[str, Any]:
        """Set tooltip text on a widget. Shown when user hovers over the widget."""
        for val, param_name in [(blueprint_name, "blueprint_name"), (widget_name, "widget_name")]:
            if error := validate_name(val, param_name):
                return create_error_response(error)
        return get_unreal_client().execute_command("set_widget_tooltip", {
            "blueprint_name": blueprint_name, "widget_name": widget_name,
            "tooltip_text": tooltip_text, "blueprint_path": blueprint_path
        })

    # =========================================================================
    # Tier 8: Variable Management
    # =========================================================================

    @mcp.tool()
    def add_widget_variable(
        blueprint_name: str,
        variable_name: str,
        variable_type: str,
        sub_type: str = "",
        is_array: bool = False,
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """Add a member variable to a Widget Blueprint.

        Args:
            blueprint_name: Name of the Widget Blueprint
            variable_name: Name for the new variable
            variable_type: Type - Boolean, Int, Float, String, Text, Vector, Rotator, Transform, LinearColor, Object, Class, SoftObject, Struct
            sub_type: Sub-type for Object/Class/Struct (e.g. "PlayerController", "MyStruct")
            is_array: If True, creates an array of the specified type
            blueprint_path: Asset path (default /Game/UI/)
        """
        for val, name in [(blueprint_name, "blueprint_name"), (variable_name, "variable_name"), (variable_type, "variable_type")]:
            if error := validate_name(val, name):
                return create_error_response(error)
        params = {"blueprint_name": blueprint_name, "variable_name": variable_name,
                  "variable_type": variable_type, "blueprint_path": blueprint_path}
        if sub_type: params["sub_type"] = sub_type
        if is_array: params["is_array"] = True
        return get_unreal_client().execute_command("add_widget_variable", params)

    @mcp.tool()
    def delete_widget_variable(
        blueprint_name: str,
        variable_name: str,
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """Remove a member variable from a Widget Blueprint."""
        for val, name in [(blueprint_name, "blueprint_name"), (variable_name, "variable_name")]:
            if error := validate_name(val, name):
                return create_error_response(error)
        return get_unreal_client().execute_command("delete_widget_variable", {
            "blueprint_name": blueprint_name, "variable_name": variable_name,
            "blueprint_path": blueprint_path
        })

    @mcp.tool()
    def get_widget_variables(
        blueprint_name: str,
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """List all member variables in a Widget Blueprint. Returns name, type, category for each variable."""
        if error := validate_name(blueprint_name, "blueprint_name"):
            return create_error_response(error)
        return get_unreal_client().execute_command("get_widget_variables", {
            "blueprint_name": blueprint_name, "blueprint_path": blueprint_path
        })

    # =========================================================================
    # Tier 9: Node Connection/Deletion
    # =========================================================================

    @mcp.tool()
    def connect_widget_nodes(
        blueprint_name: str,
        source_node_id: str,
        target_node_id: str,
        connect_exec: bool = True,
        connect_data: bool = False,
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """Connect two nodes in a Widget Blueprint event graph.

        Args:
            blueprint_name: Name of the Widget Blueprint
            source_node_id: GUID of the source node
            target_node_id: GUID of the target node
            connect_exec: Connect execution pins (default True)
            connect_data: Connect data pins (default False)
        """
        for val, name in [(blueprint_name, "blueprint_name"), (source_node_id, "source_node_id"), (target_node_id, "target_node_id")]:
            if error := validate_name(val, name):
                return create_error_response(error)
        return get_unreal_client().execute_command("connect_widget_nodes", {
            "blueprint_name": blueprint_name, "source_node_id": source_node_id,
            "target_node_id": target_node_id, "connect_exec": connect_exec,
            "connect_data": connect_data, "blueprint_path": blueprint_path
        })

    @mcp.tool()
    def disconnect_widget_nodes(
        blueprint_name: str,
        node_id: str,
        pin_name: str,
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """Disconnect all links from a specific pin on a node in a Widget Blueprint.

        Args:
            blueprint_name: Name of the Widget Blueprint
            node_id: GUID of the node
            pin_name: Name of the pin to disconnect
        """
        for val, name in [(blueprint_name, "blueprint_name"), (node_id, "node_id"), (pin_name, "pin_name")]:
            if error := validate_name(val, name):
                return create_error_response(error)
        return get_unreal_client().execute_command("disconnect_widget_nodes", {
            "blueprint_name": blueprint_name, "node_id": node_id,
            "pin_name": pin_name, "blueprint_path": blueprint_path
        })

    @mcp.tool()
    def delete_widget_node(
        blueprint_name: str,
        node_id: str,
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """Delete a node from a Widget Blueprint event graph.

        Args:
            blueprint_name: Name of the Widget Blueprint
            node_id: GUID of the node to delete
        """
        for val, name in [(blueprint_name, "blueprint_name"), (node_id, "node_id")]:
            if error := validate_name(val, name):
                return create_error_response(error)
        return get_unreal_client().execute_command("delete_widget_node", {
            "blueprint_name": blueprint_name, "node_id": node_id,
            "blueprint_path": blueprint_path
        })

    # =========================================================================
    # Tier 10: Flow Control & Custom Events
    # =========================================================================

    @mcp.tool()
    def add_widget_flow_control(
        blueprint_name: str,
        control_type: str,
        node_position: Optional[List[float]] = None,
        graph_name: str = "",
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """Add a flow control node to a Widget Blueprint event graph.

        Args:
            blueprint_name: Name of the Widget Blueprint
            control_type: Type of flow control - branch, sequence, forloop, foreachloop, whileloop, doonce, multigate, flipflop, gate
            node_position: [X, Y] position in graph (optional)
            graph_name: Target graph name (default: EventGraph)
        """
        for val, name in [(blueprint_name, "blueprint_name"), (control_type, "control_type")]:
            if error := validate_name(val, name):
                return create_error_response(error)
        params = {"blueprint_name": blueprint_name, "control_type": control_type, "blueprint_path": blueprint_path}
        if node_position: params["node_position"] = node_position
        if graph_name: params["graph_name"] = graph_name
        return get_unreal_client().execute_command("add_widget_flow_control", params)

    @mcp.tool()
    def add_widget_custom_event(
        blueprint_name: str,
        event_name: str,
        action: str = "define",
        node_position: Optional[List[float]] = None,
        graph_name: str = "",
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """Add a custom event to a Widget Blueprint.

        Args:
            blueprint_name: Name of the Widget Blueprint
            event_name: Name for the custom event
            action: 'define' to create event definition, 'call' to create call node
            node_position: [X, Y] position in graph (optional)
            graph_name: Target graph name (default: EventGraph)
        """
        for val, name in [(blueprint_name, "blueprint_name"), (event_name, "event_name")]:
            if error := validate_name(val, name):
                return create_error_response(error)
        params = {"blueprint_name": blueprint_name, "event_name": event_name,
                  "action": action, "blueprint_path": blueprint_path}
        if node_position: params["node_position"] = node_position
        if graph_name: params["graph_name"] = graph_name
        return get_unreal_client().execute_command("add_widget_custom_event", params)

    @mcp.tool()
    def add_widget_generic_node(
        blueprint_name: str,
        node_class: str,
        node_position: Optional[List[float]] = None,
        graph_name: str = "EventGraph",
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """Add an arbitrary node by class name to a Widget Blueprint event graph.

        Args:
            blueprint_name: Name of the Widget Blueprint
            node_class: UEdGraphNode class name (e.g. K2Node_MakeStruct, K2Node_DynamicCast)
            node_position: [X, Y] position in graph (optional)
            graph_name: Target graph name (default: EventGraph)
        """
        for val, name in [(blueprint_name, "blueprint_name"), (node_class, "node_class")]:
            if error := validate_name(val, name):
                return create_error_response(error)
        params = {"blueprint_name": blueprint_name, "node_class": node_class,
                  "graph_name": graph_name, "blueprint_path": blueprint_path}
        if node_position: params["node_position"] = node_position
        return get_unreal_client().execute_command("add_widget_generic_node", params)

    # =========================================================================
    # Tier 11: Pin Value Management
    # =========================================================================

    @mcp.tool()
    def set_widget_pin_default(
        blueprint_name: str,
        node_id: str,
        pin_name: str,
        value: Any = None,
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """Set the default value of a pin on a node in a Widget Blueprint.

        Args:
            blueprint_name: Name of the Widget Blueprint
            node_id: GUID of the node
            pin_name: Name of the pin
            value: Value to set (string, number, bool, or array for vectors [X,Y,Z])
        """
        for val, name in [(blueprint_name, "blueprint_name"), (node_id, "node_id"), (pin_name, "pin_name")]:
            if error := validate_name(val, name):
                return create_error_response(error)
        params = {"blueprint_name": blueprint_name, "node_id": node_id,
                  "pin_name": pin_name, "blueprint_path": blueprint_path}
        if value is not None: params["value"] = value
        return get_unreal_client().execute_command("set_widget_pin_default", params)

    @mcp.tool()
    def get_widget_pin_value(
        blueprint_name: str,
        node_id: str,
        pin_name: str,
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """Get the default value and type of a pin on a node in a Widget Blueprint.

        Args:
            blueprint_name: Name of the Widget Blueprint
            node_id: GUID of the node
            pin_name: Name of the pin
        """
        for val, name in [(blueprint_name, "blueprint_name"), (node_id, "node_id"), (pin_name, "pin_name")]:
            if error := validate_name(val, name):
                return create_error_response(error)
        return get_unreal_client().execute_command("get_widget_pin_value", {
            "blueprint_name": blueprint_name, "node_id": node_id,
            "pin_name": pin_name, "blueprint_path": blueprint_path
        })

    # =========================================================================
    # Tier 12: Graph Introspection
    # =========================================================================

    @mcp.tool()
    def list_widget_graph_nodes(
        blueprint_name: str,
        graph_name: str = "",
        node_type: str = "",
        node_title: str = "",
        limit: int = 50,
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """List nodes in a Widget Blueprint graph with optional filtering.

        Args:
            blueprint_name: Name of the Widget Blueprint
            graph_name: Target graph (default: EventGraph)
            node_type: Filter by type - Event, Function, Variable, FlowControl
            node_title: Filter by title (substring match)
            limit: Max nodes to return (default 50)
        """
        if error := validate_name(blueprint_name, "blueprint_name"):
            return create_error_response(error)
        params = {"blueprint_name": blueprint_name, "blueprint_path": blueprint_path, "limit": limit}
        if graph_name: params["graph_name"] = graph_name
        if node_type: params["node_type"] = node_type
        if node_title: params["node_title"] = node_title
        return get_unreal_client().execute_command("list_widget_graph_nodes", params)

    @mcp.tool()
    def list_widget_graphs(
        blueprint_name: str,
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """List all graphs in a Widget Blueprint (EventGraph, functions, etc.)."""
        if error := validate_name(blueprint_name, "blueprint_name"):
            return create_error_response(error)
        return get_unreal_client().execute_command("list_widget_graphs", {
            "blueprint_name": blueprint_name, "blueprint_path": blueprint_path
        })

    @mcp.tool()
    def compile_widget_blueprint(
        blueprint_name: str,
        validate_only: bool = False,
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """Compile a Widget Blueprint. Validates graphs and reports errors.

        Args:
            blueprint_name: Name of the Widget Blueprint
            validate_only: If True, only validate without compiling
        """
        if error := validate_name(blueprint_name, "blueprint_name"):
            return create_error_response(error)
        return get_unreal_client().execute_command("compile_widget_blueprint", {
            "blueprint_name": blueprint_name, "validate_only": validate_only,
            "blueprint_path": blueprint_path
        })

    # =========================================================================
    # Tier 13: Auxiliary
    # =========================================================================

    @mcp.tool()
    def add_widget_comment_box(
        blueprint_name: str,
        comment_text: str,
        position: Optional[List[float]] = None,
        size: Optional[List[float]] = None,
        graph_name: str = "",
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """Add a comment box to a Widget Blueprint event graph for documentation.

        Args:
            blueprint_name: Name of the Widget Blueprint
            comment_text: Text for the comment box
            position: [X, Y] position in graph (optional)
            size: [Width, Height] of comment box (default [400, 200])
            graph_name: Target graph (default: EventGraph)
        """
        for val, name in [(blueprint_name, "blueprint_name"), (comment_text, "comment_text")]:
            if error := validate_name(val, name):
                return create_error_response(error)
        params = {"blueprint_name": blueprint_name, "comment_text": comment_text, "blueprint_path": blueprint_path}
        if position: params["position"] = position
        if size: params["size"] = size
        if graph_name: params["graph_name"] = graph_name
        return get_unreal_client().execute_command("add_widget_comment_box", params)

    @mcp.tool()
    def add_widget_function_override(
        blueprint_name: str,
        function_name: str,
        blueprint_path: str = "/Game/UI/"
    ) -> Dict[str, Any]:
        """Override a parent function in a Widget Blueprint (e.g. NativeConstruct, NativeTick, NativeDestruct).

        Args:
            blueprint_name: Name of the Widget Blueprint
            function_name: Function to override - NativeConstruct, NativeTick, NativeDestruct, NativeOnInitialized
        """
        for val, name in [(blueprint_name, "blueprint_name"), (function_name, "function_name")]:
            if error := validate_name(val, name):
                return create_error_response(error)
        return get_unreal_client().execute_command("add_widget_function_override", {
            "blueprint_name": blueprint_name, "function_name": function_name,
            "blueprint_path": blueprint_path
        })

    log_info("Widget tools registered successfully")
