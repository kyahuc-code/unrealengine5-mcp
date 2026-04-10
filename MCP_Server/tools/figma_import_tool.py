"""
Figma Import Tool - Convert Figma design JSON to Unreal Widget Blueprints.

Transforms Figma API output (/v1/files/{key}/nodes) into UMG widget hierarchy
using build_widget_tree for atomic creation.
"""
from typing import Dict, Any, Optional, List
from mcp.server.fastmcp import FastMCP
from ..unreal_client import get_unreal_client
from ..utils import (
    log_info, log_error,
    create_error_response,
    validate_name, validate_path
)


# Figma node type → UMG widget type mapping
FIGMA_TYPE_MAP = {
    "FRAME": None,  # Depends on layoutMode (CanvasPanel / VBox / HBox)
    "GROUP": "Overlay",
    "COMPONENT": None,  # Same as FRAME
    "COMPONENT_SET": "CanvasPanel",
    "INSTANCE": None,  # Same as FRAME
    "TEXT": "TextBlock",
    "RECTANGLE": "Border",
    "ELLIPSE": "Image",
    "LINE": "Image",
    "VECTOR": "Image",
    "STAR": "Image",
    "REGULAR_POLYGON": "Image",
    "BOOLEAN_OPERATION": "Image",
    "SECTION": "CanvasPanel",
    "SLICE": "CanvasPanel",
}


def _figma_frame_to_widget_type(node: Dict) -> str:
    """Determine UMG widget type for a Figma FRAME based on layout mode."""
    layout = node.get("layoutMode")
    if layout == "VERTICAL":
        return "VerticalBox"
    elif layout == "HORIZONTAL":
        return "HorizontalBox"
    else:
        return "CanvasPanel"


def _parse_figma_color(fills: List[Dict]) -> Optional[List[float]]:
    """Extract RGBA color from Figma fills array."""
    if not fills:
        return None
    for fill in fills:
        if fill.get("type") == "SOLID" and fill.get("visible", True):
            c = fill.get("color", {})
            opacity = fill.get("opacity", 1.0)
            return [
                round(c.get("r", 1.0), 4),
                round(c.get("g", 1.0), 4),
                round(c.get("b", 1.0), 4),
                round(c.get("a", 1.0) * opacity, 4)
            ]
    return None


def _figma_constraints_to_anchors(constraints: Dict, parent_bbox: Optional[Dict]) -> Dict:
    """Convert Figma constraints to UMG canvas slot anchors."""
    h = constraints.get("horizontal", "LEFT")
    v = constraints.get("vertical", "TOP")

    anchors = {"min_x": 0.0, "min_y": 0.0, "max_x": 0.0, "max_y": 0.0}

    # Horizontal
    if h == "LEFT":
        anchors["min_x"] = 0.0
        anchors["max_x"] = 0.0
    elif h == "RIGHT":
        anchors["min_x"] = 1.0
        anchors["max_x"] = 1.0
    elif h == "CENTER":
        anchors["min_x"] = 0.5
        anchors["max_x"] = 0.5
    elif h in ("LEFT_RIGHT", "SCALE"):
        anchors["min_x"] = 0.0
        anchors["max_x"] = 1.0

    # Vertical
    if v == "TOP":
        anchors["min_y"] = 0.0
        anchors["max_y"] = 0.0
    elif v == "BOTTOM":
        anchors["min_y"] = 1.0
        anchors["max_y"] = 1.0
    elif v == "CENTER":
        anchors["min_y"] = 0.5
        anchors["max_y"] = 0.5
    elif v in ("TOP_BOTTOM", "SCALE"):
        anchors["min_y"] = 0.0
        anchors["max_y"] = 1.0

    return anchors


def _figma_align_to_umg(align: str, axis: str) -> str:
    """Convert Figma alignment to UMG alignment string."""
    if axis == "h":
        mapping = {"MIN": "Left", "CENTER": "Center", "MAX": "Right"}
    else:
        mapping = {"MIN": "Top", "CENTER": "Center", "MAX": "Bottom"}
    return mapping.get(align, "Fill")


