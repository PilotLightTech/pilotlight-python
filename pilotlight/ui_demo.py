"""
Pilot Light UI demo (python port of pilotlight/sandbox/pl__ui_demo.cpp)

usage (between plStarterI.begin_frame() & end_frame()):

    from pilotlight.ui_demo import show_ui_demo_window
    self.show_demo = show_ui_demo_window(self.show_demo)

differences from the C demo:
  * C "static" locals live on the module level _DemoState instance
  * the "Use Clipper" off path submits 10,000 rows instead of 1,000,000
    (a million python calls per frame stalls the app)
"""

import math

import pilotlight.pilotlight as pl
from pilotlight.pilotlight import plUiI, plDrawI, plGraphicsI, plStarterI, plUiClipper
from pilotlight.enums import *
from pilotlight.types import plDrawLineOptions, plDrawSolidOptions

ROW_DYNAMIC = plUiLayoutRowType.PL_UI_LAYOUT_ROW_TYPE_DYNAMIC
ROW_STATIC  = plUiLayoutRowType.PL_UI_LAYOUT_ROW_TYPE_STATIC

COMBO_ITEMS = ["Tomato", "Onion", "Carrot", "Lettuce", "Fish", "Beef", "Chicken", "Cereal", "Wheat", "Cane"]

LONG_TEXT = "Long text is happening11111111111111111111111111111111111111111111111111111111123456789"

#-----------------------------------------------------------------------------
# demo state (C: function "static" locals)
#-----------------------------------------------------------------------------

class _DemoState:

    def __init__(self):

        # menus
        self.main_menu_bar = False
        self.menu_grid = True
        self.menu_commands = 0
        self.menu_status = "Choose a command"

        # widgets/basic
        self.buff = "caa"
        self.buff2 = "ccc"
        self.f_value = 3.14
        self.i_value117 = 117
        self.i_value = 0
        self.i_value1 = 0
        self.f_value1 = 23.0
        self.f_value2 = 100.0
        self.i_value2 = 3
        self.u_value0 = 3
        self.color = [1.0, 0.0, 1.0, 0.75]
        self.menu_selection = False
        self.basic_combo_select = 0

        # widgets/text editing
        self.notes = ("Pilot Light notes\n\nEnter adds a line. Ctrl+Enter finishes editing.\n"
                      "Select text with the mouse or Shift+arrows.\nPaste, undo and redo work across lines.\n"
                      "Use the wheel or scrollbar for longer notes.")
        self.read_only = False
        self.ctrl_enter = False

        # widgets/multi widgets
        self.f_inputs = [0.0] * 4
        self.i_inputs = [0] * 4
        self.u_inputs = [0] * 4
        self.f_sliders = [10.0, 25.0, 50.0, 75.0]
        self.i_sliders = [-5, 0, 5, 10]
        self.u_sliders = [0, 3, 6, 10]
        self.f_drags = [0.0] * 4
        self.i_drags = [0] * 4
        self.u_drags = [0] * 4

        # widgets/selectables
        self.selectables = [False, False, False]

        # widgets/combo
        self.combo_height_small = False
        self.combo_height_regular = False
        self.combo_height_large = False
        self.combo_no_arrow = False
        self.combo_select = 0

        # widgets/colors
        self.color_edit_flags = 0
        self.color_to_edit = [1.0, 0.0, 0.2, 0.75]
        self.color_to_reference = [1.0, 0.0, 0.2, 0.75]

        # widgets/trees
        self.tree_flags = int(plUiTreeNodeFlags.PL_UI_TREE_NODE_FLAGS_DRAW_LINES_FULL)

        # widgets/tabs
        self.tab_selectables = [False, False, False]
        self.tab_radio = 0

        # scrolling
        self.use_clipper = True

        # layout systems
        self.sys1_count = 5
        self.sys1_height = 0.0
        self.sys2_count = 5
        self.sys2_width = 100.0
        self.sys2_height = 0.0
        self.sys3_dynamic = False
        self.sys3_count = 2
        self.sys3_static = [100.0] * 4
        self.sys3_ratios = [0.25] * 4
        self.sys3_height = 0.0
        self.sys4_dynamic = False
        self.sys4_count = 2
        self.sys4_static = [100.0] * 4
        self.sys4_ratios = [0.25] * 4
        self.sys4_height = 0.0
        self.sys5_height = 0.0

        # drawing
        self.thickness = 3.0
        self.offset = 0.0
        self.square = False
        self.miter = False
        self.no_aa = False
        self.rounding = 6.0

state = _DemoState()

#-----------------------------------------------------------------------------
# public api
#-----------------------------------------------------------------------------

