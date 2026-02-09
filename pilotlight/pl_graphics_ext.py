from typing import List, Any, Callable, Union, Tuple
import pilotlight.pilotlight as internal


class plGraphicsI:

    def flush_device(device, **kwargs) -> None:
        return internal.plGraphicsI_flush_device(device, **kwargs)

