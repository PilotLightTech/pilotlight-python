from enum import IntEnum, IntFlag
import pilotlight.pilotlight as pl
import pilotlight.imgui as imgui

########################################################################################################################
# [SECTION] enums
########################################################################################################################

class plShaderFlags(IntFlag):
    PL_SHADER_FLAGS_NONE=pl.PL_SHADER_FLAGS_NONE
    PL_SHADER_FLAGS_INCLUDE_DEBUG=pl.PL_SHADER_FLAGS_INCLUDE_DEBUG
    PL_SHADER_FLAGS_ALWAYS_COMPILE=pl.PL_SHADER_FLAGS_ALWAYS_COMPILE
    PL_SHADER_FLAGS_NEVER_CACHE=pl.PL_SHADER_FLAGS_NEVER_CACHE
    PL_SHADER_FLAGS_METAL_OUTPUT=pl.PL_SHADER_FLAGS_METAL_OUTPUT
    PL_SHADER_FLAGS_SPIRV_OUTPUT=pl.PL_SHADER_FLAGS_SPIRV_OUTPUT
    PL_SHADER_FLAGS_AUTO_OUTPUT=pl.PL_SHADER_FLAGS_AUTO_OUTPUT

class plShaderOptimization(IntEnum):
    PL_SHADER_OPTIMIZATION_NONE=pl.PL_SHADER_OPTIMIZATION_NONE
    PL_SHADER_OPTIMIZATION_SIZE=pl.PL_SHADER_OPTIMIZATION_SIZE
    PL_SHADER_OPTIMIZATION_PERFORMANCE=pl.PL_SHADER_OPTIMIZATION_PERFORMANCE

class plStarterFlag(IntFlag):
    PL_STARTER_FLAGS_NONE=pl.PL_STARTER_FLAGS_NONE
    PL_STARTER_FLAGS_DRAW_EXT=pl.PL_STARTER_FLAGS_DRAW_EXT
    PL_STARTER_FLAGS_UI_EXT=pl.PL_STARTER_FLAGS_UI_EXT
    PL_STARTER_FLAGS_CONSOLE_EXT=pl.PL_STARTER_FLAGS_CONSOLE_EXT
    PL_STARTER_FLAGS_PROFILE_EXT=pl.PL_STARTER_FLAGS_PROFILE_EXT
    PL_STARTER_FLAGS_STATS_EXT=pl.PL_STARTER_FLAGS_STATS_EXT
    PL_STARTER_FLAGS_SHADER_EXT=pl.PL_STARTER_FLAGS_SHADER_EXT
    PL_STARTER_FLAGS_SCREEN_LOG_EXT=pl.PL_STARTER_FLAGS_SCREEN_LOG_EXT
    PL_STARTER_FLAGS_GRAPHICS_EXT=pl.PL_STARTER_FLAGS_GRAPHICS_EXT
    PL_STARTER_FLAGS_TOOLS_EXT=pl.PL_STARTER_FLAGS_TOOLS_EXT
    PL_STARTER_FLAGS_DEPTH_BUFFER=pl.PL_STARTER_FLAGS_DEPTH_BUFFER
    PL_STARTER_FLAGS_MSAA=pl.PL_STARTER_FLAGS_MSAA
    PL_STARTER_FLAGS_VSYNC_OFF=pl.PL_STARTER_FLAGS_VSYNC_OFF
    PL_STARTER_FLAGS_REVERSE_Z=pl.PL_STARTER_FLAGS_REVERSE_Z
    PL_STARTER_FLAGS_ALL_EXTENSIONS=pl.PL_STARTER_FLAGS_ALL_EXTENSIONS

class plMouseButton(IntEnum):
    PL_MOUSE_BUTTON_LEFT=pl.PL_MOUSE_BUTTON_LEFT
    PL_MOUSE_BUTTON_RIGHT=pl.PL_MOUSE_BUTTON_RIGHT
    PL_MOUSE_BUTTON_MIDDLE=pl.PL_MOUSE_BUTTON_MIDDLE
    PL_MOUSE_BUTTON_COUNT=pl.PL_MOUSE_BUTTON_COUNT

