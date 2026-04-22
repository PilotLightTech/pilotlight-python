import os

# core
import pilotlight.pilotlight as pl
from pilotlight.pilotlight import *
from pilotlight.imgui import *
from pilotlight.enums import *
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

        pl_vfs_mount_directory("/cache", "cache")
        pl_vfs_mount_directory("/shaders", os.path.dirname(os.path.abspath(pl.__file__)) + "/shaders")
        pl_vfs_mount_directory("/shader-temp", "shader-temp")

        self.ptWindow = pl_window_create("Python Example", 200, 200, 500, 500, 0)
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

        self.counter = pl_stats_get_counter("python counter")

        # mod = pl_shader_load_glsl("draw_3d.frag", "main")
        # pl_shader_write_to_disk("C:/dev/pilotlight-python/sandbox/blah.spv", mod)

        # (result, pakFile) = pl_pak_begin_packing("C:/dev/pilotlight-python/sandbox/shaders.pak", 2)
        # result = pl_pak_add_from_disk(pakFile, "shaders.pak", "C:/dev/pilotlight-python/sandbox/blah.spv", False)
        # pl_pak_end_packing(pakFile)

        pl_shader_variant_initialize(pl_starter_get_device())

        pl_renderer_initialize(pl_starter_get_device(), pl_starter_get_swapchain())
        

        pl_ecs_initialize()
        pl_renderer_register_ecs_system()
        pl_script_register_ecs_system()
        pl_camera_register_ecs_system()
        pl_animation_register_ecs_system()
        pl_mesh_register_ecs_system()
        pl_physics_register_ecs_system()
        pl_material_register_ecs_system()
        pl_ecs_finalize()
        self.ptComponentLibrary = pl_ecs_get_default_library()

        self.tMainCamera = pl_camera_create_perspective(
            self.ptComponentLibrary,
            "main camera",
            [-4.012, 2.984, -1.109],
            1.04719755,
            500 / 500,
            0.1,
            30.0,
            True
            )
        
        camera = pl_ecs_get_component(self.ptComponentLibrary, pl_camera_get_ecs_type_key(), self.tMainCamera)
        pl_camera_set_fov(camera, 1.04719755)
        pl_camera_update(camera)

        pl_renderer_create_directional_light(self.ptComponentLibrary, "direction light")

        ImGui_StyleColorsDark()


    def pl_app_shutdown(self):
        pl_graphics_flush_device(pl_starter_get_device())
        pl_ecs_cleanup()
        pl_renderer_cleanup()
        pl_dear_imgui_cleanup()
        pl_starter_cleanup()
        pl_window_destroy(self.ptWindow)

    def pl_app_resize(self):

        print("resizing")
        pl_starter_resize()

    def pl_app_update(self):

        
        if not pl_starter_begin_frame():
            return
        
        pl_dear_imgui_new_frame(pl_starter_get_device(), pl_starter_get_render_pass())

        if pl_get_pointer_value(self.show_imgui_demo):
            ImGui_ShowDemoWindow(self.show_imgui_demo)
        
        if pl_get_pointer_value(self.show_implot_demo):
            ImPlot_ShowDemoWindow(self.show_implot_demo)
     
        # drawing API
        fgLayer = pl_starter_get_foreground_layer()
        pl_draw_add_triangle_filled(fgLayer, [50.0, 100.0], [200.0, 0.0], [100.0, 200.0])
        pl_draw_add_triangle(fgLayer, [50.0, 300.0], [200.0, 200.0], [100.0, 400.0], color = PL_COLOR_32_WHITE)

        # io API
        if pl_io_is_key_pressed(plKey.PL_KEY_P):
            print("P key pressed!")

        # ui API
        if pl_ui_begin_window("Debug Window"):

            if pl_ui_input_text("Input", self.some_string_array):
                print("String changed")
            if pl_ui_button("Press me"):
                print("Button Pressed")
                current_value = pl_get_pointer_value(self.counter)
                current_value += 1
                pl_set_pointer_value(self.counter, current_value)

            if pl_ui_button("Add Message"):
                pl_screen_log_add_message(1.0, "Logging from python!")

            pl_ui_checkbox("Show ImGui Demo", pointer=self.show_imgui_demo)

            bCurrentValue = pl_get_pointer_value(self.show_implot_demo)
            bChanged, bCurrentValue = pl_ui_checkbox("Show ImPlot Demo", bCurrentValue)
            if bChanged:
                pl_set_pointer_value(self.show_implot_demo, bCurrentValue)

            pl_ui_end_window()

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
        if ImGui_Begin("ImGui Window"):
            if ImGui_Button("Press Me"):
                print("Pressed Imgui Button")
        ImGui_End()

        render_encoder = pl_starter_begin_main_pass()

        pl_dear_imgui_render(render_encoder)

        pl_starter_end_main_pass()

        pl_starter_end_frame()

# run app
pl_run(App())