def show_ui_demo_window(open: bool = True) -> bool:
    """Shows the UI demo window; returns open (False once the window is closed)."""

    visible, open = plUiI.begin_window("UI Demo", open, plUiWindowFlags.PL_UI_WINDOW_FLAGS_HORIZONTAL_SCROLLBAR)
    if not visible:
        return open

    plUiI.layout_row(ROW_DYNAMIC, 0.0, 1, [1.0])

    if plUiI.begin_menu_bar():
        _show_demo_menus()
        plUiI.end_menu_bar()

    if plUiI.collapsing_header("Help"):
        plUiI.text("Under construction")

    if plUiI.collapsing_header("Window Options"):
        plUiI.text("Under construction")

    if plUiI.collapsing_header("Widgets"):
        plUiI.stash_layout()
        _show_widgets()
        plUiI.apply_layout()

    if plUiI.collapsing_header("Scrolling"):
        plUiI.stash_layout()
        _show_scrolling()
        plUiI.apply_layout()

    if plUiI.collapsing_header("Layout Systems"):
        plUiI.stash_layout()
        _show_layout_systems()
        plUiI.apply_layout()

    if plUiI.collapsing_header("Drawing"):
        _show_pixel_perfect_demo()

    plUiI.end_window()
    return open

#-----------------------------------------------------------------------------
# menus
#-----------------------------------------------------------------------------

def _show_demo_menus():
    s = state
    if plUiI.begin_menu("File"):
        if plUiI.menu_item("New example", "Ctrl+N"):
            s.menu_commands += 1
            s.menu_status = "Created an example"
        if plUiI.begin_menu("Recent"):
            if plUiI.menu_item("Terrain"):
                s.menu_status = "Selected Terrain example"
            if plUiI.begin_menu("More examples"):
                if plUiI.menu_item("Nested command"):
                    s.menu_status = "Selected nested command"
                plUiI.end_menu()
            plUiI.end_menu()
        plUiI.separator()
        if plUiI.menu_item("Save UI layout"):
            s.menu_status = "Saved UI layout" if plUiI.save_settings() else "Could not save UI layout"
        plUiI.menu_item("Unavailable command", None, False, False)
        plUiI.end_menu()
    if plUiI.begin_menu("View"):
        _, s.main_menu_bar = plUiI.menu_item_toggle("Main menu bar", None, s.main_menu_bar)
        _, s.menu_grid = plUiI.menu_item_toggle("Example grid", None, s.menu_grid)
        plUiI.end_menu()
    if plUiI.begin_menu("Help"):
        plUiI.text(f"{s.menu_status} ({s.menu_commands} new examples)")
        plUiI.text("Shortcut labels are display-only in this demo.")
        plUiI.text("Drag a title onto a window, then choose a docking target.")
        plUiI.text("Center diamond: tabs. Compass arms: split the pane.")
        plUiI.text("Drag unused tab-bar space to move the group; drag tabs to undock.")
        plUiI.end_menu()
    plUiI.begin_menu("Disabled", False)

#-----------------------------------------------------------------------------
# widgets
#-----------------------------------------------------------------------------

def _combo(label, items, selected, flags=0):
    if plUiI.begin_combo(label, items[selected], flags):
        for i, item in enumerate(items):
            pressed, _ = plUiI.selectable(item, i == selected)
            if pressed:
                selected = i
                plUiI.close_current_popup()
        plUiI.end_combo()
    return selected

