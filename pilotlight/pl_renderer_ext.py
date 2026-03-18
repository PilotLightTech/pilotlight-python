from typing import List, Any, Callable, Union, Tuple
import pilotlight.pilotlight as internal


class plRendererI:

    def initialize(device, swapchain, **kwargs):
        return internal.plRendererI_initialize(device, swapchain, **kwargs)

    def cleanup():
        return internal.plRendererI_cleanup()

    def register_ecs_system():
        return internal.plRendererI_register_ecs_system()
    
    def create_directional_light(library, name):
        return internal.plRendererI_create_directional_light(library, name)