/*
   pl_asset_ext_m.c
     * plAssetI
     * asset handles are exposed to python as integers (plAssetHandle.uData)
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] helpers
// [SECTION] plAssetI
// [SECTION] method tables
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] helpers
//-----------------------------------------------------------------------------

static inline bool
pl__asset_handle_from_py(PyObject* ptPyHandle, plAssetHandle* ptHandleOut)
{
    ptHandleOut->uData = PyLong_AsUnsignedLongLong(ptPyHandle);
    return !PyErr_Occurred();
}

static inline PyObject*
pl__asset_handle_to_py(plAssetHandle tHandle)
{
    return PyLong_FromUnsignedLongLong(tHandle.uData);
}

//-----------------------------------------------------------------------------
// [SECTION] plAssetI
//-----------------------------------------------------------------------------

PL_PY_BIND_VOID(asset_initialize,    gptAsset->initialize((plAssetInit){0}))
PL_PY_BIND_VOID(asset_cleanup,       gptAsset->cleanup())
PL_PY_BIND_VOID(asset_clear_assets,  gptAsset->clear_assets())
PL_PY_BIND_VOID(asset_clear_changes, gptAsset->clear_changes())

static PyObject*
asset_finalize(PyObject* self, PyObject* unused)
{
    // asset types of unstable extensions used by gltf & terrain (see pl_internal_m.c)
    pl__internal_register_asset_types();
    gptAsset->finalize();
    Py_RETURN_NONE;
}

static PyObject*
asset_load(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "path",
        NULL,
    };

    const char* pcPath = NULL;
	if (!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcPath))
		return NULL;

    return pl__asset_handle_to_py(gptAsset->load(pcPath));
}

static PyObject*
asset_find(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "path",
        NULL,
    };

    const char* pcPath = NULL;
	if (!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcPath))
		return NULL;

    return pl__asset_handle_to_py(gptAsset->find(pcPath));
}

static PyObject*
asset_destroy(PyObject* self, PyObject* ptPyHandle)
{
    plAssetHandle tHandle = {0};
    if(!pl__asset_handle_from_py(ptPyHandle, &tHandle))
        return NULL;
    gptAsset->destroy(tHandle);
    Py_RETURN_NONE;
}

static PyObject*
asset_reload(PyObject* self, PyObject* ptPyHandle)
{
    plAssetHandle tHandle = {0};
    if(!pl__asset_handle_from_py(ptPyHandle, &tHandle))
        return NULL;
    gptAsset->reload(tHandle);
    Py_RETURN_NONE;
}

static PyObject*
asset_is_valid(PyObject* self, PyObject* ptPyHandle)
{
    plAssetHandle tHandle = {0};
    if(!pl__asset_handle_from_py(ptPyHandle, &tHandle))
        return NULL;
    return PyBool_FromLong(gptAsset->is_valid(tHandle));
}

static PyObject*
asset_get_type_key(PyObject* self, PyObject* ptPyHandle)
{
    plAssetHandle tHandle = {0};
    if(!pl__asset_handle_from_py(ptPyHandle, &tHandle))
        return NULL;
    return PyLong_FromUnsignedLong(gptAsset->get_type_key(tHandle));
}

static PyObject*
asset_get_path(PyObject* self, PyObject* ptPyHandle)
{
    plAssetHandle tHandle = {0};
    if(!pl__asset_handle_from_py(ptPyHandle, &tHandle))
        return NULL;
    const char* pcPath = gptAsset->get_path(tHandle);
    if(pcPath == NULL)
        Py_RETURN_NONE;
    return PyUnicode_FromString(pcPath);
}

// scenes (ecs asset type) are returned as plComponentLibrary capsules,
// everything else is returned as an opaque capsule
static PyObject*
asset_get_data(PyObject* self, PyObject* ptPyHandle)
{
    plAssetHandle tHandle = {0};
    if(!pl__asset_handle_from_py(ptPyHandle, &tHandle))
        return NULL;

    if(!gptAsset->is_valid(tHandle))
        Py_RETURN_NONE;

    void* pData = gptAsset->get_data(tHandle);
    if(pData == NULL)
        Py_RETURN_NONE;

    if(gptAsset->get_type_key(tHandle) == gptEcs->get_asset_type_key())
        return pl_library_to_py((plComponentLibrary*)pData);
    return PyCapsule_New(pData, "plAssetData", NULL);
}

static PyObject*
asset_save(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "handle",
        "encoding",
        NULL,
    };

    unsigned long long ulHandle = 0;
    int iEncoding = PL_ASSET_ENCODING_AUTO;
	if (!pl_parse_args("K|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ulHandle, &iEncoding))
		return NULL;

    plAssetHandle tHandle = {.uData = ulHandle};
    return PyBool_FromLong(gptAsset->save(tHandle, iEncoding));
}

//-----------------------------------------------------------------------------
// [SECTION] method tables
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplAssetI[] =
{
    {"initialize",    (PyCFunction)asset_initialize,    METH_NOARGS | METH_STATIC, NULL},
    {"finalize",      (PyCFunction)asset_finalize,      METH_NOARGS | METH_STATIC, NULL},
    {"cleanup",       (PyCFunction)asset_cleanup,       METH_NOARGS | METH_STATIC, NULL},
    {"clear_assets",  (PyCFunction)asset_clear_assets,  METH_NOARGS | METH_STATIC, NULL},
    {"clear_changes", (PyCFunction)asset_clear_changes, METH_NOARGS | METH_STATIC, NULL},
    {"load",          (PyCFunction)asset_load,          METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"find",          (PyCFunction)asset_find,          METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"save",          (PyCFunction)asset_save,          METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"destroy",       (PyCFunction)asset_destroy,       METH_O | METH_STATIC, NULL},
    {"reload",        (PyCFunction)asset_reload,        METH_O | METH_STATIC, NULL},
    {"is_valid",      (PyCFunction)asset_is_valid,      METH_O | METH_STATIC, NULL},
    {"get_type_key",  (PyCFunction)asset_get_type_key,  METH_O | METH_STATIC, NULL},
    {"get_path",      (PyCFunction)asset_get_path,      METH_O | METH_STATIC, NULL},
    {"get_data",      (PyCFunction)asset_get_data,      METH_O | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plAssetI)

plPythonIntConstantPair gatAssetIntPairs[] = {
    PL_ADD_INT_CONSTANT(PL_ASSET_ENCODING_TEXT),
    PL_ADD_INT_CONSTANT(PL_ASSET_ENCODING_BINARY),
    PL_ADD_INT_CONSTANT(PL_ASSET_ENCODING_AUTO),
};