def _show_widgets():
    s = state

    if plUiI.tree_node("Basic"):

        plUiI.layout_static(0.0, 100, 2)
        plUiI.button("Button")
        plUiI.checkbox("Checkbox")

        plUiI.layout_dynamic(0.0, 2)
        plUiI.button("Button")
        plUiI.checkbox("Checkbox")

        plUiI.layout_dynamic(0.0, 1)
        _, s.buff = plUiI.input_text("label 0", s.buff, 0, 64)
        _, s.buff2 = plUiI.input_text_hint("label 1", "hint", s.buff2, 0, 64)

        _, s.f_value = plUiI.input_float("label 2", s.f_value, "%0.3f")
        _, s.f_value = plUiI.input_float_ex("label 2##", s.f_value, 1.0, 10.0, "%0.3f")
        _, s.i_value117 = plUiI.input_int("label 3", s.i_value117)

        plUiI.layout_row_begin(ROW_DYNAMIC, 0.0, 3)
        plUiI.layout_row_push(0.33)
        _, s.i_value = plUiI.radio_button("Option 1", s.i_value, 0)
        plUiI.layout_row_push(0.33)
        _, s.i_value = plUiI.radio_button("Option 2", s.i_value, 1)
        plUiI.layout_row_push(0.34)
        _, s.i_value = plUiI.radio_button("Option 3", s.i_value, 2)
        plUiI.layout_row_end()

        plUiI.layout_row(ROW_DYNAMIC, 0.0, 1, [1.0])
        plUiI.separator()
        plUiI.labeled_text("Label", "Value")
        _, s.f_value1 = plUiI.slider_float("float slider 1", s.f_value1, 0.0, 100.0)
        _, s.f_value2 = plUiI.slider_float("float slider 2", s.f_value2, -50.0, 100.0)
        _, s.i_value1 = plUiI.slider_int("int slider 1", s.i_value1, 0, 10)
        _, s.i_value2 = plUiI.slider_int("int slider 2", s.i_value2, -5, 10)
        _, s.u_value0 = plUiI.slider_uint("uint slider 0", s.u_value0, 0, 1)
        _, s.f_value2 = plUiI.drag_float("float drag", s.f_value2, 1.0, -100.0, 100.0)
        _, s.i_value1 = plUiI.drag_int("int drag", s.i_value1, 1.0, -100, 100)
        _, s.u_value0 = plUiI.drag_uint("uint drag", s.u_value0, 1.0, 0, 100)

        _, s.color = plUiI.color_edit3("Color Edit 3", s.color, 0, s.color)
        _, s.color = plUiI.color_edit4("Color Edit 4", s.color, 0, s.color)

        if plUiI.menu_item("Menu item 0"):
            print("menu item 0")
        if plUiI.menu_item("Menu item selected", "CTRL+M", True):
            print("menu item selected")
        if plUiI.menu_item("Menu item disabled", None, False, False):
            print("menu item disabled")
        pressed, s.menu_selection = plUiI.menu_item_toggle("Menu item toggle", None, s.menu_selection)
        if pressed:
            print("menu item toggle")

        if plUiI.begin_menu("Menu example"):
            if plUiI.menu_item("Menu item 0"):
                print("menu item 0")
            if plUiI.menu_item("Menu item selected", "CTRL+M", True):
                print("menu item selected")
            if plUiI.menu_item("Menu item disabled", None, False, False):
                print("menu item disabled")
            if plUiI.begin_menu("sub menu"):
                if plUiI.menu_item("Menu item 0"):
                    print("menu item 0")
                plUiI.end_menu()
            plUiI.end_menu()

        s.basic_combo_select = _combo("Combo", COMBO_ITEMS[:5], s.basic_combo_select)

        ratios22 = [200.0, 120.0]
        plUiI.layout_row(ROW_STATIC, 0.0, 2, ratios22)
        plUiI.button("Hover me!")
        if plUiI.was_last_item_hovered():
            plUiI.begin_tooltip()
            plUiI.layout_row(ROW_STATIC, 0.0, 1, ratios22)
            plUiI.text("I'm a tooltip!")
            plUiI.end_tooltip()
        plUiI.button("Just a button")

        plUiI.tree_pop()

    if plUiI.tree_node("Text Editing"):
        plUiI.layout_dynamic(0.0, 1)
        _, s.read_only = plUiI.checkbox("Read only", s.read_only)
        _, s.ctrl_enter = plUiI.checkbox("Ctrl+Enter inserts newline", s.ctrl_enter)
        flags = plUiInputTextFlags.PL_UI_INPUT_TEXT_FLAGS_ALLOW_TAB_INPUT
        if s.read_only:
            flags |= plUiInputTextFlags.PL_UI_INPUT_TEXT_FLAGS_READ_ONLY
        if s.ctrl_enter:
            flags |= plUiInputTextFlags.PL_UI_INPUT_TEXT_FLAGS_CTRL_ENTER_FOR_NEW_LINE
        plUiI.layout_dynamic(160.0, 1)
        _, s.notes = plUiI.input_text_multiline("##notes", s.notes, flags, 4096)
        plUiI.layout_dynamic(0.0, 1)
        plUiI.text("Fixed UTF-8 buffer; explicit newlines, no word wrapping.")
        plUiI.tree_pop()

    if plUiI.tree_node("Multi Widgets"):
        plUiI.layout_dynamic(0.0, 1)
        plUiI.text("Ctrl+click a slider or drag component to type a value.")

        # each widget edits the first N components of a shared 4 component value
        def edit(values, n, call):
            _, values[:n] = call(values[:n])

        plUiI.separator_text("Inputs")
        edit(s.f_inputs, 2, lambda v: plUiI.input_float2("input float 2", v, "%.3f"))
        edit(s.f_inputs, 3, lambda v: plUiI.input_float3("input float 3", v, "%.3f"))
        edit(s.f_inputs, 4, lambda v: plUiI.input_float4("input float 4", v, "%.3f"))
        edit(s.i_inputs, 2, lambda v: plUiI.input_int2("input int 2", v))
        edit(s.i_inputs, 3, lambda v: plUiI.input_int3("input int 3", v))
        edit(s.i_inputs, 4, lambda v: plUiI.input_int4("input int 4", v))
        edit(s.u_inputs, 2, lambda v: plUiI.input_uint2("input uint 2", v))
        edit(s.u_inputs, 3, lambda v: plUiI.input_uint3("input uint 3", v))
        edit(s.u_inputs, 4, lambda v: plUiI.input_uint4("input uint 4", v))

        plUiI.separator_text("Sliders")
        edit(s.f_sliders, 2, lambda v: plUiI.slider_float2("float slider 2", v, 0.0, 100.0))
        edit(s.f_sliders, 3, lambda v: plUiI.slider_float3("float slider 3", v, 0.0, 100.0))
        edit(s.f_sliders, 4, lambda v: plUiI.slider_float4_f("float slider 4", v, 0.0, 100.0, "%.1f"))
        edit(s.i_sliders, 2, lambda v: plUiI.slider_int2("int slider 2", v, -10, 10))
        edit(s.i_sliders, 3, lambda v: plUiI.slider_int3("int slider 3", v, -10, 10))
        edit(s.i_sliders, 4, lambda v: plUiI.slider_int4_f("int slider 4", v, -10, 10, "%+d"))
        edit(s.u_sliders, 2, lambda v: plUiI.slider_uint2("uint slider 2", v, 0, 10))
        edit(s.u_sliders, 3, lambda v: plUiI.slider_uint3("uint slider 3", v, 0, 10))
        edit(s.u_sliders, 4, lambda v: plUiI.slider_uint4_f("uint slider 4", v, 0, 10, "%02u"))

        plUiI.separator_text("Drags")
        edit(s.f_drags, 2, lambda v: plUiI.drag_float2("float drag 2", v, 1.0, -100.0, 100.0))
        edit(s.f_drags, 3, lambda v: plUiI.drag_float3("float drag 3", v, 1.0, -100.0, 100.0))
        edit(s.f_drags, 4, lambda v: plUiI.drag_float4_f("float drag 4", v, 1.0, -100.0, 100.0, "%.1f"))
        edit(s.i_drags, 2, lambda v: plUiI.drag_int2("int drag 2", v, 1.0, -100, 100))
        edit(s.i_drags, 3, lambda v: plUiI.drag_int3("int drag 3", v, 1.0, -100, 100))
        edit(s.i_drags, 4, lambda v: plUiI.drag_int4_f("int drag 4", v, 1.0, -100, 100, "%+d"))
        edit(s.u_drags, 2, lambda v: plUiI.drag_uint2("uint drag 2", v, 1.0, 0, 100))
        edit(s.u_drags, 3, lambda v: plUiI.drag_uint3("uint drag 3", v, 1.0, 0, 100))
        edit(s.u_drags, 4, lambda v: plUiI.drag_uint4_f("uint drag 4", v, 1.0, 0, 100, "%03u"))
        plUiI.tree_pop()

    if plUiI.tree_node("Selectables"):
        for i in range(3):
            _, s.selectables[i] = plUiI.selectable(f"Selectable {i + 1}", s.selectables[i])
        plUiI.tree_pop()

    if plUiI.tree_node("Combo"):
        _, s.combo_height_small = plUiI.checkbox("PL_UI_COMBO_FLAGS_HEIGHT_SMALL", s.combo_height_small)
        _, s.combo_height_regular = plUiI.checkbox("PL_UI_COMBO_FLAGS_HEIGHT_REGULAR", s.combo_height_regular)
        _, s.combo_height_large = plUiI.checkbox("PL_UI_COMBO_FLAGS_HEIGHT_LARGE", s.combo_height_large)
        _, s.combo_no_arrow = plUiI.checkbox("PL_UI_COMBO_FLAGS_NO_ARROW_BUTTON", s.combo_no_arrow)

        flags = plUiComboFlags.PL_UI_COMBO_FLAGS_NONE
        if s.combo_height_small:   flags |= plUiComboFlags.PL_UI_COMBO_FLAGS_HEIGHT_SMALL
        if s.combo_height_regular: flags |= plUiComboFlags.PL_UI_COMBO_FLAGS_HEIGHT_REGULAR
        if s.combo_height_large:   flags |= plUiComboFlags.PL_UI_COMBO_FLAGS_HEIGHT_LARGE
        if s.combo_no_arrow:       flags |= plUiComboFlags.PL_UI_COMBO_FLAGS_NO_ARROW_BUTTON

        s.combo_select = _combo("Combo", COMBO_ITEMS, s.combo_select, flags)
        plUiI.tree_pop()

    if plUiI.tree_node("Colors"):
        plUiI.separator_text("Color Picker")
        _, s.color_edit_flags = plUiI.checkbox_flags("no_alpha", s.color_edit_flags, plUiColorEditFlags.PL_UI_COLOR_EDIT_FLAGS_NO_ALPHA)
        _, s.color_edit_flags = plUiI.checkbox_flags("no_label", s.color_edit_flags, plUiColorEditFlags.PL_UI_COLOR_EDIT_FLAGS_NO_LABEL)
        _, s.color_edit_flags = plUiI.checkbox_flags("no_side_preview", s.color_edit_flags, plUiColorEditFlags.PL_UI_COLOR_EDIT_FLAGS_NO_SIDE_PREVIEW)

        _, s.color_to_edit = plUiI.color_edit4("base color##22", s.color_to_edit, s.color_edit_flags, s.color_to_reference)
        _, s.color_to_edit = plUiI.color_edit3("base color##33", s.color_to_edit, s.color_edit_flags, s.color_to_reference)

        plUiI.layout_static(400.0, 400.0, 1)
        _, s.color_to_edit = plUiI.color_picker4("base color", s.color_to_edit, s.color_edit_flags, s.color_to_reference)

        plUiI.color_button("base color button", s.color_to_edit, s.color_edit_flags, (50.0, 50.0))
        plUiI.color_button("base color button444", s.color_to_edit, s.color_edit_flags, (80.0, 50.0))
        plUiI.tree_pop()

    if plUiI.tree_node("Plotting"):
        plUiI.progress_bar(0.75, (-1.0, 0.0))
        plUiI.tree_pop()

    if plUiI.tree_node("Trees"):

        if plUiI.tree_node("Basic"):
            if plUiI.tree_node("Root Node"):
                if plUiI.tree_node("Child 1"):
                    plUiI.button("Press me")
                    plUiI.tree_pop()
                if plUiI.tree_node("Child 2"):
                    plUiI.button("Press me")
                    plUiI.tree_pop()
                plUiI.tree_pop()
            plUiI.tree_pop()

        if plUiI.tree_node("Hierarchy Lines", plUiTreeNodeFlags.PL_UI_TREE_NODE_FLAGS_DEFAULT_OPEN):
            _, s.tree_flags = plUiI.checkbox_flags("DrawLinesFull", s.tree_flags, plUiTreeNodeFlags.PL_UI_TREE_NODE_FLAGS_DRAW_LINES_FULL)
            _, s.tree_flags = plUiI.checkbox_flags("DrawLinesToNodes", s.tree_flags, plUiTreeNodeFlags.PL_UI_TREE_NODE_FLAGS_DRAW_LINES_TO_NODES)
            flags = s.tree_flags
            if plUiI.tree_node("Parent", flags):
                if plUiI.tree_node("Child 1", flags):
                    plUiI.button("Button for Child 1")
                    if plUiI.tree_node("Grand child 1", flags):
                        plUiI.button("Button for Grandchild 1")
                        plUiI.tree_pop()
                    plUiI.tree_pop()
                if plUiI.tree_node("Child 2", flags):
                    plUiI.button("Button for Child 2")
                    plUiI.tree_pop()
                plUiI.tree_pop()
            plUiI.tree_pop()

        plUiI.tree_pop()

    if plUiI.tree_node("Tabs"):
        if plUiI.begin_tab_bar("Tabs1"):
            if plUiI.begin_tab("Tab 0"):
                for i in range(3):
                    _, s.tab_selectables[i] = plUiI.selectable(f"Selectable {i + 1}", s.tab_selectables[i])
                plUiI.end_tab()

            if plUiI.begin_tab("Tab 1"):
                _, s.tab_radio = plUiI.radio_button("Option 1", s.tab_radio, 0)
                _, s.tab_radio = plUiI.radio_button("Option 2", s.tab_radio, 1)
                _, s.tab_radio = plUiI.radio_button("Option 3", s.tab_radio, 2)
                plUiI.end_tab()

            if plUiI.begin_tab("Tab 2"):
                if plUiI.begin_child("CHILD2"):
                    plUiI.layout_row(ROW_STATIC, 0.0, 1, [600.0])
                    for _ in range(25):
                        plUiI.text(LONG_TEXT)
                    plUiI.end_child()
                plUiI.end_tab()
            plUiI.end_tab_bar()
        plUiI.tree_pop()

