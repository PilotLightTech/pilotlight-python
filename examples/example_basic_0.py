import os

# core package
import pilotlight.pilotlight as pl

# core APIs
from pilotlight.pl_core import plWindowI
from pilotlight.pl_core import *

# stable extension APIs
from pilotlight.pl_starter_ext import *
from pilotlight.pl_draw_ext import *
from pilotlight.pl_vfs_ext import *
from pilotlight.pl_shader_ext import *
from pilotlight.pl_graphics_ext import *


class App:
    """
    Simple Pilot Light Python example showing how to:
      - create a window
      - initialize the starter/shader systems
      - draw basic 2D primitives every frame
      - organize drawing code in a readable way
    """

    def __init__(self):
        self.ptWindow = None

    # -------------------------------------------------------------------------
    # Application lifetime
    # -------------------------------------------------------------------------

    def pl_app_load(self):
        """
        Called once when the app starts.
        Set up virtual file system mounts, create a window, and initialize
        the Pilot Light systems we want to use.
        """

        # Mount directories used by the shader system.
        # /shaders points at the Python package shader folder.
        # /shader-temp is where compiled/intermediate shader output can go.
        plVfsI.mount_directory("/cache", "cache")
        plVfsI.mount_directory(
            "/shaders",
            os.path.dirname(os.path.abspath(pl.__file__)) + "/shaders"
        )
        plVfsI.mount_directory("/shader-temp", "shader-temp")

        # Create and show the OS window.
        self.ptWindow = plWindowI.create(
            "Pilot Light Python - Drawing API Example",
            100, 100,   # x, y
            1280, 720,  # width, height
            0
        )
        plWindowI.show(self.ptWindow)

        # Initialize the starter extension.
        # We explicitly disable the shader ext from the starter flags because
        # we are going to initialize the shader system ourselves below.
        starter_flags = PL_STARTER_FLAGS_ALL_EXTENSIONS
        starter_flags &= ~PL_STARTER_FLAGS_SHADER_EXT
        starter_flags |= PL_STARTER_FLAGS_MSAA

        plStarterI.initialize(self.ptWindow, starter_flags)

        # Initialize shader system.
        # This is required even for simple drawing examples because the
        # rendering path may still rely on shader compilation/setup.
        shader_options = plShaderOptions()
        shader_options.pcCacheOutputDirectory = "/shader-temp/"
        shader_options.apcDirectories = ["/shaders/"]
        shader_options.apcIncludeDirectories = ["/shaders/"]
        shader_options.tFlags = (
            PL_SHADER_FLAGS_AUTO_OUTPUT
            | PL_SHADER_FLAGS_INCLUDE_DEBUG
            | PL_SHADER_FLAGS_ALWAYS_COMPILE
        )

        plShaderI.initialize(shader_options)

        # Complete starter initialization after custom systems are ready.
        plStarterI.finalize()

    def pl_app_shutdown(self):
        """
        Called once when the app exits.
        Flush GPU work, clean up engine systems, then destroy the window.
        """

        plGraphicsI.flush_device(plStarterI.get_device())
        plStarterI.cleanup()
        plWindowI.destroy(self.ptWindow)

    def pl_app_resize(self):
        """
        Called when the window changes size.
        Let the starter extension rebuild any size-dependent resources.
        """
        plStarterI.resize()

    # -------------------------------------------------------------------------
    # Per-frame update
    # -------------------------------------------------------------------------

    def pl_app_update(self):
        """
        Called once per frame.
        This is where all drawing for the frame happens.
        """

        # Begin the frame. If it returns False, skip rendering this frame.
        if not plStarterI.begin_frame():
            return

        # Foreground fgLayer is a convenient draw list for 2D overlay-style
        # rendering.
        fgLayer = plStarterI.get_foreground_layer()

        # Outer frame
        plDrawI.add_rect(
            fgLayer,
            [40.0, 40.0],
            [1240.0, 680.0],
            color=PL_COLOR_32_YELLOW,
            thickness=2.0
        )

        # Diagonal guide line
        plDrawI.add_line(
            fgLayer,
            [40.0, 40.0],
            [1240.0, 680.0],
            color=PL_COLOR_32_GREEN,
            thickness=2.0
        )

        # Rounded panel region
        plDrawI.add_rect_rounded(
            fgLayer,
            [70.0, 70.0],
            [500.0, 300.0],
            16.0,                   # rounding radius
            0,                      # segment count (0 = automatic/default)
            PL_DRAW_RECT_FLAG_NONE,
            color=PL_COLOR_32_CYAN,
            thickness=2.0
        )

        # Triangle outline
        plDrawI.add_triangle(
            fgLayer,
            [120.0, 240.0],
            [240.0, 120.0],
            [320.0, 260.0],
            color=PL_COLOR_32_WHITE
        )

        # Arbitrary quad outline
        plDrawI.add_quad(
            fgLayer,
            [580.0, 100.0],
            [760.0, 120.0],
            [720.0, 260.0],
            [540.0, 220.0],
            color=PL_COLOR_32_ORANGE
        )

        # Circle outline
        plDrawI.add_circle(
            fgLayer,
            [930.0, 180.0],
            70.0,
            0,  # segment count (0 = automatic/default)
            color=PL_COLOR_32_BLUE
        )

        # Polygon outline
        plDrawI.add_polygon(
            fgLayer,
            [
                [1040.0, 110.0],
                [1140.0, 90.0],
                [1200.0, 150.0],
                [1180.0, 250.0],
                [1070.0, 260.0],
                [1010.0, 180.0],
            ],
            color=PL_COLOR_32_RED
        )

        plDrawI.add_bezier_quad(
            fgLayer,
            [120.0, 420.0],   # start
            [260.0, 320.0],   # control
            [420.0, 450.0],   # end
            0,                # segment count (0 = automatic/default)
            color=PL_COLOR_32_BLUE
        )

        # Cubic bezier with thicker line
        plDrawI.add_bezier_cubic(
            fgLayer,
            [120.0, 560.0],   # start
            [220.0, 460.0],   # control 1
            [360.0, 660.0],   # control 2
            [460.0, 540.0],   # end
            0,                # segment count
            color=PL_COLOR_32_RED,
            thickness=3.0
        )

        # Filled triangle
        plDrawI.add_triangle_filled(
            fgLayer,
            [640.0, 420.0],
            [820.0, 360.0],
            [760.0, 560.0]
        )

        # Filled-looking composition using multiple primitives
        plDrawI.add_line(
            fgLayer,
            [900.0, 360.0],
            [1180.0, 360.0],
            color=PL_COLOR_32_WHITE,
            thickness=4.0
        )

        plDrawI.add_rect(
            fgLayer,
            [920.0, 390.0],
            [1160.0, 560.0],
            color=PL_COLOR_32_WHITE,
            thickness=3.0
        )

        plDrawI.add_circle(
            fgLayer,
            [1040.0, 475.0],
            50.0,
            0,
            color=PL_COLOR_32_GREEN
        )

        # Submit/present the frame.
        plStarterI.end_frame()


# Run the application.
pl.run(App())