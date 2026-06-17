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
      - manually handle drawing extension
      - draw basic 2D primitives every frame
      - load font
      - organize drawing code in a readable way
    """

    def __init__(self):
        pass

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
        window_desc.pcTitle = "Pilot Light Python - Basic Example 1"
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
        starter_flags &= ~plStarterFlag.PL_STARTER_FLAGS_DRAW_EXT
        # starter_flags |= plStarterFlag.PL_STARTER_FLAGS_MSAA

        plStarterI.initialize(self.ptWindow, starter_flags)

        ptDevice = plStarterI.get_device()
        tDrawInit = plDrawInit(ptDevice)
        plDrawI.initialize(tDrawInit)

        self.ptFontAtlas = plDrawI.create_font_atlas()
        plDrawI.set_font_atlas(self.ptFontAtlas)

        tFontRange = plFontRange(0x0020, 0x00FF - 0x0020)

        tFontConfig = plFontConfig()
        tFontConfig.fSize = 18.0
        tFontConfig.uHOverSampling = 1
        tFontConfig.uVOverSampling = 1
        tFontConfig.ptRanges = [tFontRange]
        self.ptFont = plDrawI.add_font_from_file_ttf(self.ptFontAtlas, tFontConfig, "/assets/core/fonts/Cousine-Regular.ttf");

        self.drawlist = plDrawI.request_2d_drawlist()
        self.ptFGLayer = plDrawI.request_2d_layer(self.drawlist)

        # Initialize shader system.
        # This is required even for simple drawing examples because the
        # rendering path may still rely on shader compilation/setup.
        shader_options = plShaderOptions()
        shader_options.pcCacheOutputDirectory = "/shader-temp/"
        shader_options.apcDirectories = ["/shaders/"]
        shader_options.apcIncludeDirectories = ["/shaders/"]
        shader_options.eFlags = (
            plShaderFlags.PL_SHADER_FLAGS_AUTO_OUTPUT
            | plShaderFlags.PL_SHADER_FLAGS_INCLUDE_DEBUG
            | plShaderFlags.PL_SHADER_FLAGS_ALWAYS_COMPILE
        )

        plShaderI.initialize(shader_options)

        # Complete starter initialization after custom systems are ready.
        plStarterI.finalize()

        ptCmdBuffer = plStarterI.get_raw_command_buffer() # not recording
        plDrawI.build_font_atlas(ptCmdBuffer, self.ptFontAtlas) # actually record, submit, & wait
        plStarterI.return_raw_command_buffer(ptCmdBuffer)

    def pl_app_shutdown(self):
        """
        Called once when the app exits.
        Flush GPU work, clean up engine systems, then destroy the window.
        """

        plGraphicsI.flush_device(plStarterI.get_device())
        plDrawI.return_2d_layer(self.ptFGLayer)
        plDrawI.return_2d_drawlist(self.drawlist)
        plDrawI.cleanup_font_atlas(self.ptFontAtlas)
        plDrawI.cleanup()
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

        plDrawI.new_frame()

        # Foreground fgLayer is a convenient draw list for 2D overlay-style
        # rendering.
        fgLayer = plStarterI.get_foreground_layer()

        # Outer frame
        plDrawI.add_rect(
            self.ptFGLayer,
            [40.0, 40.0],
            [1240.0, 680.0],
            plDrawLineOptions(PL_COLOR_32_YELLOW, 2.0)
        )

        # text
        plDrawI.add_text(self.ptFGLayer, [300, 300], "Hello Python", plDrawTextOptions(self.ptFont, 18.0, PL_COLOR_32_CYAN))

        cmdBuffer = plStarterI.begin_main_pass()

        plDrawI.submit_2d_layer(self.ptFGLayer)
        io = plIOI.get_io()
        tMainViewportSize = io.tMainViewportSize
        tRenderAttachmentInfo = plRenderAttachmentInfo()
        plStarterI.get_render_attachment_info(tRenderAttachmentInfo)
        plDrawI.submit_2d_drawlist(self.drawlist, cmdBuffer, tMainViewportSize.x, tMainViewportSize.y, 1, tRenderAttachmentInfo)

        plStarterI.end_main_pass()

        # Submit/present the frame.
        plStarterI.end_frame()


# Run the application.
pl_run(App())