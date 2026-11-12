"""
pl_script_camera
  * fly camera controls (python version of pilot light's pl_script_camera.c)
  * used two ways:
      - as a script component: scenes that reference "pl_script_camera" load
        this module & call setup()/run() for the entity (see plScriptI)
      - directly: call pl_script_run(camera) every frame with a plCamera

Controls:
  * LMB + Drag: Moves camera forward & backward and rotates left & right.
  * RMB + Drag: Rotates camera.
  * LMB + RMB + Drag: Pans Camera
  * Mouse Wheel: Speed
  * Game style (when holding RMB): W/S/A/D move, Q/E down/up, Z/C zoom
"""

import math
from pilotlight.pilotlight import *
from pilotlight.enums import *

#-----------------------------------------------------------------------------
# script component interface
#-----------------------------------------------------------------------------

def setup(library, entity):
    """called once when the script loads"""
    camera = plEcsI.get_component(library, plCameraEcsI.get_ecs_type_key(), entity)
    static = pl_script_run
    static.fCameraTravelSpeed = 4.0
    if camera is not None:
        static.gfOriginalFOV = camera.fYFov

def run(library, entity):
    """called every update while the script is playing"""
    camera = plEcsI.get_component(library, plCameraEcsI.get_ecs_type_key(), entity)
    if camera is not None:
        pl_script_run(camera)

#-----------------------------------------------------------------------------
# direct use
#-----------------------------------------------------------------------------

def pl_script_run(ptCamera: plCamera):
    
    static = pl_script_run
    io = plIOI.get_io()

    if not hasattr(static, "fCameraTravelSpeed"):
        static.fCameraTravelSpeed = 4.0

    if not hasattr(static, "fCameraRotationSpeed"):
        static.fCameraRotationSpeed = 0.005

    if not hasattr(static, "gfOriginalFOV"):
        static.gfOriginalFOV = ptCamera.fYFov

    bOwnKeyboard = plUiI.wants_keyboard_capture()
    bOwnMouse = plUiI.wants_mouse_capture()

    if bOwnKeyboard == False and bOwnMouse == False:
        
        bRMB = plIOI.is_mouse_down(plMouseButton.PL_MOUSE_BUTTON_RIGHT)
        bLMB = plIOI.is_mouse_down(plMouseButton.PL_MOUSE_BUTTON_LEFT)

        if plIOI.is_mouse_clicked(plMouseButton.PL_MOUSE_BUTTON_RIGHT, False):
            static.gfOriginalFOV = ptCamera.fYFov

        elif plIOI.is_mouse_released(plMouseButton.PL_MOUSE_BUTTON_RIGHT):
            plCameraI.set_y_fov(ptCamera, static.gfOriginalFOV)

        if plIOI.get_mouse_wheel() > 0.0:
            static.fCameraTravelSpeed *= 2.0
            static.fCameraTravelSpeed = min(static.fCameraTravelSpeed, 10000.0)
        elif plIOI.get_mouse_wheel() < 0.0:
            static.fCameraTravelSpeed /= 2.0;
            static.fCameraTravelSpeed = max(static.fCameraTravelSpeed, 0.1)

        if bRMB:
            if plIOI.is_key_down(plKey.PL_KEY_W):
                plCameraI.translate_local(ptCamera, [0.0,  0.0,  static.fCameraTravelSpeed * io.fDeltaTime])
            if plIOI.is_key_down(plKey.PL_KEY_S):
                plCameraI.translate_local(ptCamera,  [0.0,  0.0, -static.fCameraTravelSpeed* io.fDeltaTime])
            if plIOI.is_key_down(plKey.PL_KEY_A):
                plCameraI.translate_local(ptCamera, [static.fCameraTravelSpeed * io.fDeltaTime,  0.0,  0.0])
            if plIOI.is_key_down(plKey.PL_KEY_D):
                plCameraI.translate_local(ptCamera, [-static.fCameraTravelSpeed * io.fDeltaTime,  0.0,  0.0])

            # world space
            if plIOI.is_key_down(plKey.PL_KEY_Q):
                plCameraI.translate(ptCamera, [0.0, -static.fCameraTravelSpeed * io.fDeltaTime,  0.0])
            if plIOI.is_key_down(plKey.PL_KEY_E):
                plCameraI.translate(ptCamera, [0.0,  static.fCameraTravelSpeed * io.fDeltaTime,  0.0])

            if plIOI.is_key_down(plKey.PL_KEY_Z):
                plCameraI.set_y_fov(ptCamera, ptCamera.fYFov + 0.25 * (math.pi / 180.0))
                plCameraI.set_y_fov(ptCamera, min(ptCamera.fYFov, 2.96706))

            if plIOI.is_key_down(plKey.PL_KEY_C):
                plCameraI.set_y_fov(ptCamera, ptCamera.fYFov - 0.25 * (math.pi / 180.0))
                plCameraI.set_y_fov(ptCamera, max(ptCamera.fYFov, 0.03))

        if bLMB and plIOI.is_mouse_dragging(plMouseButton.PL_MOUSE_BUTTON_RIGHT, 1.0):
            tMouseDelta = plIOI.get_mouse_drag_delta(plMouseButton.PL_MOUSE_BUTTON_RIGHT, 1.0)
            plCameraI.translate_local(ptCamera,  [-tMouseDelta.x * static.fCameraTravelSpeed * io.fDeltaTime, -tMouseDelta.y * static.fCameraTravelSpeed * io.fDeltaTime, 0.0])
            plIOI.reset_mouse_drag_delta(plMouseButton.PL_MOUSE_BUTTON_RIGHT)
            plIOI.reset_mouse_drag_delta(plMouseButton.PL_MOUSE_BUTTON_LEFT)

        elif plIOI.is_mouse_dragging(plMouseButton.PL_MOUSE_BUTTON_RIGHT, 1.0):
            tMouseDelta = plIOI.get_mouse_drag_delta(plMouseButton.PL_MOUSE_BUTTON_RIGHT, 1.0)
            plCameraI.rotate_euler(ptCamera,  -tMouseDelta.y * static.fCameraRotationSpeed,  -tMouseDelta.x * static.fCameraRotationSpeed, 0.0)
            plIOI.reset_mouse_drag_delta(plMouseButton.PL_MOUSE_BUTTON_RIGHT)

        elif bLMB:
            tMouseDelta = plIOI.get_mouse_drag_delta(plMouseButton.PL_MOUSE_BUTTON_LEFT, 1.0)
            plCameraI.rotate_euler(ptCamera,  0.0,  -tMouseDelta.x * static.fCameraRotationSpeed, 0.0)
            xChange = ptCamera.tPosition.x + -tMouseDelta.y * static.fCameraTravelSpeed * io.fDeltaTime * math.sin(ptCamera.fYaw)
            zChange = ptCamera.tPosition.z + -tMouseDelta.y * static.fCameraTravelSpeed * io.fDeltaTime * math.cos(ptCamera.fYaw)
            ptCamera.tPosition = plVec3(xChange, ptCamera.tPosition.y, zChange)
            plIOI.reset_mouse_drag_delta(plMouseButton.PL_MOUSE_BUTTON_LEFT)

    plCameraI.update(ptCamera)