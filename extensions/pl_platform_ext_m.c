/*
   pl_platform_ext_m.c
     * plWindowI, plFileI (platform extension)
     * file paths are physical paths (not vfs paths)
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] helpers
// [SECTION] window api
// [SECTION] file api
// [SECTION] method tables
// [SECTION] constants
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] helpers
//-----------------------------------------------------------------------------

// single "path" string argument
static inline bool
pl__file_parse_path(PyObject* args, PyObject* kwargs, const char** ppcPathOut)
{
    static const char* apcKeywords[] = {
        "path",
        NULL,
    };
    return pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, ppcPathOut);
}

//-----------------------------------------------------------------------------
// [SECTION] window api
//-----------------------------------------------------------------------------

PyObject*
window_create(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "desc",
        NULL,
    };

    
    PyObject* ptPythonDesc = NULL;
	if (!pl_parse_args("O", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDesc))
		return NULL;

    plWindowDesc tWindowDesc = {0};

    // optional (default: PL_WINDOW_PRESENTATION_MODE_GRAPHICS_API)
    if(PyObject_HasAttrString(ptPythonDesc, "tMode"))
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonDesc, "tMode");
        PyLong_AsInt32(ptPythonObject, &tWindowDesc.tMode);
        Py_DECREF(ptPythonObject);
    }

    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonDesc, "tFlags");
        PyLong_AsInt32(ptPythonObject, &tWindowDesc.tFlags);
        Py_DECREF(ptPythonObject);
    }

    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonDesc, "iXPos");
        PyLong_AsInt32(ptPythonObject, &tWindowDesc.iXPos);
        Py_DECREF(ptPythonObject);
    }

    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonDesc, "iYPos");
        PyLong_AsInt32(ptPythonObject, &tWindowDesc.iYPos);
        Py_DECREF(ptPythonObject);
    }

    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonDesc, "uWidth");
        PyLong_AsUInt32(ptPythonObject, &tWindowDesc.uWidth);
        Py_DECREF(ptPythonObject);
    }

    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonDesc, "uHeight");
        PyLong_AsUInt32(ptPythonObject, &tWindowDesc.uHeight);
        Py_DECREF(ptPythonObject);
    }

    PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonDesc, "pcTitle");
    tWindowDesc.pcTitle = PyUnicode_AsUTF8(ptPythonObject);
    
    plWindow* ptWindow = NULL;
    plWindowResult tResult = gptWindow->create(tWindowDesc, &ptWindow);
    Py_DECREF(ptPythonObject); // must stay alive until window is created for title
    PyObject* ptCapsule = PyCapsule_New(ptWindow, "plWindow", NULL);
    return Py_BuildValue("(iO)", tResult, ptCapsule);
}

PyObject*
window_show(PyObject* self, PyObject* args)
{
    plWindow* ptWindowPtr = PyCapsule_GetPointer(args, "plWindow");
    gptWindow->show(ptWindowPtr);
    Py_RETURN_NONE;
}

PyObject*
window_destroy(PyObject* self, PyObject* args)
{

    plWindow* ptWindowPtr = PyCapsule_GetPointer(args, "plWindow");
    gptWindow->destroy(ptWindowPtr);
    Py_RETURN_NONE;
}

//-----------------------------------------------------------------------------
// [SECTION] file api
//-----------------------------------------------------------------------------

static PyObject*
file_exists(PyObject* self, PyObject* args, PyObject* kwargs)
{
    const char* pcPath = NULL;
    if(!pl__file_parse_path(args, kwargs, &pcPath))
        return NULL;
    return PyBool_FromLong(gptFile->exists(pcPath));
}

static PyObject*
file_remove(PyObject* self, PyObject* args, PyObject* kwargs)
{
    const char* pcPath = NULL;
    if(!pl__file_parse_path(args, kwargs, &pcPath))
        return NULL;
    return PyBool_FromLong(gptFile->remove(pcPath) == PL_FILE_RESULT_SUCCESS);
}

static PyObject*
file_copy(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "source",
        "destination",
        NULL,
    };

    const char* pcSource = NULL;
    const char* pcDestination = NULL;
	if (!pl_parse_args("ss", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcSource, &pcDestination))
		return NULL;
    return PyBool_FromLong(gptFile->copy(pcSource, pcDestination) == PL_FILE_RESULT_SUCCESS);
}

static PyObject*
file_directory_exists(PyObject* self, PyObject* args, PyObject* kwargs)
{
    const char* pcPath = NULL;
    if(!pl__file_parse_path(args, kwargs, &pcPath))
        return NULL;
    return PyBool_FromLong(gptFile->directory_exists(pcPath));
}

// returns True if the directory exists after the call
// (creating a directory that already exists is not an error)
static PyObject*
file_create_directory(PyObject* self, PyObject* args, PyObject* kwargs)
{
    const char* pcPath = NULL;
    if(!pl__file_parse_path(args, kwargs, &pcPath))
        return NULL;

    if(gptFile->directory_exists(pcPath))
        Py_RETURN_TRUE;
    return PyBool_FromLong(gptFile->create_directory(pcPath) == PL_FILE_RESULT_SUCCESS);
}

static PyObject*
file_remove_directory(PyObject* self, PyObject* args, PyObject* kwargs)
{
    const char* pcPath = NULL;
    if(!pl__file_parse_path(args, kwargs, &pcPath))
        return NULL;
    return PyBool_FromLong(gptFile->remove_directory(pcPath) == PL_FILE_RESULT_SUCCESS);
}

// returns list of (name, plDirectoryEntryType) tuples or None on failure
static PyObject*
file_get_directory_info(PyObject* self, PyObject* args, PyObject* kwargs)
{
    const char* pcPath = NULL;
    if(!pl__file_parse_path(args, kwargs, &pcPath))
        return NULL;

    plDirectoryInfo tInfo = {0};
    if(gptFile->get_directory_info(pcPath, &tInfo) != PL_FILE_RESULT_SUCCESS)
    {
        gptFile->cleanup_directory_info(&tInfo);
        Py_RETURN_NONE;
    }

    PyObject* ptResult = PyList_New(tInfo.uEntryCount);
    for(uint32_t i = 0; i < tInfo.uEntryCount; i++)
        PyList_SetItem(ptResult, i, Py_BuildValue("(si)", tInfo.sbtEntries[i].acName, tInfo.sbtEntries[i].eType));

    gptFile->cleanup_directory_info(&tInfo);
    return ptResult;
}

//-----------------------------------------------------------------------------
// [SECTION] method tables
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplFileI[] =
{
    {"exists",             (PyCFunction)file_exists,             METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"remove",             (PyCFunction)file_remove,             METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"copy",               (PyCFunction)file_copy,               METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"directory_exists",   (PyCFunction)file_directory_exists,   METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"create_directory",   (PyCFunction)file_create_directory,   METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"remove_directory",   (PyCFunction)file_remove_directory,   METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_directory_info", (PyCFunction)file_get_directory_info, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyMethodDef gatCommandsplWindowI[] =
{
    {"create", (PyCFunction)window_create, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"show", (PyCFunction)window_show, METH_O | METH_STATIC, NULL},
    {"destroy", (PyCFunction)window_destroy, METH_O | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plWindowI)
PL_NEW_PYTHON_API(plFileI)

//-----------------------------------------------------------------------------
// [SECTION] constants
//-----------------------------------------------------------------------------

plPythonIntConstantPair gatPlatformIntPairs[] = {
    PL_ADD_INT_CONSTANT(PL_DIRECTORY_ENTRY_TYPE_UNKNOWN),
    PL_ADD_INT_CONSTANT(PL_DIRECTORY_ENTRY_TYPE_DIRECTORY),
    PL_ADD_INT_CONSTANT(PL_DIRECTORY_ENTRY_TYPE_FILE),
    PL_ADD_INT_CONSTANT(PL_DIRECTORY_ENTRY_TYPE_PIPE),
    PL_ADD_INT_CONSTANT(PL_DIRECTORY_ENTRY_TYPE_LINK),
    PL_ADD_INT_CONSTANT(PL_DIRECTORY_ENTRY_TYPE_SOCKET),
    PL_ADD_INT_CONSTANT(PL_DIRECTORY_ENTRY_TYPE_BLOCK_DEVICE),
    PL_ADD_INT_CONSTANT(PL_DIRECTORY_ENTRY_TYPE_CHARACTER_DEVICE),
};