#-----------------------------------------------------------------------------
# scrolling
#-----------------------------------------------------------------------------

def _show_scrolling():
    s = state
    ratios2 = [0.5, 0.5]

    plUiI.layout_static(0.0, 200, 1)
    _, s.use_clipper = plUiI.checkbox("Use Clipper", s.use_clipper)

    plUiI.layout_row(ROW_DYNAMIC, 300.0, 2, ratios2)
    if plUiI.begin_child("CHILD"):
        plUiI.layout_row(ROW_DYNAMIC, 0.0, 2, ratios2)
        if s.use_clipper:
            clipper = plUiClipper(1000000)
            while plUiI.step_clipper(clipper):
                for i in range(clipper.uDisplayStart, clipper.uDisplayEnd):
                    plUiI.text(f"{i} Label")
                    plUiI.text(f"{i} Value")
        else:
            for i in range(10000): # C demo: 1,000,000
                plUiI.text(f"{i} Label")
                plUiI.text(f"{i} Value")
        plUiI.end_child()

    if plUiI.begin_child("CHILD2"):
        plUiI.layout_row(ROW_STATIC, 0.0, 1, [600.0])
        for _ in range(25):
            plUiI.text(LONG_TEXT)
        plUiI.end_child()

#-----------------------------------------------------------------------------
# layout systems
#-----------------------------------------------------------------------------

