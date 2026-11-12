/*
   pl_material_ext_m.c
     * plMaterialI
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

PL_PY_BIND_VOID(material_register_asset_types, gptMaterial->register_asset_types())
PL_PY_BIND_KEY(material_get_asset_type_key,    gptMaterial->get_asset_type_key())

//-----------------------------------------------------------------------------
// [SECTION] method tables
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplMaterialI[] =
{
    {"register_asset_types", (PyCFunction)material_register_asset_types, METH_NOARGS | METH_STATIC, NULL},
    {"get_asset_type_key",   (PyCFunction)material_get_asset_type_key,   METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plMaterialI)
