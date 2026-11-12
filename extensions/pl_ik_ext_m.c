/*
   pl_ik_ext_m.c
     * plIkI
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] implementations
// [SECTION] method tables
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] implementations
//-----------------------------------------------------------------------------

PL_PY_BIND_VOID(ik_register_ecs_components,   gptIk->register_ecs_components())
PL_PY_BIND_KEY(ik_get_ecs_type_key,           gptIk->get_ecs_type_key())
PL_PY_BIND_LIBRARY(ik_run_ecs_update_system,  gptIk->run_ecs_update_system(ptLibrary))

//-----------------------------------------------------------------------------
// [SECTION] method tables
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplIkI[] =
{
    {"register_ecs_components", (PyCFunction)ik_register_ecs_components, METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key",        (PyCFunction)ik_get_ecs_type_key,        METH_NOARGS | METH_STATIC, NULL},
    {"run_ecs_update_system",   (PyCFunction)ik_run_ecs_update_system,   METH_O | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plIkI)
