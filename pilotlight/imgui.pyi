from typing import List, NewType, Tuple
from pilotlight.types import *
from pilotlight.enums import *

########################################################################################################################
# [SECTION] pilotlight hooks
########################################################################################################################

class plDearImGuiI:

    @staticmethod
    def initialize(device: plDevice, swapchain: plSwapchain, attachmentInfo, **kwargs):
        ...

    @staticmethod
    def new_frame(device: plDevice, **kwargs):
        ...

    @staticmethod
    def render(commandBuffer: plCommandBuffer, **kwargs):
        ...

    @staticmethod
    def cleanup(**kwargs):
        ...

    @staticmethod
    def get_texture_id_from_bindgroup(device, texture):
        ...

class ImGui:

    ########################################################################################################################
    # [SECTION] imgui demo/debug/information
    ########################################################################################################################

    @staticmethod
    def ShowDemoWindow(open: bool | None = None) -> bool | None:
        ...

    @staticmethod
    def ShowMetricsWindow(bool: bool | None = None):
        ...

    @staticmethod
    def ShowDebugLogWindow(bool: bool | None = None):
        ...

    @staticmethod
    def ShowIDStackToolWindow(bool: bool | None = None):
        ...

    @staticmethod
    def ShowAboutWindow(bool: bool | None = None):
        ...

    @staticmethod
    def ShowStyleEditor():
        ...

    @staticmethod
    def ShowStyleSelector(label: str) -> bool:
        ...

    @staticmethod
    def ShowFontSelector(label: str):
        ...

    @staticmethod
    def ShowUserGuide():
        ...

    @staticmethod
    def GetVersion() -> str:
        ...

    ########################################################################################################################
    # [SECTION] imgui
    ########################################################################################################################

    @staticmethod
    def StyleColorsDark():
        ...

    @staticmethod
    def StyleColorsLight():
        ...

    @staticmethod
    def StyleColorsClassic():
        ...

    @staticmethod
    @overload
    def Begin(name: str, open: None = None, flags: ImGuiWindowFlags | int = 0) -> bool:
        ...

    @staticmethod
    @overload
    def Begin(name: str, open: bool, flags: ImGuiWindowFlags | int = 0) -> Tuple[bool, bool]:
        ...

    @staticmethod
    def End():
        ...

    ########################################################################################################################
    # [SECTION] imgui window utilities
    ########################################################################################################################

    @staticmethod
    def IsWindowAppearing() -> bool:
        ...

    @staticmethod
    def IsWindowCollapsed() -> bool:
        ...

    @staticmethod
    def IsWindowFocused(flags: ImGuiFocusedFlags | int = 0) -> bool:
        ...

    @staticmethod
    def IsWindowHovered(flags: ImGuiHoveredFlags | int = 0) -> bool:
        ...

    @staticmethod
    def GetWindowDpiScale() -> float:
        ...

    @staticmethod
    def GetWindowPos() -> Tuple[float, float]:
        ...

    @staticmethod
    def GetWindowSize() -> Tuple[float, float]:
        ...

    @staticmethod
    def GetWindowWidth() -> float:
        ...

    @staticmethod
    def GetWindowHeight() -> float:
        ...

    ########################################################################################################################
    # [SECTION] imgui window manipulation
    ########################################################################################################################

    @staticmethod
    def SetNextWindowPos(pos, cond: ImGuiCond | int = 0, pivot=None):
        ...

    @staticmethod
    def SetNextWindowSize(size, cond: ImGuiCond | int = 0):
        ...

    @staticmethod
    def SetNextWindowContentSize(size):
        ...

    @staticmethod
    def SetNextWindowCollapsed(collapsed: bool, cond: ImGuiCond | int = 0):
        ...

    @staticmethod
    def SetNextWindowFocus():
        ...

    @staticmethod
    def SetNextWindowScroll(scroll):
        ...

    @staticmethod
    def SetNextWindowBgAlpha(alpha: float):
        ...

    @staticmethod
    def SetNextWindowViewport(viewport_id: int):
        ...

    @staticmethod
    def SetWindowPos(pos, cond: ImGuiCond | int = 0, name: str | None = None):
        ...

    @staticmethod
    def SetWindowSize(size, cond: ImGuiCond | int = 0, name: str | None = None):
        ...

    @staticmethod
    def SetWindowCollapsed(collapsed: bool, cond: ImGuiCond | int = 0, name: str | None = None):
        ...

    @staticmethod
    def SetWindowFocus(name: str | None = None):
        ...

    ########################################################################################################################
    # [SECTION] imgui menu stuff
    ########################################################################################################################

    @staticmethod
    def BeginMenuBar() -> bool:
        ...

    @staticmethod
    def BeginMainMenuBar() -> bool:
        ...

    @staticmethod
    def EndMenuBar():
        ...

    @staticmethod
    def EndMainMenuBar():
        ...

    @staticmethod
    def BeginMenu(label:str, enabled:bool=True) -> bool:
        ...

    @staticmethod
    def EndMenu():
        ...

    @staticmethod
    def MenuItemSimple(label:str, shortcut:str="", selected:bool=False, enabled:bool=True) -> Tuple[bool, bool]:
        ...

    @staticmethod
    def MenuItem(label:str, shortcut:str, selected:bool, enabled:bool=True) -> Tuple[bool, bool]:
        ...

    ########################################################################################################################
    # [SECTION] imgui child windows
    ########################################################################################################################

    @staticmethod
    def BeginChild(str_id: str, size:List[int] | Tuple[int, int] | None = None, child_flags: ImGuiChildFlags | int = 0, window_flags: ImGuiWindowFlags | int = 0) -> bool:
        ...

    @staticmethod
    def EndChild():
        ...

    ########################################################################################################################
    # [SECTION] imgui input utilities mouse
    ########################################################################################################################

    @staticmethod
    def IsMouseDown(button: ImGuiMouseButton) -> bool:
        ...

    @staticmethod
    def IsMouseClicked(button: ImGuiMouseButton, repeat: bool = False) -> bool:
        ...

    @staticmethod
    def IsMouseReleased(button: ImGuiMouseButton) -> bool:
        ...

    @staticmethod
    def IsMouseDoubleClicked(button: ImGuiMouseButton) -> bool:
        ...

    @staticmethod
    def GetMouseClickedCount(button: ImGuiMouseButton) -> int:
        ...

    @staticmethod
    def IsMouseHoveringRect(min, max, clip: bool = True) -> bool:
        ...

    @staticmethod
    def IsMousePosValid(mouse_pos=None) -> bool:
        ...

    @staticmethod
    def IsAnyMouseDown() -> bool:
        ...

    @staticmethod
    def GetMousePos() -> Tuple[float, float]:
        ...

    @staticmethod
    def GetMousePosOnOpeningCurrentPopup() -> Tuple[float, float]:
        ...

    @staticmethod
    def IsMouseDragging(button: ImGuiMouseButton, lock_threshold: float = -1.0) -> bool:
        ...

    @staticmethod
    def GetMouseDragDelta(button: ImGuiMouseButton | int = 0, lock_threshold: float = -1.0) -> Tuple[float, float]:
        ...

    @staticmethod
    def ResetMouseDragDelta(button: ImGuiMouseButton | int = 0):
        ...

    @staticmethod
    def GetMouseCursor() -> ImGuiMouseCursor:
        ...

    @staticmethod
    def SetMouseCursor(cursor_type: ImGuiMouseCursor):
        ...

    @staticmethod
    def SetNextFrameWantCaptureMouse(want_capture_mouse: bool):
        ...

    ########################################################################################################################
    # [SECTION] imgui clipboard utilities
    ########################################################################################################################

    @staticmethod
    def GetClipboardText() -> str | None:
        ...

    @staticmethod
    def SetClipboardText(text: str):
        ...

    ########################################################################################################################
    # [SECTION] imgui settings / ini utilities
    ########################################################################################################################

    @staticmethod
    def LoadIniSettingsFromDisk(ini_filename: str):
        ...

    @staticmethod
    def LoadIniSettingsFromMemory(ini_data: str):
        ...

    @staticmethod
    def SaveIniSettingsToDisk(ini_filename: str):
        ...

    @staticmethod
    def SaveIniSettingsToMemory() -> str | None:
        ...

    ########################################################################################################################
    # [SECTION] imgui id stack/scopes
    ########################################################################################################################

    @staticmethod
    def PushID(id: str | int):
        ...

    @staticmethod
    def PopID():
        ...

    @staticmethod
    def GetID(id: str | int) -> int:
        ...

    ########################################################################################################################
    # [SECTION] image widgets
    ########################################################################################################################

    @staticmethod
    def Image(texture, size, uv0 = [0.0, 0.0], uv1 = [1.0, 1.0]):
        ...

    ########################################################################################################################
    # [SECTION] imgui main widgets
    ########################################################################################################################

    @staticmethod
    def Button(label:str, size=None):
        ...

    @staticmethod
    def Checkbox(label:str, value: bool) -> Tuple[bool, bool]:
        ...

    @staticmethod
    def SmallButton(label: str) -> bool:
        ...

    @staticmethod
    def InvisibleButton(str_id: str, size, flags: ImGuiButtonFlags | int = 0) -> bool:
        ...

    @staticmethod
    def ArrowButton(str_id: str, dir: ImGuiDir) -> bool:
        ...

    @staticmethod
    def CheckboxFlags(label: str, flags: int, flags_value: int) -> Tuple[bool, int]:
        ...

    @staticmethod
    @overload
    def RadioButton(label: str, value: int, button_value: int) -> Tuple[bool, int]:
        ...

    @staticmethod
    @overload
    def RadioButton(label: str, active: bool) -> bool:
        ...

    @staticmethod
    def ProgressBar(fraction: float, size=None, overlay: str | None = None):
        ...

    @staticmethod
    def Bullet():
        ...

    @staticmethod
    def TextLink(label: str) -> bool:
        ...

    @staticmethod
    def TextLinkOpenURL(label: str, url: str | None = None) -> bool:
        ...

    ########################################################################################################################
    # [SECTION] imgui combo box/dropdown widgets
    ########################################################################################################################

    @staticmethod
    def BeginCombo(label: str, preview_value: str, flags: ImGuiComboFlags | int = 0) -> bool:
        ...

    @staticmethod
    def EndCombo():
        ...

    @staticmethod
    @overload
    def Combo(label: str, current_item: int, items: List[str], popup_max_height_in_items: int = -1) -> Tuple[bool, int]:
        pass

    @staticmethod
    @overload
    def Combo(label: str, current_item: int, items_separated_by_zeros: str, popup_max_height_in_items: int = -1) -> Tuple[bool, int]:
        pass

    ########################################################################################################################
    # [SECTION] imgui list boxes
    ########################################################################################################################

    @staticmethod
    def BeginListBox(label: str, size=None) -> bool:
        ...

    @staticmethod
    def EndListBox():
        ...

    ########################################################################################################################
    # [SECTION] imgui tooltips
    ########################################################################################################################

    @staticmethod
    def BeginTooltip() -> bool:
        ...

    @staticmethod
    def EndTooltip():
        ...

    @staticmethod
    def SetTooltip(text: str):
        ...

    @staticmethod
    def BeginItemTooltip() -> bool:
        ...

    @staticmethod
    def SetItemTooltip(text: str):
        ...

    ########################################################################################################################
    # [SECTION] imgui selectables
    ########################################################################################################################

    @staticmethod
    def Selectable(label: str, selected: bool = False, flags: ImGuiSelectableFlags | int = 0, size=None) -> Tuple[bool, bool]:
        ...

    ########################################################################################################################
    # [SECTION] imgui drag widgets
    ########################################################################################################################

    @staticmethod
    def DragFloat(label, value: float, speed: float = 1.0, min: float = 0.0, max: float = 0.0, format: str = "%.3f", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, float]:
        ...

    @staticmethod
    def DragFloat2(label, value: List[float], speed: float = 1.0, min: float = 0.0, max: float = 0.0, format: str = "%.3f", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, List[float]]:
        ...

    @staticmethod
    def DragFloat3(label, value: List[float], speed: float = 1.0, min: float = 0.0, max: float = 0.0, format: str = "%.3f", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, List[float]]:
        ...

    @staticmethod
    def DragFloat4(label, value: List[float], speed: float = 1.0, min: float = 0.0, max: float = 0.0, format: str = "%.3f", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, List[float]]:
        ...

    @staticmethod
    def DragFloatRange2(label, min_value: float, max_value: float, speed: float = 1.0, min: float = 0.0, max: float = 0.0, format: str = "%.3f", format_max: str | None = None, flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, float, float]:
        ...

    @staticmethod
    def DragInt(label, value: int, speed: float = 1.0, min: int = 0, max: int = 0, format: str = "%d", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, int]:
        ...

    @staticmethod
    def DragInt2(label, value: List[int], speed: float = 1.0, min: int = 0, max: int = 0, format: str = "%d", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, List[int]]:
        ...

    @staticmethod
    def DragInt3(label, value: List[int], speed: float = 1.0, min: int = 0, max: int = 0, format: str = "%d", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, List[int]]:
        ...

    @staticmethod
    def DragInt4(label, value: List[int], speed: float = 1.0, min: int = 0, max: int = 0, format: str = "%d", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, List[int]]:
        ...

    @staticmethod
    def DragIntRange2(label, min_value: int, max_value: int, speed: float = 1.0, min: int = 0, max: int = 0, format: str = "%d", format_max: str | None = None, flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, int, int]:
        ...

    ########################################################################################################################
    # [SECTION] imgui slider widgets
    ########################################################################################################################

    @staticmethod
    def SliderFloat(label, value: float, min: float, max: float, format: str = "%.3f", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, float]:
        ...

    @staticmethod
    def SliderFloat2(label, value: List[float], min: float, max: float, format: str = "%.3f", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, List[float]]:
        ...

    @staticmethod
    def SliderFloat3(label, value: List[float], min: float, max: float, format: str = "%.3f", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, List[float]]:
        ...

    @staticmethod
    def SliderFloat4(label, value: List[float], min: float, max: float, format: str = "%.3f", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, List[float]]:
        ...

    @staticmethod
    def SliderAngle(label, radians: float, degrees_min: float = -360.0, degrees_max: float = 360.0, format: str = "%.0f deg", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, float]:
        ...

    @staticmethod
    def SliderInt(label, value: int, min: int, max: int, format: str = "%d", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, int]:
        ...

    @staticmethod
    def SliderInt2(label, value: List[int], min: int, max: int, format: str = "%d", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, List[int]]:
        ...

    @staticmethod
    def SliderInt3(label, value: List[int], min: int, max: int, format: str = "%d", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, List[int]]:
        ...

    @staticmethod
    def SliderInt4(label, value: List[int], min: int, max: int, format: str = "%d", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, List[int]]:
        ...

    @staticmethod
    def VSliderFloat(label, size, value: float, min: float, max: float, format: str = "%.3f", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, float]:
        ...

    @staticmethod
    def VSliderInt(label, size, value: int, min: int, max: int, format: str = "%d", flags: ImGuiSliderFlags | int = 0) -> Tuple[bool, int]:
        ...

    ########################################################################################################################
    # [SECTION] imgui input widgets
    ########################################################################################################################

    @staticmethod
    def InputText(label: str, value: str, flags: ImGuiInputTextFlags | int = 0) -> Tuple[bool, str]:
        ...

    @staticmethod
    def InputTextMultiline(label: str, value: str, size=None, flags: ImGuiInputTextFlags | int = 0) -> Tuple[bool, str]:
        ...

    @staticmethod
    def InputTextWithHint(label: str, hint: str, value: str, flags: ImGuiInputTextFlags | int = 0) -> Tuple[bool, str]:
        ...

    @staticmethod
    def InputFloat(label: str, value: float, step: float = 0.0, step_fast: float = 0.0, format: str = "%.3f", flags: ImGuiInputTextFlags | int = 0) -> Tuple[bool, float]:
        ...

    @staticmethod
    def InputFloat2(label: str, value: List[float], format: str = "%.3f", flags: ImGuiInputTextFlags | int = 0) -> Tuple[bool, List[float]]:
        ...

    @staticmethod
    def InputFloat3(label: str, value: List[float], format: str = "%.3f", flags: ImGuiInputTextFlags | int = 0) -> Tuple[bool, List[float]]:
        ...

    @staticmethod
    def InputFloat4(label: str, value: List[float], format: str = "%.3f", flags: ImGuiInputTextFlags | int = 0) -> Tuple[bool, List[float]]:
        ...

    @staticmethod
    def InputInt(label: str, value: int, step: int = 1, step_fast: int = 100, flags: ImGuiInputTextFlags | int = 0) -> bool:
        ...

    @staticmethod
    def InputInt2(label: str, value: List[int], flags: ImGuiInputTextFlags | int = 0) -> Tuple[bool, int]:
        ...

    @staticmethod
    def InputInt3(label: str, value: List[int], flags: ImGuiInputTextFlags | int = 0) -> Tuple[bool, List[int]]:
        ...

    @staticmethod
    def InputInt4(label: str, value: List[int], flags: ImGuiInputTextFlags | int = 0) -> Tuple[bool, List[int]]:
        ...

    @staticmethod
    def InputDouble(label: str, value: float, step: float = 0.0, step_fast: float = 0.0, format: str = "%.6f", flags: ImGuiInputTextFlags | int = 0) -> Tuple[bool, float]:
        ...

    ########################################################################################################################
    # [SECTION] imgui text widgets
    ########################################################################################################################

    @staticmethod
    def TextUnformatted(text: str):
        ...

    @staticmethod
    def Text(text: str):
        ...

    @staticmethod
    def TextColored(color, text: str):
        ...

    @staticmethod
    def TextDisabled(text: str):
        ...

    @staticmethod
    def TextWrapped(text: str):
        ...

    @staticmethod
    def LabelText(label: str, text: str):
        ...

    @staticmethod
    def BulletText(text: str):
        ...

    @staticmethod
    def SeparatorText(label: str):
        ...

    ########################################################################################################################
    # [SECTION] imgui layout
    ########################################################################################################################

    @staticmethod
    def Separator():
        ...

    @staticmethod
    def SameLine(offset_from_start_x: float = 0.0, spacing: float = -1.0):
        ...

    @staticmethod
    def NewLine():
        ...

    @staticmethod
    def Spacing():
        ...

    @staticmethod
    def Dummy(size):
        ...

    @staticmethod
    def Indent(indent_w: float = 0.0):
        ...

    @staticmethod
    def Unindent(indent_w: float = 0.0):
        ...

    @staticmethod
    def BeginGroup():
        ...

    @staticmethod
    def EndGroup():
        ...

    @staticmethod
    def AlignTextToFramePadding():
        ...

    @staticmethod
    def GetTextLineHeight() -> float:
        ...

    @staticmethod
    def GetTextLineHeightWithSpacing() -> float:
        ...

    @staticmethod
    def GetFrameHeight() -> float:
        ...

    @staticmethod
    def GetFrameHeightWithSpacing() -> float:
        ...

    ########################################################################################################################
    # [SECTION] imgui layout cursor positioning
    ########################################################################################################################

    @staticmethod
    def GetCursorScreenPos() -> Tuple[float, float]:
        ...

    @staticmethod
    def SetCursorScreenPos(pos):
        ...

    @staticmethod
    def GetContentRegionAvail() -> Tuple[float, float]:
        ...

    @staticmethod
    def GetCursorPos() -> Tuple[float, float]:
        ...

    @staticmethod
    def GetCursorPosX() -> float:
        ...

    @staticmethod
    def GetCursorPosY() -> float:
        ...

    @staticmethod
    def SetCursorPos(local_pos):
        ...

    @staticmethod
    def SetCursorPosX(local_x: float):
        ...

    @staticmethod
    def SetCursorPosY(local_y: float):
        ...

    @staticmethod
    def GetCursorStartPos() -> Tuple[float, float]:
        ...

    ########################################################################################################################
    # [SECTION] imgui window scrolling
    ########################################################################################################################

    @staticmethod
    def GetScrollX() -> float:
        ...

    @staticmethod
    def GetScrollY() -> float:
        ...

    @staticmethod
    def SetScrollX(scroll_x: float):
        ...

    @staticmethod
    def SetScrollY(scroll_y: float):
        ...

    @staticmethod
    def GetScrollMaxX() -> float:
        ...

    @staticmethod
    def GetScrollMaxY() -> float:
        ...

    @staticmethod
    def SetScrollHereX(center_x_ratio: float = 0.5):
        ...

    @staticmethod
    def SetScrollHereY(center_y_ratio: float = 0.5):
        ...

    @staticmethod
    def SetScrollFromPosX(local_x: float, center_x_ratio: float = 0.5):
        ...

    @staticmethod
    def SetScrollFromPosY(local_y: float, center_y_ratio: float = 0.5):
        ...

    ########################################################################################################################
    # [SECTION] imgui parameters stacks current window
    ########################################################################################################################

    @staticmethod
    def PushItemWidth(item_width: float):
        ...

    @staticmethod
    def PopItemWidth():
        ...

    @staticmethod
    def SetNextItemWidth(item_width: float):
        ...

    @staticmethod
    def CalcItemWidth() -> float:
        ...

    @staticmethod
    def PushTextWrapPos(wrap_local_pos_x: float = 0.0):
        ...

    @staticmethod
    def PopTextWrapPos():
        ...

    ########################################################################################################################
    # [SECTION] imgui color editor/picker widgets
    ########################################################################################################################

    @staticmethod
    def ColorEdit3(label: str, color: List[float], flags: ImGuiColorEditFlags | int = 0) -> Tuple[bool, List[float]]:
        ...

    @staticmethod
    def ColorEdit4(label: str, color: List[float], flags: ImGuiColorEditFlags | int = 0) -> Tuple[bool, List[float]]:
        ...

    @staticmethod
    def ColorPicker3(label: str, color: List[float], flags: ImGuiColorEditFlags | int = 0) -> Tuple[bool, List[float]]:
        ...

    @staticmethod
    def ColorPicker4(label: str, color: List[float], flags: ImGuiColorEditFlags | int = 0, ref_color: List[float] | None = None) -> Tuple[bool, List[float]]:
        ...

    @staticmethod
    def ColorButton(desc_id: str, color: List[float], flags: ImGuiColorEditFlags | int = 0, size=None) -> bool:
        ...

    @staticmethod
    def SetColorEditOptions(flags: ImGuiColorEditFlags):
        ...

    ########################################################################################################################
    # [SECTION] imgui tree widgets
    ########################################################################################################################

    @staticmethod
    def TreeNode(label: str) -> bool:
        ...

    @staticmethod
    def TreeNodeEx(label: str, flags: ImGuiTreeNodeFlags | int = 0) -> bool:
        ...

    @staticmethod
    def TreePush(str_id: str):
        ...

    @staticmethod
    def TreePop():
        ...

    @staticmethod
    def GetTreeNodeToLabelSpacing() -> float:
        ...

    @staticmethod
    @overload
    def CollapsingHeader(label: str, flags: ImGuiTreeNodeFlags | int = 0) -> bool:
        ...

    @staticmethod
    @overload
    def CollapsingHeader(label: str, visible: bool, flags: ImGuiTreeNodeFlags | int = 0) -> Tuple[bool, bool]:
        ...

    @staticmethod
    def SetNextItemOpen(is_open: bool, cond: ImGuiCond | int = 0):
        ...

    @staticmethod
    def SetNextItemStorageID(storage_id: int):
        ...

    @staticmethod
    def TreeNodeGetOpen(storage_id: int) -> bool:
        ...

    ########################################################################################################################
    # [SECTION] imgui popups
    ########################################################################################################################

    @staticmethod
    def BeginPopup(str_id: str, flags: ImGuiWindowFlags | int = 0) -> bool:
        ...

    @staticmethod
    def BeginPopupModal(name: str, open: bool | None = None, flags: ImGuiWindowFlags | int = 0) -> Tuple[bool, bool | None]:
        ...

    @staticmethod
    def EndPopup():
        ...

    @staticmethod
    def OpenPopup(str_id: str, popup_flags: ImGuiPopupFlags | int = 0):
        ...

    @staticmethod
    def OpenPopupOnItemClick(str_id: str | None = None, popup_flags: ImGuiPopupFlags | int = 0):
        ...

    @staticmethod
    def CloseCurrentPopup():
        ...

    @staticmethod
    def BeginPopupContextItem(str_id: str | None = None, popup_flags: ImGuiPopupFlags | int = 0) -> bool:
        ...

    @staticmethod
    def BeginPopupContextWindow(str_id: str | None = None, popup_flags: ImGuiPopupFlags | int = 0) -> bool:
        ...

    @staticmethod
    def BeginPopupContextVoid(str_id: str | None = None, popup_flags: ImGuiPopupFlags | int = 0) -> bool:
        ...

    @staticmethod
    def IsPopupOpen(str_id: str, flags: ImGuiPopupFlags | int = 0) -> bool:
        ...

    ########################################################################################################################
    # [SECTION] imgui tab bars / tabs
    ########################################################################################################################

    @staticmethod
    def BeginTabBar(str_id: str, flags: ImGuiTabBarFlags | int = 0) -> bool:
        ...

    @staticmethod
    def EndTabBar():
        ...

    @staticmethod
    def BeginTabItem(label: str, open: bool | None = None, flags: ImGuiTabItemFlags | int = 0) -> Tuple[bool, bool | None]:
        ...

    @staticmethod
    def EndTabItem():
        ...

    @staticmethod
    def TabItemButton(label: str, flags: ImGuiTabItemFlags | int = 0) -> bool:
        ...

    @staticmethod
    def SetTabItemClosed(tab_or_docked_window_label: str):
        ...

    ########################################################################################################################
    # [SECTION] imgui item/widget utilities and query functions
    ########################################################################################################################

    @staticmethod
    def IsItemHovered(flags: ImGuiHoveredFlags | int = 0) -> bool:
        ...

    @staticmethod
    def IsItemActive() -> bool:
        ...

    @staticmethod
    def IsItemFocused() -> bool:
        ...

    @staticmethod
    def IsItemClicked(mouse_button: int = 0) -> bool:
        ...

    @staticmethod
    def IsItemVisible() -> bool:
        ...

    @staticmethod
    def IsItemEdited() -> bool:
        ...

    @staticmethod
    def IsItemActivated() -> bool:
        ...

    @staticmethod
    def IsItemDeactivated() -> bool:
        ...

    @staticmethod
    def IsItemDeactivatedAfterEdit() -> bool:
        ...

    @staticmethod
    def IsItemToggledOpen() -> bool:
        ...

    @staticmethod
    def IsAnyItemHovered() -> bool:
        ...

    @staticmethod
    def IsAnyItemActive() -> bool:
        ...

    @staticmethod
    def IsAnyItemFocused() -> bool:
        ...

    @staticmethod
    def GetItemID() -> int:
        ...

    @staticmethod
    def GetItemRectMin() -> Tuple[float, float]:
        ...

    @staticmethod
    def GetItemRectMax() -> Tuple[float, float]:
        ...

    @staticmethod
    def GetItemRectSize() -> Tuple[float, float]:
        ...

    @staticmethod
    def SetItemAllowOverlap():
        ...

    @staticmethod
    def SetItemDefaultFocus():
        ...

    @staticmethod
    def SetKeyboardFocusHere(offset: int = 0):
        ...

    @staticmethod
    def SetNextItemAllowOverlap():
        ...

########################################################################################################################
# [SECTION] implot
########################################################################################################################

class ImPlot:

    @staticmethod
    def ShowDemoWindow(open: bool | None = None) -> bool | None:
        ...