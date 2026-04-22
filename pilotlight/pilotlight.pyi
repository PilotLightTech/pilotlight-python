from typing import List, dict
from enum import IntEnum, IntFlag
import pilotlight.pilotlight as pl

########################################################################################################################
# [SECTION] core api
########################################################################################################################

def pl_run(app):
    ...

def pl_get_pointer_value(pointer):
    ...

def pl_set_pointer_value(pointer, value):
    ...

def pl_create_bool_pointer():
    ...

def pl_destroy_bool_pointer(pointer):
    ...

def pl_create_int_pointer():
    ...

def pl_destroy_int_pointer(pointer):
    ...

def pl_create_float_pointer():
    ...

def pl_destroy_float_pointer(pointer):
    ...

########################################################################################################################
# [SECTION] io api
########################################################################################################################

class plKey(IntEnum):
    ...

class plMouseCursor(IntEnum):
    ...

class plMouseButton(IntEnum):
    ...

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

def pl_window_create(pcTitle, iXPos, iYPos, uWidth, uHeight, tFlags):
    ...

def pl_window_show(window):
    ...

def pl_window_destroy(window):
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

def pl_pack_begin_packing(file, content_version):
    ...

def pl_pack_add_from_disk(pak, pcPakPath, pcFilePath, bCompress):
    ...

def pl_pack_end_packing(pak):
    ...

########################################################################################################################
# [SECTION] graphics api
########################################################################################################################

def pl_graphics_flush_device(device, **kwargs) -> None:
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

def pl_draw_add_triangle_filled(layer, p0, p1, p2, *, color=PL_COLOR_32_WHITE, **kwargs):
    ...

def pl_draw_add_triangle(layer, p0, p1, p2, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
    ...

def pl_draw_add_line(layer, p0, p1, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
    ...

def pl_draw_add_rect(layer, pMin, pMax, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
    ...

def pl_draw_add_rect_rounded(layer, pMin, pMax, radius, segments, flags, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
    ...

def pl_draw_add_quad(layer, p0, p1, p2, p3, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
    ...

def pl_draw_add_circle(layer, p, radius, segments, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
    ...

def pl_draw_add_polygon(layer, points, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
    ...

def pl_draw_add_bezier_quad(layer, p0, p1, p2, segments, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
    ...

def pl_draw_add_bezier_cubic(layer, p0, p1, p2, p3, segments, *, color=PL_COLOR_32_WHITE, thickness=1.0, **kwargs):
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

def pl_ecs_get_component(library, key, entity):
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

def pl_camera_create_perspective(library, name, pos, yFov, aspect, nearZ, farZ, reverseZ):
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

class plStarterFlag(IntFlag):
    ...

def pl_starter_initialize(window, flags: plStarterFlag):
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

def pl_starter_get_foreground_layer(**kwargs) -> None:
    ...

def pl_starter_get_background_layer(**kwargs) -> None:
    ...

def pl_starter_get_device(**kwargs) -> None:
    ...

def pl_starter_get_swapchain(**kwargs) -> None:
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

def pl_renderer_initialize(device, swapchain, **kwargs):
    ...

def pl_renderer_cleanup():
    ...

def pl_renderer_register_ecs_system():
    ...

def pl_renderer_create_directional_light(library, name):
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