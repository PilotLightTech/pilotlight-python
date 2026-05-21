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

from typing import List, Dict, Tuple, overload, NewType
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
    tSampleCount: int

type plVec2Like = plVec2 | Tuple[float] | List[float]
type plVec2ListLike = Tuple[plVec2Like] | List[plVec2Like]

type plVec3Like = plVec3 | Tuple[float] | List[float]
type plVec3ListLike = Tuple[plVec3Like] | List[plVec3Like]

type plVec4Like = plVec4 | Tuple[float] | List[float]
type plVec4ListLike = Tuple[plVec4Like] | List[plVec4Like]

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
    def submit_3d_drawlist(drawlist: plDrawList3D, encoder: plRenderEncoder, width: float, height: float, mvp, flags, sampleCount: int):
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

class plAnimationI:

    @staticmethod
    def register_ecs_system():
        ...

class plCameraEcsI:

    @staticmethod
    def register_ecs_system():
        ...

    @staticmethod
    def get_ecs_type_key():
        ...

    @staticmethod
    def create_perspective(library: plComponentLibrary, name: str, desc: plCameraPerspectiveDesc):
        ...

class plMaterialI:

    @staticmethod
    def register_ecs_system():
        ...

class plMeshI:

    @staticmethod
    def register_ecs_system():
        ...

class plPhysicsI:

    @staticmethod
    def register_ecs_system():
        ...

class plScriptI:

    @staticmethod
    def register_ecs_system():
        ...

class plShaderVariantI:

    @staticmethod
    def initialize(device: plDevice):
        ...

class plRendererI:

    @staticmethod
    def initialize(device: plDevice, swapchain: plSwapchain, **kwargs):
        ...

    @staticmethod
    def cleanup():
        ...

class plRendererEcsI:

    @staticmethod
    def register_system():
        ...

    @staticmethod
    def create_directional_light(library: plComponentLibrary, name):
        ...