/*
   pl_transform_ext_m.c
     * plTransformI
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

PL_PY_BIND_VOID(transform_register_ecs_components,      gptTransform->register_ecs_components())
PL_PY_BIND_KEY(transform_get_ecs_type_key_transform,    gptTransform->get_ecs_type_key_transform())
PL_PY_BIND_KEY(transform_get_ecs_type_key_hierarchy,    gptTransform->get_ecs_type_key_hierarchy())
PL_PY_BIND_LIBRARY(transform_run_transform_update_system, gptTransform->run_transform_update_system(ptLibrary))
PL_PY_BIND_LIBRARY(transform_run_hierarchy_update_system, gptTransform->run_hierarchy_update_system(ptLibrary))

//-----------------------------------------------------------------------------
// [SECTION] method tables
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplTransformI[] =
{
    {"register_ecs_components",     (PyCFunction)transform_register_ecs_components,     METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_transform",  (PyCFunction)transform_get_ecs_type_key_transform,  METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_hierarchy",  (PyCFunction)transform_get_ecs_type_key_hierarchy,  METH_NOARGS | METH_STATIC, NULL},
    {"run_transform_update_system", (PyCFunction)transform_run_transform_update_system, METH_O | METH_STATIC, NULL},
    {"run_hierarchy_update_system", (PyCFunction)transform_run_hierarchy_update_system, METH_O | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plTransformI)
