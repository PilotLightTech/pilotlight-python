from typing import List, Any, Callable, Union, Tuple
import pilotlight.pilotlight as internal


class plCameraI:

    def register_ecs_system():
        return internal.plCameraI_register_ecs_system()
    
    def get_ecs_type_key():
        return internal.plCameraI_get_ecs_type_key()

    def create_perspective(library, name, pos, yFov, aspect, nearZ, farZ, reverseZ):
        return internal.plCameraI_create_perspective(library, name, pos, yFov, aspect, nearZ, farZ, reverseZ)

    def set_fov(camera, yFov):
        return internal.plCameraI_set_fov(camera, yFov)

    def update(camera):
        return internal.plCameraI_update(camera)