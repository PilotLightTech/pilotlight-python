########################################################################################################################
# User API Index
#
#    * Sections
#      - imports
#      - types
#      - apis
#      - enums
#
########################################################################################################################

########################################################################################################################
# [SECTION] imports
########################################################################################################################

import pilotlight.pilotlight as internal
from typing import List

########################################################################################################################
# [SECTION] types
########################################################################################################################

class plIO:
    def __init__(self):
        self.fDeltaTime = 0
        self.fMouseDragThreshold = 0
        self.fMouseDoubleClickTime = 0
        self.fMouseDoubleClickMaxDist = 0
        self.fMouseDoubleClickMaxDist = 0
        self.fKeyRepeatDelay = 0
        self.fKeyRepeatRate = 0
        self.fFrameRate = 0
        self.ulFrameCount = 0
        self.dTime = 0
        self.bKeyCtrl = False
        self.bKeyShift = False
        self.bKeyAlt = False
        self.bKeySuper = False
        self.bRunning = False
        self.tMainViewportSize = [0, 0]
        self.tMainFramebufferScale = [0, 0]

########################################################################################################################
# [SECTION] apis
########################################################################################################################

class plCoreI:

    def get_pointer_value(pointer):
        return internal.get_pointer_value(pointer)
    
    def set_pointer_value(pointer, value):
        return internal.set_pointer_value(pointer, value)

    def create_bool_pointer():
        return internal.create_bool_pointer()
    
    def destroy_bool_pointer(pointer):
        return internal.destroy_bool_pointer(pointer)

    def create_int_pointer():
        return internal.create_int_pointer()
    
    def destroy_int_pointer(pointer):
        return internal.destroy_int_pointer(pointer)

    def create_float_pointer():
        return internal.create_float_pointer()
    
    def destroy_float_pointer(pointer):
        return internal.destroy_float_pointer(pointer)

class plIOI:

    def get_version_string() -> str:
        return internal.plIOI_get_version_string()

    def get_io() -> dict:
        io_dict = internal.plIOI_get_io()
        io_struct = plIO()
        io_struct.fDeltaTime = io_dict["fDeltaTime"]
        io_struct.fMouseDragThreshold = io_dict["fMouseDragThreshold"]
        io_struct.fMouseDoubleClickTime = io_dict["fMouseDoubleClickTime"]
        io_struct.fMouseDoubleClickMaxDist = io_dict["fMouseDoubleClickMaxDist"]
        io_struct.fMouseDoubleClickMaxDist = io_dict["fMouseDoubleClickMaxDist"]
        io_struct.fKeyRepeatDelay = io_dict["fKeyRepeatDelay"]
        io_struct.fKeyRepeatRate = io_dict["fKeyRepeatRate"]
        io_struct.fFrameRate = io_dict["fFrameRate"]
        io_struct.ulFrameCount = io_dict["ulFrameCount"]
        io_struct.dTime = io_dict["dTime"]
        io_struct.bKeyCtrl = io_dict["bKeyCtrl"]
        io_struct.bKeyShift = io_dict["bKeyShift"]
        io_struct.bKeyAlt = io_dict["bKeyAlt"]
        io_struct.bKeySuper = io_dict["bKeySuper"]
        io_struct.bRunning = io_dict["bRunning"]
        io_struct.tMainViewportSize = io_dict["tMainViewportSize"]
        io_struct.tMainFramebufferScale = io_dict["tMainFramebufferScale"]

    def new_frame() -> None:
        return internal.pl_core_plIOI_new_frame()

    def is_key_pressed(key: int, repeat: bool = False) -> bool:
        return internal.plIOI_is_key_pressed(key, repeat)

    def is_key_released(key: int) -> bool:
        return internal.plIOI_is_key_released(key)
    
    def is_key_down(key: int) -> bool:
        return internal.plIOI_is_key_down(key)
    
    def get_key_pressed_amount(key: int, fRepeatDelay: float, fRate: float) -> int:
        return internal.plIOI_get_key_pressed_amount(key, fRepeatDelay, fRate)
    
    def is_mouse_down(button: int) -> bool:
        return internal.plIOI_is_mouse_down(button)
    
    def is_mouse_released(button: int) -> bool:
        return internal.plIOI_is_mouse_released(button)
    
    def is_mouse_double_clicked(button: int) -> bool:
        return internal.plIOI_is_mouse_double_clicked(button)
    
    def is_mouse_clicked(button: int, repeat: bool = False) -> bool:
        return internal.plIOI_is_mouse_clicked(button, repeat)
    
    def is_mouse_dragging(button: int, threshold: float) -> bool:
        return internal.plIOI_is_mouse_dragging(button, threshold)
    
    def is_mouse_hovering_rect(minvec: List[float], maxvec: List[float]) -> bool:
        return internal.plIOI_is_mouse_hovering_rect(minvec, maxvec)
    
    def reset_mouse_drag_delta(button: int) -> None:
        return internal.plIOI_reset_mouse_drag_delta(button)
    
    def get_mouse_drag_delta(button: int, threshold: float) -> List[float]:
        return internal.plIOI_get_mouse_drag_delta(button, threshold)
    
    def get_mouse_pos() -> None:
        return internal.plIOI_get_mouse_pos()
    
    def get_mouse_wheel() -> float:
        return internal.plIOI_get_mouse_wheel()

    def is_mouse_pos_valid(vec: List[float]) -> bool:
        return internal.plIOI_is_mouse_pos_valid(vec)

    def set_mouse_cursor(vec: int) -> None:
        return internal.plIOI_set_mouse_cursor(vec)