def _notes(*lines):
    plUiI.separator_text("Notes")
    for line in lines:
        plUiI.text(line)
    plUiI.vertical_spacing()

def _buttons(count):
    for i in range(count):
        plUiI.button(f"Button {i}")

def _row_widths(dynamic, static_widths, dynamic_widths, count):
    widths = dynamic_widths if dynamic else static_widths
    lo, hi = (0.05, 1.2) if dynamic else (50.0, 500.0)
    for i in range(count):
        plUiI.push_id_uint(i)
        _, widths[i] = plUiI.slider_float("Widget Width", widths[i], lo, hi)
        plUiI.pop_id()
    return widths

def _show_layout_systems():
    s = state
    plUiI.text("General Notes")
    plUiI.text("  - systems ordered by increasing flexibility")
    plUiI.separator()

    height_note = ("  - height of 0.0f sets row height equal to minimum height", "    of maximum height widget")

    if plUiI.tree_node("System 1 - simple dynamic"):
        _notes("  - wraps (i.e. will add rows)", "  - evenly spaces widgets based on available space", *height_note)
        plUiI.separator_text("Options")
        _, s.sys1_count = plUiI.slider_int("Widget Count", s.sys1_count, 1, 10)
        _, s.sys1_height = plUiI.slider_float("Height", s.sys1_height, 0.0, 100.0)
        plUiI.vertical_spacing()
        plUiI.separator_text("Example")
        plUiI.layout_dynamic(s.sys1_height, s.sys1_count)
        plUiI.vertical_spacing()
        _buttons(s.sys1_count * 2)
        plUiI.tree_pop()

    if plUiI.tree_node("System 2 - simple static"):
        _notes("  - wraps (i.e. will add rows)", "  - provides each widget with the same specified width", *height_note)
        plUiI.separator_text("Options")
        _, s.sys2_count = plUiI.slider_int("Widget Count", s.sys2_count, 1, 10)
        _, s.sys2_width = plUiI.slider_float("Width", s.sys2_width, 50.0, 500.0)
        _, s.sys2_height = plUiI.slider_float("Height", s.sys2_height, 0.0, 100.0)
        plUiI.vertical_spacing()
        plUiI.separator_text("Example")
        plUiI.layout_static(s.sys2_height, s.sys2_width, s.sys2_count)
        plUiI.vertical_spacing()
        _buttons(s.sys2_count * 2)
        plUiI.tree_pop()

    row_notes = ("  - allows user to change widget widths individually",
                 "  - widths interpreted as ratios of available width when",
                 "    using PL_UI_LAYOUT_ROW_TYPE_DYNAMIC",
                 "  - widths interpreted as pixel width when using PL_UI_LAYOUT_ROW_TYPE_STATIC") + height_note

    if plUiI.tree_node("System 3 - single system row"):
        _notes("  - does not wrap (i.e. will not add rows)", *row_notes)
        plUiI.separator_text("Options")
        _, s.sys3_dynamic = plUiI.checkbox("Dynamic", s.sys3_dynamic)
        _, s.sys3_count = plUiI.slider_int("Widget Count", s.sys3_count, 1, 4)
        _, s.sys3_height = plUiI.slider_float("Height", s.sys3_height, 0.0, 100.0)
        widths = _row_widths(s.sys3_dynamic, s.sys3_static, s.sys3_ratios, s.sys3_count)
        plUiI.vertical_spacing()
        plUiI.separator_text("Example")
        plUiI.layout_row_begin(ROW_DYNAMIC if s.sys3_dynamic else ROW_STATIC, s.sys3_height, s.sys3_count)
        for i in range(s.sys3_count):
            plUiI.layout_row_push(widths[i])
            plUiI.button(f"Button {i}")
        plUiI.layout_row_end()
        plUiI.vertical_spacing()
        plUiI.tree_pop()

    if plUiI.tree_node("System 4 - single system row (array form)"):
        _notes("  - same as System 3 but array form", "  - wraps (i.e. will add rows)", *row_notes)
        plUiI.separator_text("Options")
        _, s.sys4_dynamic = plUiI.checkbox("Dynamic", s.sys4_dynamic)
        _, s.sys4_count = plUiI.slider_int("Widget Count", s.sys4_count, 1, 4)
        _, s.sys4_height = plUiI.slider_float("Height", s.sys4_height, 0.0, 100.0)
        widths = _row_widths(s.sys4_dynamic, s.sys4_static, s.sys4_ratios, s.sys4_count)
        plUiI.vertical_spacing()
        plUiI.separator_text("Example")
        plUiI.layout_row(ROW_DYNAMIC if s.sys4_dynamic else ROW_STATIC, s.sys4_height, s.sys4_count, widths)
        _buttons(s.sys4_count * 2)
        plUiI.vertical_spacing()
        plUiI.tree_pop()

    if plUiI.tree_node("System 5 - template"):
        _notes("  - most complex and second most flexible system",
               "  - wraps (i.e. will add rows)",
               "  - allows user to change widget systems individually",
               "    - dynamic: changes based on available space",
               "    - variable: same as dynamic but minimum width specified by user",
               "    - static: pixel width explicitely specified by user", *height_note)
        plUiI.separator_text("Options")
        _, s.sys5_height = plUiI.slider_float("Height", s.sys5_height, 0.0, 100.0)
        plUiI.vertical_spacing()

        plUiI.separator_text("Example 0")
        plUiI.layout_template_begin(s.sys5_height)
        plUiI.layout_template_push_dynamic()
        plUiI.layout_template_push_variable(150.0)
        plUiI.layout_template_push_static(150.0)
        plUiI.layout_template_end()
        plUiI.button("dynamic##0")
        plUiI.button("variable 150.0f##0")
        plUiI.button("static 150.0f##0")
        plUiI.checkbox("dynamic##1")
        plUiI.checkbox("variable 150.0f##1")
        plUiI.checkbox("static 150.0f##1")
        plUiI.vertical_spacing()

        plUiI.layout_dynamic(0.0, 1)
        plUiI.separator_text("Example 1")
        plUiI.layout_template_begin(s.sys5_height)
        plUiI.layout_template_push_static(150.0)
        plUiI.layout_template_push_variable(150.0)
        plUiI.layout_template_push_dynamic()
        plUiI.layout_template_end()
        plUiI.button("static 150.0f##2")
        plUiI.button("variable 150.0f##2")
        plUiI.button("dynamic##2")
        plUiI.checkbox("static 150.0f##3")
        plUiI.checkbox("variable 150.0f##3")
        plUiI.checkbox("dynamic##3")

        plUiI.layout_dynamic(0.0, 1)
        plUiI.separator_text("Example 2")
        plUiI.layout_template_begin(s.sys5_height)
        plUiI.layout_template_push_variable(150.0)
        plUiI.layout_template_push_variable(300.0)
        plUiI.layout_template_push_dynamic()
        plUiI.layout_template_end()
        plUiI.button("variable 150.0f##4")
        plUiI.button("variable 300.0f##4")
        plUiI.button("dynamic##4")
        plUiI.checkbox("static 150.0f##5")
        plUiI.button("variable 300.0f##5")
        plUiI.checkbox("dynamic##5")

        plUiI.vertical_spacing()
        plUiI.tree_pop()

    if plUiI.tree_node("System 6 - space"):
        _notes("  - most flexible system")

        plUiI.separator_text("Example - static")
        plUiI.layout_space_begin(ROW_STATIC, 500.0, 0xFFFFFFFF)
        plUiI.layout_space_push(0.0, 0.0, 100.0, 100.0)
        plUiI.button("w100 h100")
        plUiI.layout_space_push(105.0, 105.0, 300.0, 100.0)
        plUiI.button("x105 y105 w300 h100")
        plUiI.layout_space_end()

        plUiI.layout_dynamic(0.0, 1)
        plUiI.separator_text("Example - dynamic")
        plUiI.layout_space_begin(ROW_DYNAMIC, 300.0, 2)
        plUiI.layout_space_push(0.0, 0.0, 0.5, 0.5)
        plUiI.button("x0 y0 w0.5 h0.5")
        plUiI.layout_space_push(0.5, 0.5, 0.5, 0.5)
        plUiI.button("x0.5 y0.5 w0.5 h0.5")
        plUiI.layout_space_end()

        plUiI.tree_pop()

    if plUiI.tree_node("Misc. Testing"):
        version = f"Pilot Light {pl.PILOT_LIGHT_VERSION_STRING}"
        plUiI.layout_row(ROW_DYNAMIC, 0.0, 2, [0.5, 0.5])
        if plUiI.collapsing_header("Information"):
            plUiI.text(version)
            plUiI.text(f"Graphics Backend: {plGraphicsI.get_backend_string()}")

            plUiI.layout_row(ROW_DYNAMIC, 0.0, 3, [0.5 * 0.5, 0.25 * 0.5, 0.25 * 0.5])
            if plUiI.collapsing_header("sub0"):
                plUiI.text(version)
            if plUiI.collapsing_header("sub1"):
                for _ in range(2):
                    plUiI.text(version)
            if plUiI.collapsing_header("sub2"):
                for _ in range(3):
                    plUiI.text(version)

        # (C: locals reset every frame, so these widgets don't keep their values)
        if plUiI.collapsing_header("App Options"):
            plUiI.checkbox("Freeze Culling Camera")
            plUiI.slider_int("Sunlight Cascades", 2, 1, 4)

        if plUiI.collapsing_header("Graphics"):
            plUiI.checkbox("Freeze Culling Camera")
            plUiI.slider_int("Sunlight Cascades", 2, 1, 4)

        if plUiI.begin_tab_bar("tab bar2"):
            if plUiI.begin_tab("tab0000000000"):
                plUiI.checkbox("Entities")
                plUiI.end_tab()
            if plUiI.begin_tab("tab1"):
                for _ in range(4):
                    plUiI.checkbox("Profiling")
                plUiI.end_tab()
            plUiI.end_tab_bar()

        plUiI.layout_row(ROW_DYNAMIC, 0.0, 1, [1.0])
        if plUiI.collapsing_header("Tools"):
            plUiI.checkbox("Device Memory Analyzer")
            plUiI.checkbox("Device Memory Analyzer")

        if plUiI.collapsing_header("Debug"):
            plUiI.button("resize")
            plUiI.checkbox("Always Resize")

        plUiI.tree_pop()

