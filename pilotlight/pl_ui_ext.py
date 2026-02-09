from typing import List, Any, Callable, Union, Tuple
import pilotlight.pilotlight as internal


class plUiI:

    def end_window(**kwargs) -> None:
        return internal.plUiI_end_window(**kwargs)

    def button(name : str, **kwargs) -> None:
        return internal.plUiI_button(name, **kwargs)

    def begin_window(name : str, **kwargs) -> None:
        return internal.plUiI_begin_window(name, **kwargs)

