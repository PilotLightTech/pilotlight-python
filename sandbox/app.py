import os

# core
import pilotlight.pilotlight as pl
from pilotlight.pilotlight import *
from pilotlight.enums import *
from pilotlight.imgui import *
from pilotlight.types import *

class App:

    def __init__(self):
        self.ptWindow = None
        self.counter = None
        self.show_imgui_demo = None
        self.show_implot_demo = None
        self.tMainCamera = None
        self.ptComponentLibrary = None
        self.some_string_array = bytearray("pizza", 'utf-8')
        self.some_string_array.resize(256)

    def pl_app_load(self):

        self.show_imgui_demo = pl_create_bool_pointer()
        self.show_implot_demo = pl_create_bool_pointer()

        plVfsI.mount_directory("/cache", "cache")
        plVfsI.mount_directory("/shaders", os.path.dirname(os.path.abspath(pl.__file__)) + "/shaders")
        plVfsI.mount_directory("/shader-temp", "shader-temp")


        window_desc = plWindowDesc()
        window_desc.pcTitle = "Python Example"
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

        self.counter = plStatsI.get_counter("python counter")

        # mod = plShaderI.load_glsl("draw_3d.frag", "main")
        # plShaderI.write_to_disk("C:/dev/pilotlight-python/sandbox/blah.spv", mod)

        # (result, pakFile) = pl_pak_begin_packing("C:/dev/pilotlight-python/sandbox/shaders.pak", 2)
        # result = pl_pak_add_from_disk(pakFile, "shaders.pak", "C:/dev/pilotlight-python/sandbox/blah.spv", False)
        # pl_pak_end_packing(pakFile)

        plShaderVariantI.initialize(plStarterI.get_device())

        plRendererI.initialize(plStarterI.get_device(), plStarterI.get_swapchain())
        

        plEcsI.initialize()
        plRendererEcsI.register_system()
        plScriptI.register_ecs_system()
        plCameraI.register_ecs_system()
        plAnimationI.register_ecs_system()
        plMeshI.register_ecs_system()
        plPhysicsI.register_ecs_system()
        plMaterialI.register_ecs_system()
        plEcsI.finalize()
        self.ptComponentLibrary = plEcsI.get_default_library()

        self.tMainCamera = plCameraI.create_perspective(
            self.ptComponentLibrary,
            "main camera",
            [-4.012, 2.984, -1.109],
            1.04719755,
            500 / 500,
            0.1,
            30.0,
            True
            )
        
        camera = plEcsI.get_component(self.ptComponentLibrary, plCameraI.get_ecs_type_key(), self.tMainCamera)
        plCameraI.set_fov(camera, 1.04719755)
        plCameraI.update(camera)

        plRendererEcsI.create_directional_light(self.ptComponentLibrary, "direction light")

        ImGui.StyleColorsDark()


    def pl_app_shutdown(self):
        plGraphicsI.flush_device(plStarterI.get_device())
        plEcsI.cleanup()
        plRendererI.cleanup()
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

        if pl_get_pointer_value(self.show_imgui_demo):
            ImGui.ShowDemoWindow(self.show_imgui_demo)
        
        if pl_get_pointer_value(self.show_implot_demo):
            ImPlot.ShowDemoWindow(self.show_implot_demo)
     
        # drawing API
        fgLayer = plStarterI.get_foreground_layer()
        plDrawI.add_triangle_filled(fgLayer, [50.0, 100.0], [200.0, 0.0], [100.0, 200.0], plDrawSolidOptions(PL_COLOR_32_GREEN))
        plDrawI.add_triangle(fgLayer, plVec2(50.0, 300.0), plVec2(200.0, 200.0), plVec2(100.0, 400.0), plDrawLineOptions(PL_COLOR_32_WHITE))

        # io API
        if plIOI.is_key_pressed(plKey.PL_KEY_P):
            print("P key pressed!")

        # ui API
        if plUiI.begin_window("Debug Window"):

            if plUiI.input_text("Input", self.some_string_array):
                print("String changed")
            if plUiI.button("Press me"):
                print("Button Pressed")
                current_value = pl_get_pointer_value(self.counter)
                current_value += 1
                pl_set_pointer_value(self.counter, current_value)

            if plUiI.button("Add Message"):
                plScreenLogI.add_message(1.0, "Logging from python!")

            plUiI.checkbox("Show ImGui Demo", pointer=self.show_imgui_demo)

            bCurrentValue = pl_get_pointer_value(self.show_implot_demo)
            bChanged, bCurrentValue = plUiI.checkbox("Show ImPlot Demo", bCurrentValue)
            if bChanged:
                pl_set_pointer_value(self.show_implot_demo, bCurrentValue)

            plUiI.end_window()

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
        if ImGui.Begin("ImGui Window"):
            if ImGui.Button("Press Me"):
                print("Pressed Imgui Button")
        ImGui.End()

        render_encoder = plStarterI.begin_main_pass()

        plDearImGuiI.render(render_encoder)

        plStarterI.end_main_pass()

        plStarterI.end_frame()

# run app
pl_run(App())