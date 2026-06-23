import os
import math
from pathlib import Path

# core
import pilotlight.pilotlight as pl
from pilotlight.pilotlight import *
from pilotlight.enums import *
from pilotlight.imgui import *
from pilotlight.types import *
from pilotlight.pl_script_camera import pl_script_run


class App:

    def __init__(self):
        self.ptWindow = None
        self.bResize = False
        self.counter = None
        self.test_bool = True
        self.show_imgui_demo = False
        self.show_implot_demo = False
        self.tMainCamera = None
        self.some_string_array = "pizza"
        self.float_array = [1.0, 2.0, 3.0, 4.0]

    def pl_app_load(self):

        plVfsI.mount_directory("/cache", str(Path.cwd()) + "/../cache")
        plVfsI.mount_directory("/shaders", os.path.dirname(os.path.abspath(pl.__file__)) + "/shaders")
        plVfsI.mount_directory("/shader-temp", str(Path.cwd()) + "/../shader-temp")
        plVfsI.mount_directory("/assets", str(Path.cwd()) + "/../../pilotlight/assets")
        plVfsI.mount_directory("/environments", str(Path.cwd()) + "/../../pilotlight/assets/development/environments")
        plVfsI.mount_directory("/gltf-samples", str(Path.cwd()) + "/../../pilotlight/assets/gltf-samples/Models")

        plJobI.initialize()

        window_desc = plWindowDesc()
        window_desc.pcTitle = "Python Example"
        _, self.ptWindow = plWindowI.create(window_desc)
        plWindowI.show(self.ptWindow)

        starterInit = plStarterInit(plStarterFlag.PL_STARTER_FLAGS_ALL_EXTENSIONS | plStarterFlag.PL_STARTER_FLAGS_MSAA, self.ptWindow)
        starterInit.eFlags &= ~plStarterFlag.PL_STARTER_FLAGS_SHADER_EXT
        plStarterI.initialize(starterInit)

        shader_options = plShaderOptions()
        shader_options.pcCacheOutputDirectory = "/shader-temp/"
        shader_options.apcDirectories = ["/shaders/"]
        shader_options.apcIncludeDirectories = ["/shaders/"]
        shader_options.eFlags = plShaderFlags.PL_SHADER_FLAGS_AUTO_OUTPUT | plShaderFlags.PL_SHADER_FLAGS_INCLUDE_DEBUG | plShaderFlags.PL_SHADER_FLAGS_ALWAYS_COMPILE
        plShaderI.initialize(shader_options)

        plStarterI.finalize()

        tRenderAttachmentInfo = plRenderAttachmentInfo()
        plStarterI.get_render_attachment_info(tRenderAttachmentInfo)
        plDearImGuiI.initialize(plStarterI.get_device(), plStarterI.get_swapchain(), tRenderAttachmentInfo)

        self.counter = plStatsI.get_counter("python counter")

        plShaderVariantI.initialize(plStarterI.get_device())

        plRendererI.initialize(plStarterI.get_device(), plStarterI.get_swapchain())
        

        plEcsI.initialize()
        plRendererEcsI.register_system()
        plScriptI.register_ecs_system()
        plCameraEcsI.register_ecs_system()
        plAnimationI.register_ecs_system()
        plMeshI.register_ecs_system()
        plPhysicsI.register_ecs_system()
        plMaterialI.register_ecs_system()
        plEcsI.finalize()
        self.ptComponentLibrary = plEcsI.get_default_library()

        result, self.tMainCamera, self.ptScene, self.ptView = plRendererI.load_test_world("/assets/core/scenes/scene-humanoid.json", self.ptComponentLibrary)


        self.drawlist = plDrawI.request_2d_drawlist()
        self.ptFGLayer = plDrawI.request_2d_layer(self.drawlist)

        self.texture_resource = plResourceI.load("/assets/core/textures/sprite_map.png", 0)
        self.texture_handle = plResourceI.get_texture(self.texture_resource)
        self.texture_bg = plDrawI.create_bind_group_for_texture(self.texture_handle)
        self.texture_bg2 = plDearImGuiI.get_texture_id_from_bindgroup(plStarterI.get_device(), self.texture_bg)

        ImGui.StyleColorsDark()

    def pl_app_shutdown(self):
        plGraphicsI.flush_device(plStarterI.get_device())
        plEcsI.cleanup()
        plRendererI.cleanup()
        plDearImGuiI.cleanup()
        plStarterI.cleanup()
        plWindowI.destroy(self.ptWindow)

    def pl_app_resize(self):

        io = plIOI.get_io()
        camera = plEcsI.get_component(self.ptComponentLibrary, plCameraEcsI.get_ecs_type_key(), self.tMainCamera)
        plCameraI.set_viewport(camera, io.tMainViewportSize.x, io.tMainViewportSize.y)
        plCameraI.update(camera)
        plStarterI.resize()
        self.bResize = True

    def pl_app_update(self):

        io = plIOI.get_io()

        if not plStarterI.begin_frame():
            return
        
        plResourceI.new_frame()
        plRendererI.begin_frame()

        if self.bResize:
            plRendererI.resize_view(self.ptView, io.tMainViewportSize)
            self.bResize = False

        plDearImGuiI.new_frame(plStarterI.get_device())

        if self.show_imgui_demo:
            self.show_imgui_demo = ImGui.ShowDemoWindow(self.show_imgui_demo)
        
        if self.show_implot_demo:
            self.show_implot_demo = ImPlot.ShowDemoWindow(self.show_implot_demo)

        # script here
        ptCamera = plEcsI.get_component(self.ptComponentLibrary, plCameraEcsI.get_ecs_type_key(), self.tMainCamera)
        pl_script_run(ptCamera)
        plAnimationI.run_animation_update_system(self.ptComponentLibrary, io.fDeltaTime)
        plPhysicsI.update(io.fDeltaTime, self.ptComponentLibrary)
        plEcsI.run_transform_update_system(self.ptComponentLibrary)
        plEcsI.run_hierarchy_update_system(self.ptComponentLibrary)
        plRendererEcsI.run_light_update_system(self.ptComponentLibrary)
        plCameraEcsI.run_ecs(self.ptComponentLibrary)
        plAnimationI.run_inverse_kinematics_update_system(self.ptComponentLibrary)
        plRendererEcsI.run_skin_update_system(self.ptComponentLibrary)
        plRendererEcsI.run_object_update_system(self.ptComponentLibrary)
        plRendererEcsI.run_environment_probe_update_system(self.ptComponentLibrary)

        # drawing API
        fgLayer = plStarterI.get_foreground_layer()
        plDrawI.add_triangle_filled(fgLayer, [50.0, 100.0], [200.0, 0.0], [100.0, 200.0], plDrawSolidOptions(PL_COLOR_32_GREEN))
        plDrawI.add_triangle(fgLayer, plVec2(50.0, 300.0), plVec2(200.0, 200.0), plVec2(100.0, 400.0), plDrawLineOptions(PL_COLOR_32_WHITE))

        # io API
        if plIOI.is_key_pressed(plKey.PL_KEY_P):
            print("P key pressed!")

        # ui API
        if plUiI.begin_window("Debug Window"):

            _, self.some_string_array = plUiI.input_text("Input", self.some_string_array)
            if plUiI.button("Press me"):
                print("Button Pressed")
                current_value = pl_get_pointer_value(self.counter)
                current_value += 1
                pl_set_pointer_value(self.counter, current_value)

            if plUiI.button("Add Message"):
                plScreenLogI.add_message(1.0, "Logging from python!")

            _, self.show_imgui_demo = plUiI.checkbox("Show ImGui Demo", self.show_imgui_demo)
            _, self.show_implot_demo = plUiI.checkbox("Show ImPlot Demo", self.show_implot_demo)

            plUiI.end_window()

        # dear imgui API
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
            ImGui.DragFloat2("DragFloat2", self.float_array)
        ImGui.End()

        camera = plEcsI.get_component(self.ptComponentLibrary, plCameraEcsI.get_ecs_type_key(), self.tMainCamera)
        plRendererI.prepare_scene(self.ptScene, [camera])
        plRendererI.prepare_view(self.ptView, camera)
        plRendererI.render_view(self.ptView, camera)

        tUV = plVec2()
        
        uTexture, tUV.x, tUV.y = plRendererI.get_view_color_bind_group(self.ptView)
        plDrawI.add_image(self.ptFGLayer, uTexture, plVec2(), io.tMainViewportSize, plVec2(), tUV, PL_COLOR_32_WHITE)
        plDrawI.submit_2d_layer(self.ptFGLayer)

        if self.test_bool:
            _, self.test_bool = ImGui.Begin("Testing", open=self.test_bool)
            ImGui.Image(self.texture_bg2, [500, 500])
            ImGui.End()

        cmdBuffer = plStarterI.begin_main_pass()
        
        tRenderAttachmentInfo = plRenderAttachmentInfo()
        plStarterI.get_render_attachment_info(tRenderAttachmentInfo)
        plDrawI.submit_2d_drawlist(self.drawlist, cmdBuffer, io.tMainViewportSize.x, io.tMainViewportSize.y, plGraphicsI.get_swapchain_info(plStarterI.get_swapchain()).eSampleCount, tRenderAttachmentInfo)
        plDearImGuiI.render(cmdBuffer)

        plStarterI.end_main_pass()

        plStarterI.end_frame()

# run app
pl_run(App())