class plMouseCursor(IntEnum):
    PL_MOUSE_CURSOR_NONE=pl.PL_MOUSE_CURSOR_NONE
    PL_MOUSE_CURSOR_ARROW=pl.PL_MOUSE_CURSOR_ARROW
    PL_MOUSE_CURSOR_TEXT_INPUT=pl.PL_MOUSE_CURSOR_TEXT_INPUT
    PL_MOUSE_CURSOR_RESIZE_ALL=pl.PL_MOUSE_CURSOR_RESIZE_ALL
    PL_MOUSE_CURSOR_RESIZE_NS=pl.PL_MOUSE_CURSOR_RESIZE_NS
    PL_MOUSE_CURSOR_RESIZE_EW=pl.PL_MOUSE_CURSOR_RESIZE_EW
    PL_MOUSE_CURSOR_RESIZE_NESW=pl.PL_MOUSE_CURSOR_RESIZE_NESW
    PL_MOUSE_CURSOR_RESIZE_NWSE=pl.PL_MOUSE_CURSOR_RESIZE_NWSE
    PL_MOUSE_CURSOR_HAND=pl.PL_MOUSE_CURSOR_HAND
    PL_MOUSE_CURSOR_WAIT=pl.PL_MOUSE_CURSOR_WAIT
    PL_MOUSE_CURSOR_PROGRESS=pl.PL_MOUSE_CURSOR_PROGRESS
    PL_MOUSE_CURSOR_NOT_ALLOWED=pl.PL_MOUSE_CURSOR_NOT_ALLOWED
    PL_MOUSE_CURSOR_COUNT=pl.PL_MOUSE_CURSOR_COUNT

