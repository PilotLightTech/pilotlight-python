from typing import List, Dict, Tuple, overload, NewType
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

type plVec2Like = plVec2 | Tuple[float] | List[float]
type plVec2ListLike = Tuple[plVec2Like] | List[plVec2Like]

@dataclass(slots=True)
class plIO:

    # configurable
    bRunning: bool
    fMouseDragThreshold: float 
    fMouseDoubleClickTime: float 
    fMouseDoubleClickMaxDist: float 
    fKeyRepeatDelay: float 
    fKeyRepeatRate: float

    # do not set these
    fFrameRate: float
    dTime: float
    tMainViewportSize: plVec2
    tMainFramebufferScale: plVec2

########################################################################################################################
# [SECTION] core api
########################################################################################################################

def pl_run(app):
    ...

def pl_get_pointer_value(pointer: plPointer | None, index: int = 0):
    ...

def pl_set_pointer_value(pointer: plPointer | None, value, index: int = 0):
    ...

def pl_destroy_pointer(pointer: plPointer | None):
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

class plIOI:

    @staticmethod
    def get_io() -> plIO:
        ...

    @staticmethod
    def get_version_string() -> str:
        ...

    @staticmethod
    def new_frame() -> None:
        ...

    @staticmethod
    def is_key_pressed(key: plKey, repeat: bool = False) -> bool:
        ...

    @staticmethod
    def is_key_released(key: plKey) -> bool:
        ...

    @staticmethod
    def is_key_down(key: plKey) -> bool:
        ...

    @staticmethod
    def get_key_pressed_amount(key: plKey, fRepeatDelay: float, fRate: float) -> int:
        ...

    @staticmethod
    def is_mouse_down(button: plMouseButton) -> bool:
        ...

    @staticmethod
    def is_mouse_released(button: plMouseButton) -> bool:
        ...

    @staticmethod
    def is_mouse_double_clicked(button: plMouseButton) -> bool:
        ...

    @staticmethod
    def is_mouse_clicked(button: plMouseButton, repeat: bool = False) -> bool:
        ...

    @staticmethod
    def is_mouse_dragging(button: plMouseButton, threshold: float) -> bool:
        ...

    @staticmethod
    def is_mouse_hovering_rect(minvec: List[float], maxvec: List[float]) -> bool:
        ...

    @staticmethod
    def reset_mouse_drag_delta(button: plMouseButton) -> None:
        ...

    @staticmethod
    def get_mouse_drag_delta(button: plMouseButton, threshold: float) -> List[float]:
        ...

    @staticmethod
    def get_mouse_pos() -> None:
        ...

    @staticmethod
    def get_mouse_wheel() -> float:
        ...

    @staticmethod
    def is_mouse_pos_valid(vec: List[float]) -> bool:
        ...

    @staticmethod
    def set_mouse_cursor(vec: int) -> None:
        ...

########################################################################################################################
# [SECTION] window api
########################################################################################################################

class plWindowI:

    @staticmethod
    def create(desc: plWindowDesc) -> Tuple[int, plWindow]:
        ...

    @staticmethod
    def show(window: plWindow):
        ...

    @staticmethod
    def destroy(window: plWindow | None):
        ...

########################################################################################################################
# [SECTION] stats api
########################################################################################################################

class plStatsI:

    @staticmethod
    def new_frame(name):
        ...

    @staticmethod
    def get_counter(name):
        ...

########################################################################################################################
# [SECTION] vfs api
########################################################################################################################

class plVfsI:

    @staticmethod
    def mount_directory(directory, physical_directory, **kwargs) -> None:
        ...

########################################################################################################################
# [SECTION] pack api
########################################################################################################################

class plPakI:

    @staticmethod
    def begin_packing(file, content_version) -> Tuple[bool, plPakFile]:
        ...

    @staticmethod
    def add_from_disk(pak: plPakFile, pcPakPath, pcFilePath, bCompress) -> bool:
        ...
    @staticmethod
    def end_packing(pak: plPakFile):
        ...

########################################################################################################################
# [SECTION] graphics api
########################################################################################################################

class plGraphicsI:

    @staticmethod
    def flush_device(device: plDevice, **kwargs) -> None:
        ...

########################################################################################################################
# [SECTION] shader api
########################################################################################################################

class plShaderI:

    @staticmethod
    def initialize(options):
        ...

    @staticmethod
    def cleanup():
        ...

    @staticmethod
    def load_glsl(shader, entry_func, **kwargs):
        ...

    @staticmethod
    def compile_gls(shader, entry_func, **kwargs):
        ...

    @staticmethod
    def write_to_disk(shader, module):
        ...

########################################################################################################################
# [SECTION] draw api
########################################################################################################################

