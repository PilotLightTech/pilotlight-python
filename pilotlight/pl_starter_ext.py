########################################################################################################################
# [SECTION] imports
########################################################################################################################

from typing import List, Any, Callable, Union, Tuple
import pilotlight.pilotlight as internal

########################################################################################################################
# [SECTION] apis
########################################################################################################################

class plStarterI:

    def initialize(window, flags):
        return internal.plStarterI_initialize(window, flags)

    def cleanup(**kwargs):
        return internal.plStarterI_cleanup(**kwargs)

    def begin_frame(**kwargs) -> None:
        return internal.plStarterI_begin_frame(**kwargs)

    def finalize(**kwargs) -> None:
        return internal.plStarterI_finalize(**kwargs)

    def resize(**kwargs) -> None:
        return internal.plStarterI_resize(**kwargs)

    def end_frame(**kwargs) -> None:
        return internal.plStarterI_end_frame(**kwargs)

    def get_foreground_layer(**kwargs) -> None:
        return internal.plStarterI_get_foreground_layer(**kwargs)

    def get_background_layer(**kwargs) -> None:
        return internal.plStarterI_get_background_layer(**kwargs)
    
    def get_device(**kwargs) -> None:
        return internal.plStarterI_get_device(**kwargs)
    
    def get_swapchain(**kwargs) -> None:
        return internal.plStarterI_get_swapchain(**kwargs)
    
    def get_render_pass(**kwargs) -> None:
        return internal.plStarterI_get_render_pass(**kwargs)
    
    def begin_main_pass(**kwargs) -> None:
        return internal.plStarterI_begin_main_pass(**kwargs)
    
    def end_main_pass(**kwargs) -> None:
        return internal.plStarterI_end_main_pass(**kwargs)

########################################################################################################################
# [SECTION] enums
########################################################################################################################

# PL_MOUSE_BUTTON CONSTANTS
PL_STARTER_FLAGS_NONE=internal.PL_STARTER_FLAGS_NONE
PL_STARTER_FLAGS_DRAW_EXT=internal.PL_STARTER_FLAGS_DRAW_EXT
PL_STARTER_FLAGS_UI_EXT=internal.PL_STARTER_FLAGS_UI_EXT
PL_STARTER_FLAGS_CONSOLE_EXT=internal.PL_STARTER_FLAGS_CONSOLE_EXT
PL_STARTER_FLAGS_PROFILE_EXT=internal.PL_STARTER_FLAGS_PROFILE_EXT
PL_STARTER_FLAGS_STATS_EXT=internal.PL_STARTER_FLAGS_STATS_EXT
PL_STARTER_FLAGS_SHADER_EXT=internal.PL_STARTER_FLAGS_SHADER_EXT
PL_STARTER_FLAGS_SCREEN_LOG_EXT=internal.PL_STARTER_FLAGS_SCREEN_LOG_EXT
PL_STARTER_FLAGS_GRAPHICS_EXT=internal.PL_STARTER_FLAGS_GRAPHICS_EXT
PL_STARTER_FLAGS_TOOLS_EXT=internal.PL_STARTER_FLAGS_TOOLS_EXT
PL_STARTER_FLAGS_DEPTH_BUFFER=internal.PL_STARTER_FLAGS_DEPTH_BUFFER
PL_STARTER_FLAGS_MSAA=internal.PL_STARTER_FLAGS_MSAA
PL_STARTER_FLAGS_VSYNC_OFF=internal.PL_STARTER_FLAGS_VSYNC_OFF
PL_STARTER_FLAGS_REVERSE_Z=internal.PL_STARTER_FLAGS_REVERSE_Z
PL_STARTER_FLAGS_ALL_EXTENSIONS=internal.PL_STARTER_FLAGS_ALL_EXTENSIONS