class plKey(IntEnum):
    PL_KEY_NONE=pl.PL_KEY_NONE
    PL_KEY_NAMED_KEY_BEGIN=pl.PL_KEY_NAMED_KEY_BEGIN
    PL_KEY_TAB=pl.PL_KEY_TAB
    PL_KEY_LEFT_ARROW=pl.PL_KEY_LEFT_ARROW
    PL_KEY_RIGHT_ARROW=pl.PL_KEY_RIGHT_ARROW
    PL_KEY_UP_ARROW=pl.PL_KEY_UP_ARROW
    PL_KEY_DOWN_ARROW=pl.PL_KEY_DOWN_ARROW
    PL_KEY_PAGE_UP=pl.PL_KEY_PAGE_UP
    PL_KEY_PAGE_DOWN=pl.PL_KEY_PAGE_DOWN
    PL_KEY_HOME=pl.PL_KEY_HOME
    PL_KEY_END=pl.PL_KEY_END
    PL_KEY_INSERT=pl.PL_KEY_INSERT
    PL_KEY_DELETE=pl.PL_KEY_DELETE
    PL_KEY_BACKSPACE=pl.PL_KEY_BACKSPACE
    PL_KEY_SPACE=pl.PL_KEY_SPACE
    PL_KEY_ENTER=pl.PL_KEY_ENTER
    PL_KEY_ESCAPE=pl.PL_KEY_ESCAPE
    PL_KEY_LEFT_CTRL=pl.PL_KEY_LEFT_CTRL
    PL_KEY_LEFT_SHIFT=pl.PL_KEY_LEFT_SHIFT
    PL_KEY_LEFT_ALT=pl.PL_KEY_LEFT_ALT
    PL_KEY_LEFT_SUPER=pl.PL_KEY_LEFT_SUPER
    PL_KEY_RIGHT_CTRL=pl.PL_KEY_RIGHT_CTRL
    PL_KEY_RIGHT_SHIFT=pl.PL_KEY_RIGHT_SHIFT
    PL_KEY_RIGHT_ALT=pl.PL_KEY_RIGHT_ALT
    PL_KEY_RIGHT_SUPER=pl.PL_KEY_RIGHT_SUPER
    PL_KEY_MENU=pl.PL_KEY_MENU
    PL_KEY_0=pl.PL_KEY_0
    PL_KEY_1=pl.PL_KEY_1
    PL_KEY_2=pl.PL_KEY_2
    PL_KEY_3=pl.PL_KEY_3
    PL_KEY_4=pl.PL_KEY_4
    PL_KEY_5=pl.PL_KEY_5
    PL_KEY_6=pl.PL_KEY_6
    PL_KEY_7=pl.PL_KEY_7
    PL_KEY_8=pl.PL_KEY_8
    PL_KEY_9=pl.PL_KEY_9
    PL_KEY_A=pl.PL_KEY_A
    PL_KEY_B=pl.PL_KEY_B
    PL_KEY_C=pl.PL_KEY_C
    PL_KEY_D=pl.PL_KEY_D
    PL_KEY_E=pl.PL_KEY_E
    PL_KEY_F=pl.PL_KEY_F
    PL_KEY_G=pl.PL_KEY_G
    PL_KEY_H=pl.PL_KEY_H
    PL_KEY_I=pl.PL_KEY_I
    PL_KEY_J=pl.PL_KEY_J
    PL_KEY_K=pl.PL_KEY_K
    PL_KEY_L=pl.PL_KEY_L
    PL_KEY_M=pl.PL_KEY_M
    PL_KEY_N=pl.PL_KEY_N
    PL_KEY_O=pl.PL_KEY_O
    PL_KEY_P=pl.PL_KEY_P
    PL_KEY_Q=pl.PL_KEY_Q
    PL_KEY_R=pl.PL_KEY_R
    PL_KEY_S=pl.PL_KEY_S
    PL_KEY_T=pl.PL_KEY_T
    PL_KEY_U=pl.PL_KEY_U
    PL_KEY_V=pl.PL_KEY_V
    PL_KEY_W=pl.PL_KEY_W
    PL_KEY_X=pl.PL_KEY_X
    PL_KEY_Y=pl.PL_KEY_Y
    PL_KEY_Z=pl.PL_KEY_Z
    PL_KEY_F1=pl.PL_KEY_F1
    PL_KEY_F2=pl.PL_KEY_F2
    PL_KEY_F3=pl.PL_KEY_F3
    PL_KEY_F4=pl.PL_KEY_F4
    PL_KEY_F5=pl.PL_KEY_F5
    PL_KEY_F6=pl.PL_KEY_F6
    PL_KEY_F7=pl.PL_KEY_F7
    PL_KEY_F8=pl.PL_KEY_F8
    PL_KEY_F9=pl.PL_KEY_F9
    PL_KEY_F10=pl.PL_KEY_F10
    PL_KEY_F11=pl.PL_KEY_F11
    PL_KEY_F12=pl.PL_KEY_F12
    PL_KEY_F13=pl.PL_KEY_F13
    PL_KEY_F14=pl.PL_KEY_F14
    PL_KEY_F15=pl.PL_KEY_F15
    PL_KEY_F16=pl.PL_KEY_F16
    PL_KEY_F17=pl.PL_KEY_F17
    PL_KEY_F18=pl.PL_KEY_F18
    PL_KEY_F19=pl.PL_KEY_F19
    PL_KEY_F20=pl.PL_KEY_F20
    PL_KEY_F21=pl.PL_KEY_F21
    PL_KEY_F22=pl.PL_KEY_F22
    PL_KEY_F23=pl.PL_KEY_F23
    PL_KEY_F24=pl.PL_KEY_F24
    PL_KEY_APOSTROPHE=pl.PL_KEY_APOSTROPHE
    PL_KEY_COMMA=pl.PL_KEY_COMMA
    PL_KEY_MINUS=pl.PL_KEY_MINUS
    PL_KEY_PERIOD=pl.PL_KEY_PERIOD
    PL_KEY_SLASH=pl.PL_KEY_SLASH
    PL_KEY_SEMICOLON=pl.PL_KEY_SEMICOLON
    PL_KEY_EQUAL=pl.PL_KEY_EQUAL
    PL_KEY_LEFT_BRACKET=pl.PL_KEY_LEFT_BRACKET
    PL_KEY_BACKSLASH=pl.PL_KEY_BACKSLASH
    PL_KEY_RIGHT_BRACKET=pl.PL_KEY_RIGHT_BRACKET
    PL_KEY_GRAVE_ACCENT=pl.PL_KEY_GRAVE_ACCENT
    PL_KEY_CAPS_LOCK=pl.PL_KEY_CAPS_LOCK
    PL_KEY_SCROLL_LOCK=pl.PL_KEY_SCROLL_LOCK
    PL_KEY_NUM_LOCK=pl.PL_KEY_NUM_LOCK
    PL_KEY_PRINT_SCREEN=pl.PL_KEY_PRINT_SCREEN
    PL_KEY_PAUSE=pl.PL_KEY_PAUSE
    PL_KEY_KEYPAD_0=pl.PL_KEY_KEYPAD_0
    PL_KEY_KEYPAD_1=pl.PL_KEY_KEYPAD_1
    PL_KEY_KEYPAD_2=pl.PL_KEY_KEYPAD_2
    PL_KEY_KEYPAD_3=pl.PL_KEY_KEYPAD_3
    PL_KEY_KEYPAD_4=pl.PL_KEY_KEYPAD_4
    PL_KEY_KEYPAD_5=pl.PL_KEY_KEYPAD_5
    PL_KEY_KEYPAD_6=pl.PL_KEY_KEYPAD_6
    PL_KEY_KEYPAD_7=pl.PL_KEY_KEYPAD_7
    PL_KEY_KEYPAD_8=pl.PL_KEY_KEYPAD_8
    PL_KEY_KEYPAD_9=pl.PL_KEY_KEYPAD_9
    PL_KEY_KEYPAD_DECIMAL=pl.PL_KEY_KEYPAD_DECIMAL
    PL_KEY_KEYPAD_DIVIDE=pl.PL_KEY_KEYPAD_DIVIDE
    PL_KEY_KEYPAD_MULTIPLY=pl.PL_KEY_KEYPAD_MULTIPLY
    PL_KEY_KEYPAD_SUBTRACT=pl.PL_KEY_KEYPAD_SUBTRACT
    PL_KEY_KEYPAD_ADD=pl.PL_KEY_KEYPAD_ADD
    PL_KEY_KEYPAD_ENTER=pl.PL_KEY_KEYPAD_ENTER
    PL_KEY_KEYPAD_EQUAL=pl.PL_KEY_KEYPAD_EQUAL
    PL_KEY_RESERVED_MOD_CTRL=pl.PL_KEY_RESERVED_MOD_CTRL
    PL_KEY_RESERVED_MOD_SHIFT=pl.PL_KEY_RESERVED_MOD_SHIFT
    PL_KEY_RESERVED_MOD_ALT=pl.PL_KEY_RESERVED_MOD_ALT
    PL_RESERVED_KEY_MOD_SUPER=pl.PL_RESERVED_KEY_MOD_SUPER
    PL_KEY_NAMED_KEY_END=pl.PL_KEY_NAMED_KEY_END
    PL_KEY_COUNT=pl.PL_KEY_COUNT
    PL_KEY_MOD_NONE=pl.PL_KEY_MOD_NONE
    PL_KEY_MOD_CTRL=pl.PL_KEY_MOD_CTRL
    PL_KEY_MOD_SHIFT=pl.PL_KEY_MOD_SHIFT
    PL_KEY_MOD_ALT=pl.PL_KEY_MOD_ALT
    PL_KEY_MOD_SUPER=pl.PL_KEY_MOD_SUPER
    PL_KEY_MOD_SHORTCUT=pl.PL_KEY_MOD_SHORTCUT

