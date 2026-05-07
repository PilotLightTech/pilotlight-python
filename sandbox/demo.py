import os

# core
import pilotlight.pilotlight as pl
from pilotlight.pilotlight import *
from pilotlight.imgui import *
from pilotlight.enums import *
from pilotlight.types import *

def show_example_menu_file():
    static = show_example_menu_file

    ImGui_MenuItem("(demo menu)", "", False, False)
    if ImGui_MenuItem("New"):
        pass
    if ImGui_MenuItem("Open", "Ctrl+O"):
        pass
    if ImGui_BeginMenu("Open Recent"):
        ImGui_MenuItem("Hello")
        ImGui_MenuItem("Sailor")
        if ImGui_BeginMenu("Recurse.."):
            show_example_menu_file()
            ImGui_EndMenu()
        ImGui_EndMenu()
    if ImGui_MenuItem("Save", "Ctrl+S"):
        pass
    if ImGui_MenuItem("Save As.."):
        pass
    ImGui_Separator()
    if ImGui_BeginMenu("Options"):
        if not hasattr(static, "enabled"):
            static.enabled = True
        _, static.enabled = ImGui_MenuItem("Enabled", "", static.enabled)
        ImGui_BeginChild("child", (0, 60), ImGuiChildFlags.Borders)
        for i in range(0, 10):
            ImGui_Text("Scrolling Text %d" % i)
        ImGui_EndChild()
        if not hasattr(static, "f"):
            static.f = pl_create_float_pointer()
            static.n = pl_create_int_pointer()
        ImGui_SliderFloat("Value", static.f, 0.0, 1.0)
        ImGui_InputFloat("Input", static.f, 0.1)
        items = ["Yes", "No", "Maybe"]
        ImGui_Combo("Combo", static.n, items)
        ImGui_EndMenu()

class App:

    def __init__(self):
        self.ptWindow = None
        self.show_imgui_demo = None
        self.show_implot_demo = None
        self.some_string_array = bytearray("pizza", 'utf-8')
        self.some_string_array.resize(256)

    def pl_app_load(self):

        self.show_imgui_demo = pl_create_bool_pointer()
        self.show_implot_demo = pl_create_bool_pointer()

        pl_vfs_mount_directory("/cache", "cache")
        pl_vfs_mount_directory("/shaders", os.path.dirname(os.path.abspath(pl.__file__)) + "/shaders")
        pl_vfs_mount_directory("/shader-temp", "shader-temp")

        self.ptWindow = pl_window_create("Demo", 200, 200, 500, 500, 0)
        pl_window_show(self.ptWindow)

        starter_flags = plStarterFlag.PL_STARTER_FLAGS_ALL_EXTENSIONS
        starter_flags |= plStarterFlag.PL_STARTER_FLAGS_MSAA
        starter_flags &= ~plStarterFlag.PL_STARTER_FLAGS_SHADER_EXT
        pl_starter_initialize(self.ptWindow, starter_flags)

        shader_options = plShaderOptions()
        shader_options.pcCacheOutputDirectory = "/shader-temp/"
        shader_options.apcDirectories = ["/shaders/"]
        shader_options.apcIncludeDirectories = ["/shaders/"]
        shader_options.tFlags = plShaderFlags.PL_SHADER_FLAGS_AUTO_OUTPUT | plShaderFlags.PL_SHADER_FLAGS_INCLUDE_DEBUG | plShaderFlags.PL_SHADER_FLAGS_ALWAYS_COMPILE
        pl_shader_initialize(shader_options)

        pl_starter_finalize()
        pl_dear_imgui_initialize(pl_starter_get_device(), pl_starter_get_swapchain(), pl_starter_get_render_pass())
        pl_shader_variant_initialize(pl_starter_get_device())
        
        ImGui_StyleColorsDark()


    def pl_app_shutdown(self):
        pl_graphics_flush_device(pl_starter_get_device())
        pl_dear_imgui_cleanup()
        pl_starter_cleanup()
        pl_window_destroy(self.ptWindow)

    def pl_app_resize(self):
        pl_starter_resize()

    def pl_app_update(self):

        
        if not pl_starter_begin_frame():
            return
        
        pl_dear_imgui_new_frame(pl_starter_get_device(), pl_starter_get_render_pass())

        if pl_get_pointer_value(self.show_imgui_demo):
            ImGui_ShowDemoWindow(self.show_imgui_demo)
        
        if pl_get_pointer_value(self.show_implot_demo):
            ImPlot_ShowDemoWindow(self.show_implot_demo)
     
        # dear imgui API
        if ImGui_BeginMainMenuBar():
            if ImGui_BeginMenu("File"):
                ImGui_EndMenu()
            if ImGui_BeginMenu("Edit", False):
                ImGui_EndMenu()
            if ImGui_BeginMenu("Tools"):
                ImGui_MenuItem("Show ImGui Demo", selected_pointer=self.show_imgui_demo)
                ImGui_MenuItem("Show ImPlot Demo", selected_pointer=self.show_implot_demo)
                ImGui_EndMenu()
            if ImGui_BeginMenu("Help"):
                ImGui_MenuItem("Check For Update")
                ImGui_MenuItem("About", "-a")
                ImGui_EndMenu()
            ImGui_EndMainMenuBar()


        if ImGui_Begin("Dear ImGui Demo", None, ImGuiWindowFlags.MenuBar):
            if ImGui_BeginMenuBar():
                if ImGui_BeginMenu("Menu"):
                    show_example_menu_file()
                    ImGui_EndMenu()
                ImGui_EndMenuBar()
        ImGui_End()


        render_encoder = pl_starter_begin_main_pass()

        pl_dear_imgui_render(render_encoder)

        pl_starter_end_main_pass()

        pl_starter_end_frame()

# run app
pl_run(App())