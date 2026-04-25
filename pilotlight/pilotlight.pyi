from typing import List, dict, overload, NewType
from enum import IntEnum, IntFlag
import pilotlight.pilotlight as pl
from pilotlight.types import *
from pilotlight.enums import *

########################################################################################################################
# [SECTION] custom types
########################################################################################################################

class plVec2:
    x: float
    y: float

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, x: float, y: float) -> None: ...

    def __repr__(self) -> str: ...

    def __add__(self, other: plVec2) -> plVec2: ...
    def __sub__(self, other: plVec2) -> plVec2: ...

    def __mul__(self, scalar: float) -> plVec2: ...
    def __rmul__(self, scalar: float) -> plVec2: ...

    def __neg__(self) -> plVec2: ...

type plVec2Like = plVec2 | tuple[float, float] | List[float, float]

########################################################################################################################
# [SECTION] opaque types
########################################################################################################################

plDrawLayer2D = NewType("plDrawLayer2D", object)
plWindow = NewType("plWindow", object)
plDevice = NewType("plDevice", object)
plSwapchain = NewType("plSwapchain", object)
plComponentLibrary = NewType("plComponentLibrary", object)
plRenderEncoder = NewType("plRenderEncoder", object)
plPakFile = NewType("plPakFile", object)
plBoolPointer = NewType("plBoolPointer", object)
plIntPointer = NewType("plIntPointer", object)
plFloatPointer = NewType("plFloatPointer", object)
plDoublePointer = NewType("plDoublePointer", object)

type plPointer = plBoolPointer | plIntPointer | plFloatPointer | plDoublePointer

########################################################################################################################
# [SECTION] core api
########################################################################################################################

def pl_run(app):
    ...

def pl_get_pointer_value(pointer: plPointer, index: int = 0):
    ...

def pl_set_pointer_value(pointer: plPointer, value, index: int = 0):
    ...

def pl_destroy_pointer(pointer: plPointer):
    ...

def pl_create_bool_pointer() -> plBoolPointer:
    ...

def pl_create_int_pointer(count: int = 1) -> plIntPointer:
    ...

def pl_create_float_pointer(count: int = 1) -> plFloatPointer:
    ...

########################################################################################################################
# [SECTION] io api
########################################################################################################################

def pl_io_get_io() -> dict:
    ...

def pl_io_get_version_string() -> str:
    return...

def pl_io_new_frame() -> None:
    ...

def pl_io_is_key_pressed(key: plKey, repeat: bool = False) -> bool:
    ...

def pl_io_is_key_released(key: plKey) -> bool:
    ...

def pl_io_is_key_down(key: plKey) -> bool:
    ...

def pl_io_get_key_pressed_amount(key: plKey, fRepeatDelay: float, fRate: float) -> int:
    ...

def pl_io_is_mouse_down(button: plMouseButton) -> bool:
    ...

def pl_io_is_mouse_released(button: plMouseButton) -> bool:
    ...

def pl_io_is_mouse_double_clicked(button: plMouseButton) -> bool:
    ...

def pl_io_is_mouse_clicked(button: plMouseButton, repeat: bool = False) -> bool:
    ...

def pl_io_is_mouse_dragging(button: plMouseButton, threshold: float) -> bool:
    ...

def pl_io_is_mouse_hovering_rect(minvec: List[float], maxvec: List[float]) -> bool:
    ...

def pl_io_reset_mouse_drag_delta(button: plMouseButton) -> None:
    ...

def pl_io_get_mouse_drag_delta(button: plMouseButton, threshold: float) -> List[float]:
    ...

def pl_io_get_mouse_pos() -> None:
    ...

def pl_io_get_mouse_wheel() -> float:
    ...

def pl_io_is_mouse_pos_valid(vec: List[float]) -> bool:
    ...

def pl_io_set_mouse_cursor(vec: int) -> None:
    ...

########################################################################################################################
# [SECTION] window api
########################################################################################################################

def pl_window_create(pcTitle: str, iXPos, iYPos, uWidth, uHeight, tFlags) -> plWindow:
    ...

def pl_window_show(window: plWindow):
    ...

def pl_window_destroy(window: plWindow):
    ...

########################################################################################################################
# [SECTION] stats api
########################################################################################################################

def pl_stats_new_frame(name):
    ...

def pl_stats_get_counter(name):
    ...

########################################################################################################################
# [SECTION] vfs api
########################################################################################################################

def pl_vfs_mount_directory(directory, physical_directory, **kwargs) -> None:
    ...

########################################################################################################################
# [SECTION] pack api
########################################################################################################################

def pl_pack_begin_packing(file, content_version) -> tuple[bool, plPakFile]:
    ...

def pl_pack_add_from_disk(pak: plPakFile, pcPakPath, pcFilePath, bCompress) -> bool:
    ...

def pl_pack_end_packing(pak: plPakFile):
    ...

########################################################################################################################
# [SECTION] graphics api
########################################################################################################################

def pl_graphics_flush_device(device: plDevice, **kwargs) -> None:
    ...

########################################################################################################################
# [SECTION] shader api
########################################################################################################################

def pl_shader_initialize(options):
    ...

def pl_shader_cleanup():
    ...

def pl_shader_load_glsl(shader, entry_func, **kwargs):
    ...

def pl_shader_compile_gls(shader, entry_func, **kwargs):
    ...

def pl_shader_write_to_disk(shader, module):
    ...

########################################################################################################################
# [SECTION] draw api
########################################################################################################################

def pl_draw_add_triangle_filled(layer: plDrawLayer2D, p0: plVec2Like, p1: plVec2Like, p2: plVec2Like, options: plDrawSolidOptions):
    ...