#-----------------------------------------------------------------------------
# drawing (pixel perfect demo)
#-----------------------------------------------------------------------------

def _show_pixel_perfect_demo():
    s = state
    plUiI.text("Compare without MSAA. Coordinates are never implicitly snapped.")
    msaa = plGraphicsI.get_swapchain_info(plStarterI.get_swapchain()).eSampleCount != 1
    changed, msaa = plUiI.checkbox("MSAA", msaa)
    if changed:
        if msaa:
            plStarterI.activate_msaa()
        else:
            plStarterI.deactivate_msaa()
    _, s.thickness = plUiI.slider_float("Stroke width", s.thickness, 0.25, 12.0)
    _, s.offset = plUiI.slider_float("Subpixel offset", s.offset, 0.0, 1.0)
    _, s.square = plUiI.checkbox("Square caps", s.square)
    _, s.miter = plUiI.checkbox("Force miter joins", s.miter)
    _, s.no_aa = plUiI.checkbox("Disable stroke AA", s.no_aa)

    modes = [
        (plDrawStrokeFlag.PL_DRAW_STROKE_NONE,          "Centered"),
        (plDrawStrokeFlag.PL_DRAW_STROKE_CENTER_BIASED, "Center biased"),
        (plDrawStrokeFlag.PL_DRAW_STROKE_INSIDE,        "Inside (right of path)"),
        (plDrawStrokeFlag.PL_DRAW_STROKE_OUTSIDE,       "Outside (left of path)"),
    ]
    extra = 0
    if s.square: extra |= plDrawStrokeFlag.PL_DRAW_STROKE_CAP_SQUARE
    if s.miter:  extra |= plDrawStrokeFlag.PL_DRAW_STROKE_JOIN_MITER
    if s.no_aa:  extra |= plDrawStrokeFlag.PL_DRAW_STROKE_NO_AA

    layer = plUiI.get_window_fg_drawlayer()

    # red hairlines mark the actual path boundary, not the stroke's outside edge
    guide = plDrawLineOptions(0x807070ff, 0.5)
    for mode, name in modes:
        plUiI.text(name)
        p = plUiI.get_cursor_pos()
        px = math.floor(p.x) + 12.0 + s.offset
        py = math.floor(p.y) + 12.0 + s.offset
        options = plDrawLineOptions(0xc0f0b060, s.thickness, mode | extra)
        plDrawI.add_rect(layer, [px, py], [px + 70, py + 44], guide)
        plDrawI.add_rect(layer, [px, py], [px + 70, py + 44], options)
        plDrawI.add_circle(layer, [px + 110.5, py + 22.5], 7.5, 0, options)
        plDrawI.add_circle(layer, [px + 150, py + 22], 8.0, 0, options)
        plDrawI.path_line_to(layer, [px + 190, py + 44])
        plDrawI.path_line_to(layer, [px + 220, py])
        plDrawI.path_line_to(layer, [px + 224, py + 44])
        plDrawI.path_line_to(layer, [px + 270, py + 22])
        plDrawI.path_stroke(layer, options)
        plDrawI.add_line_h(layer, px + 300, px + 370, py + 10, options)
        plDrawI.add_line_v(layer, px + 335, py + 26, py + 48, options)
        plUiI.dummy([400, 76])

    plUiI.text("Path API: rounded rectangle, rotated ellipse, quadratic + cubic curves, convex fill")
    p = plUiI.get_cursor_pos()
    px, py = p.x + 12, p.y + 12
    options = plDrawLineOptions(0xff80d0ff, s.thickness)
    plDrawI.path_rect(layer, [px, py], [px + 80, py + 50], 12, 0, plDrawRectFlag.PL_DRAW_RECT_FLAG_NONE)
    plDrawI.path_stroke(layer, plDrawLineOptions(0xff80d0ff, s.thickness, plDrawStrokeFlag.PL_DRAW_STROKE_CLOSED))
    plDrawI.path_ellipse_to(layer, [px + 125, py + 25], [30, 15], 0.4, 0.0, 2.0 * math.pi, 0)
    plDrawI.path_fill_convex(layer, plDrawSolidOptions(0x8080d0ff))
    plDrawI.path_line_to(layer, [px + 180, py + 45])
    plDrawI.path_bezier_quad_to(layer, [px + 205, py - 30], [px + 230, py + 25], 0)
    plDrawI.path_bezier_cubic_to(layer, [px + 250, py + 80], [px + 290, py - 40], [px + 330, py + 25], 0)
    plDrawI.path_stroke(layer, options)
    plUiI.dummy([400, 90])

    plUiI.text("Filled triangle and quad: geometry AA (no MSAA required)")
    p = plUiI.get_cursor_pos()
    px, py = p.x + 12, p.y + 12
    fill = plDrawSolidOptions(0xc080d0ff)
    plDrawI.add_triangle_filled(layer, [px, py], [px + 60, py + 12], [px + 20, py + 45], fill)
    plDrawI.add_quad_filled(layer, [px + 90, py], [px + 150, py + 12], [px + 140, py + 45], [px + 100, py + 35], fill)
    plUiI.dummy([400, 70])

    _, s.rounding = plUiI.slider_float("Corner radius", s.rounding, 1, 16)
    plUiI.text("Rounded rectangles: automatic/baked (left), explicit geometry (right)")
    p = plUiI.get_cursor_pos()
    px, py = math.floor(p.x) + 12, math.floor(p.y) + 12
    for i in range(2):
        a = [px + i * 190, py]
        b = [px + i * 190 + 150, py + 50]
        segments = 16 if i else 0
        plDrawI.add_rect_rounded_filled(layer, a, b, s.rounding, segments, plDrawRectFlag.PL_DRAW_RECT_FLAG_NONE, plDrawSolidOptions(0xff805030))
        plDrawI.add_rect_rounded(layer, a, b, s.rounding, segments, plDrawRectFlag.PL_DRAW_RECT_FLAG_NONE,
                                 plDrawLineOptions(0xff80d0ff, s.thickness, plDrawStrokeFlag.PL_DRAW_STROKE_INSIDE))
    plUiI.dummy([400, 80])
