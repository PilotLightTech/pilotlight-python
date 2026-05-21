import os
from pathlib import Path

# core
import pilotlight.pilotlight as pl
from pilotlight.pilotlight import *
from pilotlight.imgui import *
from pilotlight.enums import *
from pilotlight.types import *

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
        self.ptFont = None

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
        plVfsI.mount_directory("/cache", str(Path.cwd()) + "/../cache")
        plVfsI.mount_directory("/shaders", os.path.dirname(os.path.abspath(pl.__file__)) + "/shaders")
        plVfsI.mount_directory("/shader-temp", str(Path.cwd()) + "/../shader-temp")
        plVfsI.mount_directory("/assets", str(Path.cwd()) + "/../../pilotlight/assets")

        # Create and show the OS window.
        window_desc = plWindowDesc()
        window_desc.pcTitle = "Pilot Light Python - Basic Example 0"
        window_desc.iXPos = 100
        window_desc.iYPos = 100
        window_desc.uWidth = 1280
        window_desc.uHeight = 720
        _, self.ptWindow = plWindowI.create(window_desc)
        plWindowI.show(self.ptWindow)

        # Initialize the starter extension.
        # We explicitly disable the shader ext from the starter flags because
        # we are going to initialize the shader system ourselves below.
        starter_flags = plStarterFlag.PL_STARTER_FLAGS_ALL_EXTENSIONS
        starter_flags &= ~plStarterFlag.PL_STARTER_FLAGS_SHADER_EXT
        starter_flags |= plStarterFlag.PL_STARTER_FLAGS_MSAA

        plStarterI.initialize(self.ptWindow, starter_flags)

        # Initialize shader system.
        # This is required even for simple drawing examples because the
        # rendering path may still rely on shader compilation/setup.
        shader_options = plShaderOptions()
        shader_options.pcCacheOutputDirectory = "/shader-temp/"
        shader_options.apcDirectories = ["/shaders/"]
        shader_options.apcIncludeDirectories = ["/shaders/"]
        shader_options.tFlags = (
            plShaderFlags.PL_SHADER_FLAGS_AUTO_OUTPUT
            | plShaderFlags.PL_SHADER_FLAGS_INCLUDE_DEBUG
            | plShaderFlags.PL_SHADER_FLAGS_ALWAYS_COMPILE
        )

        plShaderI.initialize(shader_options)

        # Complete starter initialization after custom systems are ready.
        plStarterI.finalize()

        ptFontAtlas = plDrawI.get_current_font_atlas()
        self.ptFont = plDrawI.get_first_font(ptFontAtlas)

        self.ptDevice = plStarterI.get_device()
        resourceManagerInit = plResourceManagerInit()
        resourceManagerInit.ptDevice = self.ptDevice
        plResourceI.initialize(resourceManagerInit)

        self.texture_resource = plResourceI.load("/assets/core/textures/sprite_map.png", 0)
        self.texture_handle = plResourceI.get_texture(self.texture_resource)
        self.texture_bg = plDrawI.create_bind_group_for_texture(self.texture_handle)

    def pl_app_shutdown(self):
        """
        Called once when the app exits.
        Flush GPU work, clean up engine systems, then destroy the window.
        """

        plGraphicsI.flush_device(plStarterI.get_device())
        plResourceI.cleanup()
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
        
        plResourceI.new_frame()

        # Foreground fgLayer is a convenient draw list for 2D overlay-style
        # rendering.
        fgLayer = plStarterI.get_foreground_layer()

        plDrawI.add_bezier_quad(
            fgLayer,
            [120.0, 420.0],   # start
            [260.0, 320.0],   # control
            [420.0, 450.0],   # end
            0,                # segment count (0 = automatic/default)
            plDrawLineOptions(PL_COLOR_32_BLUE)
        )

        # Cubic bezier with thicker line
        plDrawI.add_bezier_cubic(
            fgLayer,
            [120.0, 560.0],   # start
            [220.0, 460.0],   # control 1
            [360.0, 660.0],   # control 2
            [460.0, 540.0],   # end
            0,                # segment count
            plDrawLineOptions(PL_COLOR_32_RED, 3.0)
        )

        plDrawI.add_image(fgLayer, self.texture_bg, [0, 0], [500, 500])

        # Submit/present the frame.
        plStarterI.end_frame()


# Run the application.
pl_run(App())