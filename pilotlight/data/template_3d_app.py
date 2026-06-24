########################################################################################################################
# Pilot Light - 3D App Template
#
#
# Controls:
#   W/S/A/D : move camera
#   R/F     : move camera up/down
#   LMB drag: rotate camera
#
########################################################################################################################

import os
import math
from pathlib import Path

# pilot light stuff
import pilotlight.pilotlight as pl
from pilotlight.pilotlight import *
from pilotlight.imgui import *
from pilotlight.enums import *
from pilotlight.types import *
from pilotlight.pl_script_camera import pl_script_run

class App:

    def __init__(self):
        pass

    def pl_app_load(self): # called on load

        # mount directories used by the shader extension
        file_directory = os.path.dirname(os.path.abspath(__file__))
        plVfsI.mount_directory("/shaders", file_directory + "/../shaders")
        plVfsI.mount_directory("/cache", file_directory + "/../cache")
        plVfsI.mount_directory("/shader-temp", file_directory + "/../shader-temp")
        plVfsI.mount_directory("/assets", file_directory + "/../assets")

        # Create window.
        window_desc = plWindowDesc()
        window_desc.pcTitle = "Pilot Light - 3D App Template"
        window_desc.iXPos = 100
        window_desc.iYPos = 100
        window_desc.uWidth = 1280
        window_desc.uHeight = 720

        _, self.ptWindow = plWindowI.create(window_desc)
        plWindowI.show(self.ptWindow)

        # initialize starter extension
        starterInit = plStarterInit(plStarterFlag.PL_STARTER_FLAGS_ALL_EXTENSIONS | plStarterFlag.PL_STARTER_FLAGS_MSAA | plStarterFlag.PL_STARTER_FLAGS_DEPTH_BUFFER, self.ptWindow)

        # we need to handle the shader extension since we need to find
        # the shaders provided with the python package
        starterInit.eFlags &= ~plStarterFlag.PL_STARTER_FLAGS_SHADER_EXT
        starterInit.eFlags &= ~plStarterFlag.PL_STARTER_FLAGS_DRAW_EXT
        plStarterI.initialize(starterInit)

        # shader system
        shader_options = plShaderOptions()
        shader_options.pcCacheOutputDirectory = "/shader-temp/"
        shader_options.apcDirectories = ["/shaders/"]
        shader_options.apcIncludeDirectories = ["/shaders/"]
        shader_options.eFlags = plShaderFlags.PL_SHADER_FLAGS_AUTO_OUTPUT

        plShaderI.initialize(shader_options)

        # finish starter setup (since we are done setting up shader extension)
        plStarterI.finalize()

        # setup camera
        self.camera = plCamera()
        plCameraI.init(self.camera)
        tCameraDesc = plCameraPerspectiveDesc()
        tCameraDesc.eDepthMode = plCameraDepthMode.PL_CAMERA_DEPTH_MODE_STANDARD
        tCameraDesc.fAspectRatio = 1280.0 / 720.0
        tCameraDesc.fNearZ = 0.1
        tCameraDesc.fFarZ = 100.0
        tCameraDesc.fYFov = math.radians(60.0)
        plCameraI.set_perspective(self.camera, tCameraDesc)
        plCameraI.set_position(self.camera, [-10.0, 4.0, -10.0])
        plCameraI.rotate_euler(self.camera, -0.295, 0.7, 0.0)
        plCameraI.update(self.camera)

        self.drawlist = plDrawI.request_3d_drawlist()
        ptFontAtlas = plDrawI.get_current_font_atlas()
        self.ptFont = plDrawI.get_first_font(ptFontAtlas)

    def pl_app_shutdown(self): # called on shutdown

        plGraphicsI.flush_device(plStarterI.get_device()) # ensure GPU is finished
        plDrawI.return_3d_drawlist(self.drawlist)
        plStarterI.cleanup()
        plWindowI.destroy(self.ptWindow)

    def pl_app_resize(self): # called on resize
        plStarterI.resize()
        tIO = plIOI.get_io()
        plCameraI.set_viewport(self.camera, tIO.tMainViewportSize.x, tIO.tMainViewportSize.y)

    def pl_app_update(self): # called every frame

        if not plStarterI.begin_frame():
            return

        ptIO = plIOI.get_io()

        pl_script_run(self.camera) # camera FPS control 

        # 2D overlay
        fgLayer = plStarterI.get_foreground_layer()

        overlay = (
            "3D App Template\n"
            "Unreal Camera Controls\n"
            f"FPS: {ptIO.fFrameRate:.1f}"
        )

        plDrawI.add_text(
            fgLayer,
            [16.0, 16.0],
            overlay,
            plDrawTextOptions(self.ptFont, 18.0, PL_COLOR_32_CYAN)
        )

        # draw grid
        thin = plDrawLineOptions(PL_COLOR_32_GREY, 0.03)
        x_axis = plDrawLineOptions(PL_COLOR_32_RED, 0.06)
        z_axis = plDrawLineOptions(PL_COLOR_32_BLUE, 0.06)

        extent = 10
        spacing = 1.0

        for i in range(-extent, extent + 1):
            x = i * spacing
            z = i * spacing

            # lines parallel to z
            plDrawI.add_3d_line(self.drawlist,
                plVec3(x, 0.0, -extent * spacing),
                plVec3(x, 0.0,  extent * spacing),
                x_axis if i == 0 else thin)

            # lines parallel to x
            plDrawI.add_3d_line(self.drawlist,
                plVec3(-extent * spacing, 0.0, z),
                plVec3( extent * spacing, 0.0, z),
                z_axis if i == 0 else thin)

        # begin render pass
        cmdBuffer = plStarterI.begin_main_pass()

        tMainViewportSize = ptIO.tMainViewportSize

        tRenderAttachmentInfo = plRenderAttachmentInfo()
        plStarterI.get_render_attachment_info(tRenderAttachmentInfo)
        
        plDrawI.submit_3d_drawlist(
            self.drawlist,
            cmdBuffer,
            tMainViewportSize.x,
            tMainViewportSize.y,
            self.camera.tViewProjMat,
            plDrawFlag.PL_DRAW_FLAG_DEPTH_TEST | plDrawFlag.PL_DRAW_FLAG_DEPTH_WRITE,
            plGraphicsI.get_swapchain_info(plStarterI.get_swapchain()).eSampleCount,
            tRenderAttachmentInfo
        )

        plStarterI.end_main_pass()

        plStarterI.end_frame()

# let pilot light run application
pl_run(App())