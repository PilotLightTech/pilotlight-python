########################################################################################################################
# Pilot Light Public Interface
#
# User API Index
#
#    * Sections
#      - imports
#      - constants
#      - custom types
#      - core api
#      - io api
#      - window api
#      - stats api
#      - vfs api
#      - pak api
#      - graphics api
#      - shader api
#      - draw api
#      - screen log api
#      - ui api
#      - ecs api
#      - starter api
#      - camera api
#      - unstable apis
#
########################################################################################################################

########################################################################################################################
# [SECTION] imports
########################################################################################################################

from typing import List, Dict, Tuple, overload, NewType, Callable
from enum import IntEnum, IntFlag
import pilotlight.pilotlight as pl
from pilotlight.types import *
from pilotlight.enums import *

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
    def __neg__(self) -> plVec2: ...

class plVec3:
    x: float
    y: float
    z: float

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, x: float, y: float, z: float) -> None: ...

    def __repr__(self) -> str: ...
    def __add__(self, other: plVec3) -> plVec3: ...
    def __sub__(self, other: plVec3) -> plVec3: ...
    def __neg__(self) -> plVec3: ...

class plVec4:
    x: float
    y: float
    z: float
    w: float

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, x: float, y: float, z: float, w: float) -> None: ...

    def __repr__(self) -> str: ...
    def __add__(self, other: plVec4) -> plVec4: ...
    def __sub__(self, other: plVec4) -> plVec4: ...
    def __neg__(self) -> plVec4: ...

class plMat4:
    x11: float
    x21: float
    x31: float
    x41: float
    x12: float
    x22: float
    x32: float
    x42: float
    x13: float
    x23: float
    x33: float
    x43: float
    x14: float
    x24: float
    x34: float
    x44: float

    def __repr__(self) -> str: ...
    def __mul__(self, right: plMat4) -> plMat4: ...

class plCamera:
    
    # projection
    eProjectionType: plCameraProjectionType
    eDepthMode: plCameraDepthMode
    fNearZ: float
    fFarZ: float

    # perspective
    fYFov: float
    fAspectRatio: float # width/height

    # orthographic
    fWidth: float
    fHeight: float

    # pose
    tPosition: plVec3
    tRotation: plQuat
    
    # cached matrices
    tViewMat: plMat4                 # world to camera/view
    tProjMat: plMat4                 # view to clip
    tViewProjMat: plMat4             # world to clip
    tInvViewMat: plMat4              # camera/view to world
    tInvProjMat: plMat4              # clip to view
    tInvViewProjMat: plMat4          # clip to world
    tViewMatNoTranslation: plMat4    # view matrix using camera-relative origin
    tInvViewMatNoTranslation: plMat4 # inverse view matrix using camera-relative origin

    # convenience rotations
    fPitch: float # rotation about right vector
    fYaw: float   # rotation about up vector
    fRoll: float  # rotation about forward vector

    # cached orientation vectors
    tUpVec: plVec3
    tForwardVec: plVec3
    tRightVec: plVec3

class plIO:

    # configurable
    bRunning: bool
    fMouseDragThreshold: float 
    fMouseDoubleClickTime: float 
    fMouseDoubleClickMaxDist: float 
    fKeyRepeatDelay: float 
    fKeyRepeatRate: float

    # do not set these
    fDeltaTime: float
    fFrameRate: float
    dTime: float
    tMainViewportSize: plVec2
    tMainFramebufferScale: plVec2

class plSwapchainInfo:
    eSampleCount: int

type plVec2Like = plVec2 | Tuple[float] | List[float]
type plVec2ListLike = Tuple[plVec2Like] | List[plVec2Like]

type plVec3Like = plVec3 | Tuple[float] | List[float]
type plVec3ListLike = Tuple[plVec3Like] | List[plVec3Like]

type plVec4Like = plVec4 | Tuple[float] | List[float]
type plVec4ListLike = Tuple[plVec4Like] | List[plVec4Like]

type plPointer = plDoublePointer

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
    def get_mouse_drag_delta(button: plMouseButton, threshold: float) -> plVec2:
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
    def new_frame():
        ...

    @staticmethod
    def get_counter(name: str) -> plDoublePointer:
        ...

