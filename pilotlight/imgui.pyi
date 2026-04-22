from typing import List
from enum import IntEnum, IntFlag

def pl_dear_imgui_initialize(device, swapchain, renderpass_handle, **kwargs) -> None:
    ...

def pl_dear_imgui_new_frame(device, renderpass_handle, **kwargs) -> None:
    ...

def pl_dear_imgui_render(encoder, **kwargs) -> None:
    ...

def pl_dear_imgui_cleanup(**kwargs) -> None:
    ...

########################################################################################################################
# [SECTION] imgui
########################################################################################################################

class ImGuiWindowFlags(IntFlag):
    ...

def ImGui_StyleColorsDark():
    ...

def ImGui_StyleColorsLight():
    ...

def ImGui_StyleColorsClassic():
    ...

def ImGui_ShowDemoWindow(bool_pointer = None) -> None:
    ...

def ImGui_Begin(name, bool_pointer = None, flags: ImGuiWindowFlags = 0) -> None:
    ...

def ImGui_End():
    ...

def ImGui_BeginMenuBar():
    ...

def ImGui_BeginMainMenuBar():
    ...

def ImGui_EndMenuBar():
    ...

def ImGui_EndMainMenuBar():
    ...

def ImGui_BeginMenu(label, enabled=True):
    ...

def ImGui_EndMenu():
    ...

def ImGui_MenuItem(label, shortcut="", selected=False, enabled=True, selected_pointer = None):
    ...

def ImGui_Button(label, size=None):
    ...

def ImGui_Checkbox(label, bool_pointer):
    ...

########################################################################################################################
# [SECTION] implot
########################################################################################################################

def ImPlot_ShowDemoWindow(bool_pointer = None) -> None:
    ...