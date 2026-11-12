/*
   pl_mesh_ext_m.c
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

PL_PY_BIND_VOID(mesh_register_asset_types, gptMesh->register_asset_types())
PL_PY_BIND_KEY(mesh_get_asset_type_key,    gptMesh->get_asset_type_key())

//-----------------------------------------------------------------------------
// [SECTION] method tables
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplMeshI[] =
{
    {"register_asset_types", (PyCFunction)mesh_register_asset_types, METH_NOARGS | METH_STATIC, NULL},
    {"get_asset_type_key",   (PyCFunction)mesh_get_asset_type_key,   METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plMeshI)