def pl_draw_add_triangle(layer: plDrawLayer2D, p0: plVec2Like, p1: plVec2Like, p2: plVec2Like, options: plDrawLineOptions):
    ...

def pl_draw_add_line(layer: plDrawLayer2D, p0: plVec2Like, p1: plVec2Like, options: plDrawLineOptions):
    ...

def pl_draw_add_rect(layer: plDrawLayer2D, pMin: plVec2Like, pMax: plVec2Like, options: plDrawLineOptions):
    ...

def pl_draw_add_rect_rounded(layer: plDrawLayer2D, pMin: plVec2Like, pMax: plVec2Like, radius: float, segments: int, flags: plDrawRectFlag, options: plDrawLineOptions):
    ...

def pl_draw_add_quad(layer: plDrawLayer2D, p0: plVec2Like, p1: plVec2Like, p2: plVec2Like, p3: plVec2Like, options: plDrawLineOptions):
    ...

def pl_draw_add_circle(layer: plDrawLayer2D, p: plVec2Like, radius: float, segments: int, options: plDrawLineOptions):
    ...

def pl_draw_add_polygon(layer: plDrawLayer2D, points, options: plDrawLineOptions):
    ...

def pl_draw_add_bezier_quad(layer: plDrawLayer2D, p0: plVec2Like, p1: plVec2Like, p2: plVec2Like, segments: int, options: plDrawLineOptions):
    ...

def pl_draw_add_bezier_cubic(layer: plDrawLayer2D, p0: plVec2Like, p1: plVec2Like, p2: plVec2Like, p3: plVec2Like, segments: int, options: plDrawLineOptions):
    ...

########################################################################################################################
# [SECTION] screen log api
########################################################################################################################

def pl_screen_log_clear():
    ...

def pl_screen_log_add_message(time_to_display, message):
    ...

########################################################################################################################
# [SECTION] ui api
########################################################################################################################

def pl_ui_begin_window(name : str, **kwargs):
    ...

def pl_ui_end_window(**kwargs) -> None:
    ...

def pl_ui_button(name : str, **kwargs):
    ...

def pl_ui_checkbox(name, value = None, **kwargs):
    ...

def pl_ui_input_text(name, value, **kwargs):
    ...

########################################################################################################################
# [SECTION] ecs api
########################################################################################################################

def pl_ecs_initialize():
    ...

def pl_ecs_finalize():
    ...

def pl_ecs_cleanup():
    ...

def pl_ecs_get_default_library():
    ...

def pl_ecs_get_component(library: plComponentLibrary, key, entity):
    ...

########################################################################################################################
# [SECTION] animation api
########################################################################################################################

def pl_animation_register_ecs_system():
    ...

########################################################################################################################
# [SECTION] camera api
########################################################################################################################

def pl_camera_register_ecs_system():
    ...

def pl_camera_get_ecs_type_key():
    ...

def pl_camera_create_perspective(library: plComponentLibrary, name, pos, yFov, aspect, nearZ, farZ, reverseZ):
    ...

def pl_camera_set_fov(camera, yFov):
    ...

def pl_camera_update(camera):
    ...

########################################################################################################################
# [SECTION] material api
########################################################################################################################

def pl_material_register_ecs_system():
    ...

########################################################################################################################
# [SECTION] mesh api
########################################################################################################################

def pl_mesh_register_ecs_system():
    ...

########################################################################################################################
# [SECTION] physics api
########################################################################################################################

def pl_physics_register_ecs_system():
    ...

########################################################################################################################
# [SECTION] script api
########################################################################################################################

def pl_script_register_ecs_system():
    ...

########################################################################################################################
# [SECTION] shader variant api
########################################################################################################################

def pl_shader_variant_initialize(device):
    ...

########################################################################################################################
# [SECTION] starter api
########################################################################################################################

def pl_starter_initialize(window: plWindow, flags: plStarterFlag):
    ...

def pl_starter_cleanup(**kwargs):
    ...

def pl_starter_begin_frame(**kwargs) -> None:
    ...

def pl_starter_finalize(**kwargs) -> None:
    ...

def pl_starter_resize(**kwargs) -> None:
    ...

def pl_starter_end_frame(**kwargs) -> None:
    ...

def pl_starter_get_foreground_layer(**kwargs) -> plDrawLayer2D:
    ...

def pl_starter_get_background_layer(**kwargs) -> plDrawLayer2D:
    ...

def pl_starter_get_device(**kwargs) -> plDevice:
    ...

def pl_starter_get_swapchain(**kwargs) -> plSwapchain:
    ...

def pl_starter_get_render_pass(**kwargs) -> None:
    ...

def pl_starter_begin_main_pass(**kwargs) -> None:
    ...

def pl_starter_end_main_pass(**kwargs) -> None:
    ...

########################################################################################################################
# [SECTION] renderer api
########################################################################################################################

def pl_renderer_initialize(device: plDevice, swapchain: plSwapchain, **kwargs):
    ...

def pl_renderer_cleanup():
    ...

def pl_renderer_register_ecs_system():
    ...

def pl_renderer_create_directional_light(library: plComponentLibrary, name):
    ...

########################################################################################################################
# [SECTION] constants
########################################################################################################################

PL_COLOR_32_WHITE: int
PL_COLOR_32_BLACK: int
PL_COLOR_32_RED: int
PL_COLOR_32_BLUE: int
PL_COLOR_32_DARK_BLUE: int
PL_COLOR_32_GREEN: int
PL_COLOR_32_YELLOW: int
PL_COLOR_32_ORANGE: int
PL_COLOR_32_MAGENTA: int
PL_COLOR_32_CYAN: int
PL_COLOR_32_GREY: int
PL_COLOR_32_LIGHT_GREY: int