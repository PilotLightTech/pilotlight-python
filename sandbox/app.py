"""
Pilot Light Python sandbox
  * 3D drawing scene (plDrawI 3D drawlist + fly camera)
  * UI (plUiI)
  * texture (plResourceI -> plDrawI)
  * 2D drawing demo (stroke modes, path api, etc.) drawn on the main view
    when toggled on

Camera controls (when the UI doesn't want the mouse/keyboard):
  * LMB + Drag: Moves camera forward & backward and rotates left & right.
  * RMB + Drag: Rotates camera.
  * LMB + RMB + Drag: Pans Camera
  * Mouse Wheel: Speed
  * Game style (when holding RMB): W/S/A/D move, Q/E down/up, Z/C zoom
"""

import math
from pathlib import Path

# core
import pilotlight.pilotlight as pl
from pilotlight.pilotlight import *
from pilotlight.enums import *
from pilotlight.types import *
from pilotlight.pl_script_camera import pl_script_run

#-----------------------------------------------------------------------------
# 2D drawing demo constants
#-----------------------------------------------------------------------------

# stroke modes shown in the demo (one row each)
STROKE_MODES = [
    (plDrawStrokeFlag.PL_DRAW_STROKE_NONE,          "Centered"),
    (plDrawStrokeFlag.PL_DRAW_STROKE_CENTER_BIASED, "Center biased"),
    (plDrawStrokeFlag.PL_DRAW_STROKE_INSIDE,        "Inside (right of path)"),
    (plDrawStrokeFlag.PL_DRAW_STROKE_OUTSIDE,       "Outside (left of path)"),
]

# colors (ABGR, same as the C demo)
COLOR_STROKE  = 0xc0f0b060
COLOR_GUIDE   = 0x807070ff
COLOR_PATH    = 0xff80d0ff
COLOR_ELLIPSE = 0x8080d0ff
COLOR_FILLED  = 0xc080d0ff
COLOR_RECT    = 0xff805030
COLOR_PANEL   = 0xe0201810

