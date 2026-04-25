from typing import List, NewType
from pilotlight.types import *
from pilotlight.enums import *

########################################################################################################################
# [SECTION] opaque types
########################################################################################################################

plBoolPointer = NewType("plBoolPointer", object)
plRenderEncoder = NewType("plRenderEncoder", object)
plDevice = NewType("plDevice", object)
plSwapchain = NewType("plSwapchain", object)
plFloatPointer = NewType("plFloatPointer", object)
plDoublePointer = NewType("plDoublePointer", object)
plIntPointer = NewType("plIntPointer", object)

########################################################################################################################
# [SECTION] pilotlight hooks
########################################################################################################################

def pl_dear_imgui_initialize(device: plDevice, swapchain: plSwapchain, renderpass_handle, **kwargs) -> None:
    ...

def pl_dear_imgui_new_frame(device: plDevice, renderpass_handle, **kwargs) -> None:
    ...

def pl_dear_imgui_render(encoder: plRenderEncoder, **kwargs) -> None:
    ...

def pl_dear_imgui_cleanup(**kwargs) -> None:
    ...

########################################################################################################################
# [SECTION] imgui demo/debug/information
########################################################################################################################

def ImGui_ShowDemoWindow(bool_pointer = None) -> None:
    ...

def ImGui_ShowDemoWindow(bool_pointer: plBoolPointer = None) -> None:
    ...

def ImGui_ShowMetricsWindow(bool_pointer: plBoolPointer = None) -> None:
    ...

def ImGui_ShowDebugLogWindow(bool_pointer: plBoolPointer = None) -> None:
    ...

def ImGui_ShowIDStackToolWindow(bool_pointer: plBoolPointer = None) -> None:
    ...

def ImGui_ShowAboutWindow(bool_pointer: plBoolPointer = None) -> None:
    ...

def ImGui_ShowStyleEditor() -> None:
    ...

def ImGui_ShowStyleSelector(label: str) -> bool:
    ...

def ImGui_ShowFontSelector(label: str) -> None:
    ...

def ImGui_ShowUserGuide() -> None:
    ...

def ImGui_GetVersion() -> str:
    ...

########################################################################################################################
# [SECTION] imgui
########################################################################################################################

def ImGui_StyleColorsDark():
    ...

def ImGui_StyleColorsLight():
    ...

def ImGui_StyleColorsClassic():
    ...

def ImGui_Begin(name, bool_pointer: plBoolPointer = None, flags: ImGuiWindowFlags = 0) -> None:
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

def ImGui_MenuItem(label, shortcut="", selected=False, enabled=True, selected_pointer: plBoolPointer = None):
    ...

########################################################################################################################
# [SECTION] imgui window utilities
########################################################################################################################

def ImGui_IsWindowAppearing() -> bool:
    ...

def ImGui_IsWindowCollapsed() -> bool:
    ...

def ImGui_IsWindowFocused(flags: ImGuiFocusedFlags = 0) -> bool:
    ...

def ImGui_IsWindowHovered(flags: ImGuiHoveredFlags = 0) -> bool:
    ...

def ImGui_GetWindowDpiScale() -> float:
    ...

def ImGui_GetWindowPos() -> tuple[float, float]:
    ...

def ImGui_GetWindowSize() -> tuple[float, float]:
    ...

def ImGui_GetWindowWidth() -> float:
    ...

def ImGui_GetWindowHeight() -> float:
    ...

########################################################################################################################
# [SECTION] imgui window manipulation
########################################################################################################################

def ImGui_SetNextWindowPos(pos, cond: ImGuiCond = 0, pivot=None) -> None:
    ...

def ImGui_SetNextWindowSize(size, cond: ImGuiCond = 0) -> None:
    ...

def ImGui_SetNextWindowContentSize(size) -> None:
    ...

def ImGui_SetNextWindowCollapsed(collapsed: bool, cond: ImGuiCond = 0) -> None:
    ...

def ImGui_SetNextWindowFocus() -> None:
    ...

def ImGui_SetNextWindowScroll(scroll) -> None:
    ...

def ImGui_SetNextWindowBgAlpha(alpha: float) -> None:
    ...