class plWindowI:

    def create(pcTitle, iXPos, iYPos, uWidth, uHeight, tFlags):
        return internal.plWindowI_create(pcTitle, iXPos, iYPos, uWidth, uHeight, tFlags)

    def show(window):
        return internal.plWindowI_show(window)
    
    def destroy(window):
        return internal.plWindowI_destroy(window)

########################################################################################################################
# [SECTION] enums
########################################################################################################################

# PL_MOUSE_BUTTON CONSTANTS
PL_MOUSE_BUTTON_LEFT=internal.PL_MOUSE_BUTTON_LEFT
PL_MOUSE_BUTTON_RIGHT=internal.PL_MOUSE_BUTTON_RIGHT
PL_MOUSE_BUTTON_MIDDLE=internal.PL_MOUSE_BUTTON_MIDDLE
PL_MOUSE_BUTTON_COUNT=internal.PL_MOUSE_BUTTON_COUNT

# PL_MOUSE_CURSOR CONSTANTS
PL_MOUSE_CURSOR_NONE=internal.PL_MOUSE_CURSOR_NONE
PL_MOUSE_CURSOR_ARROW=internal.PL_MOUSE_CURSOR_ARROW
PL_MOUSE_CURSOR_TEXT_INPUT=internal.PL_MOUSE_CURSOR_TEXT_INPUT
PL_MOUSE_CURSOR_RESIZE_ALL=internal.PL_MOUSE_CURSOR_RESIZE_ALL
PL_MOUSE_CURSOR_RESIZE_NS=internal.PL_MOUSE_CURSOR_RESIZE_NS
PL_MOUSE_CURSOR_RESIZE_EW=internal.PL_MOUSE_CURSOR_RESIZE_EW
PL_MOUSE_CURSOR_RESIZE_NESW=internal.PL_MOUSE_CURSOR_RESIZE_NESW
PL_MOUSE_CURSOR_RESIZE_NWSE=internal.PL_MOUSE_CURSOR_RESIZE_NWSE
PL_MOUSE_CURSOR_HAND=internal.PL_MOUSE_CURSOR_HAND
PL_MOUSE_CURSOR_WAIT=internal.PL_MOUSE_CURSOR_WAIT
PL_MOUSE_CURSOR_PROGRESS=internal.PL_MOUSE_CURSOR_PROGRESS
PL_MOUSE_CURSOR_NOT_ALLOWED=internal.PL_MOUSE_CURSOR_NOT_ALLOWED
PL_MOUSE_CURSOR_COUNT=internal.PL_MOUSE_CURSOR_COUNT

