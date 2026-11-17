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

// window attribute values by type:
//   SIZE         -> (width, height)        (uint)
//   POSITION     -> (x, y)                 (int)
//   SIZE_LIMITS  -> (minWidth, minHeight, maxWidth, maxHeight) (uint)
//   others       -> bool
static PyObject*
window_set_attribute(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"window", "attribute", "value", NULL};
    PyObject* ptPyWindow = NULL;
    int iAttribute = 0;
    PyObject* ptValue = NULL;
    if(!pl_parse_args("OiO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptPyWindow, &iAttribute, &ptValue))
        return NULL;
    plWindow* ptWindow = PyCapsule_GetPointer(ptPyWindow, "plWindow");
    if(ptWindow == NULL)
        return NULL;

    plWindowAttributeValue tValue = {0};
    switch(iAttribute)
    {
        case PL_WINDOW_ATTRIBUTE_SIZE:
            if(!PyArg_ParseTuple(ptValue, "II;SIZE expects (width, height)", &tValue.tuVec2.x, &tValue.tuVec2.y))
                return NULL;
            break;
        case PL_WINDOW_ATTRIBUTE_POSITION:
            if(!PyArg_ParseTuple(ptValue, "ii;POSITION expects (x, y)", &tValue.tiVec2.x, &tValue.tiVec2.y))
                return NULL;
            break;
        case PL_WINDOW_ATTRIBUTE_SIZE_LIMITS:
            if(!PyArg_ParseTuple(ptValue, "IIII;SIZE_LIMITS expects (minWidth, minHeight, maxWidth, maxHeight)",
                &tValue.tuVec4.x, &tValue.tuVec4.y, &tValue.tuVec4.z, &tValue.tuVec4.w))
                return NULL;
            break;
        default:
        {
            if(iAttribute < 0 || iAttribute >= PL_WINDOW_ATTRIBUTE_COUNT)
            {
                PyErr_Format(PyExc_ValueError, "unknown window attribute %d", iAttribute);
                return NULL;
            }
            const int iTruth = PyObject_IsTrue(ptValue);
            if(iTruth < 0)
                return NULL;
            tValue.bValue = iTruth != 0;
            break;
        }
    }
    return PyBool_FromLong(gptWindow->set_attribute(ptWindow, iAttribute, &tValue));
}

// returns the value (see window_set_attribute) or None if unsupported
static PyObject*
window_get_attribute(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"window", "attribute", NULL};
    PyObject* ptPyWindow = NULL;
    int iAttribute = 0;
    if(!pl_parse_args("Oi", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptPyWindow, &iAttribute))
        return NULL;
    plWindow* ptWindow = PyCapsule_GetPointer(ptPyWindow, "plWindow");
    if(ptWindow == NULL)
        return NULL;
    if(iAttribute < 0 || iAttribute >= PL_WINDOW_ATTRIBUTE_COUNT)
    {
        PyErr_Format(PyExc_ValueError, "unknown window attribute %d", iAttribute);
        return NULL;
    }

    plWindowAttributeValue tValue = {0};
    if(!gptWindow->get_attribute(ptWindow, iAttribute, &tValue))
        Py_RETURN_NONE;
    switch(iAttribute)
    {
        case PL_WINDOW_ATTRIBUTE_SIZE:        return Py_BuildValue("(II)", tValue.tuVec2.x, tValue.tuVec2.y);
        case PL_WINDOW_ATTRIBUTE_POSITION:    return Py_BuildValue("(ii)", tValue.tiVec2.x, tValue.tiVec2.y);
        case PL_WINDOW_ATTRIBUTE_SIZE_LIMITS: return Py_BuildValue("(IIII)", tValue.tuVec4.x, tValue.tuVec4.y, tValue.tuVec4.z, tValue.tuVec4.w);
        default:                              return PyBool_FromLong(tValue.bValue);
    }
}

static PyObject*
window_set_cursor_mode(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"window", "mode", NULL};
    PyObject* ptPyWindow = NULL;
    int iMode = 0;
    if(!pl_parse_args("Oi", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptPyWindow, &iMode))
        return NULL;
    plWindow* ptWindow = PyCapsule_GetPointer(ptPyWindow, "plWindow");
    if(ptWindow == NULL)
        return NULL;
    return PyBool_FromLong(gptWindow->set_cursor_mode(ptWindow, iMode));
}

static PyObject*
window_get_cursor_mode(PyObject* self, PyObject* arg)
{
    plWindow* ptWindow = PyCapsule_GetPointer(arg, "plWindow");
    if(ptWindow == NULL)
        return NULL;
    return PyLong_FromLong((long)gptWindow->get_cursor_mode(ptWindow));
}

