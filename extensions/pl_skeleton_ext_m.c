/*
   pl_skeleton_ext_m.c
     * plSkeletonI
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

PL_PY_BIND_VOID(skeleton_register_asset_types,       gptSkeleton->register_asset_types())
PL_PY_BIND_VOID(skeleton_register_ecs_components,    gptSkeleton->register_ecs_components())
PL_PY_BIND_KEY(skeleton_get_asset_type_key_skeleton, gptSkeleton->get_asset_type_key_skeleton())
PL_PY_BIND_KEY(skeleton_get_asset_type_key_skin,     gptSkeleton->get_asset_type_key_skin())
PL_PY_BIND_KEY(skeleton_get_ecs_type_key_skin,       gptSkeleton->get_ecs_type_key_skin())
PL_PY_BIND_LIBRARY(skeleton_run_skin_update_system,  gptSkeleton->run_skin_update_system(ptLibrary))

//-----------------------------------------------------------------------------
// [SECTION] method tables
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplSkeletonI[] =
{
    {"register_asset_types",        (PyCFunction)skeleton_register_asset_types,        METH_NOARGS | METH_STATIC, NULL},
    {"register_ecs_components",     (PyCFunction)skeleton_register_ecs_components,     METH_NOARGS | METH_STATIC, NULL},
    {"get_asset_type_key_skeleton", (PyCFunction)skeleton_get_asset_type_key_skeleton, METH_NOARGS | METH_STATIC, NULL},
    {"get_asset_type_key_skin",     (PyCFunction)skeleton_get_asset_type_key_skin,     METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_skin",       (PyCFunction)skeleton_get_ecs_type_key_skin,       METH_NOARGS | METH_STATIC, NULL},
    {"run_skin_update_system",      (PyCFunction)skeleton_run_skin_update_system,      METH_O | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plSkeletonI)