class plDrawI:

    @staticmethod
    def initialize(init: plDrawInit):
        ...

    @staticmethod
    def cleanup():
        ...

    @staticmethod
    def new_frame():
        ...

    @staticmethod
    def create_font_atlas() -> plFontAtlas:
        ...

    @staticmethod
    def build_font_atlas(commandBuffer: plCommandBuffer, atlas: plFontAtlas) -> bool:
        ...

    @staticmethod
    def get_current_font_atlas() -> plFontAtlas:
        ...

    @staticmethod
    def cleanup_font_atlas(atlas: plFontAtlas | None):
        ...

    @staticmethod
    def set_font_atlas(atlas: plFontAtlas):
        ...

    @staticmethod
    def get_first_font(atlas: plFontAtlas) -> plFont:
        ...

    @staticmethod
    def add_default_font(atlas: plFontAtlas) -> plFont:
        ...

    @staticmethod
    def add_font_from_file_ttf(atlas: plFontAtlas, config: plFontConfig, file: str) -> plFont:
        ...

    @staticmethod
    def submit_2d_drawlist(drawlist: plDrawList2D, encoder: plRenderEncoder, width: float, height: float, sampleCount: int):
        ...

    @staticmethod
    def request_2d_drawlist() -> plDrawList2D:
        ...

    @staticmethod
    def return_2d_drawlist(drawlist: plDrawList2D):
        ...

    @staticmethod
    def request_2d_layer(drawlist: plDrawList2D) -> plDrawLayer2D:
        ...

    @staticmethod
    def return_2d_layer(layer: plDrawLayer2D):
        ...

    @staticmethod
    def submit_2d_layer(layer: plDrawLayer2D):
        ...

    @staticmethod
    def calculate_text_size(text: str, options: plDrawTextOptions) -> plVec2:
        ...

    @staticmethod
    def add_text(layer: plDrawLayer2D, p: plVec2Like, text: str, options: plDrawTextOptions):
        ...

    @staticmethod
    def add_text_clipped(layer: plDrawLayer2D, p: plVec2Like, text: str, clipMin: plVec2Like, clipMax: plVec2Like, options: plDrawTextOptions):
        ...

    @staticmethod
    def add_triangle(layer: plDrawLayer2D, p0: plVec2Like, p1: plVec2Like, p2: plVec2Like, options: plDrawLineOptions):
        ...

    @staticmethod
    def add_line(layer: plDrawLayer2D, p0: plVec2Like, p1: plVec2Like, options: plDrawLineOptions):
        ...

    @staticmethod
    def add_lines(layer: plDrawLayer2D, points: plVec2ListLike, options: plDrawLineOptions):
        ...

    @staticmethod
    def add_rect(layer: plDrawLayer2D, pMin: plVec2Like, pMax: plVec2Like, options: plDrawLineOptions):
        ...

    @staticmethod
    def add_rect_rounded(layer: plDrawLayer2D, pMin: plVec2Like, pMax: plVec2Like, radius: float, segments: int, flags: plDrawRectFlag, options: plDrawLineOptions):
        ...

    @staticmethod
    def add_quad(layer: plDrawLayer2D, p0: plVec2Like, p1: plVec2Like, p2: plVec2Like, p3: plVec2Like, options: plDrawLineOptions):
        ...

    @staticmethod
    def add_circle(layer: plDrawLayer2D, p: plVec2Like, radius: float, segments: int, options: plDrawLineOptions):
        ...

    @staticmethod
    def add_polygon(layer: plDrawLayer2D, points, options: plDrawLineOptions):
        ...

    @staticmethod
    def add_bezier_quad(layer: plDrawLayer2D, p0: plVec2Like, p1: plVec2Like, p2: plVec2Like, segments: int, options: plDrawLineOptions):
        ...

    @staticmethod
    def add_bezier_cubic(layer: plDrawLayer2D, p0: plVec2Like, p1: plVec2Like, p2: plVec2Like, p3: plVec2Like, segments: int, options: plDrawLineOptions):
        ...

    @staticmethod
    def add_triangle_filled(layer: plDrawLayer2D, p0: plVec2Like, p1: plVec2Like, p2: plVec2Like, options: plDrawSolidOptions):
        ...

    @staticmethod
    def add_triangles_filled(layer: plDrawLayer2D, points: plVec2ListLike, options: plDrawSolidOptions):
        ...

    @staticmethod
    def add_rect_filled(layer: plDrawLayer2D, pMin: plVec2Like, pMax: plVec2Like, options: plDrawSolidOptions):
        ...

    @staticmethod
    def add_rect_rounded_filled(layer: plDrawLayer2D, pMin: plVec2Like, pMax: plVec2Like, radius: float, segments: int, flags: plDrawRectFlag, options: plDrawSolidOptions):
        ...

    @staticmethod
    def add_quad_filled(layer: plDrawLayer2D, p0: plVec2Like, p1: plVec2Like, p2: plVec2Like, p3: plVec2Like, options: plDrawSolidOptions):
        ...

    @staticmethod
    def add_circle_filled(layer: plDrawLayer2D, p: plVec2Like, radius: float, segments: int, options: plDrawSolidOptions):
        ...

    @staticmethod
    def add_convex_polygon_filled(layer: plDrawLayer2D, points: plVec2ListLike, options: plDrawLineOptions):
        ...

