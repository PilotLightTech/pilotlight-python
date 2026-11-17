/*
   pl_platform_ext_m.c
     * plWindowI, plFileI (platform extension)
     * file paths are physical paths (not vfs paths)
     * window icon paths are vfs paths (loaded through plImageI)
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

// copies a python str attribute into a fixed size UTF-8 buffer (missing/None -> empty);
// truncates on a codepoint boundary
static bool
pl__copy_str_attr(PyObject* ptObject, const char* pcAttr, char* pcBuffer, size_t szBufferSize)
{
    pcBuffer[0] = 0;
    if(!PyObject_HasAttrString(ptObject, pcAttr))
        return true;
    PyObject* ptValue = PyObject_GetAttrString(ptObject, pcAttr);
    if(ptValue == NULL)
        return false;
    if(ptValue == Py_None)
    {
        Py_DECREF(ptValue);
        return true;
    }
    Py_ssize_t szLength = 0;
    const char* pcValue = PyUnicode_AsUTF8AndSize(ptValue, &szLength);
    if(pcValue == NULL)
    {
        Py_DECREF(ptValue);
        return false;
    }
    size_t szCopy = (size_t)szLength;
    if(szCopy >= szBufferSize)
    {
        szCopy = szBufferSize - 1;
        while(szCopy > 0 && (((unsigned char)pcValue[szCopy]) & 0xC0) == 0x80)
            szCopy--;
    }
    memcpy(pcBuffer, pcValue, szCopy);
    pcBuffer[szCopy] = 0;
    Py_DECREF(ptValue);
    return true;
}

// reads an unsigned 32 bit attribute (missing/None -> 0)
static bool
pl__get_u32_attr(PyObject* ptObject, const char* pcAttr, uint32_t* puOut)
{
    *puOut = 0;
    if(!PyObject_HasAttrString(ptObject, pcAttr))
        return true;
    PyObject* ptValue = PyObject_GetAttrString(ptObject, pcAttr);
    if(ptValue == NULL)
        return false;
    if(ptValue != Py_None)
    {
        unsigned long ulValue = PyLong_AsUnsignedLong(ptValue);
        if(PyErr_Occurred() || ulValue > 0xFFFFFFFFul)
        {
            Py_DECREF(ptValue);
            if(!PyErr_Occurred())
                PyErr_Format(PyExc_OverflowError, "%s does not fit in 32 bits", pcAttr);
            return false;
        }
        *puOut = (uint32_t)ulValue;
    }
    Py_DECREF(ptValue);
    return true;
}

//-----------------------------------------------------------------------------
// [SECTION] window api
//-----------------------------------------------------------------------------

// windows taskbar identity (see pl__python_set_app_id in pl_main.c); opt-in
// since it is process wide and the python process belongs to the app, not us

PyObject*
window_set_app_id(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "app_id",
        NULL,
    };

    const char* pcAppId = NULL;
    if (!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcAppId))
        return NULL;

    return PyBool_FromLong(pl__python_set_app_id(pcAppId));
}

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

PyObject*
window_get_capabilities(PyObject* self, PyObject* unused)
{
    const plWindowCapabilities* ptCapabilities = gptWindow->get_capabilities();
    return PyLong_FromLong(ptCapabilities ? (long)ptCapabilities->tFlags : 0);
}

// icons: sequence of plWindowIcon (uWidth, uHeight, pucPixels: RGBA8 bytes-like)
//        and/or vfs image paths (str, loaded as RGBA8 via plImageI);
//        None or an empty sequence restores the OS default
PyObject*
window_set_icon(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "window",
        "icons",
        NULL,
    };

    PyObject* ptPyWindow = NULL;
    PyObject* ptPyIcons = Py_None;
    if (!pl_parse_args("O|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyWindow, &ptPyIcons))
        return NULL;

    plWindow* ptWindow = PyCapsule_GetPointer(ptPyWindow, "plWindow");
    if(ptWindow == NULL)
        return NULL;

    if(ptPyIcons == Py_None)
        return PyBool_FromLong(gptWindow->set_icon(ptWindow, 0, NULL));

    PyObject* ptSequence = PySequence_Fast(ptPyIcons, "set_icon(): icons must be a sequence of plWindowIcon or str (vfs path)");
    if(ptSequence == NULL)
        return NULL;

    const Py_ssize_t szCount = PySequence_Fast_GET_SIZE(ptSequence);
    if(szCount > 16)
    {
        Py_DECREF(ptSequence);
        PyErr_SetString(PyExc_ValueError, "set_icon(): at most 16 icons are supported");
        return NULL;
    }

    plWindowIcon atIcons[16]  = {0};
    Py_buffer    atBuffers[16] = {0};
    bool         abBuffer[16]  = {0}; // atBuffers[i] needs releasing
    bool         abLoaded[16]  = {0}; // atIcons[i].pucPixels came from plImageI
    uint32_t     uIconCount = 0;
    bool         bError = false;

    for(Py_ssize_t i = 0; i < szCount && !bError; i++)
    {
        PyObject* ptItem = PySequence_Fast_GET_ITEM(ptSequence, i);

        if(PyUnicode_Check(ptItem))
        {
            const char* pcPath = PyUnicode_AsUTF8(ptItem);
            if(pcPath == NULL) { bError = true; break; }
            if(!gptVfs->does_file_exist(pcPath))
            {
                PyErr_Format(PyExc_FileNotFoundError, "set_icon(): icon not found: %s", pcPath);
                bError = true;
                break;
            }
            int iWidth = 0;
            int iHeight = 0;
            int iChannels = 0;
            unsigned char* pucPixels = gptImage->load_from_file(pcPath, &iWidth, &iHeight, &iChannels, 4);
            if(pucPixels == NULL)
            {
                PyErr_Format(PyExc_ValueError, "set_icon(): unable to load icon: %s", pcPath);
                bError = true;
                break;
            }
            atIcons[uIconCount] = (plWindowIcon){(uint32_t)iWidth, (uint32_t)iHeight, pucPixels};
            abLoaded[uIconCount] = true;
            uIconCount++;
            continue;
        }

        plWindowIcon tIcon = {0};
        if(!pl__get_u32_attr(ptItem, "uWidth", &tIcon.uWidth) || !pl__get_u32_attr(ptItem, "uHeight", &tIcon.uHeight))
        {
            bError = true;
            break;
        }

        PyObject* ptPixels = PyObject_GetAttrString(ptItem, "pucPixels");
        if(ptPixels == NULL)
        {
            bError = true;
            break;
        }
        if(PyObject_GetBuffer(ptPixels, &atBuffers[uIconCount], PyBUF_SIMPLE) != 0)
        {
            Py_DECREF(ptPixels);
            bError = true;
            break;
        }
        Py_DECREF(ptPixels); // buffer view keeps the object alive
        abBuffer[uIconCount] = true;

        const size_t szExpected = (size_t)tIcon.uWidth * (size_t)tIcon.uHeight * 4;
        if(szExpected == 0 || (size_t)atBuffers[uIconCount].len != szExpected)
        {
            PyErr_Format(PyExc_ValueError, "set_icon(): icon %zd expects %zu bytes (uWidth * uHeight * 4 RGBA8), got %zd",
                i, szExpected, atBuffers[uIconCount].len);
            uIconCount++;
            bError = true;
            break;
        }
        tIcon.pucPixels = atBuffers[uIconCount].buf;
        atIcons[uIconCount++] = tIcon;
    }

    bool bResult = false;
    if(!bError)
        bResult = gptWindow->set_icon(ptWindow, uIconCount, atIcons);

    // pixels are copied by the platform, safe to release
    for(uint32_t i = 0; i < uIconCount; i++)
    {
        if(abLoaded[i])
            gptImage->free((void*)atIcons[i].pucPixels);
        if(abBuffer[i])
            PyBuffer_Release(&atBuffers[i]);
    }
    Py_DECREF(ptSequence);

    if(bError)
        return NULL;
    return PyBool_FromLong(bResult);
}

// desc: plWindowTitleBarDesc or None (restores the native title bar)
PyObject*
window_set_title_bar(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "window",
        "desc",
        NULL,
    };

    PyObject* ptPyWindow = NULL;
    PyObject* ptPyDesc = Py_None;
    if (!pl_parse_args("O|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyWindow, &ptPyDesc))
        return NULL;

    plWindow* ptWindow = PyCapsule_GetPointer(ptPyWindow, "plWindow");
    if(ptWindow == NULL)
        return NULL;

    if(ptPyDesc == Py_None)
        return PyBool_FromLong(gptWindow->set_title_bar(ptWindow, NULL));

    plWindowTitleBarDesc tDesc = {0};
    if(!pl__get_u32_attr(ptPyDesc, "uHeight",                  &tDesc.uHeight)                  ||
       !pl__get_u32_attr(ptPyDesc, "uBackgroundColor",         &tDesc.uBackgroundColor)         ||
       !pl__get_u32_attr(ptPyDesc, "uInactiveBackgroundColor", &tDesc.uInactiveBackgroundColor) ||
       !pl__get_u32_attr(ptPyDesc, "uTextColor",               &tDesc.uTextColor)               ||
       !pl__get_u32_attr(ptPyDesc, "uAccentColor",             &tDesc.uAccentColor)             ||
       !pl__get_u32_attr(ptPyDesc, "uBrandColor",              &tDesc.uBrandColor)              ||
       !pl__get_u32_attr(ptPyDesc, "uStatusColor",             &tDesc.uStatusColor)             ||
       !pl__copy_str_attr(ptPyDesc, "acBrand",  tDesc.acBrand,  sizeof(tDesc.acBrand))          ||
       !pl__copy_str_attr(ptPyDesc, "acTitle",  tDesc.acTitle,  sizeof(tDesc.acTitle))          ||
       !pl__copy_str_attr(ptPyDesc, "acStatus", tDesc.acStatus, sizeof(tDesc.acStatus)))
        return NULL;

    return PyBool_FromLong(gptWindow->set_title_bar(ptWindow, &tDesc));
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
    {"set_app_id", (PyCFunction)window_set_app_id, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_capabilities", (PyCFunction)window_get_capabilities, METH_NOARGS | METH_STATIC, NULL},
    {"set_icon", (PyCFunction)window_set_icon, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_title_bar", (PyCFunction)window_set_title_bar, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
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
    PL_ADD_INT_CONSTANT(PL_WINDOW_CAPABILITY_FLAGS_NONE),
    PL_ADD_INT_CONSTANT(PL_WINDOW_CAPABILITY_FLAGS_RAW_MOUSE_INPUT),
    PL_ADD_INT_CONSTANT(PL_WINDOW_CAPABILITY_FLAGS_WINDOW_ICON),
    PL_ADD_INT_CONSTANT(PL_WINDOW_CAPABILITY_FLAGS_APP_ICON),
    PL_ADD_INT_CONSTANT(PL_WINDOW_CAPABILITY_FLAGS_CUSTOM_TITLE_BAR),
    PL_ADD_INT_CONSTANT(PL_WINDOW_CAPABILITY_FLAGS_TITLE_BAR_TEXT),
};