class plDrawFlag(IntFlag):
    PL_DRAW_FLAG_NONE=pl.PL_DRAW_FLAG_NONE
    PL_DRAW_FLAG_DEPTH_TEST=pl.PL_DRAW_FLAG_DEPTH_TEST
    PL_DRAW_FLAG_DEPTH_WRITE=pl.PL_DRAW_FLAG_DEPTH_WRITE
    PL_DRAW_FLAG_CULL_FRONT=pl.PL_DRAW_FLAG_CULL_FRONT
    PL_DRAW_FLAG_CULL_BACK=pl.PL_DRAW_FLAG_CULL_BACK
    PL_DRAW_FLAG_FRONT_FACE_CW=pl.PL_DRAW_FLAG_FRONT_FACE_CW
    PL_DRAW_FLAG_REVERSE_Z_DEPTH=pl.PL_DRAW_FLAG_REVERSE_Z_DEPTH

class plDrawRectFlag(IntFlag):
    PL_DRAW_RECT_FLAG_NONE=pl.PL_DRAW_RECT_FLAG_NONE
    PL_DRAW_RECT_FLAG_ROUND_CORNERS_TOP_LEFT=pl.PL_DRAW_RECT_FLAG_ROUND_CORNERS_TOP_LEFT
    PL_DRAW_RECT_FLAG_ROUND_CORNERS_TOP_RIGHT=pl.PL_DRAW_RECT_FLAG_ROUND_CORNERS_TOP_RIGHT
    PL_DRAW_RECT_FLAG_ROUND_CORNERS_BOTTOM_LEFT=pl.PL_DRAW_RECT_FLAG_ROUND_CORNERS_BOTTOM_LEFT
    PL_DRAW_RECT_FLAG_ROUND_CORNERS_BOTTOM_RIGHT=pl.PL_DRAW_RECT_FLAG_ROUND_CORNERS_BOTTOM_RIGHT
    PL_DRAW_RECT_FLAG_ROUND_CORNERS_NONE=pl.PL_DRAW_RECT_FLAG_ROUND_CORNERS_NONE
    PL_DRAW_RECT_FLAG_ROUND_CORNERS_TOP=pl.PL_DRAW_RECT_FLAG_ROUND_CORNERS_TOP
    PL_DRAW_RECT_FLAG_ROUND_CORNERS_BOTTOM=pl.PL_DRAW_RECT_FLAG_ROUND_CORNERS_BOTTOM
    PL_DRAW_RECT_FLAG_ROUND_CORNERS_LEFT=pl.PL_DRAW_RECT_FLAG_ROUND_CORNERS_LEFT
    PL_DRAW_RECT_FLAG_ROUND_CORNERS_RIGHT=pl.PL_DRAW_RECT_FLAG_ROUND_CORNERS_RIGHT
    PL_DRAW_RECT_FLAG_ROUND_CORNERS_All=pl.PL_DRAW_RECT_FLAG_ROUND_CORNERS_All

########################################################################################################################
# [SECTION] Dear ImGui enums
########################################################################################################################