########################################################################################################################
# [SECTION] screen log api
########################################################################################################################

class plScreenLogI:

    @staticmethod
    def clear():
        ...

    @staticmethod
    def add_message(time_to_display, message):
        ...

########################################################################################################################
# [SECTION] ui api
########################################################################################################################

class plUiI:

    @staticmethod
    def begin_window(name : str, **kwargs):
        ...

    @staticmethod
    def end_window(**kwargs) -> None:
        ...

    @staticmethod
    def button(name : str, **kwargs):
        ...

    @staticmethod
    def checkbox(name, value = None, **kwargs):
        ...

    @staticmethod
    def input_text(name, value, **kwargs):
        ...

########################################################################################################################
# [SECTION] ecs api
########################################################################################################################

class plEcsI:

    @staticmethod
    def initialize():
        ...

    @staticmethod
    def finalize():
        ...

    @staticmethod
    def cleanup():
        ...

    @staticmethod
    def get_default_library():
        ...

    @staticmethod
    def get_component(library: plComponentLibrary, key, entity):
        ...

########################################################################################################################
# [SECTION] animation api
########################################################################################################################

class plAnimationI:

    @staticmethod
    def register_ecs_system():
        ...

########################################################################################################################
# [SECTION] camera api
########################################################################################################################

class plCameraI:

    @staticmethod
    def register_ecs_system():
        ...

    @staticmethod
    def get_ecs_type_key():
        ...

    @staticmethod
    def create_perspective(library: plComponentLibrary, name, pos, yFov, aspect, nearZ, farZ, reverseZ):
        ...

    @staticmethod
    def set_fov(camera, yFov):
        ...

    @staticmethod
    def update(camera):
        ...

########################################################################################################################
# [SECTION] material api
########################################################################################################################

class plMaterialI:

    @staticmethod
    def register_ecs_system():
        ...

########################################################################################################################
# [SECTION] mesh api
########################################################################################################################

class plMeshI:

    @staticmethod
    def register_ecs_system():
        ...

########################################################################################################################
# [SECTION] physics api
########################################################################################################################

class plPhysicsI:

    @staticmethod
    def register_ecs_system():
        ...

########################################################################################################################
# [SECTION] script api
########################################################################################################################

class plScriptI:

    @staticmethod
    def register_ecs_system():
        ...

########################################################################################################################
# [SECTION] shader variant api
########################################################################################################################

class plShaderVariantI:

    @staticmethod
    def initialize(device: plDevice):
        ...

########################################################################################################################
# [SECTION] starter api
########################################################################################################################

class plStarterI:

    @staticmethod
    def initialize(window: plWindow, flags: plStarterFlag):
        ...

    @staticmethod
    def cleanup():
        ...

    @staticmethod
    def begin_frame():
        ...

    @staticmethod
    def finalize():
        ...

    @staticmethod
    def resize():
        ...

    @staticmethod
    def end_frame():
        ...

    @staticmethod
    def get_foreground_layer() -> plDrawLayer2D:
        ...

    @staticmethod
    def get_background_layer() -> plDrawLayer2D:
        ...

    @staticmethod
    def get_device() -> plDevice:
        ...

    @staticmethod
    def get_swapchain() -> plSwapchain:
        ...

    @staticmethod
    def get_render_pass() -> int: # TODO: figure out handles
        ...

    @staticmethod
    def begin_main_pass() -> plRenderEncoder:
        ...

    @staticmethod
    def end_main_pass():
        ...

    @staticmethod
    def get_command_buffer() -> plCommandBuffer:
        ...

    @staticmethod
    def get_temporary_command_buffer() -> plCommandBuffer:
        ...

    @staticmethod
    def get_raw_command_buffer() -> plCommandBuffer:
        ...

    @staticmethod
    def submit_command_buffer(commandBuffer: plCommandBuffer):
        ...

    @staticmethod
    def submit_temporary_command_buffer(commandBuffer: plCommandBuffer):
        ...

    @staticmethod
    def return_raw_command_buffer(commandBuffer: plCommandBuffer):
        ...

########################################################################################################################
# [SECTION] renderer api
########################################################################################################################

class plRendererI:

    @staticmethod
    def initialize(device: plDevice, swapchain: plSwapchain, **kwargs):
        ...

    @staticmethod
    def cleanup():
        ...

########################################################################################################################
# [SECTION] renderer ecs api
########################################################################################################################

class plRendererEcsI:

    @staticmethod
    def register_system():
        ...

    @staticmethod
    def create_directional_light(library: plComponentLibrary, name):
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