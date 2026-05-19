import os
import math

# core
import pilotlight.pilotlight as pl
from pilotlight.pilotlight import *
from pilotlight.imgui import *
from pilotlight.enums import *
from pilotlight.types import *

class App:

    def __init__(self):
        pass

    # -------------------------------------------------------------------------
    # Application lifetime
    # -------------------------------------------------------------------------

    def pl_app_load(self):
        """
        Demonstrates the 3D plDrawI API from Python.

        Controls:
          W/S/A/D : move camera
          R/F     : move camera up/down
          LMB drag: rotate camera
        """

        # Mount directories used by the shader/font systems.
        package_dir = os.path.dirname(os.path.abspath(pl.__file__))

        plVfsI.mount_directory("/data", package_dir + "/../data")
        plVfsI.mount_directory("/cache", "cache")
        plVfsI.mount_directory("/shaders", package_dir + "/shaders")
        plVfsI.mount_directory("/shader-temp", "shader-temp")

        # Create window.
        window_desc = plWindowDesc()
        window_desc.pcTitle = "Pilot Light Python - 3D Draw Example"
        window_desc.iXPos = 100
        window_desc.iYPos = 100
        window_desc.uWidth = 1280
        window_desc.uHeight = 720

        _, self.ptWindow = plWindowI.create(window_desc)
        plWindowI.show(self.ptWindow)

        # Initialize starter, but manually initialize shader/draw so this example
        # clearly shows what is required.
        starter_flags = plStarterFlag.PL_STARTER_FLAGS_ALL_EXTENSIONS
        starter_flags |= plStarterFlag.PL_STARTER_FLAGS_DEPTH_BUFFER
        starter_flags |= plStarterFlag.PL_STARTER_FLAGS_MSAA
        starter_flags &= ~plStarterFlag.PL_STARTER_FLAGS_SHADER_EXT
        starter_flags &= ~plStarterFlag.PL_STARTER_FLAGS_DRAW_EXT

        plStarterI.initialize(self.ptWindow, starter_flags)

        # Draw system.
        ptDevice = plStarterI.get_device()

        tDrawInit = plDrawInit(ptDevice)
        plDrawI.initialize(tDrawInit)

        # Font atlas for 2D overlay text.
        self.ptFontAtlas = plDrawI.create_font_atlas()
        plDrawI.set_font_atlas(self.ptFontAtlas)

        tFontRange = plFontRange(0x0020, 0x00FF - 0x0020)

        tFontConfig = plFontConfig()
        tFontConfig.fSize = 18.0
        tFontConfig.uHOverSampling = 1
        tFontConfig.uVOverSampling = 1
        tFontConfig.ptRanges = [tFontRange]

        self.ptFont = plDrawI.add_font_from_file_ttf(
            self.ptFontAtlas,
            tFontConfig,
            "/data/Cousine-Regular.ttf"
        )

        # Persistent 3D drawlist.
        self.drawlist = plDrawI.request_3d_drawlist()

        # Shader system.
        shader_options = plShaderOptions()
        shader_options.pcCacheOutputDirectory = "/shader-temp/"
        shader_options.apcDirectories = ["/shaders/"]
        shader_options.apcIncludeDirectories = ["/shaders/"]
        shader_options.tFlags = (
            plShaderFlags.PL_SHADER_FLAGS_AUTO_OUTPUT
            | plShaderFlags.PL_SHADER_FLAGS_INCLUDE_DEBUG
            | plShaderFlags.PL_SHADER_FLAGS_ALWAYS_COMPILE
        )

        plShaderI.initialize(shader_options)

        # Finish starter setup after custom systems are initialized.
        plStarterI.finalize()

        # Build font atlas.
        ptCmdBuffer = plStarterI.get_raw_command_buffer()
        plDrawI.build_font_atlas(ptCmdBuffer, self.ptFontAtlas)
        plStarterI.return_raw_command_buffer(ptCmdBuffer)

        # Camera.
        self.camera = plCamera()
        plCameraI.init_perspective(
            self.camera,
            [-10.0, 4.0, -10.0],
            math.radians(60.0),
            1280.0 / 720.0,
            0.1,
            100.0,
            False
        )
        plCameraI.rotate(self.camera, -0.295, 0.7)
        plCameraI.update(self.camera)

    def pl_app_shutdown(self):
        plGraphicsI.flush_device(plStarterI.get_device())

        plDrawI.return_3d_drawlist(self.drawlist)
        plDrawI.cleanup_font_atlas(self.ptFontAtlas)
        plDrawI.cleanup()

        plStarterI.cleanup()
        plWindowI.destroy(self.ptWindow)

    def pl_app_resize(self):
        plStarterI.resize()

        tIO = plIOI.get_io()
        plCameraI.set_aspect(self.camera, tIO.tMainViewportSize.x / tIO.tMainViewportSize.y)

    def draw_scene(self):
        """
        Draw classic debug helpers: transform, cross, AABB, centered box,
        Bezier curves, and a camera frustum.
        """

        white = plDrawLineOptions(PL_COLOR_32_WHITE, 0.04)
        yellow = plDrawLineOptions(PL_COLOR_32_YELLOW, 0.04)
        cyan = plDrawLineOptions(PL_COLOR_32_CYAN, 0.04)
        magenta = plDrawLineOptions(PL_COLOR_32_MAGENTA, 0.04)
        orange = plDrawLineOptions(PL_COLOR_32_ORANGE, 0.04)
        green = plDrawLineOptions(PL_COLOR_32_GREEN, 0.03)

        red_solid = plDrawSolidOptions(PL_COLOR_32_RED)
        green_solid = plDrawSolidOptions(PL_COLOR_32_GREEN)
        blue_solid = plDrawSolidOptions(PL_COLOR_32_BLUE)
        cyan_solid = plDrawSolidOptions(PL_COLOR_32_CYAN)
        orange_solid = plDrawSolidOptions(PL_COLOR_32_ORANGE)
        magenta_solid = plDrawSolidOptions(PL_COLOR_32_MAGENTA)
        yellow_solid = plDrawSolidOptions(PL_COLOR_32_YELLOW)
        grey_solid = plDrawSolidOptions(PL_COLOR_32_GREY)

        plDrawI.add_3d_text(
            self.drawlist,
            plVec3(0.0, 5.0, 0.0),
            "3D Text @ (0.0, 5.0, 0.0)",
            plDrawTextOptions(self.ptFont, 18.0, PL_COLOR_32_CYAN)
        )

        plDrawI.add_3d_cross(
            self.drawlist,
            plVec3(0.0, 5.0, 0.0),
            0.1,
            cyan
        )

        # World transform gizmo at origin.
        identity = plMat4()
        plDrawI.add_3d_transform(self.drawlist, identity, 1.5, white)

        # Cross marker.
        plDrawI.add_3d_cross(
            self.drawlist,
            plVec3(-4.0, 1.0, -2.0),
            0.75,
            yellow
        )

        # Wire centered box.
        plDrawI.add_3d_centered_box(
            self.drawlist,
            plVec3(-4.0, 1.0, 1.0),
            1.5,
            2.0,
            1.0,
            cyan
        )

        # Wire AABB.
        plDrawI.add_3d_aabb(
            self.drawlist,
            plVec3(-5.0, 0.25, 3.0),
            plVec3(-3.0, 2.0, 4.5),
            magenta
        )

        # Quadratic Bezier.
        plDrawI.add_3d_bezier_quad(
            self.drawlist,
            plVec3(-1.0, 0.25, -4.0),
            plVec3( 0.0, 3.00, -4.0),
            plVec3( 1.0, 0.25, -4.0),
            32,
            orange
        )

        # Cubic Bezier.
        plDrawI.add_3d_bezier_cubic(
            self.drawlist,
            plVec3(2.0, 0.25, -4.0),
            plVec3(3.0, 3.00, -4.0),
            plVec3(4.0, -1.0, -4.0),
            plVec3(5.0, 2.00, -4.0),
            48,
            cyan
        )

        frustum_desc = plDrawFrustumDesc(math.radians(60.0), 16.0 / 9.0, 0.25, 2.5)
        frustum_transform = plMat4()
        plDrawI.add_3d_frustum(self.drawlist, frustum_transform, frustum_desc, plDrawLineOptions(PL_COLOR_32_GREEN, 0.03))

        # Filled triangle.
        plDrawI.add_3d_triangle_filled(
            self.drawlist,
            plVec3(-1.0, 0.05, 1.0),
            plVec3( 0.0, 1.50, 1.0),
            plVec3( 1.0, 0.05, 1.0),
            yellow_solid
        )

        # Filled box.
        plDrawI.add_3d_centered_box_filled(
            self.drawlist,
            plVec3(3.0, 0.75, 0.0),
            1.5,
            1.5,
            1.5,
            blue_solid
        )

        # Filled XZ plane.
        plDrawI.add_3d_plane_xz_filled(
            self.drawlist,
            plVec3(0.0, -0.01, 0.0),
            5.0,
            5.0,
            grey_solid
        )

        # Filled XY/YZ planes, useful for testing orientation.
        plDrawI.add_3d_plane_xy_filled(
            self.drawlist,
            plVec3(-2.5, 1.25, 3.0),
            1.5,
            1.5,
            magenta_solid
        )

        plDrawI.add_3d_plane_yz_filled(
            self.drawlist,
            plVec3(2.5, 1.25, 3.0),
            1.5,
            1.5,
            cyan_solid
        )

        # Filled XZ disk and ring.
        plDrawI.add_3d_circle_xz_filled(
            self.drawlist,
            plVec3(-3.0, 0.03, -2.0),
            0.75,
            48,
            green_solid
        )

        plDrawI.add_3d_band_xz_filled(
            self.drawlist,
            plVec3(-3.0, 0.06, -4.0),
            0.45,
            0.90,
            64,
            orange_solid
        )

        # Vertical rings.
        plDrawI.add_3d_band_xy_filled(
            self.drawlist,
            plVec3(0.0, 1.25, 3.0),
            0.45,
            0.85,
            48,
            red_solid
        )

        plDrawI.add_3d_band_yz_filled(
            self.drawlist,
            plVec3(0.0, 1.25, 4.5),
            0.45,
            0.85,
            48,
            green_solid
        )

        # Sphere.
        sphere = plSphere(
            0.85,
            plVec3(0.0, 1.0, -1.5)
        )

        plDrawI.add_3d_sphere_filled(
            self.drawlist,
            sphere,
            16,
            32,
            cyan_solid
        )

        # Cylinder.
        cylinder = plCylinder(
            plVec3(4.0, 0.0, -3.0),
            plVec3(4.0, 2.0, -3.0),
            0.45
        )

        plDrawI.add_3d_cylinder_filled(
            self.drawlist,
            cylinder,
            32,
            orange_solid
        )

        # Cone.
        cone = plCone(
            plVec3(5.5, 0.0, -3.0),
            plVec3(5.5, 2.0, -3.0),
            0.65
        )

        plDrawI.add_3d_cone_filled(
            self.drawlist,
            cone,
            32,
            magenta_solid
        )

        # Wire circle.
        plDrawI.add_3d_circle_xz(
            self.drawlist,
            plVec3(-5.5, 0.05, -2.0),
            0.85,
            64,
            green
        )

        # Wire sphere.
        sphere = plSphere(
            0.85,
            plVec3(0.0, 1.0, -3.5)
        )

        plDrawI.add_3d_sphere(
            self.drawlist,
            sphere,
            12,
            24,
            cyan
        )

        # Wire capsule.
        capsule = plCapsule(
            plVec3(2.0, 0.5, -3.5),
            plVec3(2.0, 2.5, -3.5),
            0.45
        )

        plDrawI.add_3d_capsule(
            self.drawlist,
            capsule,
            12,
            24,
            white
        )

        # Wire cylinder.
        cylinder = plCylinder(
            plVec3(4.0, 0.0, -5.0),
            plVec3(4.0, 2.0, -5.0),
            0.45
        )

        plDrawI.add_3d_cylinder(
            self.drawlist,
            cylinder,
            32,
            orange
        )

        # Wire cone.
        cone = plCone(
            plVec3(5.5, 0.0, -5.0),
            plVec3(5.5, 2.0, -5.0),
            0.65
        )

        plDrawI.add_3d_cone(
            self.drawlist,
            cone,
            32,
            white
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

            # Lines parallel to Z.
            plDrawI.add_3d_line(self.drawlist,
                plVec3(x, 0.0, -extent * spacing),
                plVec3(x, 0.0,  extent * spacing),
                x_axis if i == 0 else thin)

            # Lines parallel to X.
            plDrawI.add_3d_line(self.drawlist,
                plVec3(-extent * spacing, 0.0, z),
                plVec3( extent * spacing, 0.0, z),
                z_axis if i == 0 else thin)

    def pl_app_update(self):
        if not plStarterI.begin_frame():
            return

        ptIO = plIOI.get_io()

        camera_travel_speed = 5.0
        camera_rotation_speed = 0.005

        dt = ptIO.fDeltaTime

        if plIOI.is_key_down(plKey.PL_KEY_W):
            plCameraI.translate(self.camera, 0.0, 0.0, camera_travel_speed * dt)

        if plIOI.is_key_down(plKey.PL_KEY_S):
            plCameraI.translate(self.camera, 0.0, 0.0, -camera_travel_speed * dt)

        if plIOI.is_key_down(plKey.PL_KEY_A):
            plCameraI.translate(self.camera, -camera_travel_speed * dt, 0.0, 0.0)

        if plIOI.is_key_down(plKey.PL_KEY_D):
            plCameraI.translate(self.camera, camera_travel_speed * dt, 0.0, 0.0)

        if plIOI.is_key_down(plKey.PL_KEY_R):
            plCameraI.translate(self.camera, 0.0, camera_travel_speed * dt, 0.0)

        if plIOI.is_key_down(plKey.PL_KEY_F):
            plCameraI.translate(self.camera, 0.0, -camera_travel_speed * dt, 0.0)

        if plIOI.is_mouse_dragging(plMouseButton.PL_MOUSE_BUTTON_LEFT, 1.0):
            tMouseDelta = plIOI.get_mouse_drag_delta(plMouseButton.PL_MOUSE_BUTTON_LEFT, 1.0)
            plCameraI.rotate(self.camera, -tMouseDelta.y * camera_rotation_speed, -tMouseDelta.x * camera_rotation_speed)
            plIOI.reset_mouse_drag_delta(plMouseButton.PL_MOUSE_BUTTON_LEFT)

        plCameraI.update(self.camera)

        # Start a new draw frame. This clears/refreshes internal draw state.
        plDrawI.new_frame()

        # 2D overlay.
        fgLayer = plStarterI.get_foreground_layer()

        overlay = (
            "3D plDrawI Python Example\n"
            "W/S/A/D: move | R/F: up/down | LMB drag: look\n"
            f"FPS: {ptIO.fFrameRate:.1f}"
        )

        plDrawI.add_text(
            fgLayer,
            [16.0, 16.0],
            overlay,
            plDrawTextOptions(self.ptFont, 18.0, PL_COLOR_32_CYAN)
        )

        # Fill the 3D drawlist for this frame.
        self.draw_scene()

        # Main render pass.
        encoder = plStarterI.begin_main_pass()

        tMainViewportSize = ptIO.tMainViewportSize

        mvp = self.camera.tProjMat * self.camera.tViewMat

        plDrawI.submit_3d_drawlist(
            self.drawlist,
            encoder,
            tMainViewportSize.x,
            tMainViewportSize.y,
            mvp,
            plDrawFlag.PL_DRAW_FLAG_DEPTH_TEST | plDrawFlag.PL_DRAW_FLAG_DEPTH_WRITE,
            plGraphicsI.get_swapchain_info(plStarterI.get_swapchain()).tSampleCount
        )

        plStarterI.end_main_pass()

        plStarterI.end_frame()


pl_run(App())