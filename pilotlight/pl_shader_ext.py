########################################################################################################################
# User API Index
#
#    * Sections
#      - imports
#      - types
#      - apis
#      - enums
#
########################################################################################################################

########################################################################################################################
# [SECTION] imports
########################################################################################################################

import pilotlight.pilotlight as internal
from typing import List

########################################################################################################################
# [SECTION] types
########################################################################################################################

class plShaderMacroDefinition:
    def __init__(self):
        self.pcName = None
        self.pcValue = None

class plShaderOptions:
    def __init__(self):
        self.tFlags = 0
        self.tOptimizationLevel = 0
        self.ptMacroDefinitions = []
        self.apcIncludeDirectories = []
        self.apcDirectories = []
        self.pcCacheOutputDirectory = None

########################################################################################################################
# [SECTION] apis
########################################################################################################################

class plShaderI:

    def initialize(options):
        return internal.plShaderI_initialize(options)
    
    def cleanup():
        return internal.plShaderI_cleanup()

    def load_glsl(shader, entry_func, **kwargs):
        return internal.plShaderI_load_glsl(shader, entry_func, **kwargs)

    def compile_gls(shader, entry_func, **kwargs):
        return internal.plShaderI_compile_gls(shader, entry_func, **kwargs)

    def write_to_disk(shader, module):
        return internal.plShaderI_write_to_disk(shader, module)
    
########################################################################################################################
# [SECTION] enums
########################################################################################################################

# PL_SHADER_FLAGS CONSTANTS
PL_SHADER_FLAGS_NONE=internal.PL_SHADER_FLAGS_NONE
PL_SHADER_FLAGS_INCLUDE_DEBUG=internal.PL_SHADER_FLAGS_INCLUDE_DEBUG
PL_SHADER_FLAGS_ALWAYS_COMPILE=internal.PL_SHADER_FLAGS_ALWAYS_COMPILE
PL_SHADER_FLAGS_NEVER_CACHE=internal.PL_SHADER_FLAGS_NEVER_CACHE
PL_SHADER_FLAGS_METAL_OUTPUT=internal.PL_SHADER_FLAGS_METAL_OUTPUT
PL_SHADER_FLAGS_SPIRV_OUTPUT=internal.PL_SHADER_FLAGS_SPIRV_OUTPUT
PL_SHADER_FLAGS_AUTO_OUTPUT=internal.PL_SHADER_FLAGS_AUTO_OUTPUT

# PL_SHADER_OPTIMIZATION CONSTANTS
PL_SHADER_OPTIMIZATION_NONE=internal.PL_SHADER_OPTIMIZATION_NONE
PL_SHADER_OPTIMIZATION_SIZE=internal.PL_SHADER_OPTIMIZATION_SIZE
PL_SHADER_OPTIMIZATION_PERFORMANCE=internal.PL_SHADER_OPTIMIZATION_PERFORMANCE