########################################################################################################################
# [SECTION] vfs api
########################################################################################################################

class plFileI:
    """physical file system helpers (paths are not vfs paths)"""

    @staticmethod
    def exists(path: str) -> bool:
        ...

    @staticmethod
    def remove(path: str) -> bool:
        ...

    @staticmethod
    def copy(source: str, destination: str) -> bool:
        ...

    @staticmethod
    def directory_exists(path: str) -> bool:
        ...

    @staticmethod
    def create_directory(path: str) -> bool:
        """returns True if the directory exists after the call"""
        ...

    @staticmethod
    def remove_directory(path: str) -> bool:
        ...

    @staticmethod
    def get_directory_info(path: str) -> List[Tuple[str, plDirectoryEntryType]] | None:
        ...

class plVfsI:

    @staticmethod
    def mount_directory(directory, physical_directory, **kwargs) -> None:
        ...

########################################################################################################################
# [SECTION] pak api
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

    @staticmethod
    def get_swapchain_info(swapchain: plSwapchain) -> plSwapchainInfo:
        ...

    @staticmethod
    def get_backend_string() -> str:
        ...

    @staticmethod
    def set_gpu_timing_enabled(device: plDevice, enabled: bool) -> None:
        """takes effect at the next begin_frame; times push/pop debug group scopes"""
        ...

    @staticmethod
    def get_gpu_timings(device: plDevice) -> dict:
        """{'samples': [{'name', 'start', 'duration', 'depth'}], 'frame_index', 'duration', 'dropped_samples', 'supported', 'enabled', 'cpu'} (times in seconds)"""
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
    def compile_glsl(shader, entry_func, **kwargs):
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

    ####################################################################################################################
    # fonts
    ####################################################################################################################

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
    def calculate_text_size(text: str, options: plDrawTextOptions) -> plVec2:
        ...

    ####################################################################################################################
    # 2D
    ####################################################################################################################

    @staticmethod
    def submit_2d_drawlist(drawlist: plDrawList2D, commandBuffer: plCommandBuffer, width: float, height: float, sampleCount: int, attachmentInfo: plRenderAttachmentInfo):
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

    @staticmethod
    def create_bind_group_for_texture(handle: int) -> int:
        ...

    @staticmethod
    def add_image(layer: plDrawLayer2D, textureID, minP: plVec2Like, maxP: plVec2Like, minUV: plVec2Like = [0.0, 0.0], maxUV: plVec2Like = [1.0, 1.0], color: int = PL_COLOR_32_WHITE):
        ...

    @staticmethod
    def add_image_rounded(layer: plDrawLayer2D, textureID, minP: plVec2Like, maxP: plVec2Like, radius: float, segments: int = 0, flags: plDrawRectFlag = plDrawRectFlag.PL_DRAW_RECT_FLAG_NONE, minUV: plVec2Like = [0.0, 0.0], maxUV: plVec2Like = [1.0, 1.0], color: int = PL_COLOR_32_WHITE) -> None:
        """segments=0 is adaptive; flags=0 rounds every corner"""
        ...

    @staticmethod
    def add_line_h(layer: plDrawLayer2D, x0: float, x1: float, y: float, options: plDrawLineOptions) -> None:
        ...

    @staticmethod
    def add_line_v(layer: plDrawLayer2D, x: float, y0: float, y1: float, options: plDrawLineOptions) -> None:
        ...

    @staticmethod
    def path_clear(layer: plDrawLayer2D) -> None:
        """paths belong to the layer; stroke/fill consumes the path"""
        ...

    @staticmethod
    def path_line_to(layer: plDrawLayer2D, p: plVec2Like) -> None:
        ...

    @staticmethod
    def path_arc_to(layer: plDrawLayer2D, center: plVec2Like, radius: float, angleMin: float, angleMax: float, segments: int = 0) -> None:
        """angles are radians; segments=0 is adaptive"""
        ...

    @staticmethod
    def path_ellipse_to(layer: plDrawLayer2D, center: plVec2Like, radii: plVec2Like, rotation: float, angleMin: float, angleMax: float, segments: int = 0) -> None:
        ...

    @staticmethod
    def path_bezier_quad_to(layer: plDrawLayer2D, control: plVec2Like, end: plVec2Like, segments: int = 0) -> None:
        ...

    @staticmethod
    def path_bezier_cubic_to(layer: plDrawLayer2D, control0: plVec2Like, control1: plVec2Like, end: plVec2Like, segments: int = 0) -> None:
        ...

    @staticmethod
    def path_rect(layer: plDrawLayer2D, minP: plVec2Like, maxP: plVec2Like, radius: float = 0.0, cornerSegments: int = 0, flags: plDrawRectFlag = plDrawRectFlag.PL_DRAW_RECT_FLAG_NONE) -> None:
        """appends a clockwise outline"""
        ...

    @staticmethod
    def path_stroke(layer: plDrawLayer2D, options: plDrawLineOptions) -> None:
        ...

    @staticmethod
    def path_fill_convex(layer: plDrawLayer2D, options: plDrawSolidOptions) -> None:
        """convex only, either winding"""
        ...

    @staticmethod
    def add_image_quad(layer: plDrawLayer2D, textureID, p0: plVec2Like, p1: plVec2Like, p2: plVec2Like, p3: plVec2Like,
                       p0UV: plVec2Like = [0.0, 0.0], p1UV: plVec2Like = [0.0, 1.0], p2UV: plVec2Like = [1.0, 1.0],
                       p3UV: plVec2Like = [1.0, 0.0], color: int = PL_COLOR_32_WHITE):
        ...

    @staticmethod
    def use_nearest_sampler(layer: plDrawLayer2D):
        ...

    @staticmethod
    def use_linear_sampler(layer: plDrawLayer2D):
        ...

    ####################################################################################################################
    # 3D
    ####################################################################################################################

    @staticmethod
    def request_3d_drawlist() -> plDrawList3D:
        ...

    @staticmethod
    def return_3d_drawlist(drawlist: plDrawList3D):
        ...

    @staticmethod
    def submit_3d_drawlist(drawlist: plDrawList3D, commandBuffer: plCommandBuffer, width: float, height: float, mvp, flags, sampleCount: int, attachmentInfo: plRenderAttachmentInfo):
        ...

    @staticmethod
    def add_3d_text(drawlist: plDrawList3D, p: plVec3Like, text: str, options: plDrawTextOptions):
        ...

    @staticmethod
    def add_3d_triangle_filled(drawlist: plDrawList3D, p0: plVec3, p1: plVec3, p2: plVec3, options: plDrawSolidOptions):
        ...
    
    @staticmethod
    def add_3d_circle_xz_filled(drawlist: plDrawList3D, center: plVec3, radius: float, segments: int , options: plDrawSolidOptions):
        ...
    
    @staticmethod
    def add_3d_band_xz_filled(drawlist: plDrawList3D, center: plVec3, innerRadius: float, outerRadius: float, segments: int, options: plDrawSolidOptions):
        ...
    
    @staticmethod
    def add_3d_band_xy_filled(drawlist: plDrawList3D, center: plVec3, innerRadius: float, outerRadius: float, segments: int, options: plDrawSolidOptions):
        ...
    
    @staticmethod
    def add_3d_band_yz_filled(drawlist: plDrawList3D, center: plVec3, innerRadius: float, outerRadius: float, segments: int, options: plDrawSolidOptions):
        ...
    
    @staticmethod
    def add_3d_centered_box_filled(drawlist: plDrawList3D, center: plVec3, width: float, height: float, depth: float, options: plDrawSolidOptions):
        ...
    
    @staticmethod
    def add_3d_plane_xz_filled(drawlist: plDrawList3D, center: plVec3, width: float, height: float, options: plDrawSolidOptions):
        ...
    
    @staticmethod
    def add_3d_plane_xy_filled(drawlist: plDrawList3D, center: plVec3, width: float, height: float, options: plDrawSolidOptions):
        ...
    
    @staticmethod
    def add_3d_plane_yz_filled(drawlist: plDrawList3D, center: plVec3, width: float, height: float, options: plDrawSolidOptions):
        ...
    
    @staticmethod
    def add_3d_sphere_filled(drawlist: plDrawList3D, sphere: plSphere, latBands: int, longBands: int, options: plDrawSolidOptions):
        ...
    
    @staticmethod
    def add_3d_cylinder_filled(drawlist: plDrawList3D, cylinder: plCylinder, segments: int, options: plDrawSolidOptions):
        ...
    
    @staticmethod
    def add_3d_cone_filled(drawlist: plDrawList3D, cone: plCone, segments: int, options: plDrawSolidOptions):
        ...
    
    @staticmethod
    def add_3d_line(drawlist: plDrawList3D, p0: plVec3, p1: plVec3, options: plDrawLineOptions):
        ...
    
    @staticmethod
    def add_3d_cross(drawlist: plDrawList3D, p: plVec3, length: float, options: plDrawLineOptions):
        ...
    
    @staticmethod
    def add_3d_transform(drawlist: plDrawList3D, transform: plMat4, length: float, options: plDrawLineOptions):
        ...
    
    @staticmethod
    def add_3d_frustum(drawlist: plDrawList3D, transform: plMat4, desc: plDrawFrustumDesc, options: plDrawLineOptions):
        ...
    @staticmethod
    def add_3d_centered_box(drawlist: plDrawList3D, center: plVec3, width: float, height: float, depth: float, options: plDrawLineOptions):
        ...
    
    @staticmethod
    def add_3d_aabb(drawlist: plDrawList3D, minP: plVec3, maxP: plVec3, options: plDrawLineOptions):
        ...
    
    @staticmethod
    def add_3d_bezier_quad(drawlist: plDrawList3D, p0: plVec3, p1: plVec3, p2: plVec3, segments: int, options: plDrawLineOptions):
        ...
    
    @staticmethod
    def add_3d_bezier_cubic(drawlist: plDrawList3D, p0: plVec3, p1: plVec3, p2: plVec3, tP3: plVec3, segments: int, options: plDrawLineOptions):
        ...
    
    @staticmethod
    def add_3d_circle_xz(drawlist: plDrawList3D, center: plVec3, radius: float, segments: int, options: plDrawLineOptions):
        ...
    
    @staticmethod
    def add_3d_sphere(drawlist: plDrawList3D, sphere: plSphere, latBands: int, longBands: int, options: plDrawLineOptions):
        ...
    
    @staticmethod
    def add_3d_capsule(drawlist: plDrawList3D, capsule: plCapsule, latBands: int, longBands: int, options: plDrawLineOptions):
        ...
    
    @staticmethod
    def add_3d_cylinder(drawlist: plDrawList3D, cylinder: plCylinder, segments: int, options: plDrawLineOptions):
        ...
    
    @staticmethod
    def add_3d_cone(drawlist: plDrawList3D, cone: plCone, segments: int, options: plDrawLineOptions):
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
    def text(text: str) -> None:
        ...

    @staticmethod
    def separator() -> None:
        ...

    @staticmethod
    def slider_float(label: str, value: float, minValue: float, maxValue: float, flags: int = 0) -> Tuple[bool, float]:
        """returns (changed, value)"""
        ...

    @staticmethod
    def slider_int(label: str, value: int, minValue: int, maxValue: int, flags: int = 0) -> Tuple[bool, int]:
        """returns (changed, value)"""
        ...

    @staticmethod
    def radio_button(text: str, current: int, value: int) -> Tuple[bool, int]:
        """returns (pressed, current selection)"""
        ...

    @staticmethod
    def set_next_window_pos(pos: plVec2Like, condition: plUiCondition = plUiCondition.PL_UI_COND_ONCE) -> None:
        ...

    @staticmethod
    def set_next_window_size(size: plVec2Like, condition: plUiCondition = plUiCondition.PL_UI_COND_ONCE) -> None:
        ...

    @staticmethod
    def begin_window(name : str, **kwargs):
        ...

    @staticmethod
    def end_window(**kwargs) -> None:
        ...

    @staticmethod
    def button(name : str, **kwargs) -> bool:
        ...

    @staticmethod
    def checkbox(name, value: bool) ->Tuple[bool, bool]:
        ...

    @staticmethod
    def input_text(name, value, **kwargs):
        ...

    @staticmethod
    def wants_keyboard_capture() -> bool:
        ...

    @staticmethod
    def wants_mouse_capture() -> bool:
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
    def register_asset_types():
        ...

    @staticmethod
    def get_asset_type_key() -> int:
        ...

    @staticmethod
    def get_ecs_type_key_tag() -> int:
        ...

    @staticmethod
    def create_library() -> plComponentLibrary:
        ...

    @staticmethod
    def destroy_library(library: plComponentLibrary):
        ...

    @staticmethod
    def clear_changes(library: plComponentLibrary):
        ...

    @staticmethod
    def create_entity(library: plComponentLibrary, name: str) -> plEntity:
        ...

    @staticmethod
    def remove_entity(library: plComponentLibrary, entity: plEntity):
        ...

    @staticmethod
    def is_entity_valid(library: plComponentLibrary, entity: plEntity) -> bool:
        ...

    @staticmethod
    def has_component(library: plComponentLibrary, key: int, entity: plEntity) -> bool:
        ...

    @staticmethod
    def get_components(library: plComponentLibrary, key: int) -> List[plEntity]:
        ...

    @staticmethod
    def copy_component(library: plComponentLibrary, key: int, source: plEntity, destination: plEntity) -> bool:
        """call between system updates; adds/replaces the component on destination"""
        ...

    @staticmethod
    def clone_entity(library: plComponentLibrary, source: plEntity) -> plEntity:
        """new local entity with a new ID; does not clone children"""
        ...

