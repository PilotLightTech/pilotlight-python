from typing import List, Any, Callable, Union, Tuple
import pilotlight.pilotlight as internal


class plEcsI:

    def initialize():
        return internal.plEcsI_initialize()

    def finalize():
        return internal.plEcsI_finalize()

    def cleanup():
        return internal.plEcsI_cleanup()
    
    def get_default_library():
        return internal.plEcsI_get_default_library()

    def get_component(library, key, entity):
        return internal.plEcsI_get_component(library, key, entity)