# PL_KEY CONSTANTS
PL_KEY_NONE=internal.PL_KEY_NONE
PL_KEY_NAMED_KEY_BEGIN=internal.PL_KEY_NAMED_KEY_BEGIN
PL_KEY_TAB=internal.PL_KEY_TAB
PL_KEY_LEFT_ARROW=internal.PL_KEY_LEFT_ARROW
PL_KEY_RIGHT_ARROW=internal.PL_KEY_RIGHT_ARROW
PL_KEY_UP_ARROW=internal.PL_KEY_UP_ARROW
PL_KEY_DOWN_ARROW=internal.PL_KEY_DOWN_ARROW
PL_KEY_PAGE_UP=internal.PL_KEY_PAGE_UP
PL_KEY_PAGE_DOWN=internal.PL_KEY_PAGE_DOWN
PL_KEY_HOME=internal.PL_KEY_HOME
PL_KEY_END=internal.PL_KEY_END
PL_KEY_INSERT=internal.PL_KEY_INSERT
PL_KEY_DELETE=internal.PL_KEY_DELETE
PL_KEY_BACKSPACE=internal.PL_KEY_BACKSPACE
PL_KEY_SPACE=internal.PL_KEY_SPACE
PL_KEY_ENTER=internal.PL_KEY_ENTER
PL_KEY_ESCAPE=internal.PL_KEY_ESCAPE
PL_KEY_LEFT_CTRL=internal.PL_KEY_LEFT_CTRL
PL_KEY_LEFT_SHIFT=internal.PL_KEY_LEFT_SHIFT
PL_KEY_LEFT_ALT=internal.PL_KEY_LEFT_ALT
PL_KEY_LEFT_SUPER=internal.PL_KEY_LEFT_SUPER
PL_KEY_RIGHT_CTRL=internal.PL_KEY_RIGHT_CTRL
PL_KEY_RIGHT_SHIFT=internal.PL_KEY_RIGHT_SHIFT
PL_KEY_RIGHT_ALT=internal.PL_KEY_RIGHT_ALT
PL_KEY_RIGHT_SUPER=internal.PL_KEY_RIGHT_SUPER
PL_KEY_MENU=internal.PL_KEY_MENU
PL_KEY_0=internal.PL_KEY_0
PL_KEY_1=internal.PL_KEY_1
PL_KEY_2=internal.PL_KEY_2
PL_KEY_3=internal.PL_KEY_3
PL_KEY_4=internal.PL_KEY_4
PL_KEY_5=internal.PL_KEY_5
PL_KEY_6=internal.PL_KEY_6
PL_KEY_7=internal.PL_KEY_7
PL_KEY_8=internal.PL_KEY_8
PL_KEY_9=internal.PL_KEY_9
PL_KEY_A=internal.PL_KEY_A
PL_KEY_B=internal.PL_KEY_B
PL_KEY_C=internal.PL_KEY_C
PL_KEY_D=internal.PL_KEY_D
PL_KEY_E=internal.PL_KEY_E
PL_KEY_F=internal.PL_KEY_F
PL_KEY_G=internal.PL_KEY_G
PL_KEY_H=internal.PL_KEY_H
PL_KEY_I=internal.PL_KEY_I
PL_KEY_J=internal.PL_KEY_J
PL_KEY_K=internal.PL_KEY_K
PL_KEY_L=internal.PL_KEY_L
PL_KEY_M=internal.PL_KEY_M
PL_KEY_N=internal.PL_KEY_N
PL_KEY_O=internal.PL_KEY_O
PL_KEY_P=internal.PL_KEY_P
PL_KEY_Q=internal.PL_KEY_Q
PL_KEY_R=internal.PL_KEY_R
PL_KEY_S=internal.PL_KEY_S
PL_KEY_T=internal.PL_KEY_T
PL_KEY_U=internal.PL_KEY_U
PL_KEY_V=internal.PL_KEY_V
PL_KEY_W=internal.PL_KEY_W
PL_KEY_X=internal.PL_KEY_X
PL_KEY_Y=internal.PL_KEY_Y
PL_KEY_Z=internal.PL_KEY_Z
PL_KEY_F1=internal.PL_KEY_F1
PL_KEY_F2=internal.PL_KEY_F2
PL_KEY_F3=internal.PL_KEY_F3
PL_KEY_F4=internal.PL_KEY_F4
PL_KEY_F5=internal.PL_KEY_F5
PL_KEY_F6=internal.PL_KEY_F6
PL_KEY_F7=internal.PL_KEY_F7
PL_KEY_F8=internal.PL_KEY_F8
PL_KEY_F9=internal.PL_KEY_F9
PL_KEY_F10=internal.PL_KEY_F10
PL_KEY_F11=internal.PL_KEY_F11
PL_KEY_F12=internal.PL_KEY_F12
PL_KEY_F13=internal.PL_KEY_F13
PL_KEY_F14=internal.PL_KEY_F14
PL_KEY_F15=internal.PL_KEY_F15
PL_KEY_F16=internal.PL_KEY_F16
PL_KEY_F17=internal.PL_KEY_F17
PL_KEY_F18=internal.PL_KEY_F18
PL_KEY_F19=internal.PL_KEY_F19
PL_KEY_F20=internal.PL_KEY_F20
PL_KEY_F21=internal.PL_KEY_F21
PL_KEY_F22=internal.PL_KEY_F22
PL_KEY_F23=internal.PL_KEY_F23
PL_KEY_F24=internal.PL_KEY_F24
PL_KEY_APOSTROPHE=internal.PL_KEY_APOSTROPHE
PL_KEY_COMMA=internal.PL_KEY_COMMA
PL_KEY_MINUS=internal.PL_KEY_MINUS
PL_KEY_PERIOD=internal.PL_KEY_PERIOD
PL_KEY_SLASH=internal.PL_KEY_SLASH
PL_KEY_SEMICOLON=internal.PL_KEY_SEMICOLON
PL_KEY_EQUAL=internal.PL_KEY_EQUAL
PL_KEY_LEFT_BRACKET=internal.PL_KEY_LEFT_BRACKET
PL_KEY_BACKSLASH=internal.PL_KEY_BACKSLASH
PL_KEY_RIGHT_BRACKET=internal.PL_KEY_RIGHT_BRACKET
PL_KEY_GRAVE_ACCENT=internal.PL_KEY_GRAVE_ACCENT
PL_KEY_CAPS_LOCK=internal.PL_KEY_CAPS_LOCK
PL_KEY_SCROLL_LOCK=internal.PL_KEY_SCROLL_LOCK
PL_KEY_NUM_LOCK=internal.PL_KEY_NUM_LOCK
PL_KEY_PRINT_SCREEN=internal.PL_KEY_PRINT_SCREEN
PL_KEY_PAUSE=internal.PL_KEY_PAUSE
PL_KEY_KEYPAD_0=internal.PL_KEY_KEYPAD_0
PL_KEY_KEYPAD_1=internal.PL_KEY_KEYPAD_1
PL_KEY_KEYPAD_2=internal.PL_KEY_KEYPAD_2
PL_KEY_KEYPAD_3=internal.PL_KEY_KEYPAD_3
PL_KEY_KEYPAD_4=internal.PL_KEY_KEYPAD_4
PL_KEY_KEYPAD_5=internal.PL_KEY_KEYPAD_5
PL_KEY_KEYPAD_6=internal.PL_KEY_KEYPAD_6
PL_KEY_KEYPAD_7=internal.PL_KEY_KEYPAD_7
PL_KEY_KEYPAD_8=internal.PL_KEY_KEYPAD_8
PL_KEY_KEYPAD_9=internal.PL_KEY_KEYPAD_9
PL_KEY_KEYPAD_DECIMAL=internal.PL_KEY_KEYPAD_DECIMAL
PL_KEY_KEYPAD_DIVIDE=internal.PL_KEY_KEYPAD_DIVIDE
PL_KEY_KEYPAD_MULTIPLY=internal.PL_KEY_KEYPAD_MULTIPLY
PL_KEY_KEYPAD_SUBTRACT=internal.PL_KEY_KEYPAD_SUBTRACT
PL_KEY_KEYPAD_ADD=internal.PL_KEY_KEYPAD_ADD
PL_KEY_KEYPAD_ENTER=internal.PL_KEY_KEYPAD_ENTER
PL_KEY_KEYPAD_EQUAL=internal.PL_KEY_KEYPAD_EQUAL
PL_KEY_RESERVED_MOD_CTRL=internal.PL_KEY_RESERVED_MOD_CTRL
PL_KEY_RESERVED_MOD_SHIFT=internal.PL_KEY_RESERVED_MOD_SHIFT
PL_KEY_RESERVED_MOD_ALT=internal.PL_KEY_RESERVED_MOD_ALT
PL_RESERVED_KEY_MOD_SUPER=internal.PL_RESERVED_KEY_MOD_SUPER
PL_KEY_NAMED_KEY_END=internal.PL_KEY_NAMED_KEY_END
PL_KEY_COUNT=internal.PL_KEY_COUNT
PL_KEY_MOD_NONE=internal.PL_KEY_MOD_NONE
PL_KEY_MOD_CTRL=internal.PL_KEY_MOD_CTRL
PL_KEY_MOD_SHIFT=internal.PL_KEY_MOD_SHIFT
PL_KEY_MOD_ALT=internal.PL_KEY_MOD_ALT
PL_KEY_MOD_SUPER=internal.PL_KEY_MOD_SUPER
PL_KEY_MOD_SHORTCUT=internal.PL_KEY_MOD_SHORTCUT

# PL_COLOR_32_ CONSTANTS
PL_MOUSE_CURSOR_NONE=internal.PL_MOUSE_CURSOR_NONE

PL_COLOR_32_WHITE=internal.PL_COLOR_32_WHITE
PL_COLOR_32_BLACK=internal.PL_COLOR_32_BLACK
PL_COLOR_32_RED=internal.PL_COLOR_32_RED
PL_COLOR_32_BLUE=internal.PL_COLOR_32_BLUE
PL_COLOR_32_DARK_BLUE=internal.PL_COLOR_32_DARK_BLUE
PL_COLOR_32_GREEN=internal.PL_COLOR_32_GREEN
PL_COLOR_32_YELLOW=internal.PL_COLOR_32_YELLOW
PL_COLOR_32_ORANGE=internal.PL_COLOR_32_ORANGE
PL_COLOR_32_MAGENTA=internal.PL_COLOR_32_MAGENTA
PL_COLOR_32_CYAN=internal.PL_COLOR_32_CYAN
PL_COLOR_32_GREY=internal.PL_COLOR_32_GREY
PL_COLOR_32_LIGHT_GREY=internal.PL_COLOR_32_LIGHT_GREY