class plTransformI:

    @staticmethod
    def register_ecs_components():
        ...

    @staticmethod
    def get_ecs_type_key_transform() -> int:
        ...

    @staticmethod
    def get_ecs_type_key_hierarchy() -> int:
        ...

    @staticmethod
    def run_transform_update_system(library: plComponentLibrary):
        ...

    @staticmethod
    def run_hierarchy_update_system(library: plComponentLibrary):
        ...

########################################################################################################################
# [SECTION] starter api
########################################################################################################################

class plStarterI:

    @staticmethod
    def activate_msaa() -> None:
        """queued until the next begin_frame"""
        ...

    @staticmethod
    def deactivate_msaa() -> None:
        """queued until the next begin_frame"""
        ...

    @staticmethod
    def activate_vsync() -> None:
        ...

    @staticmethod
    def deactivate_vsync() -> None:
        ...

    @staticmethod
    def initialize(starterInit: plStarterInit):
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
    def get_render_attachment_info(infoOut: plRenderAttachmentInfo):
        ...

    @staticmethod
    def begin_main_pass() -> plCommandBuffer:
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
# [SECTION] camera api
########################################################################################################################

class plCameraI:

    @staticmethod
    def init(camera: plCamera):
        ...

    @staticmethod
    def set_perspective(camera: plCamera, desc: plCameraPerspectiveDesc):
        ...

    @staticmethod
    def set_orthographic(camera: plCamera, desc: plCameraOrthographicDesc):
        ...

    @staticmethod
    def set_viewport(camera: plCamera, width: float, height: float):
        ...

    @staticmethod
    def set_y_fov(camera: plCamera, fov: float):
        ...

    @staticmethod
    def set_clip_planes(camera: plCamera, nearZ: float, farZ: float):
        ...

    @staticmethod
    def set_depth_mode(camera: plCamera, mode: plCameraDepthMode):
        ...

    @staticmethod
    def set_position(camera: plCamera, position: plVec3Like):
        ...

    @staticmethod
    def set_rotation(camera: plCamera, rotation: plQuat):
        ...

    @staticmethod
    def set_transform(camera: plCamera, position: plVec3Like, rotation: plQuat):
        ...

    @staticmethod
    def translate(camera: plCamera, delta: plVec3Like):
        ...

    @staticmethod
    def translate_local(camera: plCamera, delta: plVec3Like):
        ...

    @staticmethod
    def look_at(camera: plCamera, eye: plVec3Like, target: plVec3Like, up: plVec3Like):
        ...

    @staticmethod
    def rotate_euler_local(camera: plCamera, pitch: float, yaw: float, roll: float):
        ...

    @staticmethod
    def rotate_euler(camera: plCamera, pitch: float, yaw: float, roll: float):
        ...

    @staticmethod
    def set_euler(camera: plCamera, pitch: float, yaw: float, roll: float):
        ...

    @staticmethod
    def update(camera: plCamera):
        ...