def _sanitize_name(name: str) -> str:
    """Sanitize a Figma layer name to a valid UE widget name."""
    # Replace spaces and special chars with underscores
    sanitized = ""
    for c in name:
        if c.isalnum() or c == '_':
            sanitized += c
        else:
            sanitized += '_'
    # Remove leading digits
    if sanitized and sanitized[0].isdigit():
        sanitized = '_' + sanitized
    # Ensure non-empty
    if not sanitized:
        sanitized = "Widget"
    return sanitized


def _convert_figma_node(
    node: Dict,
    parent_type: str,
    parent_bbox: Optional[Dict],
    name_counter: Dict[str, int],
    scale_factor: float,
    mapping_overrides: Dict[str, str],
    depth: int = 0,
    max_depth: int = 50
) -> Optional[Dict[str, Any]]:
    """Recursively convert a Figma node to a build_widget_tree widget spec."""
    if depth > max_depth:
        return None

    node_type = node.get("type", "")
    node_name = node.get("name", "Unknown")

    # Skip invisible nodes
    if not node.get("visible", True):
        return None

    # Determine widget type
    if node_name in mapping_overrides:
        widget_type = mapping_overrides[node_name]
    elif node_type in ("FRAME", "COMPONENT", "INSTANCE"):
        widget_type = _figma_frame_to_widget_type(node)
    elif node_type in FIGMA_TYPE_MAP:
        widget_type = FIGMA_TYPE_MAP[node_type]
    else:
        return None  # Skip unsupported types

    if widget_type is None:
        return None

    # Generate unique name
    base_name = _sanitize_name(node_name)
    if base_name in name_counter:
        name_counter[base_name] += 1
        widget_name = f"{base_name}_{name_counter[base_name]}"
    else:
        name_counter[base_name] = 0
        widget_name = base_name

    spec: Dict[str, Any] = {
        "name": widget_name,
        "type": widget_type,
    }

    # Bounding box
    bbox = node.get("absoluteBoundingBox", {})
    rel_x = bbox.get("x", 0)
    rel_y = bbox.get("y", 0)
    width = bbox.get("width", 100)
    height = bbox.get("height", 100)

    # Adjust position relative to parent
    if parent_bbox:
        rel_x -= parent_bbox.get("x", 0)
        rel_y -= parent_bbox.get("y", 0)

    # Apply scale factor
    rel_x *= scale_factor
    rel_y *= scale_factor
    width *= scale_factor
    height *= scale_factor

    # Build slot based on parent type
    if parent_type == "CanvasPanel":
        slot: Dict[str, Any] = {
            "position": [round(rel_x, 1), round(rel_y, 1)],
            "size": [round(width, 1), round(height, 1)]
        }
        # Constraints → Anchors
        constraints = node.get("constraints")
        if constraints:
            slot["anchors"] = _figma_constraints_to_anchors(constraints, parent_bbox)
        spec["slot"] = slot

    elif parent_type in ("VerticalBox", "HorizontalBox"):
        slot = {}
        # Layout alignment
        layout_align = node.get("layoutAlign", "INHERIT")
        if layout_align == "STRETCH":
            if parent_type == "VerticalBox":
                slot["h_align"] = "Fill"
            else:
                slot["v_align"] = "Fill"
        elif layout_align == "MIN":
            slot["h_align" if parent_type == "VerticalBox" else "v_align"] = "Left" if parent_type == "VerticalBox" else "Top"
        elif layout_align == "CENTER":
            slot["h_align" if parent_type == "VerticalBox" else "v_align"] = "Center"
        elif layout_align == "MAX":
            slot["h_align" if parent_type == "VerticalBox" else "v_align"] = "Right" if parent_type == "VerticalBox" else "Bottom"

        # Fill/Auto sizing
        layout_grow = node.get("layoutGrow", 0)
        if layout_grow > 0:
            slot["size_rule"] = "Fill"
            slot["fill_weight"] = layout_grow
        else:
            slot["size_rule"] = "Auto"

        if slot:
            spec["slot"] = slot

    elif parent_type == "Overlay":
        slot = {}
        counter_align = node.get("counterAxisAlignSelf", "INHERIT")
        if counter_align != "INHERIT":
            slot["h_align"] = _figma_align_to_umg(counter_align, "h")
        if slot:
            spec["slot"] = slot

    # Opacity
    opacity = node.get("opacity")
    if opacity is not None and opacity < 1.0:
        spec["render_opacity"] = round(opacity, 4)

    # Color
    fills = node.get("fills", [])
    color = _parse_figma_color(fills)
    if color and widget_type in ("Border", "Image"):
        spec["color"] = color

    # Text content
    if node_type == "TEXT":
        characters = node.get("characters", "")
        if characters:
            spec["text"] = characters
        # Font
        style = node.get("style", {})
        font_size = style.get("fontSize")
        if font_size:
            spec["font_size"] = int(font_size * scale_factor)

    # Padding (for frames with auto-layout)
    has_padding = any(node.get(k, 0) > 0 for k in ("paddingLeft", "paddingRight", "paddingTop", "paddingBottom"))
    if has_padding and widget_type in ("VerticalBox", "HorizontalBox", "CanvasPanel"):
        # Store padding as properties on the widget's content padding
        pl = node.get("paddingLeft", 0) * scale_factor
        pr = node.get("paddingRight", 0) * scale_factor
        pt = node.get("paddingTop", 0) * scale_factor
        pb = node.get("paddingBottom", 0) * scale_factor
        # For now, encode as ContentPadding if it's a Border, or store in slot
        if widget_type == "CanvasPanel":
            # CanvasPanel doesn't have padding directly, but its children are positioned absolutely
            pass

    # Rotation
    rotation = node.get("rotation")
    if rotation and rotation != 0:
        spec["transform"] = {"angle": round(-rotation, 2)}  # Figma uses CCW, UE uses CW

    # Process children
    children = node.get("children", [])
    if children and widget_type in ("CanvasPanel", "VerticalBox", "HorizontalBox", "Overlay",
                                     "ScrollBox", "SizeBox", "Border", "WidgetSwitcher",
                                     "UniformGridPanel"):
        child_specs = []
        for child in children:
            child_spec = _convert_figma_node(
                child, widget_type, bbox, name_counter,
                scale_factor, mapping_overrides, depth + 1, max_depth
            )
            if child_spec:
                child_specs.append(child_spec)
        if child_specs:
            spec["children"] = child_specs

    return spec