def ImGui_SetNextWindowViewport(viewport_id: int) -> None:
    ...

def ImGui_SetWindowPos(pos, cond: ImGuiCond = 0, name: str = None) -> None:
    ...

def ImGui_SetWindowSize(size, cond: ImGuiCond = 0, name: str = None) -> None:
    ...

def ImGui_SetWindowCollapsed(collapsed: bool, cond: ImGuiCond = 0, name: str = None) -> None:
    ...

def ImGui_SetWindowFocus(name: str = None) -> None:
    ...

########################################################################################################################
# [SECTION] imgui input utilities mouse
########################################################################################################################

def ImGui_IsMouseDown(button: ImGuiMouseButton) -> bool:
    ...

def ImGui_IsMouseClicked(button: ImGuiMouseButton, repeat: bool = False) -> bool:
    ...

def ImGui_IsMouseReleased(button: ImGuiMouseButton) -> bool:
    ...

def ImGui_IsMouseDoubleClicked(button: ImGuiMouseButton) -> bool:
    ...

def ImGui_GetMouseClickedCount(button: ImGuiMouseButton) -> int:
    ...

def ImGui_IsMouseHoveringRect(min, max, clip: bool = True) -> bool:
    ...

def ImGui_IsMousePosValid(mouse_pos=None) -> bool:
    ...

def ImGui_IsAnyMouseDown() -> bool:
    ...

def ImGui_GetMousePos() -> tuple[float, float]:
    ...

def ImGui_GetMousePosOnOpeningCurrentPopup() -> tuple[float, float]:
    ...

def ImGui_IsMouseDragging(button: ImGuiMouseButton, lock_threshold: float = -1.0) -> bool:
    ...

def ImGui_GetMouseDragDelta(button: ImGuiMouseButton = 0, lock_threshold: float = -1.0) -> tuple[float, float]:
    ...

def ImGui_ResetMouseDragDelta(button: ImGuiMouseButton = 0) -> None:
    ...

def ImGui_GetMouseCursor() -> ImGuiMouseCursor:
    ...

def ImGui_SetMouseCursor(cursor_type: ImGuiMouseCursor) -> None:
    ...

def ImGui_SetNextFrameWantCaptureMouse(want_capture_mouse: bool) -> None:
    ...

########################################################################################################################
# [SECTION] imgui clipboard utilities
########################################################################################################################

def ImGui_GetClipboardText() -> str | None:
    ...

def ImGui_SetClipboardText(text: str) -> None:
    ...


########################################################################################################################
# [SECTION] imgui settings / ini utilities
########################################################################################################################

def ImGui_LoadIniSettingsFromDisk(ini_filename: str) -> None:
    ...

def ImGui_LoadIniSettingsFromMemory(ini_data: str) -> None:
    ...

def ImGui_SaveIniSettingsToDisk(ini_filename: str) -> None:
    ...

def ImGui_SaveIniSettingsToMemory() -> str | None:
    ...

########################################################################################################################
# [SECTION] imgui child windows
########################################################################################################################

def ImGui_BeginChild(str_id: str, size=None, child_flags: ImGuiChildFlags = 0, window_flags: ImGuiWindowFlags = 0) -> bool:
    ...

def ImGui_EndChild() -> None:
    ...

########################################################################################################################
# [SECTION] imgui id stack/scopes
########################################################################################################################

def ImGui_PushID(id: str | int) -> None:
    ...

def ImGui_PopID() -> None:
    ...

def ImGui_GetID(id: str | int) -> int:
    ...

########################################################################################################################
# [SECTION] imgui main widgets
########################################################################################################################

def ImGui_Button(label, size=None):
    ...

def ImGui_Checkbox(label, bool_pointer: plBoolPointer):
    ...

def ImGui_SmallButton(label: str) -> bool:
    ...

def ImGui_InvisibleButton(str_id: str, size, flags: ImGuiButtonFlags = 0) -> bool:
    ...

def ImGui_ArrowButton(str_id: str, dir: ImGuiDir) -> bool:
    ...

def ImGui_CheckboxFlags(label: str, flags_pointer: plIntPointer, flags_value: int) -> bool:
    ...

