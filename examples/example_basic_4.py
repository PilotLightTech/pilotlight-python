import os
import math
from pathlib import Path

# core
import pilotlight.pilotlight as pl
from pilotlight.pilotlight import *
from pilotlight.enums import *
from pilotlight.types import *

# stroke modes shown in the demo (one row each)
STROKE_MODES = [
    (plDrawStrokeFlag.PL_DRAW_STROKE_NONE,           "Centered"),
    (plDrawStrokeFlag.PL_DRAW_STROKE_CENTER_BIASED,  "Center biased"),
    (plDrawStrokeFlag.PL_DRAW_STROKE_INSIDE,         "Inside (right of path)"),
    (plDrawStrokeFlag.PL_DRAW_STROKE_OUTSIDE,        "Outside (left of path)"),
]

# colors (ABGR, same as the C demo)
COLOR_STROKE  = 0xc0f0b060
COLOR_GUIDE   = 0x807070ff
COLOR_PATH    = 0xff80d0ff
COLOR_ELLIPSE = 0x8080d0ff
COLOR_FILLED  = 0xc080d0ff
COLOR_RECT    = 0xff805030

class App:
    """
    Pilot Light Python example showing the 2D drawing features from the
    "Drawing" section of the Pilot Light UI demo (stroke modes, caps & joins,
    the path API, geometry anti-aliasing, and rounded rectangles).

    The settings are UI widgets, but all of the drawing goes straight to the
    starter extension's foreground layer (screen space, outside of the UI).

    Compare with MSAA turned off to see the stroke/geometry anti-aliasing.
    """

    def __init__(self):
        self.ptWindow = None
        self.ptFont = None

        # settings (controlled by the UI)
        self.thickness = 3.0
        self.offset = 0.0
        self.square = False
        self.miter = False
        self.no_aa = False
        self.rounding = 6.0

    # -------------------------------------------------------------------------
    # Application lifetime
    # -------------------------------------------------------------------------

    def pl_app_load(self):

        # Mount directories used by the shader system.
        plVfsI.mount_directory("/cache", str(Path.cwd()) + "/../cache")
        plVfsI.mount_directory("/shaders", os.path.dirname(os.path.abspath(pl.__file__)) + "/shaders")
        plVfsI.mount_directory("/shader-temp", str(Path.cwd()) + "/../shader-temp")

        # Create and show the OS window.
        window_desc = plWindowDesc()
        window_desc.pcTitle = "Pilot Light Python - Drawing Example"
        window_desc.iXPos = 100
        window_desc.iYPos = 100
        window_desc.uWidth = 1280
        window_desc.uHeight = 800
        _, self.ptWindow = plWindowI.create(window_desc)
        plWindowI.show(self.ptWindow)

        # Initialize the starter extension (we initialize the shader extension
        # ourselves so it can find the shaders shipped with the python package).
        starterInit = plStarterInit(plStarterFlag.PL_STARTER_FLAGS_ALL_EXTENSIONS, self.ptWindow)
        starterInit.eFlags &= ~plStarterFlag.PL_STARTER_FLAGS_SHADER_EXT
        plStarterI.initialize(starterInit)

        shader_options = plShaderOptions()
        shader_options.apcDirectories = ["/shaders/"]
        shader_options.apcIncludeDirectories = ["/shaders/"]
        shader_options.eFlags = plShaderFlags.PL_SHADER_FLAGS_AUTO_OUTPUT
        plShaderI.initialize(shader_options)

        plStarterI.finalize()

        self.ptFont = plDrawI.get_first_font(plDrawI.get_current_font_atlas())

    def pl_app_shutdown(self):
        plGraphicsI.flush_device(plStarterI.get_device())
        plStarterI.cleanup()
        plWindowI.destroy(self.ptWindow)

    def pl_app_resize(self):
        plStarterI.resize()

    # -------------------------------------------------------------------------
    # Per-frame update
    # -------------------------------------------------------------------------

    def pl_app_update(self):

        if not plStarterI.begin_frame():
            return

        self.show_controls()

        layer = plStarterI.get_foreground_layer()
        x = 40.0
        y = 30.0
        y = self.draw_stroke_modes(layer, x, y)
        y = self.draw_path_api(layer, x, y)
        y = self.draw_filled_shapes(layer, x, y)
        y = self.draw_rounded_rects(layer, x, y)

        plStarterI.end_frame()

    # -------------------------------------------------------------------------
    # UI (settings only)
    # -------------------------------------------------------------------------

    def show_controls(self):

        io = plIOI.get_io()
        plUiI.set_next_window_pos([io.tMainViewportSize.x - 420.0, 30.0], plUiCondition.PL_UI_COND_ONCE)
        plUiI.set_next_window_size([390.0, 290.0], plUiCondition.PL_UI_COND_ONCE)
        if plUiI.begin_window("Drawing Settings"):

            plUiI.text("Compare without MSAA.")
            plUiI.text("Coordinates are never implicitly snapped.")

            # MSAA changes are queued until the next begin_frame
            msaa = plGraphicsI.get_swapchain_info(plStarterI.get_swapchain()).eSampleCount != 1
            changed, msaa = plUiI.checkbox("MSAA", msaa)
            if changed:
                if msaa:
                    plStarterI.activate_msaa()
                else:
                    plStarterI.deactivate_msaa()

            _, self.thickness = plUiI.slider_float("Stroke width", self.thickness, 0.25, 12.0)
            _, self.offset = plUiI.slider_float("Subpixel offset", self.offset, 0.0, 1.0)
            _, self.square = plUiI.checkbox("Square caps", self.square)
            _, self.miter = plUiI.checkbox("Force miter joins", self.miter)
            _, self.no_aa = plUiI.checkbox("Disable stroke AA", self.no_aa)
            plUiI.separator()
            _, self.rounding = plUiI.slider_float("Corner radius", self.rounding, 1.0, 16.0)

            plUiI.end_window()

    # -------------------------------------------------------------------------
    # Drawing (directly to the foreground layer)
    # -------------------------------------------------------------------------

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

        self.label(layer, x, y, "Path API: rounded rectangle, rotated ellipse, quadratic + cubic curves, convex fill")
        px = x + 12.0
        py = y + 32.0

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

        return y + 110.0

    def draw_filled_shapes(self, layer, x, y):

        self.label(layer, x, y, "Filled triangle and quad: geometry AA (no MSAA required)")
        px = x + 12.0
        py = y + 32.0

        plDrawI.add_triangle_filled(layer, [px, py], [px + 60.0, py + 12.0], [px + 20.0, py + 45.0], plDrawSolidOptions(COLOR_FILLED))
        plDrawI.add_quad_filled(layer, [px + 90.0, py], [px + 150.0, py + 12.0], [px + 140.0, py + 45.0], [px + 100.0, py + 35.0], plDrawSolidOptions(COLOR_FILLED))

        return y + 90.0

    def draw_rounded_rects(self, layer, x, y):

        self.label(layer, x, y, "Rounded rectangles: automatic/baked (left), explicit geometry (right)")
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


# Run the application.
pl_run(App())