class ImGuiWindowFlags(IntFlag):
    NoTitleBar=imgui.ImGuiWindowFlags_NoTitleBar
    NoResize=imgui.ImGuiWindowFlags_NoResize
    NoMove=imgui.ImGuiWindowFlags_NoMove
    NoScrollbar=imgui.ImGuiWindowFlags_NoScrollbar
    NoScrollWithMouse=imgui.ImGuiWindowFlags_NoScrollWithMouse
    NoCollapse=imgui.ImGuiWindowFlags_NoCollapse
    AlwaysAutoResize=imgui.ImGuiWindowFlags_AlwaysAutoResize
    NoBackground=imgui.ImGuiWindowFlags_NoBackground
    NoSavedSettings=imgui.ImGuiWindowFlags_NoSavedSettings
    NoMouseInputs=imgui.ImGuiWindowFlags_NoMouseInputs
    MenuBar=imgui.ImGuiWindowFlags_MenuBar
    HorizontalScrollbar=imgui.ImGuiWindowFlags_HorizontalScrollbar
    NoFocusOnAppearing=imgui.ImGuiWindowFlags_NoFocusOnAppearing
    NoBringToFrontOnFocus=imgui.ImGuiWindowFlags_NoBringToFrontOnFocus
    AlwaysVerticalScrollbar=imgui.ImGuiWindowFlags_AlwaysVerticalScrollbar
    AlwaysHorizontalScrollbar=imgui.ImGuiWindowFlags_AlwaysHorizontalScrollbar
    NoNavInputs=imgui.ImGuiWindowFlags_NoNavInputs
    NoNavFocus=imgui.ImGuiWindowFlags_NoNavFocus
    UnsavedDocument=imgui.ImGuiWindowFlags_UnsavedDocument
    NoDocking=imgui.ImGuiWindowFlags_NoDocking
    NoNav=imgui.ImGuiWindowFlags_NoNav
    NoDecoration=imgui.ImGuiWindowFlags_NoDecoration
    NoInputs=imgui.ImGuiWindowFlags_NoInputs

class ImGuiSliderFlags(IntFlag):
    Logarithmic=imgui.ImGuiSliderFlags_Logarithmic
    NoRoundToFormat=imgui.ImGuiSliderFlags_NoRoundToFormat
    NoInput=imgui.ImGuiSliderFlags_NoInput
    WrapAround=imgui.ImGuiSliderFlags_WrapAround
    ClampOnInput=imgui.ImGuiSliderFlags_ClampOnInput
    ClampZeroRange=imgui.ImGuiSliderFlags_ClampZeroRange
    NoSpeedTweaks=imgui.ImGuiSliderFlags_NoSpeedTweaks
    ColorMarkers=imgui.ImGuiSliderFlags_ColorMarkers
    AlwaysClamp=imgui.ImGuiSliderFlags_AlwaysClamp

class ImGuiInputTextFlags(IntFlag):
    CharsDecimal=imgui.ImGuiInputTextFlags_CharsDecimal
    CharsHexadecimal=imgui.ImGuiInputTextFlags_CharsHexadecimal
    CharsScientific=imgui.ImGuiInputTextFlags_CharsScientific
    CharsUppercase=imgui.ImGuiInputTextFlags_CharsUppercase
    CharsNoBlank=imgui.ImGuiInputTextFlags_CharsNoBlank
    AllowTabInput=imgui.ImGuiInputTextFlags_AllowTabInput
    EnterReturnsTrue=imgui.ImGuiInputTextFlags_EnterReturnsTrue
    EscapeClearsAll=imgui.ImGuiInputTextFlags_EscapeClearsAll
    CtrlEnterForNewLine=imgui.ImGuiInputTextFlags_CtrlEnterForNewLine
    ReadOnly=imgui.ImGuiInputTextFlags_ReadOnly
    Password=imgui.ImGuiInputTextFlags_Password
    AlwaysOverwrite=imgui.ImGuiInputTextFlags_AlwaysOverwrite
    AutoSelectAll=imgui.ImGuiInputTextFlags_AutoSelectAll
    ParseEmptyRefVal=imgui.ImGuiInputTextFlags_ParseEmptyRefVal
    DisplayEmptyRefVal=imgui.ImGuiInputTextFlags_DisplayEmptyRefVal
    NoHorizontalScroll=imgui.ImGuiInputTextFlags_NoHorizontalScroll
    NoUndoRedo=imgui.ImGuiInputTextFlags_NoUndoRedo
    ElideLeft=imgui.ImGuiInputTextFlags_ElideLeft
    CallbackCompletion=imgui.ImGuiInputTextFlags_CallbackCompletion
    CallbackHistory=imgui.ImGuiInputTextFlags_CallbackHistory
    CallbackAlways=imgui.ImGuiInputTextFlags_CallbackAlways
    CallbackCharFilter=imgui.ImGuiInputTextFlags_CallbackCharFilter
    CallbackResize=imgui.ImGuiInputTextFlags_CallbackResize
    CallbackEdit=imgui.ImGuiInputTextFlags_CallbackEdit
    WordWrap=imgui.ImGuiInputTextFlags_WordWrap

