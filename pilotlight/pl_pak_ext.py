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
# [SECTION] apis
########################################################################################################################

class plPakI:

    def begin_packing(file, content_version):
        return internal.plPakI_begin_packing(file, content_version)
    
    def add_from_disk(pak, pcPakPath, pcFilePath, bCompress):
        return internal.plPakI_add_from_disk(pak, pcPakPath, pcFilePath, bCompress)

    def end_packing(pak):
        return internal.plPakI_end_packing(pak)
    