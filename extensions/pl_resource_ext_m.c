/*
   pl_ecs_ext_m.c
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

    {
        PyObject* ptPythonObject = PyObject_GetAttrString(pyptInit, "uMaxTextureResolution");
        PyLong_AsUInt32(ptPythonObject, &tInit.uMaxTextureResolution);
        Py_DECREF(ptPythonObject);
    }

    {
        PyObject* ptPythonObject = PyObject_GetAttrString(pyptInit, "pcCacheDirectory");
        tInit.pcCacheDirectory = PyUnicode_AsUTF8(ptPythonObject);
        Py_DECREF(ptPythonObject);
    }

    {
        PyObject* ptPythonObject = PyObject_GetAttrString(pyptInit, "ptDevice");
        tInit.ptDevice = PyCapsule_GetPointer(ptPythonObject, "plDevice");
        Py_DECREF(ptPythonObject);
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

static PyMethodDef gatCommandsplResourceI[] =
{
    {"new_frame", (PyCFunction)resource_new_frame, METH_NOARGS | METH_STATIC, NULL},
    {"cleanup", (PyCFunction)resource_cleanup, METH_NOARGS | METH_STATIC, NULL},
    {"clear", (PyCFunction)resource_clear, METH_NOARGS | METH_STATIC, NULL},
    {"load", (PyCFunction)resource_load, METH_VARARGS | METH_STATIC, NULL},
    {"get_texture", (PyCFunction)resource_get_texture, METH_VARARGS | METH_STATIC, NULL},
    {"initialize", (PyCFunction)resource_initialize, METH_VARARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plResourceI)