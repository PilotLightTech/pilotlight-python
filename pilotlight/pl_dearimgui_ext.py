from typing import List, Any, Callable, Union, Tuple
import pilotlight.imgui as imgui


class plDearImGuiI:

    def initialize(device, swapchain, renderpass_handle, **kwargs) -> None:
        return imgui.plImgui_initialize(device, swapchain, renderpass_handle, **kwargs)

    def new_frame(device, renderpass_handle, **kwargs) -> None:
        return imgui.plImgui_new_frame(device, renderpass_handle, **kwargs)
    
    def render(encoder, **kwargs) -> None:
        return imgui.plImgui_render(encoder, **kwargs)
    
    def cleanup(**kwargs) -> None:
        return imgui.plImgui_cleanup(**kwargs)
