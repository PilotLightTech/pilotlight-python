/*
   pl_platform_unit.c
     * compiles Pilot Light's platform extension for the current OS as its
       own translation unit
*/

#define PL_PY_UNIT pl_py_platform
#include "pl_py_shared_rename.h"

#if defined(_WIN32)
    #include "pl_platform_win32_ext.c"
#elif defined(__APPLE__)
    #include "pl_platform_macos_ext.m"
#else
    #include "pl_platform_x11_ext.c"
#endif
