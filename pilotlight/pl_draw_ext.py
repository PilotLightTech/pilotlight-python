from typing import List, Any, Callable, Union, Tuple
import pilotlight.pilotlight as internal


class plDrawI:

    def add_triangle_filled(layer : Any, p0 : Union[List[float], Tuple[float, ...]], p1 : Union[List[float], Tuple[float, ...]], p2 : Union[List[float], Tuple[float, ...]], *, uColor: int = 4294967295, **kwargs) -> None:
        return internal.plDrawI_add_triangle_filled(layer, p0, p1, p2, uColor=uColor, **kwargs)

