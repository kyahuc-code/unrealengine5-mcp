"""
Validators - Parameter validation utilities for MCP tools.
"""
from typing import List, Dict, Any, Optional

# Constraints
MAX_NAME_LENGTH = 256
MAX_PATH_LENGTH = 512
MAX_LIMIT = 10000
MAX_CODE_LENGTH = 50000


def validate_name(value: str, name: str) -> Optional[str]:
    """
    Validate a name/identifier parameter (actor name, blueprint name, etc.).
    Returns error message if invalid, None if valid.
    """
    if value is None:
        return f"{name} cannot be None"
    if not isinstance(value, str):
        return f"{name} must be a string, got {type(value).__name__}"
    if not value.strip():
        return f"{name} cannot be empty"
    if len(value) > MAX_NAME_LENGTH:
        return f"{name} exceeds maximum length of {MAX_NAME_LENGTH} characters"
    return None


def validate_path(value: str, name: str) -> Optional[str]:
    """
    Validate an Unreal asset path parameter.
    Returns error message if invalid, None if valid.
    """
    if value is None:
        return f"{name} cannot be None"
    if not isinstance(value, str):
        return f"{name} must be a string, got {type(value).__name__}"
    if not value.strip():
        return f"{name} cannot be empty"
    if len(value) > MAX_PATH_LENGTH:
        return f"{name} exceeds maximum length of {MAX_PATH_LENGTH} characters"
    if not value.startswith("/"):
        return f"{name} must start with '/' (Unreal content path), got '{value}'"
    return None


def validate_limit(value: int, name: str = "limit", min_val: int = 1, max_val: int = MAX_LIMIT) -> Optional[str]:
    """
    Validate a limit/count parameter.
    Returns error message if invalid, None if valid.
    """
    if not isinstance(value, int):
        return f"{name} must be an integer, got {type(value).__name__}"
    if value < min_val:
        return f"{name} must be at least {min_val}, got {value}"
    if value > max_val:
        return f"{name} cannot exceed {max_val}, got {value}"
    return None


def validate_positive_float(value: float, name: str) -> Optional[str]:
    """
    Validate a positive float parameter (radius, mass, etc.).
    Returns error message if invalid, None if valid.
    """
    try:
        fv = float(value)
    except (TypeError, ValueError):
        return f"{name} must be a number, got {type(value).__name__}"
    if fv <= 0.0:
        return f"{name} must be positive, got {fv}"
    return None


def validate_non_negative_int(value: int, name: str) -> Optional[str]:
    """
    Validate a non-negative integer parameter (material_slot, etc.).
    Returns error message if invalid, None if valid.
    """
    if not isinstance(value, int):
        return f"{name} must be an integer, got {type(value).__name__}"
    if value < 0:
        return f"{name} cannot be negative, got {value}"
    return None


def validate_vector2(value: List[float], name: str) -> Optional[str]:
    """
    Validate a 2D vector parameter.
    Returns error message if invalid, None if valid.
    """
    if value is None:
        return None

    if not isinstance(value, (list, tuple)):
        return f"Invalid {name}: must be a list, got {type(value).__name__}"

    if len(value) != 2:
        return f"Invalid {name}: must have 2 elements, got {len(value)}"

    try:
        for v in value:
            float(v)
    except (TypeError, ValueError) as e:
        return f"Invalid {name}: all elements must be numbers - {e}"

    return None


def validate_vector3(value: List[float], name: str) -> Optional[str]:
    """
    Validate a 3D vector parameter.
    Returns error message if invalid, None if valid.
    """
    if value is None:
        return None

    if not isinstance(value, (list, tuple)):
        return f"Invalid {name}: must be a list, got {type(value).__name__}"

    if len(value) != 3:
        return f"Invalid {name}: must have 3 elements, got {len(value)}"

    try:
        for v in value:
            float(v)
    except (TypeError, ValueError) as e:
        return f"Invalid {name}: all elements must be numbers - {e}"

    return None


def validate_color(value: List[float], name: str = "color") -> Optional[str]:
    """
    Validate a color parameter (RGB, 0.0-1.0).
    Returns error message if invalid, None if valid.
    """
    if value is None:
        return None

    if not isinstance(value, (list, tuple)):
        return f"Invalid {name}: must be a list, got {type(value).__name__}"

    if len(value) != 3:
        return f"Invalid {name}: must have 3 elements (RGB), got {len(value)}"

    try:
        for i, v in enumerate(value):
            fv = float(v)
            if fv < 0.0 or fv > 1.0:
                return f"Invalid {name}: element {i} must be between 0.0 and 1.0, got {fv}"
    except (TypeError, ValueError) as e:
        return f"Invalid {name}: all elements must be numbers - {e}"

    return None


def ensure_floats(value: Optional[List[float]]) -> Optional[List[float]]:
    """Convert all elements to float. Returns None if input is None."""
    if value is None:
        return None
    return [float(v) for v in value]


def create_error_response(error: str) -> Dict[str, Any]:
    """Create a standardized error response."""
    return {"status": "error", "error": error}


def validate_vectors(
    func_name: str,
    log_error_func,
    vectors: List[tuple],
    optional: bool = False
) -> Optional[Dict[str, Any]]:
    """
    Validate multiple vectors and return error response if any fails.

    Args:
        func_name: Name of the calling function (for logging)
        log_error_func: Logger function to call on error
        vectors: List of (value, name, validator_func) tuples
        optional: If True, skip validation when value is None/falsy

    Returns:
        Error response dict if validation fails, None if all valid
    """
    for value, name, validator_func in vectors:
        if optional and not value:
            continue
        if error := validator_func(value, name):
            log_error_func(f"{func_name} validation failed: {error}")
            return create_error_response(error)
    return None
