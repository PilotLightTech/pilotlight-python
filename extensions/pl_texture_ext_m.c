/*
   pl_texture_ext_m.c
     * plTextureI
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

PL_PY_BIND_VOID(texture_register_asset_types,  gptTexture->register_asset_types())
PL_PY_BIND_KEY(texture_get_asset_type_key,     gptTexture->get_asset_type_key())

//-----------------------------------------------------------------------------
// [SECTION] method tables
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplTextureI[] =
{
    {"register_asset_types", (PyCFunction)texture_register_asset_types, METH_NOARGS | METH_STATIC, NULL},
    {"get_asset_type_key",   (PyCFunction)texture_get_asset_type_key,   METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plTextureI)