static PyObject*
window_set_raw_mouse_input(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"window", "enabled", NULL};
    PyObject* ptPyWindow = NULL;
    int iEnabled = 0;
    if(!pl_parse_args("Op", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptPyWindow, &iEnabled))
        return NULL;
    plWindow* ptWindow = PyCapsule_GetPointer(ptPyWindow, "plWindow");
    if(ptWindow == NULL)
        return NULL;
    return PyBool_FromLong(gptWindow->set_raw_mouse_input(ptWindow, iEnabled != 0));
}

// desc: plFullScreenDesc (tMode, iMonitor; -1 = automatic)
static PyObject*
window_set_fullscreen(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"window", "desc", NULL};
    PyObject* ptPyWindow = NULL;
    PyObject* ptPyDesc = NULL;
    if(!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptPyWindow, &ptPyDesc))
        return NULL;
    plWindow* ptWindow = PyCapsule_GetPointer(ptPyWindow, "plWindow");
    if(ptWindow == NULL)
        return NULL;
    uint32_t uMode = 0;
    if(!pl__get_u32_attr(ptPyDesc, "tMode", &uMode))
        return NULL;
    plFullScreenDesc tDesc = {.tMode = (plFullScreenMode)uMode, .iMonitor = -1};
    if(PyObject_HasAttrString(ptPyDesc, "iMonitor"))
    {
        PyObject* ptMonitor = PyObject_GetAttrString(ptPyDesc, "iMonitor");
        if(ptMonitor == NULL)
            return NULL;
        tDesc.iMonitor = (int)PyLong_AsLong(ptMonitor);
        Py_DECREF(ptMonitor);
        if(PyErr_Occurred())
            return NULL;
    }
    return PyBool_FromLong(gptWindow->set_fullscreen(ptWindow, &tDesc));
}

//-----------------------------------------------------------------------------
// [SECTION] timer api
//-----------------------------------------------------------------------------

static PyObject*
timer_get_time(PyObject* self, PyObject* unused)
{
    return PyFloat_FromDouble(gptTimer->get_time());
}

static PyObject*
timer_get_raw_time(PyObject* self, PyObject* unused)
{
    return PyFloat_FromDouble(gptTimer->get_raw_time());
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
    {"set_attribute", (PyCFunction)window_set_attribute, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_attribute", (PyCFunction)window_get_attribute, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_cursor_mode", (PyCFunction)window_set_cursor_mode, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_cursor_mode", (PyCFunction)window_get_cursor_mode, METH_O | METH_STATIC, NULL},
    {"set_raw_mouse_input", (PyCFunction)window_set_raw_mouse_input, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_fullscreen", (PyCFunction)window_set_fullscreen, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyMethodDef gatCommandsplTimerI[] =
{
    {"get_time",     (PyCFunction)timer_get_time,     METH_NOARGS | METH_STATIC, NULL},
    {"get_raw_time", (PyCFunction)timer_get_raw_time, METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plWindowI)
PL_NEW_PYTHON_API(plFileI)
PL_NEW_PYTHON_API(plTimerI)

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
    PL_ADD_INT_CONSTANT(PL_WINDOW_ATTRIBUTE_SIZE),
    PL_ADD_INT_CONSTANT(PL_WINDOW_ATTRIBUTE_POSITION),
    PL_ADD_INT_CONSTANT(PL_WINDOW_ATTRIBUTE_RESIZABLE),
    PL_ADD_INT_CONSTANT(PL_WINDOW_ATTRIBUTE_DECORATED),
    PL_ADD_INT_CONSTANT(PL_WINDOW_ATTRIBUTE_TOP_MOST),
    PL_ADD_INT_CONSTANT(PL_WINDOW_ATTRIBUTE_MINIMIZED),
    PL_ADD_INT_CONSTANT(PL_WINDOW_ATTRIBUTE_MAXIMIZED),
    PL_ADD_INT_CONSTANT(PL_WINDOW_ATTRIBUTE_VISIBLE),
    PL_ADD_INT_CONSTANT(PL_WINDOW_ATTRIBUTE_FOCUSED),
    PL_ADD_INT_CONSTANT(PL_WINDOW_ATTRIBUTE_HOVERED),
    PL_ADD_INT_CONSTANT(PL_WINDOW_ATTRIBUTE_SIZE_LIMITS),
    PL_ADD_INT_CONSTANT(PL_CURSOR_MODE_NORMAL),
    PL_ADD_INT_CONSTANT(PL_CURSOR_MODE_HIDDEN),
    PL_ADD_INT_CONSTANT(PL_CURSOR_MODE_CAPTURED),
    PL_ADD_INT_CONSTANT(PL_CURSOR_MODE_DISABLED),
    PL_ADD_INT_CONSTANT(PL_FULLSCREEN_MODE_NONE),
    PL_ADD_INT_CONSTANT(PL_FULLSCREEN_MODE_EXCLUSIVE),
    PL_ADD_INT_CONSTANT(PL_FULLSCREEN_MODE_BORDERLESS),
};