########################################################################################################################
# [SECTION] unstable apis
########################################################################################################################

class plTerrainI:

    @staticmethod
    def register_asset_types():
        ...

    @staticmethod
    def get_asset_type_key() -> int:
        ...

class plJobI:

    @staticmethod
    def initialize():
        ...

    @staticmethod
    def cleanup():
        ...

class plScriptI:

    @staticmethod
    def register_ecs_components():
        ...

    @staticmethod
    def get_ecs_type_key() -> int:
        ...

    @staticmethod
    def run_update_system(library: plComponentLibrary):
        ...

    @staticmethod
    def register(name: str, run: Callable[[plComponentLibrary, plEntity], None], setup: Callable[[plComponentLibrary, plEntity], None] | None = None) -> None:
        """register a python script explicitly. Otherwise a script named in a component is resolved to a python module
        of that name (normal import, then pilotlight.<name>) providing run(library, entity) & optionally setup(library, entity)"""
        ...

    @staticmethod
    def unregister(name: str) -> None:
        ...

    @staticmethod
    def create(library: plComponentLibrary, script: str, flags: plScriptFlag = plScriptFlag.PL_SCRIPT_FLAG_PLAYING) -> plEntity:
        """creates an entity with a script component"""
        ...

    @staticmethod
    def attach(library: plComponentLibrary, script: str, entity: plEntity, flags: plScriptFlag = plScriptFlag.PL_SCRIPT_FLAG_PLAYING) -> None:
        """adds a script component to an entity"""
        ...

    @staticmethod
    def load(library: plComponentLibrary, entity: plEntity) -> None:
        """(re)loads an entity's script"""
        ...

