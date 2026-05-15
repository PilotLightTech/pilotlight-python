import os

# core
import pilotlight.pilotlight as pl
from pilotlight.pilotlight import *
from pilotlight.imgui import *
from pilotlight.enums import *
from pilotlight.types import *

def show_example_menu_file():
    static = show_example_menu_file

    ImGui.MenuItem("(demo menu)", "", False, False)
    if ImGui.MenuItem("New"):
        pass
    if ImGui.MenuItem("Open", "Ctrl+O"):
        pass
    if ImGui.BeginMenu("Open Recent"):
        ImGui.MenuItem("Hello")
        ImGui.MenuItem("Sailor")
        if ImGui.BeginMenu("Recurse.."):
            show_example_menu_file()
            ImGui.EndMenu()
        ImGui.EndMenu()
    if ImGui.MenuItem("Save", "Ctrl+S"):
        pass
    if ImGui.MenuItem("Save As.."):
        pass
    ImGui.Separator()
    if ImGui.BeginMenu("Options"):
        if not hasattr(static, "enabled"):
            static.enabled = True
        _, static.enabled = ImGui.MenuItem("Enabled", "", static.enabled)
        ImGui.BeginChild("child", (0, 60), ImGuiChildFlags.Borders)
        for i in range(0, 10):
            ImGui.Text("Scrolling Text %d" % i)
        ImGui.EndChild()
        if not hasattr(static, "f"):
            static.f = pl_create_float_pointer()
            static.n = pl_create_int_pointer()
        ImGui.SliderFloat("Value", static.f, 0.0, 1.0)
        ImGui.InputFloat("Input", static.f, 0.1)
        items = ["Yes", "No", "Maybe"]
        ImGui.Combo("Combo", static.n, items)
        ImGui.EndMenu()

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

        plVfsI.mount_directory("/cache", "cache")
        plVfsI.mount_directory("/shaders", os.path.dirname(os.path.abspath(pl.__file__)) + "/shaders")
        plVfsI.mount_directory("/shader-temp", "shader-temp")

        window_desc = plWindowDesc()
        window_desc.pcTitle = "Demo"
        _, self.ptWindow = plWindowI.create(window_desc)
        plWindowI.show(self.ptWindow)

        starter_flags = plStarterFlag.PL_STARTER_FLAGS_ALL_EXTENSIONS
        starter_flags |= plStarterFlag.PL_STARTER_FLAGS_MSAA
        starter_flags &= ~plStarterFlag.PL_STARTER_FLAGS_SHADER_EXT
        plStarterI.initialize(self.ptWindow, starter_flags)

        shader_options = plShaderOptions()
        shader_options.pcCacheOutputDirectory = "/shader-temp/"
        shader_options.apcDirectories = ["/shaders/"]
        shader_options.apcIncludeDirectories = ["/shaders/"]
        shader_options.tFlags = plShaderFlags.PL_SHADER_FLAGS_AUTO_OUTPUT | plShaderFlags.PL_SHADER_FLAGS_INCLUDE_DEBUG | plShaderFlags.PL_SHADER_FLAGS_ALWAYS_COMPILE
        plShaderI.initialize(shader_options)

        plStarterI.finalize()
        plDearImGuiI.initialize(plStarterI.get_device(), plStarterI.get_swapchain(), plStarterI.get_render_pass())
        plShaderVariantI.initialize(plStarterI.get_device())
        
        ImGui.StyleColorsDark()


    def pl_app_shutdown(self):
        plGraphicsI.flush_device(plStarterI.get_device())
        plDearImGuiI.cleanup()
        plStarterI.cleanup()
        plWindowI.destroy(self.ptWindow)

    def pl_app_resize(self):
        plStarterI.resize()

    def pl_app_update(self):

        
        if not plStarterI.begin_frame():
            return
        
        plDearImGuiI.new_frame(plStarterI.get_device(), plStarterI.get_render_pass())

        if pl_get_pointer_value(self.show_imgui_demo):
            ImGui.ShowDemoWindow(self.show_imgui_demo)
        
        if pl_get_pointer_value(self.show_implot_demo):
            ImPlot.ShowDemoWindow(self.show_implot_demo)
     
        # dear imgui API
        if ImGui.BeginMainMenuBar():
            if ImGui.BeginMenu("File"):
                ImGui.EndMenu()
            if ImGui.BeginMenu("Edit", False):
                ImGui.EndMenu()
            if ImGui.BeginMenu("Tools"):
                ImGui.MenuItem("Show ImGui Demo", selected_pointer=self.show_imgui_demo)
                ImGui.MenuItem("Show ImPlot Demo", selected_pointer=self.show_implot_demo)
                ImGui.EndMenu()
            if ImGui.BeginMenu("Help"):
                ImGui.MenuItem("Check For Update")
                ImGui.MenuItem("About", "-a")
                ImGui.EndMenu()
            ImGui.EndMainMenuBar()


        if ImGui.Begin("Dear ImGui Demo", None, ImGuiWindowFlags.MenuBar):
            if ImGui.BeginMenuBar():
                if ImGui.BeginMenu("Menu"):
                    show_example_menu_file()
                    ImGui.EndMenu()
                ImGui.EndMenuBar()
        ImGui.End()


        render_encoder = plStarterI.begin_main_pass()

        plDearImGuiI.render(render_encoder)

        plStarterI.end_main_pass()

        plStarterI.end_frame()

# run app
pl_run(App())