def ImGui_RadioButton(label: str, active: bool = False, value_pointer: plIntPointer = None, button_value: int = 0) -> bool:
    ...

def ImGui_ProgressBar(fraction: float, size=None, overlay: str = None) -> None:
    ...

def ImGui_Bullet() -> None:
    ...

def ImGui_TextLink(label: str) -> bool:
    ...

def ImGui_TextLinkOpenURL(label: str, url: str = None) -> bool:
    ...

########################################################################################################################
# [SECTION] imgui combo box/dropdown widgets
########################################################################################################################

def ImGui_BeginCombo(label: str, preview_value: str, flags: ImGuiComboFlags = 0) -> bool:
    ...

def ImGui_EndCombo() -> None:
    ...

########################################################################################################################
# [SECTION] imgui list boxes
########################################################################################################################

def ImGui_BeginListBox(label: str, size=None) -> bool:
    ...

def ImGui_EndListBox() -> None:
    ...

########################################################################################################################
# [SECTION] imgui tooltips
########################################################################################################################

def ImGui_BeginTooltip() -> bool:
    ...

def ImGui_EndTooltip() -> None:
    ...

def ImGui_SetTooltip(text: str) -> None:
    ...

def ImGui_BeginItemTooltip() -> bool:
    ...

def ImGui_SetItemTooltip(text: str) -> None:
    ...

########################################################################################################################
# [SECTION] imgui selectables
########################################################################################################################

def ImGui_Selectable(label: str, selected: bool = False, flags: ImGuiSelectableFlags = 0, size=None, selected_pointer: plBoolPointer = None) -> bool:
    ...

########################################################################################################################
# [SECTION] imgui drag widgets
########################################################################################################################

