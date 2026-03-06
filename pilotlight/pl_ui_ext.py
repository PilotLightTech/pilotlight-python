from typing import List, Any, Callable, Union, Tuple
import pilotlight.pilotlight as internal


class plUiI:

    def begin_window(name : str, **kwargs):
        return internal.plUiI_begin_window(name, **kwargs)
    
    def end_window(**kwargs) -> None:
        return internal.plUiI_end_window(**kwargs)

    def button(name : str, **kwargs):
        return internal.plUiI_button(name, **kwargs)

    def checkbox(name, value = None, **kwargs):
        return internal.plUiI_checkbox(name, value, **kwargs)
