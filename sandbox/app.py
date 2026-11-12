import os
import sys
from pathlib import Path

# core
import pilotlight.pilotlight as pl
from pilotlight.pilotlight import *
from pilotlight.enums import *
from pilotlight.types import *

# scene to load (uses the core Pilot Light repo's assets, same as its sandbox)
#   usage: python app.py [scene name]   (i.e. humanoid, helmet, terrain)
SCENE_NAME = sys.argv[1] if len(sys.argv) > 1 else "terrain"
SCENE_PATH = f"/assets/scenes/{SCENE_NAME}.plscene"

class App:

    def __init__(self):
        self.ptWindow = None
        self.bResize = False
        self.counter = None
        self.tMainCamera = None
        self.ptLibrary = None
        self.ptScene = None
        self.ptView = None
        self.tSceneHandle = None
        self.some_string_array = "pizza"

    def pl_app_load(self):

        cwd = Path.cwd().resolve()
        package_dir = Path(pl.__file__).resolve().parent
        pilotlight_repo = str(cwd / "../../pilotlight")

        # cache directories written to by various extensions
        # (i.e. terrain chunk files go to /cache/terrain)
        for cache_dir in ["", "/shaders", "/imports", "/textures", "/terrain"]:
            plFileI.create_directory(str(cwd / "../cache") + cache_dir)
        plVfsI.mount_directory("/cache", str(cwd / "../cache"))
        plVfsI.mount_directory("/shaders", str(package_dir / "shaders"))
        plVfsI.mount_directory("/shader-temp", str(cwd / "../shader-temp"))
        plVfsI.mount_directory("/assets", str(package_dir / "assets"))
        # plVfsI.mount_directory("/assets", pilotlight_repo + "/assets")
        plVfsI.mount_directory("/resources", pilotlight_repo + "/resources")

        # NOTE: the renderer extension currently loads its shaders from
        #       "../shaders/" relative to the working directory (not through
        #       the vfs), so run from a directory next to the package's shaders
        os.chdir(package_dir / "data")

        plJobI.initialize()

        window_desc = plWindowDesc()
        window_desc.pcTitle = "Python Sandbox"
        window_desc.uWidth = 1500
        window_desc.uHeight = 900
        window_desc.iXPos = 50
        window_desc.iYPos = 50
        _, self.ptWindow = plWindowI.create(window_desc)
        plWindowI.show(self.ptWindow)

        starterInit = plStarterInit(plStarterFlag.PL_STARTER_FLAGS_ALL_EXTENSIONS | plStarterFlag.PL_STARTER_FLAGS_MSAA, self.ptWindow)
        starterInit.eFlags &= ~plStarterFlag.PL_STARTER_FLAGS_SHADER_EXT
        plStarterI.initialize(starterInit)

        shader_options = plShaderOptions()
        shader_options.apcDirectories = ["/shaders/"]
        shader_options.apcIncludeDirectories = ["/shaders/"]
        shader_options.eFlags = plShaderFlags.PL_SHADER_FLAGS_AUTO_OUTPUT | plShaderFlags.PL_SHADER_FLAGS_INCLUDE_DEBUG
        plShaderI.initialize(shader_options)

        plStarterI.finalize()

        self.counter = plStatsI.get_counter("python counter")

        # systems required by the renderer
        plShaderLibraryI.initialize(plStarterI.get_device())
        plRendererI.initialize(plStarterI.get_device(), plStarterI.get_swapchain())
        plPhysicsI.initialize()

        # asset types
        plAssetI.initialize()
        plTextureI.register_asset_types()
        plMaterialI.register_asset_types()
        plAnimationI.register_asset_types()
        plMeshI.register_asset_types()
        plSkeletonI.register_asset_types()
        plEcsI.register_asset_types()
        plTerrainI.register_asset_types()
        plRendererI.register_asset_types()
        plAssetI.finalize()

        # ecs components
        plEcsI.initialize()
        plIkI.register_ecs_components()
        plSkeletonI.register_ecs_components()
        plTransformI.register_ecs_components()
        plRendererI.register_ecs_components()
        plScriptI.register_ecs_components()
        plAnimationI.register_ecs_components()
        plCameraEcsI.register_ecs_components()
        plPhysicsI.register_ecs_components()
        plEcsI.finalize()

        # load scene (scenes are assets that hold a component library)
        self.tSceneHandle = plAssetI.load(SCENE_PATH)
        if plAssetI.is_valid(self.tSceneHandle):
            io = plIOI.get_io()
            self.ptLibrary = plAssetI.get_data(self.tSceneHandle)
            self.ptScene = plRendererI.create_scene()
            self.ptView = plRendererI.create_view(self.ptScene, io.tMainViewportSize)
            plRendererI.load_component_library(self.ptScene, self.ptLibrary)
            plEcsI.clear_changes(self.ptLibrary)

            cameras = plEcsI.get_components(self.ptLibrary, plCameraEcsI.get_ecs_type_key())
            if len(cameras) > 0:
                self.tMainCamera = cameras[0]
        else:
            print(f"[python sandbox] failed to load '{SCENE_PATH}'")
            print("[python sandbox] run the Pilot Light sandbox once so the scene's assets are imported")

        self.drawlist = plDrawI.request_2d_drawlist()
        self.ptFGLayer = plDrawI.request_2d_layer(self.drawlist)

    def pl_app_shutdown(self):
        plGraphicsI.flush_device(plStarterI.get_device())
        plJobI.cleanup()
        plPhysicsI.cleanup()
        if self.ptScene is not None:
            plRendererI.destroy_view(self.ptView)
            plRendererI.destroy_scene(self.ptScene)
        plAssetI.cleanup()
        plEcsI.cleanup()
        plRendererI.cleanup()
        plStarterI.cleanup()
        plWindowI.destroy(self.ptWindow)

    def pl_app_resize(self):
        plStarterI.resize()
        self.bResize = True

    def pl_app_update(self):

        io = plIOI.get_io()

        if not plStarterI.begin_frame():
            return

        plRendererI.begin_frame()

        if self.bResize:
            if self.ptView is not None:
                plRendererI.resize_view(self.ptView, io.tMainViewportSize)
            self.bResize = False

        if self.ptScene is not None and self.tMainCamera is not None:

            camera = plEcsI.get_component(self.ptLibrary, plCameraEcsI.get_ecs_type_key(), self.tMainCamera)
            plCameraI.set_viewport(camera, io.tMainViewportSize.x, io.tMainViewportSize.y)

            # run ecs systems (order matters, camera controls are a script component in the scene)
            plScriptI.run_update_system(self.ptLibrary)
            plAnimationI.run_animation_update_system(self.ptLibrary, io.fDeltaTime)
            plPhysicsI.update(io.fDeltaTime, self.ptLibrary, plRendererI.get_drawlist(self.ptView))
            plTransformI.run_transform_update_system(self.ptLibrary)
            plTransformI.run_hierarchy_update_system(self.ptLibrary)
            plRendererI.run_light_update_system(self.ptLibrary)
            plCameraEcsI.run_ecs(self.ptLibrary)
            plIkI.run_ecs_update_system(self.ptLibrary)
            plSkeletonI.run_skin_update_system(self.ptLibrary)
            plRendererI.run_object_update_system(self.ptLibrary)
            plRendererI.run_environment_probe_update_system(self.ptLibrary)

            plRendererI.prepare_scene(self.ptScene, [camera])
            plRendererI.prepare_view(self.ptView, camera)
            plRendererI.render_view(self.ptView, camera)

            uTexture, fU, fV = plRendererI.get_view_color_bind_group(self.ptView)
            plDrawI.add_image(self.ptFGLayer, uTexture, plVec2(), io.tMainViewportSize, plVec2(), plVec2(fU, fV), PL_COLOR_32_WHITE)

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

            plUiI.end_window()

        plDrawI.submit_2d_layer(self.ptFGLayer)

        cmdBuffer = plStarterI.begin_main_pass()

        tRenderAttachmentInfo = plRenderAttachmentInfo()
        plStarterI.get_render_attachment_info(tRenderAttachmentInfo)
        plDrawI.submit_2d_drawlist(self.drawlist, cmdBuffer, io.tMainViewportSize.x, io.tMainViewportSize.y, plGraphicsI.get_swapchain_info(plStarterI.get_swapchain()).eSampleCount, tRenderAttachmentInfo)

        plStarterI.end_main_pass()

        plStarterI.end_frame()

# run app
pl_run(App())
