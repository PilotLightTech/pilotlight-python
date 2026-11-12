/*
   pl_gltf_ext_m.c
     * plGltfI
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] implementations
// [SECTION] method tables
// [SECTION] constants
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] implementations
//-----------------------------------------------------------------------------

static PyObject*
gltf_import(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "path",
        "flags",
        NULL,
    };

    const char* pcPath = NULL;
    int iFlags = PL_GLTF_IMPORT_FLAGS_IMPORT_ALL_ASSETS;
	if (!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcPath, &iFlags))
		return NULL;

    // gltf import adds renderer object components (see pl_internal_m.c)
    if(!gbPyRendererInitialized)
    {
        PyErr_SetString(PyExc_RuntimeError, "gltf import requires plStarterI to be initialized before plAssetI.finalize()");
        return NULL;
    }

    const plGltfImportOptions tOptions = {
        .eFlags = iFlags
    };
    return PyBool_FromLong(gptGltf->import(pcPath, &tOptions));
}

//-----------------------------------------------------------------------------
// [SECTION] method tables
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplGltfI[] =
{
    {"import_file", (PyCFunction)gltf_import, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plGltfI)

//-----------------------------------------------------------------------------
// [SECTION] constants
//-----------------------------------------------------------------------------

plPythonIntConstantPair gatGltfIntPairs[] = {
    PL_ADD_INT_CONSTANT(PL_GLTF_IMPORT_FLAGS_NONE),
    PL_ADD_INT_CONSTANT(PL_GLTF_IMPORT_FLAGS_IMPORT_MESHES),
    PL_ADD_INT_CONSTANT(PL_GLTF_IMPORT_FLAGS_IMPORT_MATERIALS),
    PL_ADD_INT_CONSTANT(PL_GLTF_IMPORT_FLAGS_IMPORT_TEXTURES),
    PL_ADD_INT_CONSTANT(PL_GLTF_IMPORT_FLAGS_IMPORT_ANIMATIONS),
    PL_ADD_INT_CONSTANT(PL_GLTF_IMPORT_FLAGS_IMPORT_SKELETONS),
    PL_ADD_INT_CONSTANT(PL_GLTF_IMPORT_FLAGS_IMPORT_SCENES),
    PL_ADD_INT_CONSTANT(PL_GLTF_IMPORT_FLAGS_IMPORT_SKINS),
    PL_ADD_INT_CONSTANT(PL_GLTF_IMPORT_FLAGS_IMPORT_ALL_ASSETS),
};