class plAssetI:

    @staticmethod
    def initialize():
        ...

    @staticmethod
    def finalize():
        """also registers the asset types gltf & terrain rely on (texture, material, mesh, etc.)"""
        ...

    @staticmethod
    def cleanup():
        ...

    @staticmethod
    def clear_assets():
        ...

    @staticmethod
    def clear_changes():
        ...

    @staticmethod
    def load(path: str) -> plAssetHandle:
        ...

    @staticmethod
    def find(path: str) -> plAssetHandle:
        ...

    @staticmethod
    def save(handle: plAssetHandle, encoding: plAssetEncoding = plAssetEncoding.PL_ASSET_ENCODING_AUTO) -> bool:
        ...

    @staticmethod
    def destroy(handle: plAssetHandle):
        ...

    @staticmethod
    def reload(handle: plAssetHandle):
        ...

    @staticmethod
    def is_valid(handle: plAssetHandle) -> bool:
        ...

    @staticmethod
    def get_type_key(handle: plAssetHandle) -> int:
        ...

    @staticmethod
    def get_path(handle: plAssetHandle) -> str | None:
        ...

    @staticmethod
    def get_data(handle: plAssetHandle) -> plComponentLibrary | object | None:
        ...

class plGltfI:

    @staticmethod
    def import_file(path: str, flags: plGltfImportFlags = plGltfImportFlags.PL_GLTF_IMPORT_FLAGS_IMPORT_ALL_ASSETS) -> bool:
        """requires plStarterI to be initialized before plAssetI.finalize()"""
        ...

class plResourceI:

    @staticmethod
    def initialize(init: plResourceManagerInit):
        ...

    @staticmethod
    def cleanup():
        ...

    @staticmethod
    def clear():
        ...

    @staticmethod
    def new_frame():
        ...

    @staticmethod
    def load(file: str, flags: int) -> int:
        ...

    @staticmethod
    def get_texture(handle: int) -> int:
        ...