class ImGuiMouseButton(IntEnum):
    Left=imgui.ImGuiMouseButton_Left
    Right=imgui.ImGuiMouseButton_Right
    Middle=imgui.ImGuiMouseButton_Middle
    COUNT=imgui.ImGuiMouseButton_COUNT

class ImGuiMouseCursor(IntEnum):
    Arrow=imgui.ImGuiMouseCursor_Arrow
    TextInput=imgui.ImGuiMouseCursor_TextInput
    ResizeAll=imgui.ImGuiMouseCursor_ResizeAll
    ResizeNS=imgui.ImGuiMouseCursor_ResizeNS
    ResizeEW=imgui.ImGuiMouseCursor_ResizeEW
    ResizeNESW=imgui.ImGuiMouseCursor_ResizeNESW
    ResizeNWSE=imgui.ImGuiMouseCursor_ResizeNWSE
    Hand=imgui.ImGuiMouseCursor_Hand
    Wait=imgui.ImGuiMouseCursor_Wait
    Progress=imgui.ImGuiMouseCursor_Progress
    NotAllowed=imgui.ImGuiMouseCursor_NotAllowed
    COUNT=imgui.ImGuiMouseCursor_COUNT

class ImGuiButtonFlags(IntFlag):
    MouseButtonLeft=imgui.ImGuiButtonFlags_MouseButtonLeft
    MouseButtonRight=imgui.ImGuiButtonFlags_MouseButtonRight
    MouseButtonMiddle=imgui.ImGuiButtonFlags_MouseButtonMiddle
    EnableNav=imgui.ImGuiButtonFlags_EnableNav
    AllowOverlap=imgui.ImGuiButtonFlags_AllowOverlap

class ImGuiDir(IntEnum):
    Left=imgui.ImGuiDir_Left
    Right=imgui.ImGuiDir_Right
    Up=imgui.ImGuiDir_Up
    Down=imgui.ImGuiDir_Down
    COUNT=imgui.ImGuiDir_COUNT

class ImGuiColorEditFlags(IntFlag):
    NoAlpha=imgui.ImGuiColorEditFlags_NoAlpha
    NoPicker=imgui.ImGuiColorEditFlags_NoPicker
    NoOptions=imgui.ImGuiColorEditFlags_NoOptions
    NoSmallPreview=imgui.ImGuiColorEditFlags_NoSmallPreview
    NoInputs=imgui.ImGuiColorEditFlags_NoInputs
    NoTooltip=imgui.ImGuiColorEditFlags_NoTooltip
    NoLabel=imgui.ImGuiColorEditFlags_NoLabel
    NoSidePreview=imgui.ImGuiColorEditFlags_NoSidePreview
    NoDragDrop=imgui.ImGuiColorEditFlags_NoDragDrop
    NoBorder=imgui.ImGuiColorEditFlags_NoBorder
    NoColorMarkers=imgui.ImGuiColorEditFlags_NoColorMarkers
    AlphaOpaque=imgui.ImGuiColorEditFlags_AlphaOpaque
    AlphaNoBg=imgui.ImGuiColorEditFlags_AlphaNoBg
    AlphaPreviewHalf=imgui.ImGuiColorEditFlags_AlphaPreviewHalf
    AlphaBar=imgui.ImGuiColorEditFlags_AlphaBar
    HDR=imgui.ImGuiColorEditFlags_HDR
    DisplayRGB=imgui.ImGuiColorEditFlags_DisplayRGB
    DisplayHSV=imgui.ImGuiColorEditFlags_DisplayHSV
    DisplayHex=imgui.ImGuiColorEditFlags_DisplayHex
    Uint8=imgui.ImGuiColorEditFlags_Uint8
    Float=imgui.ImGuiColorEditFlags_Float
    PickerHueBar=imgui.ImGuiColorEditFlags_PickerHueBar
    PickerHueWheel=imgui.ImGuiColorEditFlags_PickerHueWheel
    InputRGB=imgui.ImGuiColorEditFlags_InputRGB
    InputHSV=imgui.ImGuiColorEditFlags_InputHSV

