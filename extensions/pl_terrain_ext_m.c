/*
   pl_terrain_ext_m.c
     * plTerrainI
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

PL_PY_BIND_VOID(terrain_register_asset_types, gptTerrain->register_asset_types())
PL_PY_BIND_KEY(terrain_get_asset_type_key,    gptTerrain->get_asset_type_key())

//-----------------------------------------------------------------------------
// [SECTION] method tables
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplTerrainI[] =
{
    {"register_asset_types", (PyCFunction)terrain_register_asset_types, METH_NOARGS | METH_STATIC, NULL},
    {"get_asset_type_key",   (PyCFunction)terrain_get_asset_type_key,   METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plTerrainI)
