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

class ImGui:

    def StyleColorsDark():
        return imgui.plImgui_StyleColorsDark()
    
    def StyleColorsLight():
        return imgui.plImgui_StyleColorsLight()
    
    def StyleColorsClassic():
        return imgui.plImgui_StyleColorsClassic()

    def ShowDemoWindow(bool_pointer = None) -> None:
        return imgui.plImGui_ShowDemoWindow(bool_pointer)
    
    def Begin(name, bool_pointer = None, flags = imgui.ImGuiWindowFlags_None) -> None:
        return imgui.plImgui_Begin(name, bool_pointer, flags)
    
    def End():
        return imgui.plImgui_End()

    def BeginMenuBar():
        return imgui.plImgui_BeginMenuBar()
    
    def BeginMainMenuBar():
        return imgui.plImgui_BeginMainMenuBar()
    
    def EndMenuBar():
        return imgui.plImgui_EndMenuBar()
    
    def EndMainMenuBar():
        return imgui.plImgui_EndMainMenuBar()
    
    def BeginMenu(label, enabled=True):
        return imgui.plImgui_BeginMenu(label, enabled)
    
    def EndMenu():
        return imgui.plImgui_EndMenu()

    def MenuItem(label, shortcut="", selected=False, enabled=True, selected_pointer = None):
        return imgui.plImgui_MenuItem(label, shortcut, selected, enabled, selected_pointer=selected_pointer)

    def Button(label, size=None):
        return imgui.plImgui_Button(label, size)

    def Checkbox(label, bool_pointer):
        return imgui.plImgui_Checkbox(label, bool_pointer)
    
class ImPlot:

    def ShowDemoWindow(bool_pointer = None) -> None:
        return imgui.plImPlot_ShowDemoWindow(bool_pointer)

########################################################################################################################
# [SECTION] enums
########################################################################################################################

# ImGuiWindowFlags CONSTANTS
ImGuiWindowFlags_None=imgui.ImGuiWindowFlags_None
ImGuiWindowFlags_NoTitleBar=imgui.ImGuiWindowFlags_NoTitleBar
ImGuiWindowFlags_NoResize=imgui.ImGuiWindowFlags_NoResize
ImGuiWindowFlags_NoMove=imgui.ImGuiWindowFlags_NoMove
ImGuiWindowFlags_NoScrollbar=imgui.ImGuiWindowFlags_NoScrollbar
ImGuiWindowFlags_NoScrollWithMouse=imgui.ImGuiWindowFlags_NoScrollWithMouse
ImGuiWindowFlags_NoCollapse=imgui.ImGuiWindowFlags_NoCollapse
ImGuiWindowFlags_AlwaysAutoResize=imgui.ImGuiWindowFlags_AlwaysAutoResize
ImGuiWindowFlags_NoBackground=imgui.ImGuiWindowFlags_NoBackground
ImGuiWindowFlags_NoSavedSettings=imgui.ImGuiWindowFlags_NoSavedSettings
ImGuiWindowFlags_NoMouseInputs=imgui.ImGuiWindowFlags_NoMouseInputs
ImGuiWindowFlags_MenuBar=imgui.ImGuiWindowFlags_MenuBar
ImGuiWindowFlags_HorizontalScrollbar=imgui.ImGuiWindowFlags_HorizontalScrollbar
ImGuiWindowFlags_NoFocusOnAppearing=imgui.ImGuiWindowFlags_NoFocusOnAppearing
ImGuiWindowFlags_NoBringToFrontOnFocus=imgui.ImGuiWindowFlags_NoBringToFrontOnFocus
ImGuiWindowFlags_AlwaysVerticalScrollbar=imgui.ImGuiWindowFlags_AlwaysVerticalScrollbar
ImGuiWindowFlags_AlwaysHorizontalScrollbar=imgui.ImGuiWindowFlags_AlwaysHorizontalScrollbar
ImGuiWindowFlags_NoNavInputs=imgui.ImGuiWindowFlags_NoNavInputs
ImGuiWindowFlags_NoNavFocus=imgui.ImGuiWindowFlags_NoNavFocus
ImGuiWindowFlags_UnsavedDocument=imgui.ImGuiWindowFlags_UnsavedDocument
ImGuiWindowFlags_NoDocking=imgui.ImGuiWindowFlags_NoDocking
ImGuiWindowFlags_NoNav=imgui.ImGuiWindowFlags_NoNav
ImGuiWindowFlags_NoDecoration=imgui.ImGuiWindowFlags_NoDecoration
ImGuiWindowFlags_NoInputs=imgui.ImGuiWindowFlags_NoInputs