def ImGui_DragFloat(label, value_pointer: plFloatPointer, speed: float = 1.0, min: float = 0.0, max: float = 0.0, format: str = "%.3f", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_DragFloat2(label, value_pointer: plFloatPointer, speed: float = 1.0, min: float = 0.0, max: float = 0.0, format: str = "%.3f", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_DragFloat3(label, value_pointer: plFloatPointer, speed: float = 1.0, min: float = 0.0, max: float = 0.0, format: str = "%.3f", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_DragFloat4(label, value_pointer: plFloatPointer, speed: float = 1.0, min: float = 0.0, max: float = 0.0, format: str = "%.3f", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_DragFloatRange2(label, current_min_pointer: plFloatPointer, current_max_pointer: plFloatPointer, speed: float = 1.0, min: float = 0.0, max: float = 0.0, format: str = "%.3f", format_max: str = None, flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_DragInt(label, value_pointer: plIntPointer, speed: float = 1.0, min: int = 0, max: int = 0, format: str = "%d", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_DragInt2(label, value_pointer: plIntPointer, speed: float = 1.0, min: int = 0, max: int = 0, format: str = "%d", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_DragInt3(label, value_pointer: plIntPointer, speed: float = 1.0, min: int = 0, max: int = 0, format: str = "%d", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_DragInt4(label, value_pointer: plIntPointer, speed: float = 1.0, min: int = 0, max: int = 0, format: str = "%d", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_DragIntRange2(label, current_min_pointer: plIntPointer, current_max_pointer: plIntPointer, speed: float = 1.0, min: int = 0, max: int = 0, format: str = "%d", format_max: str = None, flags: ImGuiSliderFlags = 0) -> bool:
    ...

########################################################################################################################
# [SECTION] imgui slider widgets
########################################################################################################################

def ImGui_SliderFloat(label, value_pointer: plFloatPointer, min: float, max: float, format: str = "%.3f", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_SliderFloat2(label, value_pointer: plFloatPointer, min: float, max: float, format: str = "%.3f", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_SliderFloat3(label, value_pointer: plFloatPointer, min: float, max: float, format: str = "%.3f", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_SliderFloat4(label, value_pointer: plFloatPointer, min: float, max: float, format: str = "%.3f", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_SliderAngle(label, rad_pointer: plFloatPointer, degrees_min: float = -360.0, degrees_max: float = 360.0, format: str = "%.0f deg", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_SliderInt(label, value_pointer: plIntPointer, min: int, max: int, format: str = "%d", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_SliderInt2(label, value_pointer: plIntPointer, min: int, max: int, format: str = "%d", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_SliderInt3(label, value_pointer: plIntPointer, min: int, max: int, format: str = "%d", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_SliderInt4(label, value_pointer: plIntPointer, min: int, max: int, format: str = "%d", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_VSliderFloat(label, size, value_pointer: plFloatPointer, min: float, max: float, format: str = "%.3f", flags: ImGuiSliderFlags = 0) -> bool:
    ...

def ImGui_VSliderInt(label, size, value_pointer: plIntPointer, min: int, max: int, format: str = "%d", flags: ImGuiSliderFlags = 0) -> bool:
    ...

########################################################################################################################
# [SECTION] imgui input widgets
########################################################################################################################

def ImGui_InputText(label: str, value: bytearray, flags: ImGuiInputTextFlags = 0) -> bool:
    ...

def ImGui_InputTextMultiline(label: str, value: bytearray, size=None, flags: ImGuiInputTextFlags = 0) -> bool:
    ...

def ImGui_InputTextWithHint(label: str, hint: str, value: bytearray, flags: ImGuiInputTextFlags = 0) -> bool:
    ...

def ImGui_InputFloat(label: str, value_pointer: plFloatPointer, step: float = 0.0, step_fast: float = 0.0, format: str = "%.3f", flags: ImGuiInputTextFlags = 0) -> bool:
    ...

def ImGui_InputFloat2(label: str, value_pointer: plFloatPointer, format: str = "%.3f", flags: ImGuiInputTextFlags = 0) -> bool:
    ...

def ImGui_InputFloat3(label: str, value_pointer: plFloatPointer, format: str = "%.3f", flags: ImGuiInputTextFlags = 0) -> bool:
    ...

def ImGui_InputFloat4(label: str, value_pointer: plFloatPointer, format: str = "%.3f", flags: ImGuiInputTextFlags = 0) -> bool:
    ...

def ImGui_InputInt(label: str, value_pointer: plIntPointer, step: int = 1, step_fast: int = 100, flags: ImGuiInputTextFlags = 0) -> bool:
    ...

def ImGui_InputInt2(label: str, value_pointer: plIntPointer, flags: ImGuiInputTextFlags = 0) -> bool:
    ...

def ImGui_InputInt3(label: str, value_pointer: plIntPointer, flags: ImGuiInputTextFlags = 0) -> bool:
    ...

def ImGui_InputInt4(label: str, value_pointer: plIntPointer, flags: ImGuiInputTextFlags = 0) -> bool:
    ...

def ImGui_InputDouble(label: str, value_pointer: plDoublePointer, step: float = 0.0, step_fast: float = 0.0, format: str = "%.6f", flags: ImGuiInputTextFlags = 0) -> bool:
    ...

########################################################################################################################
# [SECTION] imgui text widgets
########################################################################################################################

def ImGui_TextUnformatted(text: str) -> None:
    ...

def ImGui_Text(text: str) -> None:
    ...

def ImGui_TextColored(color, text: str) -> None:
    ...

def ImGui_TextDisabled(text: str) -> None:
    ...

def ImGui_TextWrapped(text: str) -> None:
    ...

def ImGui_LabelText(label: str, text: str) -> None:
    ...

def ImGui_BulletText(text: str) -> None:
    ...

def ImGui_SeparatorText(label: str) -> None:
    ...

########################################################################################################################
# [SECTION] imgui layout
########################################################################################################################

def ImGui_Separator() -> None:
    ...

def ImGui_SameLine(offset_from_start_x: float = 0.0, spacing: float = -1.0) -> None:
    ...

def ImGui_NewLine() -> None:
    ...

def ImGui_Spacing() -> None:
    ...

def ImGui_Dummy(size) -> None:
    ...

def ImGui_Indent(indent_w: float = 0.0) -> None:
    ...

def ImGui_Unindent(indent_w: float = 0.0) -> None:
    ...

def ImGui_BeginGroup() -> None:
    ...

def ImGui_EndGroup() -> None:
    ...

def ImGui_AlignTextToFramePadding() -> None:
    ...

def ImGui_GetTextLineHeight() -> float:
    ...

def ImGui_GetTextLineHeightWithSpacing() -> float:
    ...

def ImGui_GetFrameHeight() -> float:
    ...

def ImGui_GetFrameHeightWithSpacing() -> float:
    ...

########################################################################################################################
# [SECTION] imgui layout cursor positioning
########################################################################################################################

def ImGui_GetCursorScreenPos() -> tuple[float, float]:
    ...

def ImGui_SetCursorScreenPos(pos) -> None:
    ...

def ImGui_GetContentRegionAvail() -> tuple[float, float]:
    ...

def ImGui_GetCursorPos() -> tuple[float, float]:
    ...

def ImGui_GetCursorPosX() -> float:
    ...

def ImGui_GetCursorPosY() -> float:
    ...

def ImGui_SetCursorPos(local_pos) -> None:
    ...

def ImGui_SetCursorPosX(local_x: float) -> None:
    ...

def ImGui_SetCursorPosY(local_y: float) -> None:
    ...

def ImGui_GetCursorStartPos() -> tuple[float, float]:
    ...

########################################################################################################################
# [SECTION] imgui window scrolling
########################################################################################################################

def ImGui_GetScrollX() -> float:
    ...

def ImGui_GetScrollY() -> float:
    ...

def ImGui_SetScrollX(scroll_x: float) -> None:
    ...

def ImGui_SetScrollY(scroll_y: float) -> None:
    ...

def ImGui_GetScrollMaxX() -> float:
    ...

def ImGui_GetScrollMaxY() -> float:
    ...

def ImGui_SetScrollHereX(center_x_ratio: float = 0.5) -> None:
    ...

def ImGui_SetScrollHereY(center_y_ratio: float = 0.5) -> None:
    ...

def ImGui_SetScrollFromPosX(local_x: float, center_x_ratio: float = 0.5) -> None:
    ...

def ImGui_SetScrollFromPosY(local_y: float, center_y_ratio: float = 0.5) -> None:
    ...

########################################################################################################################
# [SECTION] imgui parameters stacks current window
########################################################################################################################

def ImGui_PushItemWidth(item_width: float) -> None:
    ...

def ImGui_PopItemWidth() -> None:
    ...

def ImGui_SetNextItemWidth(item_width: float) -> None:
    ...

def ImGui_CalcItemWidth() -> float:
    ...

def ImGui_PushTextWrapPos(wrap_local_pos_x: float = 0.0) -> None:
    ...

def ImGui_PopTextWrapPos() -> None:
    ...

########################################################################################################################
# [SECTION] imgui color editor/picker widgets
########################################################################################################################

def ImGui_ColorEdit3(label: str, color_pointer: plFloatPointer, flags: ImGuiColorEditFlags = 0) -> bool:
    ...

def ImGui_ColorEdit4(label: str, color_pointer: plFloatPointer, flags: ImGuiColorEditFlags = 0) -> bool:
    ...

def ImGui_ColorPicker3(label: str, color_pointer: plFloatPointer, flags: ImGuiColorEditFlags = 0) -> bool:
    ...

def ImGui_ColorPicker4(label: str, color_pointer: plFloatPointer, flags: ImGuiColorEditFlags = 0, ref_color: plFloatPointer = None) -> bool:
    ...

def ImGui_ColorButton(desc_id: str, color, flags: ImGuiColorEditFlags = 0, size=None) -> bool:
    ...

def ImGui_SetColorEditOptions(flags: ImGuiColorEditFlags) -> None:
    ...

########################################################################################################################
# [SECTION] imgui tree widgets
########################################################################################################################

def ImGui_TreeNode(label: str) -> bool:
    ...

def ImGui_TreeNodeEx(label: str, flags: ImGuiTreeNodeFlags = 0) -> bool:
    ...

def ImGui_TreePush(str_id: str) -> None:
    ...

def ImGui_TreePop() -> None:
    ...

def ImGui_GetTreeNodeToLabelSpacing() -> float:
    ...

def ImGui_CollapsingHeader(label: str, visible_pointer: plBoolPointer = None, flags: ImGuiTreeNodeFlags = 0) -> bool:
    ...

def ImGui_SetNextItemOpen(is_open: bool, cond: ImGuiCond = 0) -> None:
    ...

def ImGui_SetNextItemStorageID(storage_id: int) -> None:
    ...

def ImGui_TreeNodeGetOpen(storage_id: int) -> bool:
    ...

########################################################################################################################
# [SECTION] imgui popups
########################################################################################################################

def ImGui_BeginPopup(str_id: str, flags: ImGuiWindowFlags = 0) -> bool:
    ...

def ImGui_BeginPopupModal(name: str, open: plBoolPointer = None, flags: ImGuiWindowFlags = 0) -> bool:
    ...

def ImGui_EndPopup() -> None:
    ...

def ImGui_OpenPopup(str_id: str, popup_flags: ImGuiPopupFlags = 0) -> None:
    ...

def ImGui_OpenPopupOnItemClick(str_id: str = None, popup_flags: ImGuiPopupFlags = 0) -> None:
    ...

def ImGui_CloseCurrentPopup() -> None:
    ...

def ImGui_BeginPopupContextItem(str_id: str = None, popup_flags: ImGuiPopupFlags = 0) -> bool:
    ...

def ImGui_BeginPopupContextWindow(str_id: str = None, popup_flags: ImGuiPopupFlags = 0) -> bool:
    ...

def ImGui_BeginPopupContextVoid(str_id: str = None, popup_flags: ImGuiPopupFlags = 0) -> bool:
    ...

def ImGui_IsPopupOpen(str_id: str, flags: ImGuiPopupFlags = 0) -> bool:
    ...

########################################################################################################################
# [SECTION] imgui tab bars / tabs
########################################################################################################################

def ImGui_BeginTabBar(str_id: str, flags: ImGuiTabBarFlags = 0) -> bool:
    ...

def ImGui_EndTabBar() -> None:
    ...

def ImGui_BeginTabItem(label: str, open: plBoolPointer = None, flags: ImGuiTabItemFlags = 0) -> bool:
    ...

def ImGui_EndTabItem() -> None:
    ...

def ImGui_TabItemButton(label: str, flags: ImGuiTabItemFlags = 0) -> bool:
    ...

def ImGui_SetTabItemClosed(tab_or_docked_window_label: str) -> None:
    ...

########################################################################################################################
# [SECTION] imgui item/widget utilities and query functions
########################################################################################################################

def ImGui_IsItemHovered(flags: ImGuiHoveredFlags = 0) -> bool:
    ...

def ImGui_IsItemActive() -> bool:
    ...

def ImGui_IsItemFocused() -> bool:
    ...

def ImGui_IsItemClicked(mouse_button: int = 0) -> bool:
    ...

def ImGui_IsItemVisible() -> bool:
    ...

def ImGui_IsItemEdited() -> bool:
    ...

def ImGui_IsItemActivated() -> bool:
    ...

def ImGui_IsItemDeactivated() -> bool:
    ...

def ImGui_IsItemDeactivatedAfterEdit() -> bool:
    ...

def ImGui_IsItemToggledOpen() -> bool:
    ...

def ImGui_IsAnyItemHovered() -> bool:
    ...

def ImGui_IsAnyItemActive() -> bool:
    ...

def ImGui_IsAnyItemFocused() -> bool:
    ...

def ImGui_GetItemID() -> int:
    ...

def ImGui_GetItemRectMin() -> tuple[float, float]:
    ...

def ImGui_GetItemRectMax() -> tuple[float, float]:
    ...

def ImGui_GetItemRectSize() -> tuple[float, float]:
    ...

def ImGui_SetItemAllowOverlap() -> None:
    ...

def ImGui_SetItemDefaultFocus() -> None:
    ...

def ImGui_SetKeyboardFocusHere(offset: int = 0) -> None:
    ...

def ImGui_SetNextItemAllowOverlap() -> None:
    ...

########################################################################################################################
# [SECTION] implot
########################################################################################################################

def ImPlot_ShowDemoWindow(bool_pointer: plBoolPointer = None) -> None:
    ...