class ImGuiHoveredFlags(IntFlag):
    ChildWindows=imgui.ImGuiHoveredFlags_ChildWindows
    RootWindow=imgui.ImGuiHoveredFlags_RootWindow
    AnyWindow=imgui.ImGuiHoveredFlags_AnyWindow
    NoPopupHierarchy=imgui.ImGuiHoveredFlags_NoPopupHierarchy
    DockHierarchy=imgui.ImGuiHoveredFlags_DockHierarchy
    AllowWhenBlockedByPopup=imgui.ImGuiHoveredFlags_AllowWhenBlockedByPopup
    AllowWhenBlockedByActiveItem=imgui.ImGuiHoveredFlags_AllowWhenBlockedByActiveItem
    AllowWhenOverlappedByItem=imgui.ImGuiHoveredFlags_AllowWhenOverlappedByItem
    AllowWhenOverlappedByWindow=imgui.ImGuiHoveredFlags_AllowWhenOverlappedByWindow
    AllowWhenDisabled=imgui.ImGuiHoveredFlags_AllowWhenDisabled
    NoNavOverride=imgui.ImGuiHoveredFlags_NoNavOverride
    AllowWhenOverlapped=imgui.ImGuiHoveredFlags_AllowWhenOverlapped
    RectOnly=imgui.ImGuiHoveredFlags_RectOnly
    RootAndChildWindows=imgui.ImGuiHoveredFlags_RootAndChildWindows
    ForTooltip=imgui.ImGuiHoveredFlags_ForTooltip
    Stationary=imgui.ImGuiHoveredFlags_Stationary
    DelayNone=imgui.ImGuiHoveredFlags_DelayNone
    DelayShort=imgui.ImGuiHoveredFlags_DelayShort
    DelayNormal=imgui.ImGuiHoveredFlags_DelayNormal
    NoSharedDelay=imgui.ImGuiHoveredFlags_NoSharedDelay

class ImGuiFocusedFlags(IntFlag):
    ChildWindows=imgui.ImGuiFocusedFlags_ChildWindows
    RootWindow=imgui.ImGuiFocusedFlags_RootWindow
    AnyWindow=imgui.ImGuiFocusedFlags_AnyWindow
    NoPopupHierarchy=imgui.ImGuiFocusedFlags_NoPopupHierarchy
    DockHierarchy=imgui.ImGuiFocusedFlags_DockHierarchy
    RootAndChildWindows=imgui.ImGuiFocusedFlags_RootAndChildWindows

class ImGuiCond(IntFlag):
    Always=imgui.ImGuiCond_Always
    Once=imgui.ImGuiCond_Once
    FirstUseEver=imgui.ImGuiCond_FirstUseEver
    Appearing=imgui.ImGuiCond_Appearing

class ImGuiPopupFlags(IntFlag):
    MouseButtonLeft=imgui.ImGuiPopupFlags_MouseButtonLeft
    MouseButtonRight=imgui.ImGuiPopupFlags_MouseButtonRight
    MouseButtonMiddle=imgui.ImGuiPopupFlags_MouseButtonMiddle
    NoReopen=imgui.ImGuiPopupFlags_NoReopen
    NoOpenOverExistingPopup=imgui.ImGuiPopupFlags_NoOpenOverExistingPopup
    NoOpenOverItems=imgui.ImGuiPopupFlags_NoOpenOverItems
    AnyPopupId=imgui.ImGuiPopupFlags_AnyPopupId
    AnyPopupLevel=imgui.ImGuiPopupFlags_AnyPopupLevel
    AnyPopup=imgui.ImGuiPopupFlags_AnyPopup

class ImGuiSelectableFlags(IntFlag):
    NoAutoClosePopups=imgui.ImGuiSelectableFlags_NoAutoClosePopups
    SpanAllColumns=imgui.ImGuiSelectableFlags_SpanAllColumns
    AllowDoubleClick=imgui.ImGuiSelectableFlags_AllowDoubleClick
    Disabled=imgui.ImGuiSelectableFlags_Disabled
    AllowOverlap=imgui.ImGuiSelectableFlags_AllowOverlap
    Highlight=imgui.ImGuiSelectableFlags_Highlight
    SelectOnNav=imgui.ImGuiSelectableFlags_SelectOnNav

class ImGuiTabBarFlags(IntFlag):
    Reorderable=imgui.ImGuiTabBarFlags_Reorderable
    AutoSelectNewTabs=imgui.ImGuiTabBarFlags_AutoSelectNewTabs
    TabListPopupButton=imgui.ImGuiTabBarFlags_TabListPopupButton
    NoCloseWithMiddleMouseButton=imgui.ImGuiTabBarFlags_NoCloseWithMiddleMouseButton
    NoTabListScrollingButtons=imgui.ImGuiTabBarFlags_NoTabListScrollingButtons
    NoTooltip=imgui.ImGuiTabBarFlags_NoTooltip
    DrawSelectedOverline=imgui.ImGuiTabBarFlags_DrawSelectedOverline
    FittingPolicyMixed=imgui.ImGuiTabBarFlags_FittingPolicyMixed
    FittingPolicyShrink=imgui.ImGuiTabBarFlags_FittingPolicyShrink
    FittingPolicyScroll=imgui.ImGuiTabBarFlags_FittingPolicyScroll

