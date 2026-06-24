########################################################################################################################
# Pilot Light - 2D App Template
########################################################################################################################


import os
from pathlib import Path

# pilot light stuff
import pilotlight.pilotlight as pl
from pilotlight.pilotlight import *
from pilotlight.imgui import *
from pilotlight.enums import *
from pilotlight.types import *

class App:

    def __init__(self):
        self.show_imgui_demo = False
        self.show_implot_demo = False
        self.some_string_array = "pizza"

    # -------------------------------------------------------------------------
    # Application lifetime
    # -------------------------------------------------------------------------

    def pl_app_load(self): # called on load

        # mount directories used by the shader extension
        file_directory = os.path.dirname(os.path.abspath(__file__))
        plVfsI.mount_directory("/shaders", file_directory + "/../shaders")
        plVfsI.mount_directory("/cache", file_directory + "/../cache")
        plVfsI.mount_directory("/shader-temp", file_directory + "/../shader-temp")
        plVfsI.mount_directory("/assets", file_directory + "/../assets")

        # create & show the OS window
        window_desc = plWindowDesc()
        window_desc.pcTitle = "Pilot Light - 2D App Template"
        window_desc.iXPos = 100
        window_desc.iYPos = 100
        window_desc.uWidth = 1280
        window_desc.uHeight = 720
        _, self.ptWindow = plWindowI.create(window_desc)
        plWindowI.show(self.ptWindow)

        # initialize starter extension
        starterInit = plStarterInit(plStarterFlag.PL_STARTER_FLAGS_ALL_EXTENSIONS | plStarterFlag.PL_STARTER_FLAGS_MSAA, self.ptWindow)

        # we need to handle the shader extension since we need to find
        # the shaders provided with the python package
        starterInit.eFlags &= ~plStarterFlag.PL_STARTER_FLAGS_SHADER_EXT
        starterInit.eFlags &= ~plStarterFlag.PL_STARTER_FLAGS_DRAW_EXT
        plStarterI.initialize(starterInit)

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

        # shader system
        shader_options = plShaderOptions()
        shader_options.pcCacheOutputDirectory = "/shader-temp/"
        shader_options.apcDirectories = ["/shaders/"]
        shader_options.apcIncludeDirectories = ["/shaders/"]
        shader_options.eFlags = plShaderFlags.PL_SHADER_FLAGS_AUTO_OUTPUT

        plShaderI.initialize(shader_options)

        # finish starter setup (since we are done setting up shader extension)
        plStarterI.finalize()

        ptCmdBuffer = plStarterI.get_raw_command_buffer() # not recording
        plDrawI.build_font_atlas(ptCmdBuffer, self.ptFontAtlas) # actually record
        plStarterI.return_raw_command_buffer(ptCmdBuffer) # will submit & wait

        tRenderAttachmentInfo = plRenderAttachmentInfo()
        plStarterI.get_render_attachment_info(tRenderAttachmentInfo) # out parameter
        plDearImGuiI.initialize(plStarterI.get_device(), plStarterI.get_swapchain(), tRenderAttachmentInfo)

    def pl_app_shutdown(self): # called on shutdown
        plGraphicsI.flush_device(plStarterI.get_device())
        plDearImGuiI.cleanup()
        plDrawI.return_2d_layer(self.ptFGLayer)
        plDrawI.return_2d_drawlist(self.drawlist)
        plDrawI.cleanup_font_atlas(self.ptFontAtlas)
        plDrawI.cleanup()
        plStarterI.cleanup()
        plWindowI.destroy(self.ptWindow)

    def pl_app_resize(self): # called on resize
        plStarterI.resize()

    def pl_app_update(self): # called every frame

        if not plStarterI.begin_frame():
            return

        plDrawI.new_frame()
        plDearImGuiI.new_frame(plStarterI.get_device())

        if self.show_imgui_demo:
            self.show_imgui_demo = ImGui.ShowDemoWindow(self.show_imgui_demo)
        
        if self.show_implot_demo:
            self.show_implot_demo = ImPlot.ShowDemoWindow(self.show_implot_demo)

        # outer frame
        plDrawI.add_rect(
            self.ptFGLayer,
            [40.0, 40.0],
            [1240.0, 680.0],
            plDrawLineOptions(PL_COLOR_32_YELLOW, 2.0)
        )

        # text
        plDrawI.add_text(self.ptFGLayer, [300, 300], "Hello Python", plDrawTextOptions(self.ptFont, 18.0, PL_COLOR_32_CYAN))

        # io API
        if plIOI.is_key_pressed(plKey.PL_KEY_P):
            print("P key pressed!")

        # ui API
        if plUiI.begin_window("Debug Window"):

            _, self.some_string_array = plUiI.input_text("Input", self.some_string_array)
            if plUiI.button("Press me"):
                print("Button Pressed")

            if plUiI.button("Add Message"):
                plScreenLogI.add_message(1.0, "Logging from python!")

            _, self.show_imgui_demo = plUiI.checkbox("Show ImGui Demo", self.show_imgui_demo)
            _, self.show_implot_demo = plUiI.checkbox("Show ImPlot Demo", self.show_implot_demo)

            plUiI.end_window()

        # dear imgui
        if ImGui.BeginMainMenuBar():
            if ImGui.BeginMenu("File"):
                ImGui.EndMenu()
            if ImGui.BeginMenu("Edit", False):
                ImGui.EndMenu()
            if ImGui.BeginMenu("Tools"):
                _, self.show_imgui_demo = ImGui.MenuItem("Show ImGui Demo", "", self.show_imgui_demo)
                _, self.show_implot_demo = ImGui.MenuItem("Show ImPlot Demo", "", self.show_implot_demo)
                ImGui.EndMenu()
            if ImGui.BeginMenu("Help"):
                ImGui.MenuItemSimple("Check For Update")
                ImGui.MenuItemSimple("About", "-a")
                ImGui.EndMenu()
            ImGui.EndMainMenuBar()
        if ImGui.Begin("ImGui Window"):
            if ImGui.Button("Press Me"):
                print("Pressed Imgui Button")
        ImGui.End()

        # begin render pass
        cmdBuffer = plStarterI.begin_main_pass()

        plDrawI.submit_2d_layer(self.ptFGLayer)
        io = plIOI.get_io()
        tMainViewportSize = io.tMainViewportSize
        tRenderAttachmentInfo = plRenderAttachmentInfo()
        plStarterI.get_render_attachment_info(tRenderAttachmentInfo)
        plDrawI.submit_2d_drawlist(self.drawlist, cmdBuffer, tMainViewportSize.x, tMainViewportSize.y, plGraphicsI.get_swapchain_info(plStarterI.get_swapchain()).eSampleCount, tRenderAttachmentInfo)
        plDearImGuiI.render(cmdBuffer)

        plStarterI.end_main_pass()

        plStarterI.end_frame()

# let pilot light run application
pl_run(App())