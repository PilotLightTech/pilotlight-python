import os

# core
import pilotlight.pilotlight as pl

# core apis
from pilotlight.pl_core import plCoreI
from pilotlight.pl_core import plIOI
from pilotlight.pl_core import plWindowI
from pilotlight.pl_core import * # constants

# extension apis
from pilotlight.pl_starter_ext import *
from pilotlight.pl_draw_ext import *
from pilotlight.pl_ui_ext import *
from pilotlight.pl_vfs_ext import *
from pilotlight.pl_shader_ext import *
from pilotlight.pl_pak_ext import *
from pilotlight.pl_dearimgui_ext import *
from pilotlight.pl_graphics_ext import *
from pilotlight.pl_stats_ext import *
from pilotlight.pl_screen_log_ext import *

class App:

    def __init__(self):
        self.ptWindow = None
        self.counter = None
        self.show_imgui_demo = None
        self.show_implot_demo = None

    def pl_app_load(self):

        self.show_imgui_demo = plCoreI.create_bool_pointer()
        self.show_implot_demo = plCoreI.create_bool_pointer()

        plVfsI.mount_directory("/cache", "cache")
        plVfsI.mount_directory("/shaders", os.path.dirname(os.path.abspath(pl.__file__)) + "/shaders")
        plVfsI.mount_directory("/shader-temp", "shader-temp")

        self.ptWindow = plWindowI.create("Python Example", 200, 200, 500, 500, 0)
        plWindowI.show(self.ptWindow)

        starter_flags = PL_STARTER_FLAGS_ALL_EXTENSIONS
        starter_flags &= ~PL_STARTER_FLAGS_SHADER_EXT
        plStarterI.initialize(self.ptWindow, starter_flags)

        shader_options = plShaderOptions()
        shader_options.pcCacheOutputDirectory = "/shader-temp/"
        shader_options.apcDirectories = ["/shaders/"]
        shader_options.apcIncludeDirectories = ["/shaders/"]
        shader_options.tFlags = PL_SHADER_FLAGS_AUTO_OUTPUT | PL_SHADER_FLAGS_INCLUDE_DEBUG | PL_SHADER_FLAGS_ALWAYS_COMPILE
        plShaderI.initialize(shader_options)

        plStarterI.finalize()

        plDearImGuiI.initialize(plStarterI.get_device(), plStarterI.get_swapchain(), plStarterI.get_render_pass())

        self.counter = plStatsI.get_counter("python counter")

        # mod = plShaderI.load_glsl("draw_3d.frag", "main")
        # plShaderI.write_to_disk("C:/dev/pilotlight-python/sandbox/blah.spv", mod)

        # (result, pakFile) = plPakI.begin_packing("C:/dev/pilotlight-python/sandbox/shaders.pak", 2)
        # result = plPakI.add_from_disk(pakFile, "shaders.pak", "C:/dev/pilotlight-python/sandbox/blah.spv", False)
        # plPakI.end_packing(pakFile)


    def pl_app_shutdown(self):
        plGraphicsI.flush_device(plStarterI.get_device())
        plDearImGuiI.cleanup()
        plStarterI.cleanup()
        plWindowI.destroy(self.ptWindow)

    def pl_app_resize(self):

        print("resizing")
        plStarterI.resize()

    def pl_app_update(self):

        if not plStarterI.begin_frame():
            return
        
        plDearImGuiI.new_frame(plStarterI.get_device(), plStarterI.get_render_pass())

        if plCoreI.get_pointer_value(self.show_imgui_demo):
            ImGui.ShowDemoWindow(self.show_imgui_demo)
        
        if plCoreI.get_pointer_value(self.show_implot_demo):
            ImPlot.ShowDemoWindow(self.show_implot_demo)
        
        # drawing API
        fgLayer = plStarterI.get_foreground_layer()
        plDrawI.add_triangle_filled(fgLayer, [50.0, 100.0], [200.0, 0.0], [100.0, 200.0])
        plDrawI.add_triangle_filled(fgLayer, [50.0, 300.0], [200.0, 200.0], [100.0, 400.0], uColor = 4278255360)

        # io API
        if plIOI.is_key_pressed(PL_KEY_P):
            print("P key pressed!")

        # ui API
        if plUiI.begin_window("Debug Window"):

            if plUiI.button("Press me"):
                print("Button Pressed")
                current_value = plCoreI.get_pointer_value(self.counter)
                current_value += 1
                plCoreI.set_pointer_value(self.counter, current_value)

            if plUiI.button("Add Message"):
                plScreenLogI.add_message(1.0, "Logging from python!")

            plUiI.checkbox("Show ImGui Demo", pointer=self.show_imgui_demo)

            bCurrentValue = plCoreI.get_pointer_value(self.show_implot_demo)
            bChanged, bCurrentValue = plUiI.checkbox("Show ImPlot Demo", bCurrentValue)
            if bChanged:
                plCoreI.set_pointer_value(self.show_implot_demo, bCurrentValue)

            plUiI.end_window()

        render_encoder = plStarterI.begin_main_pass()

        plDearImGuiI.render(render_encoder)

        plStarterI.end_main_pass()

        plStarterI.end_frame()

# run app
pl.run(App())