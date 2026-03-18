from typing import List, Any, Callable, Union, Tuple
import pilotlight.pilotlight as internal


class plShaderVariantI:

    def initialize(device):
        return internal.plShaderVariantI_initialize(device)