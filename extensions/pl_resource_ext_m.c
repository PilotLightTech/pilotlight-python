/*
   pl_resource_ext_m.c
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] binding apis
// [SECTION] implementations
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] implementations
//-----------------------------------------------------------------------------

PyObject*
resource_new_frame(PyObject* self)
{
 
    gptResource->new_frame();
    Py_RETURN_NONE;
}

PyObject*
resource_cleanup(PyObject* self)
{
 
    gptResource->cleanup();
    Py_RETURN_NONE;
}

PyObject*
resource_clear(PyObject* self)
{
 
    gptResource->clear();
    Py_RETURN_NONE;
}

PyObject*
resource_initialize(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "init",
        NULL,
    };

    PyObject* pyptInit = NULL;

	if (!pl_parse_args("O", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &pyptInit))
		return NULL;

    plResourceManagerInit tInit = {0};

    // optional (default: 1024)
    if(PyObject_HasAttrString(pyptInit, "uMaxTextureResolution"))
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(pyptInit, "uMaxTextureResolution");
        PyLong_AsUInt32(ptPythonObject, &tInit.uMaxTextureResolution);
        Py_DECREF(ptPythonObject);
    }

    {
        PyObject* ptPythonObject = PyObject_GetAttrString(pyptInit, "ptDevice");
        if(ptPythonObject == NULL)
            return NULL;
        tInit.ptDevice = PyCapsule_GetPointer(ptPythonObject, "plDevice");
        Py_DECREF(ptPythonObject);
        if(tInit.ptDevice == NULL)
            return NULL;
    }

    gptResource->initialize(tInit);

    Py_RETURN_NONE;
}

PyObject*
resource_load(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "file",
        "flags",
        NULL,
    };

    const char* pcFile = NULL;
    plResourceLoadFlags tFlags = PL_RESOURCE_LOAD_FLAG_NONE;

	if (!pl_parse_args("si", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &pcFile, &tFlags))
		return NULL;

    plResourceHandle tHandle = gptResource->load(pcFile, tFlags);

    return PyLong_FromUInt64(tHandle.ulData);
}

PyObject*
resource_get_texture(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "handle",
        NULL,
    };

    plResourceHandle tHandle = {0};

	if (!pl_parse_args("K", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &tHandle.ulData))
		return NULL;

    plTextureHandle tTextureHandle = gptResource->get_texture(tHandle);

    return PyLong_FromUInt32(tTextureHandle.uData);
}

// fileData: optional bytes-like file contents (used during the call; copied if
// PL_RESOURCE_LOAD_FLAG_RETAIN_FILE_DATA is set)
static PyObject*
resource_load_ex(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"file", "flags", "fileData", "containerFileName", "fileBytesOffset", NULL};
    const char* pcFile = NULL;
    int iFlags = PL_RESOURCE_LOAD_FLAG_NONE;
    Py_buffer tData = {0};
    const char* pcContainer = NULL;
    Py_ssize_t szOffset = 0;
    if(!pl_parse_args("s|iz*zn", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcFile, &iFlags, &tData, &pcContainer, &szOffset))
        return NULL;
    if(szOffset < 0)
    {
        PyBuffer_Release(&tData);
        PyErr_SetString(PyExc_ValueError, "fileBytesOffset must be >= 0");
        return NULL;
    }
    plResourceHandle tHandle = gptResource->load_ex(pcFile, iFlags, (uint8_t*)tData.buf, tData.buf ? (size_t)tData.len : 0, pcContainer, (size_t)szOffset);
    PyBuffer_Release(&tData);
    return PyLong_FromUnsignedLongLong(tHandle.ulData);
}

// single "handle" argument
static bool
pl__resource_parse_handle(PyObject* args, PyObject* kwargs, plResourceHandle* ptHandleOut)
{
    static const char* apcKeywords[] = {"handle", NULL};
    unsigned long long ulData = 0;
    if(!pl_parse_args("K", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ulData))
        return false;
    ptHandleOut->ulData = ulData;
    return true;
}

static PyObject*
resource_unload(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plResourceHandle tHandle = {0};
    if(!pl__resource_parse_handle(args, kwargs, &tHandle))
        return NULL;
    gptResource->unload(tHandle);
    Py_RETURN_NONE;
}

static PyObject*
resource_is_valid(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plResourceHandle tHandle = {0};
    if(!pl__resource_parse_handle(args, kwargs, &tHandle))
        return NULL;
    return PyBool_FromLong(gptResource->is_valid(tHandle));
}

static PyObject*
resource_is_loaded(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"file", NULL};
    const char* pcFile = NULL;
    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcFile))
        return NULL;
    return PyBool_FromLong(gptResource->is_loaded(pcFile));
}

// retained file data (PL_RESOURCE_LOAD_FLAG_RETAIN_FILE_DATA) as bytes, or None
static PyObject*
resource_get_file_data(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plResourceHandle tHandle = {0};
    if(!pl__resource_parse_handle(args, kwargs, &tHandle))
        return NULL;
    size_t szSize = 0;
    const uint8_t* puData = gptResource->get_file_data(tHandle, &szSize);
    if(puData == NULL)
        Py_RETURN_NONE;
    return PyBytes_FromStringAndSize((const char*)puData, (Py_ssize_t)szSize);
}

static PyMethodDef gatCommandsplResourceI[] =
{
    {"load_ex", (PyCFunction)resource_load_ex, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"unload", (PyCFunction)resource_unload, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"is_valid", (PyCFunction)resource_is_valid, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"is_loaded", (PyCFunction)resource_is_loaded, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_file_data", (PyCFunction)resource_get_file_data, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"new_frame", (PyCFunction)resource_new_frame, METH_NOARGS | METH_STATIC, NULL},
    {"cleanup", (PyCFunction)resource_cleanup, METH_NOARGS | METH_STATIC, NULL},
    {"clear", (PyCFunction)resource_clear, METH_NOARGS | METH_STATIC, NULL},
    {"load", (PyCFunction)resource_load, METH_VARARGS | METH_STATIC, NULL},
    {"get_texture", (PyCFunction)resource_get_texture, METH_VARARGS | METH_STATIC, NULL},
    {"initialize", (PyCFunction)resource_initialize, METH_VARARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plResourceI)
plPythonIntConstantPair gatResourceIntPairs[] = {
    PL_ADD_INT_CONSTANT(PL_RESOURCE_LOAD_FLAG_NONE),
    PL_ADD_INT_CONSTANT(PL_RESOURCE_LOAD_FLAG_RETAIN_FILE_DATA),
    PL_ADD_INT_CONSTANT(PL_RESOURCE_LOAD_FLAG_BLOCK_COMPRESSED),
    PL_ADD_INT_CONSTANT(PL_RESOURCE_LOAD_FLAG_NO_CACHING),
};