class App:

    def __init__(self):
        self.ptWindow = None
        self.ptFont = None
        self.camera = None
        self.drawlist = None
        self.counter = None
        self.texture_bg = None

        # ui state
        self.show_scene = True
        self.show_grid = True
        self.show_texture = True
        self.show_drawing = False
        self.some_text = "pizza"

        # 2D drawing demo settings
        self.thickness = 3.0
        self.offset = 0.0
        self.square = False
        self.miter = False
        self.no_aa = False
        self.rounding = 6.0

    #-------------------------------------------------------------------------
    # application lifetime
    #-------------------------------------------------------------------------

    def pl_app_load(self):

        # paths are relative to the working directory (out/, see .vscode/launch.json)
        package_dir = Path(pl.__file__).resolve().parent
        cache_dir = Path.cwd().resolve().parent / "cache"

        # cache directories written to by extensions (i.e. compiled shaders)
        for sub_dir in ["", "/shaders"]:
            plFileI.create_directory(str(cache_dir) + sub_dir)
        plVfsI.mount_directory("/cache", str(cache_dir))
        plVfsI.mount_directory("/shaders", str(package_dir / "shaders"))
        # sprite_map.png isn't shipped with the package, use the pilot light repo's
        plVfsI.mount_directory("/resources", str(Path.cwd().resolve().parent.parent / "pilotlight" / "resources"))

        # window
        window_desc = plWindowDesc()
        window_desc.pcTitle = "Pilot Light Python Sandbox"
        window_desc.iXPos = 50
        window_desc.iYPos = 50
        window_desc.uWidth = 1500
        window_desc.uHeight = 900
        _, self.ptWindow = plWindowI.create(window_desc)
        plWindowI.show(self.ptWindow)

        # starter (we initialize the shader extension ourselves so it can find
        # the shaders shipped with the python package)
        starterInit = plStarterInit(
            plStarterFlag.PL_STARTER_FLAGS_ALL_EXTENSIONS
            | plStarterFlag.PL_STARTER_FLAGS_MSAA
            | plStarterFlag.PL_STARTER_FLAGS_DEPTH_BUFFER,
            self.ptWindow)
        starterInit.eFlags &= ~plStarterFlag.PL_STARTER_FLAGS_SHADER_EXT
        plStarterI.initialize(starterInit)

        shader_options = plShaderOptions()
        shader_options.apcDirectories = ["/shaders/"]
        shader_options.apcIncludeDirectories = ["/shaders/"]
        shader_options.eFlags = plShaderFlags.PL_SHADER_FLAGS_AUTO_OUTPUT
        plShaderI.initialize(shader_options)

        plStarterI.finalize()

        self.ptFont = plDrawI.get_first_font(plDrawI.get_current_font_atlas())
        self.counter = plStatsI.get_counter("python counter")

        # camera
        io = plIOI.get_io()
        self.camera = plCamera()
        plCameraI.init(self.camera)
        tCameraDesc = plCameraPerspectiveDesc()
        tCameraDesc.eDepthMode = plCameraDepthMode.PL_CAMERA_DEPTH_MODE_STANDARD
        tCameraDesc.fAspectRatio = io.tMainViewportSize.x / io.tMainViewportSize.y
        tCameraDesc.fNearZ = 0.1
        tCameraDesc.fFarZ = 100.0
        tCameraDesc.fYFov = math.radians(60.0)
        plCameraI.set_perspective(self.camera, tCameraDesc)
        plCameraI.set_position(self.camera, [-10.0, 4.0, -10.0])
        plCameraI.rotate_euler(self.camera, -0.295, 0.7, 0.0)
        plCameraI.update(self.camera)

        self.drawlist = plDrawI.request_3d_drawlist()

        # texture (resource extension is initialized by the starter)
        texture_resource = plResourceI.load("/resources/sprite_map.png", 0)
        self.texture_bg = plDrawI.create_bind_group_for_texture(plResourceI.get_texture(texture_resource))

    def pl_app_shutdown(self):
        plGraphicsI.flush_device(plStarterI.get_device())
        plDrawI.return_3d_drawlist(self.drawlist)
        plStarterI.cleanup()
        plWindowI.destroy(self.ptWindow)

    def pl_app_resize(self):
        plStarterI.resize()
        io = plIOI.get_io()
        plCameraI.set_viewport(self.camera, io.tMainViewportSize.x, io.tMainViewportSize.y)

    #-------------------------------------------------------------------------
    # per frame
    #-------------------------------------------------------------------------

    def pl_app_update(self):

        if not plStarterI.begin_frame():
            return

        io = plIOI.get_io()

        # io api
        if plIOI.is_key_pressed(plKey.PL_KEY_P):
            print("P key pressed!")

        self.show_ui()

        pl_script_run(self.camera)

        # 2D (foreground layer, screen space)
        layer = plStarterI.get_foreground_layer()
        plDrawI.add_text(layer, [16.0, 16.0],
            f"Pilot Light Python Sandbox\nFPS: {io.fFrameRate:.1f}",
            plDrawTextOptions(self.ptFont, 18.0, PL_COLOR_32_CYAN))

        if self.show_texture:
            self.draw_texture(layer)

        if self.show_drawing:
            self.draw_drawing_demo(layer)

        # 3D
        if self.show_grid:
            self.draw_grid()
        if self.show_scene:
            self.draw_scene()

        cmdBuffer = plStarterI.begin_main_pass()

        tRenderAttachmentInfo = plRenderAttachmentInfo()
        plStarterI.get_render_attachment_info(tRenderAttachmentInfo)
        plDrawI.submit_3d_drawlist(
            self.drawlist,
            cmdBuffer,
            io.tMainViewportSize.x,
            io.tMainViewportSize.y,
            self.camera.tViewProjMat,
            plDrawFlag.PL_DRAW_FLAG_DEPTH_TEST | plDrawFlag.PL_DRAW_FLAG_DEPTH_WRITE,
            plGraphicsI.get_swapchain_info(plStarterI.get_swapchain()).eSampleCount,
            tRenderAttachmentInfo
        )

        plStarterI.end_main_pass()

        plStarterI.end_frame()

    #-------------------------------------------------------------------------
    # ui
    #-------------------------------------------------------------------------

    def show_ui(self):

        io = plIOI.get_io()
        plUiI.set_next_window_pos([io.tMainViewportSize.x - 420.0, 30.0], plUiCondition.PL_UI_COND_ONCE)
        plUiI.set_next_window_size([390.0, 560.0], plUiCondition.PL_UI_COND_ONCE)
        if plUiI.begin_window("Sandbox"):

            plUiI.text("Display")
            _, self.show_scene = plUiI.checkbox("3D scene", self.show_scene)
            _, self.show_grid = plUiI.checkbox("Grid", self.show_grid)
            _, self.show_texture = plUiI.checkbox("Texture", self.show_texture)
            _, self.show_drawing = plUiI.checkbox("2D drawing demo", self.show_drawing)

            # msaa/vsync changes are queued until the next begin_frame
            msaa = plGraphicsI.get_swapchain_info(plStarterI.get_swapchain()).eSampleCount != 1
            changed, msaa = plUiI.checkbox("MSAA", msaa)
            if changed:
                if msaa:
                    plStarterI.activate_msaa()
                else:
                    plStarterI.deactivate_msaa()

            if self.show_drawing:
                plUiI.separator()
                plUiI.text("2D drawing demo")
                plUiI.text("Compare without MSAA.")
                _, self.thickness = plUiI.slider_float("Stroke width", self.thickness, 0.25, 12.0)
                _, self.offset = plUiI.slider_float("Subpixel offset", self.offset, 0.0, 1.0)
                _, self.square = plUiI.checkbox("Square caps", self.square)
                _, self.miter = plUiI.checkbox("Force miter joins", self.miter)
                _, self.no_aa = plUiI.checkbox("Disable stroke AA", self.no_aa)
                _, self.rounding = plUiI.slider_float("Corner radius", self.rounding, 1.0, 16.0)

            plUiI.separator()
            plUiI.text("Misc")
            _, self.some_text = plUiI.input_text("Input", self.some_text)
            if plUiI.button("Increment stats counter"):
                pl_set_pointer_value(self.counter, pl_get_pointer_value(self.counter) + 1)
            if plUiI.button("Add screen log message"):
                plScreenLogI.add_message(2.0, f"Logging from python: {self.some_text}")

            plUiI.end_window()

    #-------------------------------------------------------------------------
    # texture
    #-------------------------------------------------------------------------

    def draw_texture(self, layer):
        io = plIOI.get_io()
        size = 256.0
        x1 = io.tMainViewportSize.x - 30.0
        y1 = io.tMainViewportSize.y - 30.0
        x0 = x1 - size
        y0 = y1 - size
        plDrawI.add_text(layer, [x0, y0 - 22.0], "Texture: /resources/sprite_map.png", plDrawTextOptions(self.ptFont, 0.0, PL_COLOR_32_WHITE))
        plDrawI.add_image(layer, self.texture_bg, [x0, y0], [x1, y1])
        plDrawI.add_rect(layer, [x0, y0], [x1, y1], plDrawLineOptions(PL_COLOR_32_WHITE, 1.0, plDrawStrokeFlag.PL_DRAW_STROKE_OUTSIDE))

    #-------------------------------------------------------------------------
    # 3D scene
    #-------------------------------------------------------------------------

    def draw_grid(self):

        thin = plDrawLineOptions(PL_COLOR_32_GREY, 0.03)
        x_axis = plDrawLineOptions(PL_COLOR_32_RED, 0.06)
        z_axis = plDrawLineOptions(PL_COLOR_32_BLUE, 0.06)

        extent = 10
        for i in range(-extent, extent + 1):
            # lines parallel to z
            plDrawI.add_3d_line(self.drawlist, plVec3(i, 0.0, -extent), plVec3(i, 0.0, extent), x_axis if i == 0 else thin)
            # lines parallel to x
            plDrawI.add_3d_line(self.drawlist, plVec3(-extent, 0.0, i), plVec3(extent, 0.0, i), z_axis if i == 0 else thin)

    def draw_scene(self):

        dl = self.drawlist

        white   = plDrawLineOptions(PL_COLOR_32_WHITE, 0.04)
        yellow  = plDrawLineOptions(PL_COLOR_32_YELLOW, 0.04)
        cyan    = plDrawLineOptions(PL_COLOR_32_CYAN, 0.04)
        magenta = plDrawLineOptions(PL_COLOR_32_MAGENTA, 0.04)
        orange  = plDrawLineOptions(PL_COLOR_32_ORANGE, 0.04)
        green   = plDrawLineOptions(PL_COLOR_32_GREEN, 0.03)

        red_solid     = plDrawSolidOptions(PL_COLOR_32_RED)
        green_solid   = plDrawSolidOptions(PL_COLOR_32_GREEN)
        blue_solid    = plDrawSolidOptions(PL_COLOR_32_BLUE)
        cyan_solid    = plDrawSolidOptions(PL_COLOR_32_CYAN)
        orange_solid  = plDrawSolidOptions(PL_COLOR_32_ORANGE)
        magenta_solid = plDrawSolidOptions(PL_COLOR_32_MAGENTA)
        yellow_solid  = plDrawSolidOptions(PL_COLOR_32_YELLOW)
        grey_solid    = plDrawSolidOptions(PL_COLOR_32_GREY)

        # text & markers
        plDrawI.add_3d_text(dl, plVec3(0.0, 5.0, 0.0), "3D Text @ (0.0, 5.0, 0.0)", plDrawTextOptions(self.ptFont, 18.0, PL_COLOR_32_CYAN))
        plDrawI.add_3d_cross(dl, plVec3(0.0, 5.0, 0.0), 0.1, cyan)
        plDrawI.add_3d_transform(dl, plMat4(), 1.5, white)
        plDrawI.add_3d_cross(dl, plVec3(-4.0, 1.0, -2.0), 0.75, yellow)

        # wire boxes
        plDrawI.add_3d_centered_box(dl, plVec3(-4.0, 1.0, 1.0), 1.5, 2.0, 1.0, cyan)
        plDrawI.add_3d_aabb(dl, plVec3(-5.0, 0.25, 3.0), plVec3(-3.0, 2.0, 4.5), magenta)

        # curves
        plDrawI.add_3d_bezier_quad(dl, plVec3(-1.0, 0.25, -4.0), plVec3(0.0, 3.0, -4.0), plVec3(1.0, 0.25, -4.0), 32, orange)
        plDrawI.add_3d_bezier_cubic(dl, plVec3(2.0, 0.25, -4.0), plVec3(3.0, 3.0, -4.0), plVec3(4.0, -1.0, -4.0), plVec3(5.0, 2.0, -4.0), 48, cyan)

        # frustum
        frustum_desc = plDrawFrustumDesc(math.radians(60.0), 16.0 / 9.0, 0.25, 2.5)
        plDrawI.add_3d_frustum(dl, plMat4(), frustum_desc, green)

        # filled shapes
        plDrawI.add_3d_triangle_filled(dl, plVec3(-1.0, 0.05, 1.0), plVec3(0.0, 1.5, 1.0), plVec3(1.0, 0.05, 1.0), yellow_solid)
        plDrawI.add_3d_centered_box_filled(dl, plVec3(3.0, 0.75, 0.0), 1.5, 1.5, 1.5, blue_solid)
        plDrawI.add_3d_plane_xz_filled(dl, plVec3(0.0, -0.01, 0.0), 5.0, 5.0, grey_solid)
        plDrawI.add_3d_plane_xy_filled(dl, plVec3(-2.5, 1.25, 3.0), 1.5, 1.5, magenta_solid)
        plDrawI.add_3d_plane_yz_filled(dl, plVec3(2.5, 1.25, 3.0), 1.5, 1.5, cyan_solid)
        plDrawI.add_3d_circle_xz_filled(dl, plVec3(-3.0, 0.03, -2.0), 0.75, 48, green_solid)
        plDrawI.add_3d_band_xz_filled(dl, plVec3(-3.0, 0.06, -4.0), 0.45, 0.90, 64, orange_solid)
        plDrawI.add_3d_band_xy_filled(dl, plVec3(0.0, 1.25, 3.0), 0.45, 0.85, 48, red_solid)
        plDrawI.add_3d_band_yz_filled(dl, plVec3(0.0, 1.25, 4.5), 0.45, 0.85, 48, green_solid)
        plDrawI.add_3d_sphere_filled(dl, plSphere(0.85, plVec3(0.0, 1.0, -1.5)), 16, 32, cyan_solid)
        plDrawI.add_3d_cylinder_filled(dl, plCylinder(plVec3(4.0, 0.0, -3.0), plVec3(4.0, 2.0, -3.0), 0.45), 32, orange_solid)
        plDrawI.add_3d_cone_filled(dl, plCone(plVec3(5.5, 0.0, -3.0), plVec3(5.5, 2.0, -3.0), 0.65), 32, magenta_solid)

        # wire shapes
        plDrawI.add_3d_circle_xz(dl, plVec3(-5.5, 0.05, -2.0), 0.85, 64, green)
        plDrawI.add_3d_sphere(dl, plSphere(0.85, plVec3(0.0, 1.0, -3.5)), 12, 24, cyan)
        plDrawI.add_3d_capsule(dl, plCapsule(plVec3(2.0, 0.5, -3.5), plVec3(2.0, 2.5, -3.5), 0.45), 12, 24, white)
        plDrawI.add_3d_cylinder(dl, plCylinder(plVec3(4.0, 0.0, -5.0), plVec3(4.0, 2.0, -5.0), 0.45), 32, orange)
        plDrawI.add_3d_cone(dl, plCone(plVec3(5.5, 0.0, -5.0), plVec3(5.5, 2.0, -5.0), 0.65), 32, white)

    #-------------------------------------------------------------------------
    # 2D drawing demo (drawn directly on the main view's foreground layer)
    #-------------------------------------------------------------------------

    def draw_drawing_demo(self, layer):
        x = 40.0
        y = 80.0

        # backdrop so the demo stays readable over the 3D scene
        plDrawI.add_rect_rounded_filled(layer, [x - 20.0, y - 15.0], [x + 430.0, y + 715.0], 8.0, 0,
                                        plDrawRectFlag.PL_DRAW_RECT_FLAG_NONE, plDrawSolidOptions(COLOR_PANEL))

        y = self.draw_stroke_modes(layer, x, y)
        y = self.draw_path_api(layer, x, y)
        y = self.draw_filled_shapes(layer, x, y)
        y = self.draw_rounded_rects(layer, x, y)

    def label(self, layer, x, y, text):
        plDrawI.add_text(layer, [x, y], text, plDrawTextOptions(self.ptFont, 0.0, PL_COLOR_32_WHITE))

    def draw_stroke_modes(self, layer, x, y):

        flags = 0
        if self.square:
            flags |= plDrawStrokeFlag.PL_DRAW_STROKE_CAP_SQUARE
        if self.miter:
            flags |= plDrawStrokeFlag.PL_DRAW_STROKE_JOIN_MITER
        if self.no_aa:
            flags |= plDrawStrokeFlag.PL_DRAW_STROKE_NO_AA

        # red hairlines mark the actual path boundary, not the stroke's outside edge
        guide = plDrawLineOptions(COLOR_GUIDE, 0.5)

        for mode, name in STROKE_MODES:

            self.label(layer, x, y, name)

            px = math.floor(x) + 12.0 + self.offset
            py = math.floor(y + 20.0) + 12.0 + self.offset
            options = plDrawLineOptions(COLOR_STROKE, self.thickness, mode | flags)

            # rect + guide
            plDrawI.add_rect(layer, [px, py], [px + 70.0, py + 44.0], guide)
            plDrawI.add_rect(layer, [px, py], [px + 70.0, py + 44.0], options)

            # circles on half and whole pixel centers
            plDrawI.add_circle(layer, [px + 110.5, py + 22.5], 7.5, 0, options)
            plDrawI.add_circle(layer, [px + 150.0, py + 22.0], 8.0, 0, options)

            # open polyline using the path api (shows joins & caps)
            plDrawI.path_line_to(layer, [px + 190.0, py + 44.0])
            plDrawI.path_line_to(layer, [px + 220.0, py])
            plDrawI.path_line_to(layer, [px + 224.0, py + 44.0])
            plDrawI.path_line_to(layer, [px + 270.0, py + 22.0])
            plDrawI.path_stroke(layer, options)

            # axis aligned lines
            plDrawI.add_line_h(layer, px + 300.0, px + 370.0, py + 10.0, options)
            plDrawI.add_line_v(layer, px + 335.0, py + 26.0, py + 48.0, options)

            y += 96.0

        return y

    def draw_path_api(self, layer, x, y):

        self.label(layer, x, y, "Path API: rounded rect, rotated ellipse,\nquadratic + cubic curves, convex fill")
        px = x + 12.0
        py = y + 48.0

        # rounded rectangle outline (closed stroke)
        plDrawI.path_rect(layer, [px, py], [px + 80.0, py + 50.0], 12.0, 0, plDrawRectFlag.PL_DRAW_RECT_FLAG_NONE)
        plDrawI.path_stroke(layer, plDrawLineOptions(COLOR_PATH, self.thickness, plDrawStrokeFlag.PL_DRAW_STROKE_CLOSED))

        # rotated ellipse (convex fill)
        plDrawI.path_ellipse_to(layer, [px + 125.0, py + 25.0], [30.0, 15.0], 0.4, 0.0, 2.0 * math.pi, 0)
        plDrawI.path_fill_convex(layer, plDrawSolidOptions(COLOR_ELLIPSE))

        # quadratic then cubic bezier in one open path
        plDrawI.path_line_to(layer, [px + 180.0, py + 45.0])
        plDrawI.path_bezier_quad_to(layer, [px + 205.0, py - 30.0], [px + 230.0, py + 25.0], 0)
        plDrawI.path_bezier_cubic_to(layer, [px + 250.0, py + 80.0], [px + 290.0, py - 40.0], [px + 330.0, py + 25.0], 0)
        plDrawI.path_stroke(layer, plDrawLineOptions(COLOR_PATH, self.thickness))

        return y + 126.0

    def draw_filled_shapes(self, layer, x, y):

        self.label(layer, x, y, "Filled triangle and quad: geometry AA")
        px = x + 12.0
        py = y + 32.0

        plDrawI.add_triangle_filled(layer, [px, py], [px + 60.0, py + 12.0], [px + 20.0, py + 45.0], plDrawSolidOptions(COLOR_FILLED))
        plDrawI.add_quad_filled(layer, [px + 90.0, py], [px + 150.0, py + 12.0], [px + 140.0, py + 45.0], [px + 100.0, py + 35.0], plDrawSolidOptions(COLOR_FILLED))

        return y + 90.0

    def draw_rounded_rects(self, layer, x, y):

        self.label(layer, x, y, "Rounded rects: baked (left), explicit (right)")
        px = math.floor(x) + 12.0
        py = math.floor(y + 20.0) + 12.0

        for i in range(2):
            a = [px + i * 190.0, py]
            b = [px + i * 190.0 + 150.0, py + 50.0]
            segments = 16 if i else 0
            plDrawI.add_rect_rounded_filled(layer, a, b, self.rounding, segments, plDrawRectFlag.PL_DRAW_RECT_FLAG_NONE, plDrawSolidOptions(COLOR_RECT))
            plDrawI.add_rect_rounded(layer, a, b, self.rounding, segments, plDrawRectFlag.PL_DRAW_RECT_FLAG_NONE,
                                     plDrawLineOptions(COLOR_PATH, self.thickness, plDrawStrokeFlag.PL_DRAW_STROKE_INSIDE))

        return y + 100.0

# run app
pl_run(App())
