from typing import List, Any, Callable, Union, Tuple
import pilotlight.pilotlight as internal


class plVfsI:

    def mount_directory(directory, physical_directory, **kwargs) -> None:
        return internal.plVfsI_mount_directory(directory, physical_directory, **kwargs)