class ImGuiItemFlags(IntFlag):
    NoTabStop=imgui.ImGuiItemFlags_NoTabStop
    NoNav=imgui.ImGuiItemFlags_NoNav
    NoNavDefaultFocus=imgui.ImGuiItemFlags_NoNavDefaultFocus
    ButtonRepeat=imgui.ImGuiItemFlags_ButtonRepeat
    AutoClosePopups=imgui.ImGuiItemFlags_AutoClosePopups
    AllowDuplicateId=imgui.ImGuiItemFlags_AllowDuplicateId
    Disabled=imgui.ImGuiItemFlags_Disabled

class ImGuiChildFlags(IntFlag):
    Borders=imgui.ImGuiChildFlags_Borders
    AlwaysUseWindowPadding=imgui.ImGuiChildFlags_AlwaysUseWindowPadding
    ResizeX=imgui.ImGuiChildFlags_ResizeX
    ResizeY=imgui.ImGuiChildFlags_ResizeY
    AutoResizeX=imgui.ImGuiChildFlags_AutoResizeX
    AutoResizeY=imgui.ImGuiChildFlags_AutoResizeY
    AlwaysAutoResize=imgui.ImGuiChildFlags_AlwaysAutoResize
    FrameStyle=imgui.ImGuiChildFlags_FrameStyle
    NavFlattened=imgui.ImGuiChildFlags_NavFlattened

class ImGuiTabItemFlags(IntFlag):
    UnsavedDocument=imgui.ImGuiTabItemFlags_UnsavedDocument
    SetSelected=imgui.ImGuiTabItemFlags_SetSelected
    NoCloseWithMiddleMouseButton=imgui.ImGuiTabItemFlags_NoCloseWithMiddleMouseButton
    NoPushId=imgui.ImGuiTabItemFlags_NoPushId
    NoTooltip=imgui.ImGuiTabItemFlags_NoTooltip
    NoReorder=imgui.ImGuiTabItemFlags_NoReorder
    Leading=imgui.ImGuiTabItemFlags_Leading
    Trailing=imgui.ImGuiTabItemFlags_Trailing
    NoAssumedClosure=imgui.ImGuiTabItemFlags_NoAssumedClosure

class ImGuiTreeNodeFlags(IntFlag):
    Selected=imgui.ImGuiTreeNodeFlags_Selected
    Framed=imgui.ImGuiTreeNodeFlags_Framed
    AllowOverlap=imgui.ImGuiTreeNodeFlags_AllowOverlap
    NoTreePushOnOpen=imgui.ImGuiTreeNodeFlags_NoTreePushOnOpen
    NoAutoOpenOnLog=imgui.ImGuiTreeNodeFlags_NoAutoOpenOnLog
    DefaultOpen=imgui.ImGuiTreeNodeFlags_DefaultOpen
    OpenOnDoubleClick=imgui.ImGuiTreeNodeFlags_OpenOnDoubleClick
    OpenOnArrow=imgui.ImGuiTreeNodeFlags_OpenOnArrow
    Leaf=imgui.ImGuiTreeNodeFlags_Leaf
    Bullet=imgui.ImGuiTreeNodeFlags_Bullet
    FramePadding=imgui.ImGuiTreeNodeFlags_FramePadding
    SpanAvailWidth=imgui.ImGuiTreeNodeFlags_SpanAvailWidth
    SpanFullWidth=imgui.ImGuiTreeNodeFlags_SpanFullWidth
    SpanLabelWidth=imgui.ImGuiTreeNodeFlags_SpanLabelWidth
    SpanAllColumns=imgui.ImGuiTreeNodeFlags_SpanAllColumns
    LabelSpanAllColumns=imgui.ImGuiTreeNodeFlags_LabelSpanAllColumns
    NavLeftJumpsToParent=imgui.ImGuiTreeNodeFlags_NavLeftJumpsToParent
    CollapsingHeader=imgui.ImGuiTreeNodeFlags_CollapsingHeader

class ImGuiComboFlags(IntFlag):
    PopupAlignLeft=imgui.ImGuiComboFlags_PopupAlignLeft
    HeightSmall=imgui.ImGuiComboFlags_HeightSmall
    HeightRegular=imgui.ImGuiComboFlags_HeightRegular
    HeightLarge=imgui.ImGuiComboFlags_HeightLarge
    HeightLargest=imgui.ImGuiComboFlags_HeightLargest
    NoArrowButton=imgui.ImGuiComboFlags_NoArrowButton
    NoPreview=imgui.ImGuiComboFlags_NoPreview
    WidthFitPreview=imgui.ImGuiComboFlags_WidthFitPreview