def register_figma_import_tools(mcp: FastMCP):
    """Register Figma import tools with MCP server."""

    @mcp.tool()
    def import_figma_design(
        figma_json: Dict[str, Any],
        blueprint_name: str,
        blueprint_path: str = "/Game/UI/",
        create_blueprint: bool = True,
        root_frame_name: str = "",
        scale_factor: float = 1.0,
        mapping_overrides: Optional[Dict[str, str]] = None
    ) -> Dict[str, Any]:
        """Import a Figma design as an Unreal Widget Blueprint.

        Converts Figma JSON (from Figma REST API /v1/files/{key}/nodes) into
        a UMG Widget Blueprint hierarchy using build_widget_tree.

        Figma to UMG type mapping:
          FRAME (vertical auto-layout) -> VerticalBox
          FRAME (horizontal auto-layout) -> HorizontalBox
          FRAME (absolute/no layout) -> CanvasPanel
          TEXT -> TextBlock
          RECTANGLE -> Border
          ELLIPSE/VECTOR/LINE -> Image
          GROUP -> Overlay
          COMPONENT/INSTANCE -> follows FRAME rules

        Figma properties mapped:
          absoluteBoundingBox -> position/size (canvas slot)
          constraints -> anchors
          layoutMode/layoutAlign/layoutGrow -> box slot properties
          fills[].color -> widget color
          characters -> text content
          style.fontSize -> font size
          opacity -> render_opacity
          rotation -> transform.angle
          visible: false -> skipped

        Args:
            figma_json: Figma API response JSON (must contain "document" or "children")
            blueprint_name: Name for the created Widget Blueprint
            blueprint_path: UE content path (default /Game/UI/)
            create_blueprint: If True, creates a new WBP first (default True)
            root_frame_name: Name of specific frame to import (empty = first frame)
            scale_factor: Scale multiplier for all sizes (e.g., 0.5 for half size)
            mapping_overrides: Custom node name -> widget type overrides
        """
        if error := validate_name(blueprint_name, "blueprint_name"):
            return create_error_response(error)
        if error := validate_path(blueprint_path, "blueprint_path"):
            return create_error_response(error)
        if not figma_json:
            return create_error_response("figma_json cannot be empty")
        if scale_factor <= 0:
            return create_error_response("scale_factor must be positive")

        overrides = mapping_overrides or {}
        client = get_unreal_client()

        # Find the root frame to import
        root_node = None

        # Handle different Figma JSON formats
        if "document" in figma_json:
            # Full file response
            doc = figma_json["document"]
            pages = doc.get("children", [])
            for page in pages:
                frames = page.get("children", [])
                for frame in frames:
                    if root_frame_name and frame.get("name") != root_frame_name:
                        continue
                    root_node = frame
                    break
                if root_node:
                    break
        elif "nodes" in figma_json:
            # Node-specific response from /v1/files/{key}/nodes
            for node_id, node_data in figma_json["nodes"].items():
                doc = node_data.get("document", {})
                if root_frame_name and doc.get("name") != root_frame_name:
                    continue
                root_node = doc
                break
        elif "children" in figma_json:
            # Direct node with children
            root_node = figma_json
        elif "type" in figma_json:
            # Single node
            root_node = figma_json

        if not root_node:
            return create_error_response(
                f"No frame found in Figma JSON" +
                (f" matching name '{root_frame_name}'" if root_frame_name else "") +
                ". Expected 'document.children[].children[]' structure."
            )

        # Create the Widget Blueprint if requested
        if create_blueprint:
            result = client.execute_command("create_widget_blueprint", {
                "name": blueprint_name,
                "path": blueprint_path
            })
            if not result.get("success"):
                return result

        # Convert Figma tree to widget specs
        name_counter: Dict[str, int] = {}
        widget_specs = []

        # If root is a frame, convert its children as top-level widgets
        root_type = root_node.get("type", "FRAME")
        root_widget_type = _figma_frame_to_widget_type(root_node) if root_type in ("FRAME", "COMPONENT", "INSTANCE") else "CanvasPanel"

        # Build the root container matching the Figma layout
        root_children = root_node.get("children", [])
        root_bbox = root_node.get("absoluteBoundingBox")

        if root_widget_type == "CanvasPanel":
            # Figma's absolute layout maps directly to CanvasPanel
            # The default root is already a CanvasPanel, so add children directly
            for child in root_children:
                child_spec = _convert_figma_node(
                    child, "CanvasPanel", root_bbox, name_counter,
                    scale_factor, overrides
                )
                if child_spec:
                    widget_specs.append(child_spec)
        else:
            # Auto-layout frame: create a container widget under root CanvasPanel
            container_spec: Dict[str, Any] = {
                "name": _sanitize_name(root_node.get("name", "Root")),
                "type": root_widget_type,
                "slot": {
                    "anchors": {"min_x": 0, "min_y": 0, "max_x": 1, "max_y": 1},
                    "alignment": [0, 0],
                    "position": [0, 0],
                    "size": [
                        root_bbox.get("width", 1920) * scale_factor if root_bbox else 1920,
                        root_bbox.get("height", 1080) * scale_factor if root_bbox else 1080
                    ]
                },
                "children": []
            }

            for child in root_children:
                child_spec = _convert_figma_node(
                    child, root_widget_type, root_bbox, name_counter,
                    scale_factor, overrides
                )
                if child_spec:
                    container_spec["children"].append(child_spec)

            widget_specs.append(container_spec)

        if not widget_specs:
            return create_error_response("No importable widgets found in Figma design")

        # Build the widget tree atomically
        result = client.execute_command("build_widget_tree", {
            "blueprint_name": blueprint_name,
            "widgets": widget_specs,
            "blueprint_path": blueprint_path
        })

        # Enrich result with import stats
        if result.get("success"):
            result["figma_source"] = root_node.get("name", "Unknown")
            result["figma_type"] = root_type
            result["umg_root_type"] = root_widget_type
            result["scale_factor"] = scale_factor

        log_info(f"Figma import: {result.get('widgets_created', 0)} widgets created for '{blueprint_name}'")
        return result

    @mcp.tool()
    def analyze_figma_json(
        figma_json: Dict[str, Any],
        root_frame_name: str = ""
    ) -> Dict[str, Any]:
        """Analyze Figma JSON structure and show how it would map to UMG widgets.

        Use this before import_figma_design to preview the conversion.
        Returns: frame list, widget type mapping preview, node count per type.
        """
        if not figma_json:
            return create_error_response("figma_json cannot be empty")

        frames = []
        type_counts: Dict[str, int] = {}
        total_nodes = 0

        def _count_nodes(node: Dict, depth: int = 0):
            nonlocal total_nodes
            total_nodes += 1
            nt = node.get("type", "UNKNOWN")
            type_counts[nt] = type_counts.get(nt, 0) + 1
            for child in node.get("children", []):
                _count_nodes(child, depth + 1)

        # Find frames
        if "document" in figma_json:
            pages = figma_json["document"].get("children", [])
            for page in pages:
                for frame in page.get("children", []):
                    bbox = frame.get("absoluteBoundingBox", {})
                    frame_info = {
                        "name": frame.get("name", "Unknown"),
                        "type": frame.get("type", "FRAME"),
                        "layout_mode": frame.get("layoutMode", "NONE"),
                        "width": bbox.get("width", 0),
                        "height": bbox.get("height", 0),
                        "child_count": len(frame.get("children", [])),
                        "umg_type": _figma_frame_to_widget_type(frame)
                    }
                    _count_nodes(frame)
                    frames.append(frame_info)
        elif "nodes" in figma_json:
            for node_id, node_data in figma_json["nodes"].items():
                doc = node_data.get("document", {})
                bbox = doc.get("absoluteBoundingBox", {})
                frame_info = {
                    "name": doc.get("name", "Unknown"),
                    "type": doc.get("type", "FRAME"),
                    "node_id": node_id,
                    "layout_mode": doc.get("layoutMode", "NONE"),
                    "width": bbox.get("width", 0),
                    "height": bbox.get("height", 0),
                    "child_count": len(doc.get("children", [])),
                    "umg_type": _figma_frame_to_widget_type(doc)
                }
                _count_nodes(doc)
                frames.append(frame_info)
        elif "children" in figma_json or "type" in figma_json:
            _count_nodes(figma_json)
            bbox = figma_json.get("absoluteBoundingBox", {})
            frames.append({
                "name": figma_json.get("name", "Root"),
                "type": figma_json.get("type", "FRAME"),
                "layout_mode": figma_json.get("layoutMode", "NONE"),
                "width": bbox.get("width", 0),
                "height": bbox.get("height", 0),
                "child_count": len(figma_json.get("children", []))
            })

        # Build type mapping preview
        type_mapping = {}
        for figma_type, count in sorted(type_counts.items()):
            umg = FIGMA_TYPE_MAP.get(figma_type, "Unknown")
            if umg is None:
                umg = "VerticalBox/HorizontalBox/CanvasPanel (depends on layout)"
            type_mapping[figma_type] = {"count": count, "umg_type": umg}

        return {
            "status": "success",
            "frames": frames,
            "total_nodes": total_nodes,
            "type_mapping": type_mapping,
            "supported_types": list(FIGMA_TYPE_MAP.keys())
        }

    log_info("Figma import tools registered successfully")
