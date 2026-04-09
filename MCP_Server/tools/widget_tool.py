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

    log_info("Widget tools registered successfully")
