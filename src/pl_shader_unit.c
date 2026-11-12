/*
   pl_shader_unit.c
     * compiles Pilot Light's shader extension as its own translation unit
       (it has its own static globals, so it can't join the unity build)
*/

#define PL_PY_UNIT pl_py_shader
#include "pl_py_shared_rename.h"
#include "pl_shader_ext.c"
