/*
   pl_graphics_unit.c
     * compiles Pilot Light's graphics extension as its own translation unit
       (it has its own static globals, so it can't join the unity build)
*/

#define PL_PY_UNIT pl_py_graphics
#include "pl_py_shared_rename.h"
#include "pl_graphics_ext.c"
