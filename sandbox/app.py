import os

# core
import pilotlight.pilotlight as pl

# core apis
from pilotlight.pl_core import plCoreI
from pilotlight.pl_core import plIOI
from pilotlight.pl_core import plWindowI
from pilotlight.pl_core import * # constants

# stable extension apis
from pilotlight.pl_starter_ext import *
from pilotlight.pl_draw_ext import *
from pilotlight.pl_ui_ext import *
from pilotlight.pl_vfs_ext import *
from pilotlight.pl_shader_ext import *
from pilotlight.pl_pak_ext import *
from pilotlight.pl_stats_ext import *
from pilotlight.pl_screen_log_ext import *
from pilotlight.pl_ecs_ext import *
from pilotlight.pl_graphics_ext import *
from pilotlight.pl_compress_ext import *
from pilotlight.pl_config_ext import *
from pilotlight.pl_console_ext import *
from pilotlight.pl_dds_ext import *
from pilotlight.pl_dxt_ext import *
from pilotlight.pl_datetime_ext import *
from pilotlight.pl_gpu_allocators_ext import *
from pilotlight.pl_image_ext import *
from pilotlight.pl_job_ext import *
from pilotlight.pl_log_ext import *
from pilotlight.pl_platform_ext import *
from pilotlight.pl_profile_ext import *
from pilotlight.pl_rect_pack_ext import *
from pilotlight.pl_resource_ext import *
from pilotlight.pl_string_intern_ext import *
from pilotlight.pl_tools_ext import *

# unstable extension apis
from pilotlight.pl_dearimgui_ext import *
from pilotlight.pl_animation_ext import *
from pilotlight.pl_bvh_ext import *
from pilotlight.pl_camera_ext import *
from pilotlight.pl_collision_ext import *
from pilotlight.pl_ecs_tools_ext import *
from pilotlight.pl_freelist_ext import *
from pilotlight.pl_gizmo_ext import *
from pilotlight.pl_image_ops_ext import *
from pilotlight.pl_material_ext import *
from pilotlight.pl_mesh_ext import *
from pilotlight.pl_model_loader_ext import *
from pilotlight.pl_physics_ext import *
from pilotlight.pl_renderer_ext import *
from pilotlight.pl_script_ext import *
from pilotlight.pl_shader_variant_ext import *
from pilotlight.pl_terrain_ext import *
from pilotlight.pl_terrain_processor_ext import *


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

        plShaderVariantI.initialize(plStarterI.get_device())

        plRendererI.initialize(plStarterI.get_device(), plStarterI.get_swapchain())

        plEcsI.initialize()
        plRendererI.register_ecs_system()
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

        plRendererI.create_directional_light(self.ptComponentLibrary, "direction light")

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

            if plUiI.input_text("Input", self.some_string_array):
                print("String changed")
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
pl.run(App())