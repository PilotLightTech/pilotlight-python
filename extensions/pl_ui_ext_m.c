/*
   pl_ui_ext_m.c
     * plUiI (immediate mode UI extension)
     * mirrors pl_ui_ext.h; differences from the C api:
         - values passed by pointer are returned: (changed, value(s))
         - text functions take an already formatted python str
           (the *_v va_list variants are not bound)
         - multi-component values accept any sequence (or plVec2/3/4) and
           are returned as lists
         - plUiClipper & plUiTextFilter are python types
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] helpers
// [SECTION] python types
// [SECTION] setup & frame
// [SECTION] windows
// [SECTION] widgets
// [SECTION] sliders & drags (generated)
// [SECTION] inputs (generated)
// [SECTION] colors
// [SECTION] combo, trees, tabs & menus
// [SECTION] text filter & clipper
// [SECTION] layout systems
// [SECTION] id stack, state & docking
// [SECTION] method table
// [SECTION] constants
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] helpers
//-----------------------------------------------------------------------------

#define PL_UI_KW METH_VARARGS | METH_KEYWORDS | METH_STATIC

// reads count floats (between szMin & szMax) from a sequence or plVec2/3/4
static bool
pl__ui_floats_from_py(PyObject* ptObj, float* afOut, Py_ssize_t szMin, Py_ssize_t szMax, Py_ssize_t* pszCountOut, const char* pcName)
{
    Py_ssize_t szCount = 0;
    if(gptVec4Type && PyObject_TypeCheck(ptObj, (PyTypeObject*)gptVec4Type))
    {
        const plVec4 tV = ((pyplVec4*)ptObj)->v;
        afOut[0] = tV.x; afOut[1] = tV.y; afOut[2] = tV.z; afOut[3] = tV.w;
        szCount = 4;
    }
    else if(gptVec3Type && PyObject_TypeCheck(ptObj, (PyTypeObject*)gptVec3Type))
    {
        const plVec3 tV = ((pyplVec3*)ptObj)->v;
        afOut[0] = tV.x; afOut[1] = tV.y; afOut[2] = tV.z;
        szCount = 3;
    }
    else if(gptVec2Type && PyObject_TypeCheck(ptObj, (PyTypeObject*)gptVec2Type))
    {
        const plVec2 tV = ((pyplVec2*)ptObj)->v;
        afOut[0] = tV.x; afOut[1] = tV.y;
        szCount = 2;
    }
    else
    {
        PyObject* ptSeq = PySequence_Fast(ptObj, "expected a sequence of numbers");
        if(ptSeq == NULL)
            return false;
        szCount = PySequence_Fast_GET_SIZE(ptSeq);
        if(szCount >= szMin && szCount <= szMax)
        {
            for(Py_ssize_t i = 0; i < szCount; i++)
                afOut[i] = (float)PyFloat_AsDouble(PySequence_Fast_GET_ITEM(ptSeq, i));
        }
        Py_DECREF(ptSeq);
        if(PyErr_Occurred())
            return false;
    }
    if(szCount < szMin || szCount > szMax)
    {
        if(szMin == szMax)
            PyErr_Format(PyExc_ValueError, "%s: expected %zd values, got %zd", pcName, szMin, szCount);
        else
            PyErr_Format(PyExc_ValueError, "%s: expected %zd to %zd values, got %zd", pcName, szMin, szMax, szCount);
        return false;
    }
    if(pszCountOut)
        *pszCountOut = szCount;
    return true;
}

static PyObject*
pl__ui_floats_to_py(const float* afValues, Py_ssize_t szCount)
{
    PyObject* ptList = PyList_New(szCount);
    if(ptList == NULL)
        return NULL;
    for(Py_ssize_t i = 0; i < szCount; i++)
        PyList_SET_ITEM(ptList, i, PyFloat_FromDouble((double)afValues[i]));
    return ptList;
}

// reads exactly szCount ints from a sequence
static bool
pl__ui_ints_from_py(PyObject* ptObj, int* aiOut, Py_ssize_t szCount, const char* pcName)
{
    PyObject* ptSeq = PySequence_Fast(ptObj, "expected a sequence of ints");
    if(ptSeq == NULL)
        return false;
    if(PySequence_Fast_GET_SIZE(ptSeq) != szCount)
    {
        PyErr_Format(PyExc_ValueError, "%s: expected %zd values, got %zd", pcName, szCount, PySequence_Fast_GET_SIZE(ptSeq));
        Py_DECREF(ptSeq);
        return false;
    }
    for(Py_ssize_t i = 0; i < szCount; i++)
        aiOut[i] = (int)PyLong_AsLong(PySequence_Fast_GET_ITEM(ptSeq, i));
    Py_DECREF(ptSeq);
    return !PyErr_Occurred();
}

static PyObject*
pl__ui_ints_to_py(const int* aiValues, Py_ssize_t szCount)
{
    PyObject* ptList = PyList_New(szCount);
    if(ptList == NULL)
        return NULL;
    for(Py_ssize_t i = 0; i < szCount; i++)
        PyList_SET_ITEM(ptList, i, PyLong_FromLong((long)aiValues[i]));
    return ptList;
}

// reads exactly szCount uints from a sequence
static bool
pl__ui_uints_from_py(PyObject* ptObj, uint32_t* auOut, Py_ssize_t szCount, const char* pcName)
{
    PyObject* ptSeq = PySequence_Fast(ptObj, "expected a sequence of ints");
    if(ptSeq == NULL)
        return false;
    if(PySequence_Fast_GET_SIZE(ptSeq) != szCount)
    {
        PyErr_Format(PyExc_ValueError, "%s: expected %zd values, got %zd", pcName, szCount, PySequence_Fast_GET_SIZE(ptSeq));
        Py_DECREF(ptSeq);
        return false;
    }
    for(Py_ssize_t i = 0; i < szCount; i++)
        auOut[i] = (uint32_t)PyLong_AsUnsignedLong(PySequence_Fast_GET_ITEM(ptSeq, i));
    Py_DECREF(ptSeq);
    return !PyErr_Occurred();
}

static PyObject*
pl__ui_uints_to_py(const uint32_t* auValues, Py_ssize_t szCount)
{
    PyObject* ptList = PyList_New(szCount);
    if(ptList == NULL)
        return NULL;
    for(Py_ssize_t i = 0; i < szCount; i++)
        PyList_SET_ITEM(ptList, i, PyLong_FromUnsignedLong((unsigned long)auValues[i]));
    return ptList;
}

// optional plVec2 argument (NULL/None -> default)
static bool
pl__ui_vec2_arg(PyObject* ptObj, plVec2 tDefault, plVec2* ptOut)
{
    *ptOut = tDefault;
    if(ptObj == NULL || ptObj == Py_None)
        return true;
    float afValues[2] = {0};
    if(!pl__ui_floats_from_py(ptObj, afValues, 2, 2, NULL, "vec2"))
        return false;
    ptOut->x = afValues[0];
    ptOut->y = afValues[1];
    return true;
}

// optional plVec4 argument (NULL/None -> default)
static bool
pl__ui_vec4_arg(PyObject* ptObj, plVec4 tDefault, plVec4* ptOut)
{
    *ptOut = tDefault;
    if(ptObj == NULL || ptObj == Py_None)
        return true;
    float afValues[4] = {0};
    if(!pl__ui_floats_from_py(ptObj, afValues, 4, 4, NULL, "vec4"))
        return false;
    *ptOut = (plVec4){afValues[0], afValues[1], afValues[2], afValues[3]};
    return true;
}

// optional bool (None -> NULL pointer)
static bool
pl__ui_optional_bool(PyObject* ptObj, bool* pbStorage, bool** ppbOut)
{
    *ppbOut = NULL;
    if(ptObj == NULL || ptObj == Py_None)
        return true;
    const int iValue = PyObject_IsTrue(ptObj);
    if(iValue < 0)
        return false;
    *pbStorage = iValue != 0;
    *ppbOut = pbStorage;
    return true;
}

static PyObject*
pl__ui_changed_and_bool(bool bChanged, const bool* pbValue)
{
    if(pbValue == NULL)
        return Py_BuildValue("(NO)", PyBool_FromLong(bChanged), Py_None);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), PyBool_FromLong(*pbValue));
}

static PyObject*
pl__ui_layer_to_py(plDrawLayer2D* ptLayer)
{
    if(ptLayer == NULL)
        Py_RETURN_NONE;
    return PyCapsule_New(ptLayer, "plDrawLayer2D", NULL);
}

// edits a python str through a temporary UTF-8 buffer (at least szBufferSize bytes)
typedef bool (*plUiTextEditFn)(const char* pcLabel, const char* pcHint, char* pcBuffer, size_t szBufferSize, int iFlags);

static PyObject*
pl__ui_edit_text(const char* pcLabel, const char* pcHint, PyObject* ptValue, Py_ssize_t szBufferSize, int iFlags, plUiTextEditFn tFn)
{
    Py_ssize_t szLength = 0;
    const char* pcValue = PyUnicode_AsUTF8AndSize(ptValue, &szLength);
    if(pcValue == NULL)
        return NULL;
    if(szBufferSize < szLength + 1)
        szBufferSize = szLength + 1;
    char* pcBuffer = PyMem_Malloc((size_t)szBufferSize);
    if(pcBuffer == NULL)
        return PyErr_NoMemory();
    memcpy(pcBuffer, pcValue, (size_t)szLength);
    memset(&pcBuffer[szLength], 0, (size_t)(szBufferSize - szLength));
    const bool bChanged = tFn(pcLabel, pcHint, pcBuffer, (size_t)szBufferSize, iFlags);
    PyObject* ptResult = Py_BuildValue("(Ns)", PyBool_FromLong(bChanged), pcBuffer);
    PyMem_Free(pcBuffer);
    return ptResult;
}

static bool pl__ui_input_text_fn          (const char* l, const char* h, char* b, size_t s, int f) { (void)h; return gptUI->input_text(l, b, s, f); }
static bool pl__ui_input_text_hint_fn     (const char* l, const char* h, char* b, size_t s, int f) { return gptUI->input_text_hint(l, h, b, s, f); }
static bool pl__ui_input_text_multiline_fn(const char* l, const char* h, char* b, size_t s, int f) { (void)h; return gptUI->input_text_multiline(l, b, s, f); }

// copies a str into a fixed buffer, truncating on a UTF-8 codepoint boundary
static void
pl__ui_copy_utf8(char* pcDest, size_t szDestSize, const char* pcSrc, size_t szSrcLength)
{
    size_t szCopy = szSrcLength;
    if(szCopy >= szDestSize)
    {
        szCopy = szDestSize - 1;
        while(szCopy > 0 && (((unsigned char)pcSrc[szCopy]) & 0xC0) == 0x80)
            szCopy--;
    }
    memcpy(pcDest, pcSrc, szCopy);
    pcDest[szCopy] = 0;
}

//-----------------------------------------------------------------------------
// [SECTION] python types
//-----------------------------------------------------------------------------

static PyObject* gptUiClipperType    = NULL;
static PyObject* gptUiTextFilterType = NULL;

// plUiClipper(uItemCount=0)
typedef struct _pyplUiClipper
{
    PyObject_HEAD
    plUiClipper tClipper;
} pyplUiClipper;

static int
pl__ui_clipper_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"uItemCount", NULL};
    unsigned int uItemCount = 0;
    if(!pl_parse_args("|I", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uItemCount))
        return -1;
    pyplUiClipper* ptSelf = (pyplUiClipper*)self;
    memset(&ptSelf->tClipper, 0, sizeof(plUiClipper));
    ptSelf->tClipper.uItemCount = uItemCount;
    return 0;
}

static PyMemberDef gatUiClipperMembers[] = {
    {"uItemCount",    Py_T_UINT, offsetof(pyplUiClipper, tClipper.uItemCount),    0,           NULL},
    {"uDisplayStart", Py_T_UINT, offsetof(pyplUiClipper, tClipper.uDisplayStart), Py_READONLY, NULL},
    {"uDisplayEnd",   Py_T_UINT, offsetof(pyplUiClipper, tClipper.uDisplayEnd),   Py_READONLY, NULL},
    {NULL}
};

static PyType_Slot gatUiClipperSlots[] = {
    {Py_tp_new,     NULL}, // set in pl__ui_add_python_types (MSVC C4232: dllimport address isn't constant)
    {Py_tp_init,    (void*)pl__ui_clipper_init},
    {Py_tp_members, (void*)gatUiClipperMembers},
    {0, NULL}
};

static PyType_Spec gtUiClipperSpec = {"pilotlight.plUiClipper", sizeof(pyplUiClipper), 0, Py_TPFLAGS_DEFAULT, gatUiClipperSlots};

// plUiTextFilter(acInputBuffer="")
typedef struct _pyplUiTextFilter
{
    PyObject_HEAD
    plUiTextFilter tFilter;
} pyplUiTextFilter;

static int
pl__ui_text_filter_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"acInputBuffer", NULL};
    const char* pcText = "";
    Py_ssize_t szLength = 0;
    if(!pl_parse_args("|s#", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText, &szLength))
        return -1;
    pyplUiTextFilter* ptSelf = (pyplUiTextFilter*)self;
    if(ptSelf->tFilter.sbtFilters)
        gptUI->text_filter_cleanup(&ptSelf->tFilter);
    memset(&ptSelf->tFilter, 0, sizeof(plUiTextFilter));
    pl__ui_copy_utf8(ptSelf->tFilter.acInputBuffer, sizeof(ptSelf->tFilter.acInputBuffer), pcText, (size_t)szLength);
    gptUI->text_filter_build(&ptSelf->tFilter);
    return 0;
}

static void
pl__ui_text_filter_dealloc(PyObject* self)
{
    pyplUiTextFilter* ptSelf = (pyplUiTextFilter*)self;
    if(ptSelf->tFilter.sbtFilters)
        gptUI->text_filter_cleanup(&ptSelf->tFilter);
    PyTypeObject* ptType = Py_TYPE(self);
    ptType->tp_free(self);
    Py_DECREF(ptType);
}

static PyObject*
pl__ui_text_filter_get_input(PyObject* self, void* closure)
{
    return PyUnicode_FromString(((pyplUiTextFilter*)self)->tFilter.acInputBuffer);
}

// note: call plUiI.text_filter_build(filter) after changing the input (same as C)
static int
pl__ui_text_filter_set_input(PyObject* self, PyObject* value, void* closure)
{
    if(value == NULL)
    {
        PyErr_SetString(PyExc_AttributeError, "acInputBuffer cannot be deleted");
        return -1;
    }
    Py_ssize_t szLength = 0;
    const char* pcText = PyUnicode_AsUTF8AndSize(value, &szLength);
    if(pcText == NULL)
        return -1;
    plUiTextFilter* ptFilter = &((pyplUiTextFilter*)self)->tFilter;
    pl__ui_copy_utf8(ptFilter->acInputBuffer, sizeof(ptFilter->acInputBuffer), pcText, (size_t)szLength);
    return 0;
}

static PyGetSetDef gatUiTextFilterGetSet[] = {
    {"acInputBuffer", pl__ui_text_filter_get_input, pl__ui_text_filter_set_input, "filter text (max 255 UTF-8 bytes)", NULL},
    {NULL}
};

static PyType_Slot gatUiTextFilterSlots[] = {
    {Py_tp_new,     NULL}, // set in pl__ui_add_python_types (MSVC C4232: dllimport address isn't constant)
    {Py_tp_init,    (void*)pl__ui_text_filter_init},
    {Py_tp_dealloc, (void*)pl__ui_text_filter_dealloc},
    {Py_tp_getset,  (void*)gatUiTextFilterGetSet},
    {0, NULL}
};

static PyType_Spec gtUiTextFilterSpec = {"pilotlight.plUiTextFilter", sizeof(pyplUiTextFilter), 0, Py_TPFLAGS_DEFAULT, gatUiTextFilterSlots};

static plUiClipper*
pl__ui_clipper_from_py(PyObject* ptObj)
{
    if(!PyObject_TypeCheck(ptObj, (PyTypeObject*)gptUiClipperType))
    {
        PyErr_SetString(PyExc_TypeError, "expected plUiClipper");
        return NULL;
    }
    return &((pyplUiClipper*)ptObj)->tClipper;
}

static plUiTextFilter*
pl__ui_text_filter_from_py(PyObject* ptObj)
{
    if(!PyObject_TypeCheck(ptObj, (PyTypeObject*)gptUiTextFilterType))
    {
        PyErr_SetString(PyExc_TypeError, "expected plUiTextFilter");
        return NULL;
    }
    return &((pyplUiTextFilter*)ptObj)->tFilter;
}

// called from PyInit_pilotlight
static int
pl__ui_add_python_types(PyObject* ptModule)
{
    gatUiClipperSlots[0].pfunc    = (void*)PyType_GenericNew;
    gatUiTextFilterSlots[0].pfunc = (void*)PyType_GenericNew;
    gptUiClipperType = PyType_FromSpec(&gtUiClipperSpec);
    gptUiTextFilterType = PyType_FromSpec(&gtUiTextFilterSpec);
    if(gptUiClipperType == NULL || gptUiTextFilterType == NULL)
        return -1;
    Py_INCREF(gptUiClipperType);    // keep module-level references for type checks
    Py_INCREF(gptUiTextFilterType);
    if(PyModule_AddObject(ptModule, "plUiClipper", gptUiClipperType) < 0)
        return -1;
    if(PyModule_AddObject(ptModule, "plUiTextFilter", gptUiTextFilterType) < 0)
        return -1;
    return 0;
}

//-----------------------------------------------------------------------------
// [SECTION] setup & frame
//-----------------------------------------------------------------------------

static PyObject*
ui_initialize(PyObject* self, PyObject* unused)
{
    gptUI->initialize();
    Py_RETURN_NONE;
}

static PyObject*
ui_cleanup(PyObject* self, PyObject* unused)
{
    gptUI->cleanup();
    Py_RETURN_NONE;
}

static PyObject*
ui_get_draw_lists(PyObject* self, PyObject* unused)
{
    uint32_t uCount = 0;
    plDrawList2D** aptLists = gptUI->get_draw_lists(&uCount);
    PyObject* ptList = PyList_New(uCount);
    if(ptList == NULL)
        return NULL;
    for(uint32_t i = 0; i < uCount; i++)
        PyList_SET_ITEM(ptList, i, PyCapsule_New(aptLists[i], "plDrawList2D", NULL));
    return ptList;
}

static PyObject*
ui_get_debug_draw_list(PyObject* self, PyObject* unused)
{
    return PyCapsule_New(gptUI->get_debug_draw_list(), "plDrawList2D", NULL);
}

static PyObject*
ui_get_debug_draw_layer(PyObject* self, PyObject* unused)
{
    return pl__ui_layer_to_py(gptUI->get_debug_draw_layer());
}

static PyObject*
ui_begin_frame(PyObject* self, PyObject* unused)
{
    gptUI->begin_frame();
    Py_RETURN_NONE;
}

static PyObject*
ui_end_frame(PyObject* self, PyObject* unused)
{
    gptUI->end_frame();
    Py_RETURN_NONE;
}

static PyObject*
ui_wants_mouse_capture(PyObject* self, PyObject* unused)
{
    return PyBool_FromLong(gptUI->wants_mouse_capture());
}

static PyObject*
ui_wants_keyboard_capture(PyObject* self, PyObject* unused)
{
    return PyBool_FromLong(gptUI->wants_keyboard_capture());
}

static PyObject*
ui_wants_text_input(PyObject* self, PyObject* unused)
{
    return PyBool_FromLong(gptUI->wants_text_input());
}

static PyObject*
ui_set_wants_mouse_capture_next_frame(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"value", NULL};
    int iValue = 0;
    if(!pl_parse_args("p", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iValue))
        return NULL;
    gptUI->set_wants_mouse_capture_next_frame(iValue != 0);
    Py_RETURN_NONE;
}

static PyObject*
ui_set_wants_keyboard_capture_next_frame(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"value", NULL};
    int iValue = 0;
    if(!pl_parse_args("p", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iValue))
        return NULL;
    gptUI->set_wants_keyboard_capture_next_frame(iValue != 0);
    Py_RETURN_NONE;
}

static PyObject*
ui_show_debug_window(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"open", NULL};
    PyObject* ptOpen = Py_None;
    if(!pl_parse_args("|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptOpen))
        return NULL;
    bool bOpen = true;
    bool* pbOpen = NULL;
    if(!pl__ui_optional_bool(ptOpen, &bOpen, &pbOpen))
        return NULL;
    gptUI->show_debug_window(pbOpen);
    if(pbOpen == NULL)
        Py_RETURN_NONE;
    return PyBool_FromLong(bOpen);
}

static PyObject*
ui_show_style_editor_window(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"open", NULL};
    PyObject* ptOpen = Py_None;
    if(!pl_parse_args("|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptOpen))
        return NULL;
    bool bOpen = true;
    bool* pbOpen = NULL;
    if(!pl__ui_optional_bool(ptOpen, &bOpen, &pbOpen))
        return NULL;
    gptUI->show_style_editor_window(pbOpen);
    if(pbOpen == NULL)
        Py_RETURN_NONE;
    return PyBool_FromLong(bOpen);
}

static PyObject*
ui_set_light_theme(PyObject* self, PyObject* unused)
{
    gptUI->set_light_theme();
    Py_RETURN_NONE;
}

static PyObject*
ui_set_dark_theme(PyObject* self, PyObject* unused)
{
    gptUI->set_dark_theme();
    Py_RETURN_NONE;
}

static PyObject*
ui_push_theme_color(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"colorIndex", "color", NULL};
    int iIndex = 0;
    PyObject* ptColor = NULL;
    if(!pl_parse_args("iO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iIndex, &ptColor))
        return NULL;
    plVec4 tColor = {0};
    if(!pl__ui_vec4_arg(ptColor, tColor, &tColor))
        return NULL;
    gptUI->push_theme_color(iIndex, tColor);
    Py_RETURN_NONE;
}

static PyObject*
ui_pop_theme_color(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"count", NULL};
    unsigned int uCount = 1;
    if(!pl_parse_args("|I", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uCount))
        return NULL;
    gptUI->pop_theme_color(uCount);
    Py_RETURN_NONE;
}

static PyObject*
ui_set_default_font(PyObject* self, PyObject* arg)
{
    plFont* ptFont = PyCapsule_GetPointer(arg, "plFont");
    if(ptFont == NULL)
        return NULL;
    gptUI->set_default_font(ptFont);
    Py_RETURN_NONE;
}

static PyObject*
ui_get_default_font(PyObject* self, PyObject* unused)
{
    plFont* ptFont = gptUI->get_default_font();
    if(ptFont == NULL)
        Py_RETURN_NONE;
    return PyCapsule_New(ptFont, "plFont", NULL);
}

//-----------------------------------------------------------------------------
// [SECTION] windows
//-----------------------------------------------------------------------------

static PyObject*
ui_begin_window(PyObject* self, PyObject* args, PyObject* kwargs)
{
    // returns visible (open=None) or (visible, open)
    static const char* apcKeywords[] = {"name", "open", "flags", NULL};
    const char* pcName = NULL;
    PyObject* ptOpen = Py_None;
    int iFlags = 0;
    if(!pl_parse_args("s|Oi", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcName, &ptOpen, &iFlags))
        return NULL;
    bool bOpen = true;
    bool* pbOpen = NULL;
    if(!pl__ui_optional_bool(ptOpen, &bOpen, &pbOpen))
        return NULL;
    const bool bVisible = gptUI->begin_window(pcName, pbOpen, iFlags);
    if(pbOpen == NULL)
        return PyBool_FromLong(bVisible);
    return Py_BuildValue("(NN)", PyBool_FromLong(bVisible), PyBool_FromLong(bOpen));
}

static PyObject*
ui_end_window(PyObject* self, PyObject* unused)
{
    gptUI->end_window();
    Py_RETURN_NONE;
}

static PyObject*
ui_open_popup(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"name", "flags", NULL};
    const char* pcName = NULL;
    int iFlags = 0;
    if(!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcName, &iFlags))
        return NULL;
    gptUI->open_popup(pcName, iFlags);
    Py_RETURN_NONE;
}

static PyObject*
ui_open_popup_id(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"id", "flags", NULL};
    unsigned int uId = 0;
    int iFlags = 0;
    if(!pl_parse_args("I|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uId, &iFlags))
        return NULL;
    gptUI->open_popup_id(uId, iFlags);
    Py_RETURN_NONE;
}

static PyObject*
ui_close_current_popup(PyObject* self, PyObject* unused)
{
    gptUI->close_current_popup();
    Py_RETURN_NONE;
}

static PyObject*
ui_is_popup_open(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"name", NULL};
    const char* pcName = NULL;
    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcName))
        return NULL;
    return PyBool_FromLong(gptUI->is_popup_open(pcName));
}

static PyObject*
ui_is_popup_open_id(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"id", NULL};
    unsigned int uId = 0;
    if(!pl_parse_args("I", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uId))
        return NULL;
    return PyBool_FromLong(gptUI->is_popup_open_id(uId));
}

static PyObject*
ui_begin_popup(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"name", "flags", NULL};
    const char* pcName = NULL;
    int iFlags = 0;
    if(!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcName, &iFlags))
        return NULL;
    return PyBool_FromLong(gptUI->begin_popup(pcName, iFlags));
}

static PyObject*
ui_begin_popup_id(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"id", "flags", NULL};
    unsigned int uId = 0;
    int iFlags = 0;
    if(!pl_parse_args("I|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uId, &iFlags))
        return NULL;
    return PyBool_FromLong(gptUI->begin_popup_id(uId, iFlags));
}

static PyObject*
ui_end_popup(PyObject* self, PyObject* unused)
{
    gptUI->end_popup();
    Py_RETURN_NONE;
}

static PyObject*
ui_get_window_fg_drawlayer(PyObject* self, PyObject* unused)
{
    return pl__ui_layer_to_py(gptUI->get_window_fg_drawlayer());
}

static PyObject*
ui_get_window_bg_drawlayer(PyObject* self, PyObject* unused)
{
    return pl__ui_layer_to_py(gptUI->get_window_bg_drawlayer());
}

static PyObject*
ui_get_cursor_pos(PyObject* self, PyObject* unused)
{
    return pl_vec2_to_py(gptUI->get_cursor_pos());
}

static PyObject*
ui_begin_child(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"name", "childFlags", "windowFlags", NULL};
    const char* pcName = NULL;
    int iChildFlags = 0;
    int iWindowFlags = 0;
    if(!pl_parse_args("s|ii", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcName, &iChildFlags, &iWindowFlags))
        return NULL;
    return PyBool_FromLong(gptUI->begin_child(pcName, iChildFlags, iWindowFlags));
}

static PyObject*
ui_end_child(PyObject* self, PyObject* unused)
{
    gptUI->end_child();
    Py_RETURN_NONE;
}

static PyObject*
ui_begin_tooltip(PyObject* self, PyObject* unused)
{
    gptUI->begin_tooltip();
    Py_RETURN_NONE;
}

static PyObject*
ui_end_tooltip(PyObject* self, PyObject* unused)
{
    gptUI->end_tooltip();
    Py_RETURN_NONE;
}

static PyObject*
ui_is_window_appearing(PyObject* self, PyObject* unused)
{
    return PyBool_FromLong(gptUI->is_window_appearing());
}

static PyObject*
ui_is_window_hovered(PyObject* self, PyObject* unused)
{
    return PyBool_FromLong(gptUI->is_window_hovered());
}

static PyObject*
ui_get_window_pos(PyObject* self, PyObject* unused)
{
    return pl_vec2_to_py(gptUI->get_window_pos());
}

static PyObject*
ui_get_window_size(PyObject* self, PyObject* unused)
{
    return pl_vec2_to_py(gptUI->get_window_size());
}

static PyObject*
ui_get_window_scroll(PyObject* self, PyObject* unused)
{
    return pl_vec2_to_py(gptUI->get_window_scroll());
}

static PyObject*
ui_get_window_scroll_max(PyObject* self, PyObject* unused)
{
    return pl_vec2_to_py(gptUI->get_window_scroll_max());
}

static PyObject*
ui_get_content_region_available(PyObject* self, PyObject* unused)
{
    return pl_vec2_to_py(gptUI->get_content_region_available());
}

static PyObject*
ui_set_window_scroll(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"scroll", NULL};
    PyObject* ptScroll = NULL;
    if(!pl_parse_args("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptScroll))
        return NULL;
    plVec2 tScroll = {0};
    if(!pl__ui_vec2_arg(ptScroll, tScroll, &tScroll))
        return NULL;
    gptUI->set_window_scroll(tScroll);
    Py_RETURN_NONE;
}

static PyObject*
ui_set_next_window_pos(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"pos", "condition", NULL};
    PyObject* ptValue = NULL;
    int iCondition = PL_UI_COND_ONCE;
    if(!pl_parse_args("O|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptValue, &iCondition))
        return NULL;
    plVec2 tValue = {0};
    if(!pl__ui_vec2_arg(ptValue, tValue, &tValue))
        return NULL;
    gptUI->set_next_window_pos(tValue, iCondition);
    Py_RETURN_NONE;
}

static PyObject*
ui_set_next_window_size(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"size", "condition", NULL};
    PyObject* ptValue = NULL;
    int iCondition = PL_UI_COND_ONCE;
    if(!pl_parse_args("O|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptValue, &iCondition))
        return NULL;
    plVec2 tValue = {0};
    if(!pl__ui_vec2_arg(ptValue, tValue, &tValue))
        return NULL;
    gptUI->set_next_window_size(tValue, iCondition);
    Py_RETURN_NONE;
}

static PyObject*
ui_set_next_window_collapse(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"collapsed", "condition", NULL};
    int iCollapsed = 0;
    int iCondition = PL_UI_COND_ONCE;
    if(!pl_parse_args("p|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iCollapsed, &iCondition))
        return NULL;
    gptUI->set_next_window_collapse(iCollapsed != 0, iCondition);
    Py_RETURN_NONE;
}

//-----------------------------------------------------------------------------
// [SECTION] widgets
//-----------------------------------------------------------------------------

static PyObject*
ui_button(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"name", NULL};
    const char* pcText = NULL;
    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText))
        return NULL;
    return PyBool_FromLong(gptUI->button(pcText));
}

static PyObject*
ui_selectable(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"text", "value", "flags", NULL};
    const char* pcText = NULL;
    int iValue = 0;
    int iFlags = 0;
    if(!pl_parse_args("s|pi", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText, &iValue, &iFlags))
        return NULL;
    bool bValue = iValue != 0;
    const bool bPressed = gptUI->selectable(pcText, &bValue, iFlags);
    return pl__ui_changed_and_bool(bPressed, &bValue);
}

static PyObject*
ui_checkbox(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"name", "value", NULL};
    const char* pcText = NULL;
    PyObject* ptValue = Py_None;
    if(!pl_parse_args("s|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText, &ptValue))
        return NULL;
    bool bValue = false;
    bool* pbValue = NULL;
    if(!pl__ui_optional_bool(ptValue, &bValue, &pbValue))
        return NULL;
    const bool bPressed = gptUI->checkbox(pcText, pbValue);
    return pl__ui_changed_and_bool(bPressed, pbValue);
}

static PyObject*
ui_checkbox_flags(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"text", "flags", "value", NULL};
    const char* pcText = NULL;
    int iFlags = 0;
    int iValue = 0;
    if(!pl_parse_args("sii", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText, &iFlags, &iValue))
        return NULL;
    const bool bPressed = gptUI->checkbox_flags(pcText, &iFlags, iValue);
    return Py_BuildValue("(Ni)", PyBool_FromLong(bPressed), iFlags);
}

static PyObject*
ui_radio_button(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"text", "current", "value", NULL};
    const char* pcText = NULL;
    int iCurrent = 0;
    int iValue = 0;
    if(!pl_parse_args("sii", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText, &iCurrent, &iValue))
        return NULL;
    const bool bPressed = gptUI->radio_button(pcText, &iCurrent, iValue);
    return Py_BuildValue("(Ni)", PyBool_FromLong(bPressed), iCurrent);
}

static PyObject*
ui_image(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"textureID", "size", NULL};
    unsigned int uTexture = 0;
    PyObject* ptSize = NULL;
    if(!pl_parse_args("IO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uTexture, &ptSize))
        return NULL;
    plVec2 tSize = {0};
    if(!pl__ui_vec2_arg(ptSize, tSize, &tSize))
        return NULL;
    gptUI->image(uTexture, tSize);
    Py_RETURN_NONE;
}

static PyObject*
ui_image_ex(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"textureID", "size", "uv0", "uv1", "tintColor", "borderColor", NULL};
    unsigned int uTexture = 0;
    PyObject* ptSize = NULL;
    PyObject* ptUv0 = NULL;
    PyObject* ptUv1 = NULL;
    PyObject* ptTint = NULL;
    PyObject* ptBorder = NULL;
    if(!pl_parse_args("IO|OOOO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uTexture, &ptSize, &ptUv0, &ptUv1, &ptTint, &ptBorder))
        return NULL;
    plVec2 tSize = {0};
    if(!pl__ui_vec2_arg(ptSize, tSize, &tSize))
        return NULL;
    plVec2 tUv0 = {0}, tUv1 = {0};
    plVec4 tTint = {0}, tBorder = {0};
    if(!pl__ui_vec2_arg(ptUv0, (plVec2){0.0f, 0.0f}, &tUv0) || !pl__ui_vec2_arg(ptUv1, (plVec2){1.0f, 1.0f}, &tUv1) ||
       !pl__ui_vec4_arg(ptTint, (plVec4){1.0f, 1.0f, 1.0f, 1.0f}, &tTint) || !pl__ui_vec4_arg(ptBorder, (plVec4){0.0f, 0.0f, 0.0f, 0.0f}, &tBorder))
        return NULL;
    gptUI->image_ex(uTexture, tSize, tUv0, tUv1, tTint, tBorder);
    Py_RETURN_NONE;
}

static PyObject*
ui_image_button(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"id", "textureID", "size", NULL};
    const char* pcId = NULL;
    unsigned int uTexture = 0;
    PyObject* ptSize = NULL;
    if(!pl_parse_args("sIO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcId, &uTexture, &ptSize))
        return NULL;
    plVec2 tSize = {0};
    if(!pl__ui_vec2_arg(ptSize, tSize, &tSize))
        return NULL;
    return PyBool_FromLong(gptUI->image_button(pcId, uTexture, tSize));
}

static PyObject*
ui_image_button_ex(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"id", "textureID", "size", "uv0", "uv1", "tintColor", "borderColor", NULL};
    const char* pcId = NULL;
    unsigned int uTexture = 0;
    PyObject* ptSize = NULL;
    PyObject* ptUv0 = NULL;
    PyObject* ptUv1 = NULL;
    PyObject* ptTint = NULL;
    PyObject* ptBorder = NULL;
    if(!pl_parse_args("sIO|OOOO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcId, &uTexture, &ptSize, &ptUv0, &ptUv1, &ptTint, &ptBorder))
        return NULL;
    plVec2 tSize = {0};
    if(!pl__ui_vec2_arg(ptSize, tSize, &tSize))
        return NULL;
    plVec2 tUv0 = {0}, tUv1 = {0};
    plVec4 tTint = {0}, tBorder = {0};
    if(!pl__ui_vec2_arg(ptUv0, (plVec2){0.0f, 0.0f}, &tUv0) || !pl__ui_vec2_arg(ptUv1, (plVec2){1.0f, 1.0f}, &tUv1) ||
       !pl__ui_vec4_arg(ptTint, (plVec4){1.0f, 1.0f, 1.0f, 1.0f}, &tTint) || !pl__ui_vec4_arg(ptBorder, (plVec4){0.0f, 0.0f, 0.0f, 0.0f}, &tBorder))
        return NULL;
    return PyBool_FromLong(gptUI->image_button_ex(pcId, uTexture, tSize, tUv0, tUv1, tTint, tBorder));
}

static PyObject*
ui_invisible_button(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"text", "size", NULL};
    const char* pcText = NULL;
    PyObject* ptSize = NULL;
    if(!pl_parse_args("sO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText, &ptSize))
        return NULL;
    plVec2 tSize = {0};
    if(!pl__ui_vec2_arg(ptSize, tSize, &tSize))
        return NULL;
    return PyBool_FromLong(gptUI->invisible_button(pcText, tSize));
}

static PyObject*
ui_dummy(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"size", NULL};
    PyObject* ptSize = NULL;
    if(!pl_parse_args("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptSize))
        return NULL;
    plVec2 tSize = {0};
    if(!pl__ui_vec2_arg(ptSize, tSize, &tSize))
        return NULL;
    gptUI->dummy(tSize);
    Py_RETURN_NONE;
}

static PyObject*
ui_progress_bar(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"fraction", "size", "overlay", NULL};
    float fFraction = 0.0f;
    PyObject* ptSize = NULL;
    const char* pcOverlay = NULL;
    if(!pl_parse_args("f|Oz", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fFraction, &ptSize, &pcOverlay))
        return NULL;
    plVec2 tSize = {0};
    if(!pl__ui_vec2_arg(ptSize, (plVec2){-1.0f, 0.0f}, &tSize))
        return NULL;
    gptUI->progress_bar(fFraction, tSize, pcOverlay);
    Py_RETURN_NONE;
}

static PyObject*
ui_text(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"text", NULL};
    const char* pcText = NULL;
    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText))
        return NULL;
    gptUI->text("%s", pcText);
    Py_RETURN_NONE;
}

static PyObject*
ui_color_text(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"color", "text", NULL};
    PyObject* ptColor = NULL;
    const char* pcText = NULL;
    if(!pl_parse_args("Os", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptColor, &pcText))
        return NULL;
    plVec4 tColor = {0};
    if(!pl__ui_vec4_arg(ptColor, tColor, &tColor))
        return NULL;
    gptUI->color_text(tColor, "%s", pcText);
    Py_RETURN_NONE;
}

static PyObject*
ui_labeled_text(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "text", NULL};
    const char* pcLabel = NULL;
    const char* pcText = NULL;
    if(!pl_parse_args("ss", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &pcText))
        return NULL;
    gptUI->labeled_text(pcLabel, "%s", pcText);
    Py_RETURN_NONE;
}

static PyObject*
ui_input_text(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "flags", "bufferSize", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValue = NULL;
    int iFlags = 0;
    Py_ssize_t szBufferSize = 256;
    if(!pl_parse_args("sU|in", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValue, &iFlags, &szBufferSize))
        return NULL;
    return pl__ui_edit_text(pcLabel, NULL, ptValue, szBufferSize, iFlags, pl__ui_input_text_fn);
}

static PyObject*
ui_input_text_hint(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "hint", "value", "flags", "bufferSize", NULL};
    const char* pcLabel = NULL;
    const char* pcHint = NULL;
    PyObject* ptValue = NULL;
    int iFlags = 0;
    Py_ssize_t szBufferSize = 256;
    if(!pl_parse_args("ssU|in", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &pcHint, &ptValue, &iFlags, &szBufferSize))
        return NULL;
    return pl__ui_edit_text(pcLabel, pcHint, ptValue, szBufferSize, iFlags, pl__ui_input_text_hint_fn);
}

static PyObject*
ui_input_text_multiline(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "flags", "bufferSize", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValue = NULL;
    int iFlags = 0;
    Py_ssize_t szBufferSize = 4096;
    if(!pl_parse_args("sU|in", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValue, &iFlags, &szBufferSize))
        return NULL;
    return pl__ui_edit_text(pcLabel, NULL, ptValue, szBufferSize, iFlags, pl__ui_input_text_multiline_fn);
}

//-----------------------------------------------------------------------------
// [SECTION] sliders & drags (generated)
//-----------------------------------------------------------------------------

static PyObject*
ui_slider_float(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    float tValue = 0;
    float tMin = 0;
    float tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sfff|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &tMin, &tMax, &iFlags))
        return NULL;
    const bool bChanged = gptUI->slider_float(pcLabel, &tValue, tMin, tMax, iFlags);
    return Py_BuildValue("(Nd)", PyBool_FromLong(bChanged), (double)tValue);
}

static PyObject*
ui_slider_float_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    float tValue = 0;
    float tMin = 0;
    float tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sfffs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    const bool bChanged = gptUI->slider_float_f(pcLabel, &tValue, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(Nd)", PyBool_FromLong(bChanged), (double)tValue);
}

static PyObject*
ui_slider_float2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float tMin = 0;
    float tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOff|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &iFlags))
        return NULL;
    float atValues[4] = {0};
    if(!pl__ui_floats_from_py(ptValues, atValues, 2, 2, NULL, "slider_float2"))
        return NULL;
    const bool bChanged = gptUI->slider_float2(pcLabel, atValues, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(atValues, 2));
}

static PyObject*
ui_slider_float2_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float tMin = 0;
    float tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOffs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    float atValues[4] = {0};
    if(!pl__ui_floats_from_py(ptValues, atValues, 2, 2, NULL, "slider_float2_f"))
        return NULL;
    const bool bChanged = gptUI->slider_float2_f(pcLabel, atValues, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(atValues, 2));
}

static PyObject*
ui_slider_float3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float tMin = 0;
    float tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOff|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &iFlags))
        return NULL;
    float atValues[4] = {0};
    if(!pl__ui_floats_from_py(ptValues, atValues, 3, 3, NULL, "slider_float3"))
        return NULL;
    const bool bChanged = gptUI->slider_float3(pcLabel, atValues, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(atValues, 3));
}

static PyObject*
ui_slider_float3_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float tMin = 0;
    float tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOffs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    float atValues[4] = {0};
    if(!pl__ui_floats_from_py(ptValues, atValues, 3, 3, NULL, "slider_float3_f"))
        return NULL;
    const bool bChanged = gptUI->slider_float3_f(pcLabel, atValues, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(atValues, 3));
}

static PyObject*
ui_slider_float4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float tMin = 0;
    float tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOff|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &iFlags))
        return NULL;
    float atValues[4] = {0};
    if(!pl__ui_floats_from_py(ptValues, atValues, 4, 4, NULL, "slider_float4"))
        return NULL;
    const bool bChanged = gptUI->slider_float4(pcLabel, atValues, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(atValues, 4));
}

static PyObject*
ui_slider_float4_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float tMin = 0;
    float tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOffs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    float atValues[4] = {0};
    if(!pl__ui_floats_from_py(ptValues, atValues, 4, 4, NULL, "slider_float4_f"))
        return NULL;
    const bool bChanged = gptUI->slider_float4_f(pcLabel, atValues, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(atValues, 4));
}

static PyObject*
ui_slider_int(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    int tValue = 0;
    int tMin = 0;
    int tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("siii|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &tMin, &tMax, &iFlags))
        return NULL;
    const bool bChanged = gptUI->slider_int(pcLabel, &tValue, tMin, tMax, iFlags);
    return Py_BuildValue("(Ni)", PyBool_FromLong(bChanged), tValue);
}

static PyObject*
ui_slider_int_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    int tValue = 0;
    int tMin = 0;
    int tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("siiis|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    const bool bChanged = gptUI->slider_int_f(pcLabel, &tValue, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(Ni)", PyBool_FromLong(bChanged), tValue);
}

static PyObject*
ui_slider_int2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    int tMin = 0;
    int tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOii|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &iFlags))
        return NULL;
    int atValues[4] = {0};
    if(!pl__ui_ints_from_py(ptValues, atValues, 2, "slider_int2"))
        return NULL;
    const bool bChanged = gptUI->slider_int2(pcLabel, atValues, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_ints_to_py(atValues, 2));
}

static PyObject*
ui_slider_int2_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    int tMin = 0;
    int tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOiis|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    int atValues[4] = {0};
    if(!pl__ui_ints_from_py(ptValues, atValues, 2, "slider_int2_f"))
        return NULL;
    const bool bChanged = gptUI->slider_int2_f(pcLabel, atValues, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_ints_to_py(atValues, 2));
}

static PyObject*
ui_slider_int3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    int tMin = 0;
    int tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOii|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &iFlags))
        return NULL;
    int atValues[4] = {0};
    if(!pl__ui_ints_from_py(ptValues, atValues, 3, "slider_int3"))
        return NULL;
    const bool bChanged = gptUI->slider_int3(pcLabel, atValues, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_ints_to_py(atValues, 3));
}

static PyObject*
ui_slider_int3_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    int tMin = 0;
    int tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOiis|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    int atValues[4] = {0};
    if(!pl__ui_ints_from_py(ptValues, atValues, 3, "slider_int3_f"))
        return NULL;
    const bool bChanged = gptUI->slider_int3_f(pcLabel, atValues, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_ints_to_py(atValues, 3));
}

static PyObject*
ui_slider_int4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    int tMin = 0;
    int tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOii|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &iFlags))
        return NULL;
    int atValues[4] = {0};
    if(!pl__ui_ints_from_py(ptValues, atValues, 4, "slider_int4"))
        return NULL;
    const bool bChanged = gptUI->slider_int4(pcLabel, atValues, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_ints_to_py(atValues, 4));
}

static PyObject*
ui_slider_int4_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    int tMin = 0;
    int tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOiis|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    int atValues[4] = {0};
    if(!pl__ui_ints_from_py(ptValues, atValues, 4, "slider_int4_f"))
        return NULL;
    const bool bChanged = gptUI->slider_int4_f(pcLabel, atValues, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_ints_to_py(atValues, 4));
}

static PyObject*
ui_slider_uint(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    uint32_t tValue = 0;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sIII|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &tMin, &tMax, &iFlags))
        return NULL;
    const bool bChanged = gptUI->slider_uint(pcLabel, &tValue, tMin, tMax, iFlags);
    return Py_BuildValue("(NI)", PyBool_FromLong(bChanged), (unsigned int)tValue);
}

static PyObject*
ui_slider_uint_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    uint32_t tValue = 0;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sIIIs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    const bool bChanged = gptUI->slider_uint_f(pcLabel, &tValue, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NI)", PyBool_FromLong(bChanged), (unsigned int)tValue);
}

static PyObject*
ui_slider_uint2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOII|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &iFlags))
        return NULL;
    uint32_t atValues[4] = {0};
    if(!pl__ui_uints_from_py(ptValues, atValues, 2, "slider_uint2"))
        return NULL;
    const bool bChanged = gptUI->slider_uint2(pcLabel, atValues, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_uints_to_py(atValues, 2));
}

static PyObject*
ui_slider_uint2_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOIIs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    uint32_t atValues[4] = {0};
    if(!pl__ui_uints_from_py(ptValues, atValues, 2, "slider_uint2_f"))
        return NULL;
    const bool bChanged = gptUI->slider_uint2_f(pcLabel, atValues, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_uints_to_py(atValues, 2));
}

static PyObject*
ui_slider_uint3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOII|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &iFlags))
        return NULL;
    uint32_t atValues[4] = {0};
    if(!pl__ui_uints_from_py(ptValues, atValues, 3, "slider_uint3"))
        return NULL;
    const bool bChanged = gptUI->slider_uint3(pcLabel, atValues, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_uints_to_py(atValues, 3));
}

static PyObject*
ui_slider_uint3_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOIIs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    uint32_t atValues[4] = {0};
    if(!pl__ui_uints_from_py(ptValues, atValues, 3, "slider_uint3_f"))
        return NULL;
    const bool bChanged = gptUI->slider_uint3_f(pcLabel, atValues, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_uints_to_py(atValues, 3));
}

static PyObject*
ui_slider_uint4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOII|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &iFlags))
        return NULL;
    uint32_t atValues[4] = {0};
    if(!pl__ui_uints_from_py(ptValues, atValues, 4, "slider_uint4"))
        return NULL;
    const bool bChanged = gptUI->slider_uint4(pcLabel, atValues, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_uints_to_py(atValues, 4));
}

static PyObject*
ui_slider_uint4_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOIIs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    uint32_t atValues[4] = {0};
    if(!pl__ui_uints_from_py(ptValues, atValues, 4, "slider_uint4_f"))
        return NULL;
    const bool bChanged = gptUI->slider_uint4_f(pcLabel, atValues, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_uints_to_py(atValues, 4));
}

static PyObject*
ui_drag_float(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "speed", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    float tValue = 0;
    float fSpeed = 1.0f;
    float tMin = 0;
    float tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sffff|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &fSpeed, &tMin, &tMax, &iFlags))
        return NULL;
    const bool bChanged = gptUI->drag_float(pcLabel, &tValue, fSpeed, tMin, tMax, iFlags);
    return Py_BuildValue("(Nd)", PyBool_FromLong(bChanged), (double)tValue);
}

static PyObject*
ui_drag_float_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "speed", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    float tValue = 0;
    float fSpeed = 1.0f;
    float tMin = 0;
    float tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sffffs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &fSpeed, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    const bool bChanged = gptUI->drag_float_f(pcLabel, &tValue, fSpeed, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(Nd)", PyBool_FromLong(bChanged), (double)tValue);
}

static PyObject*
ui_drag_float2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    float tMin = 0;
    float tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOfff|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &iFlags))
        return NULL;
    float atValues[4] = {0};
    if(!pl__ui_floats_from_py(ptValues, atValues, 2, 2, NULL, "drag_float2"))
        return NULL;
    const bool bChanged = gptUI->drag_float2(pcLabel, atValues, fSpeed, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(atValues, 2));
}

static PyObject*
ui_drag_float2_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    float tMin = 0;
    float tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOfffs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    float atValues[4] = {0};
    if(!pl__ui_floats_from_py(ptValues, atValues, 2, 2, NULL, "drag_float2_f"))
        return NULL;
    const bool bChanged = gptUI->drag_float2_f(pcLabel, atValues, fSpeed, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(atValues, 2));
}

static PyObject*
ui_drag_float3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    float tMin = 0;
    float tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOfff|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &iFlags))
        return NULL;
    float atValues[4] = {0};
    if(!pl__ui_floats_from_py(ptValues, atValues, 3, 3, NULL, "drag_float3"))
        return NULL;
    const bool bChanged = gptUI->drag_float3(pcLabel, atValues, fSpeed, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(atValues, 3));
}

static PyObject*
ui_drag_float3_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    float tMin = 0;
    float tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOfffs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    float atValues[4] = {0};
    if(!pl__ui_floats_from_py(ptValues, atValues, 3, 3, NULL, "drag_float3_f"))
        return NULL;
    const bool bChanged = gptUI->drag_float3_f(pcLabel, atValues, fSpeed, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(atValues, 3));
}

static PyObject*
ui_drag_float4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    float tMin = 0;
    float tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOfff|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &iFlags))
        return NULL;
    float atValues[4] = {0};
    if(!pl__ui_floats_from_py(ptValues, atValues, 4, 4, NULL, "drag_float4"))
        return NULL;
    const bool bChanged = gptUI->drag_float4(pcLabel, atValues, fSpeed, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(atValues, 4));
}

static PyObject*
ui_drag_float4_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    float tMin = 0;
    float tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOfffs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    float atValues[4] = {0};
    if(!pl__ui_floats_from_py(ptValues, atValues, 4, 4, NULL, "drag_float4_f"))
        return NULL;
    const bool bChanged = gptUI->drag_float4_f(pcLabel, atValues, fSpeed, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(atValues, 4));
}

static PyObject*
ui_drag_int(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "speed", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    int tValue = 0;
    float fSpeed = 1.0f;
    int tMin = 0;
    int tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sifii|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &fSpeed, &tMin, &tMax, &iFlags))
        return NULL;
    const bool bChanged = gptUI->drag_int(pcLabel, &tValue, fSpeed, tMin, tMax, iFlags);
    return Py_BuildValue("(Ni)", PyBool_FromLong(bChanged), tValue);
}

static PyObject*
ui_drag_int_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "speed", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    int tValue = 0;
    float fSpeed = 1.0f;
    int tMin = 0;
    int tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sifiis|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &fSpeed, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    const bool bChanged = gptUI->drag_int_f(pcLabel, &tValue, fSpeed, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(Ni)", PyBool_FromLong(bChanged), tValue);
}

static PyObject*
ui_drag_int2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    int tMin = 0;
    int tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOfii|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &iFlags))
        return NULL;
    int atValues[4] = {0};
    if(!pl__ui_ints_from_py(ptValues, atValues, 2, "drag_int2"))
        return NULL;
    const bool bChanged = gptUI->drag_int2(pcLabel, atValues, fSpeed, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_ints_to_py(atValues, 2));
}

static PyObject*
ui_drag_int2_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    int tMin = 0;
    int tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOfiis|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    int atValues[4] = {0};
    if(!pl__ui_ints_from_py(ptValues, atValues, 2, "drag_int2_f"))
        return NULL;
    const bool bChanged = gptUI->drag_int2_f(pcLabel, atValues, fSpeed, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_ints_to_py(atValues, 2));
}

static PyObject*
ui_drag_int3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    int tMin = 0;
    int tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOfii|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &iFlags))
        return NULL;
    int atValues[4] = {0};
    if(!pl__ui_ints_from_py(ptValues, atValues, 3, "drag_int3"))
        return NULL;
    const bool bChanged = gptUI->drag_int3(pcLabel, atValues, fSpeed, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_ints_to_py(atValues, 3));
}

static PyObject*
ui_drag_int3_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    int tMin = 0;
    int tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOfiis|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    int atValues[4] = {0};
    if(!pl__ui_ints_from_py(ptValues, atValues, 3, "drag_int3_f"))
        return NULL;
    const bool bChanged = gptUI->drag_int3_f(pcLabel, atValues, fSpeed, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_ints_to_py(atValues, 3));
}

static PyObject*
ui_drag_int4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    int tMin = 0;
    int tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOfii|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &iFlags))
        return NULL;
    int atValues[4] = {0};
    if(!pl__ui_ints_from_py(ptValues, atValues, 4, "drag_int4"))
        return NULL;
    const bool bChanged = gptUI->drag_int4(pcLabel, atValues, fSpeed, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_ints_to_py(atValues, 4));
}

static PyObject*
ui_drag_int4_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    int tMin = 0;
    int tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOfiis|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    int atValues[4] = {0};
    if(!pl__ui_ints_from_py(ptValues, atValues, 4, "drag_int4_f"))
        return NULL;
    const bool bChanged = gptUI->drag_int4_f(pcLabel, atValues, fSpeed, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_ints_to_py(atValues, 4));
}

static PyObject*
ui_drag_uint(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "speed", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    uint32_t tValue = 0;
    float fSpeed = 1.0f;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sIfII|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &fSpeed, &tMin, &tMax, &iFlags))
        return NULL;
    const bool bChanged = gptUI->drag_uint(pcLabel, &tValue, fSpeed, tMin, tMax, iFlags);
    return Py_BuildValue("(NI)", PyBool_FromLong(bChanged), (unsigned int)tValue);
}

static PyObject*
ui_drag_uint_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "speed", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    uint32_t tValue = 0;
    float fSpeed = 1.0f;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sIfIIs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &fSpeed, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    const bool bChanged = gptUI->drag_uint_f(pcLabel, &tValue, fSpeed, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NI)", PyBool_FromLong(bChanged), (unsigned int)tValue);
}

static PyObject*
ui_drag_uint2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOfII|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &iFlags))
        return NULL;
    uint32_t atValues[4] = {0};
    if(!pl__ui_uints_from_py(ptValues, atValues, 2, "drag_uint2"))
        return NULL;
    const bool bChanged = gptUI->drag_uint2(pcLabel, atValues, fSpeed, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_uints_to_py(atValues, 2));
}

static PyObject*
ui_drag_uint2_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOfIIs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    uint32_t atValues[4] = {0};
    if(!pl__ui_uints_from_py(ptValues, atValues, 2, "drag_uint2_f"))
        return NULL;
    const bool bChanged = gptUI->drag_uint2_f(pcLabel, atValues, fSpeed, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_uints_to_py(atValues, 2));
}

static PyObject*
ui_drag_uint3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOfII|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &iFlags))
        return NULL;
    uint32_t atValues[4] = {0};
    if(!pl__ui_uints_from_py(ptValues, atValues, 3, "drag_uint3"))
        return NULL;
    const bool bChanged = gptUI->drag_uint3(pcLabel, atValues, fSpeed, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_uints_to_py(atValues, 3));
}

static PyObject*
ui_drag_uint3_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOfIIs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    uint32_t atValues[4] = {0};
    if(!pl__ui_uints_from_py(ptValues, atValues, 3, "drag_uint3_f"))
        return NULL;
    const bool bChanged = gptUI->drag_uint3_f(pcLabel, atValues, fSpeed, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_uints_to_py(atValues, 3));
}

static PyObject*
ui_drag_uint4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    int iFlags = 0;
    if(!pl_parse_args("sOfII|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &iFlags))
        return NULL;
    uint32_t atValues[4] = {0};
    if(!pl__ui_uints_from_py(ptValues, atValues, 4, "drag_uint4"))
        return NULL;
    const bool bChanged = gptUI->drag_uint4(pcLabel, atValues, fSpeed, tMin, tMax, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_uints_to_py(atValues, 4));
}

static PyObject*
ui_drag_uint4_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "speed", "minValue", "maxValue", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    float fSpeed = 1.0f;
    uint32_t tMin = 0;
    uint32_t tMax = 0;
    const char* pcFmt = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sOfIIs|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &fSpeed, &tMin, &tMax, &pcFmt, &iFlags))
        return NULL;
    uint32_t atValues[4] = {0};
    if(!pl__ui_uints_from_py(ptValues, atValues, 4, "drag_uint4_f"))
        return NULL;
    const bool bChanged = gptUI->drag_uint4_f(pcLabel, atValues, fSpeed, tMin, tMax, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_uints_to_py(atValues, 4));
}

static PyObject*
ui_slider_angle(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "v_rad", "v_degrees_min", "v_degrees_max", "format", "flags", NULL};
    const char* pcLabel = NULL;
    float fRad = 0.0f;
    float fMin = -360.0f;
    float fMax = 360.0f;
    const char* pcFormat = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sf|ffzi", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &fRad, &fMin, &fMax, &pcFormat, &iFlags))
        return NULL;
    const bool bChanged = gptUI->slider_angle(pcLabel, &fRad, fMin, fMax, pcFormat, iFlags);
    return Py_BuildValue("(Nd)", PyBool_FromLong(bChanged), (double)fRad);
}

//-----------------------------------------------------------------------------
// [SECTION] inputs (generated)
//-----------------------------------------------------------------------------

static PyObject*
ui_input_float(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    float fValue = 0.0f;
    const char* pcFmt = "%.3f";
    int iFlags = 0;
    if(!pl_parse_args("sf|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &fValue, &pcFmt, &iFlags))
        return NULL;
    const bool bChanged = gptUI->input_float(pcLabel, &fValue, pcFmt, iFlags);
    return Py_BuildValue("(Nd)", PyBool_FromLong(bChanged), (double)fValue);
}

static PyObject*
ui_input_float_ex(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "step", "stepFast", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    float fValue = 0.0f;
    float fStep = 0.0f;
    float fStepFast = 0.0f;
    const char* pcFmt = "%.3f";
    int iFlags = 0;
    if(!pl_parse_args("sfff|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &fValue, &fStep, &fStepFast, &pcFmt, &iFlags))
        return NULL;
    const bool bChanged = gptUI->input_float_ex(pcLabel, &fValue, fStep, fStepFast, pcFmt, iFlags);
    return Py_BuildValue("(Nd)", PyBool_FromLong(bChanged), (double)fValue);
}

static PyObject*
ui_input_float2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    const char* pcFmt = "%.3f";
    int iFlags = 0;
    if(!pl_parse_args("sO|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &pcFmt, &iFlags))
        return NULL;
    float afValues[4] = {0};
    if(!pl__ui_floats_from_py(ptValues, afValues, 2, 2, NULL, "input_float2"))
        return NULL;
    const bool bChanged = gptUI->input_float2(pcLabel, afValues, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(afValues, 2));
}

static PyObject*
ui_input_float3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    const char* pcFmt = "%.3f";
    int iFlags = 0;
    if(!pl_parse_args("sO|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &pcFmt, &iFlags))
        return NULL;
    float afValues[4] = {0};
    if(!pl__ui_floats_from_py(ptValues, afValues, 3, 3, NULL, "input_float3"))
        return NULL;
    const bool bChanged = gptUI->input_float3(pcLabel, afValues, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(afValues, 3));
}

static PyObject*
ui_input_float4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "fmt", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    const char* pcFmt = "%.3f";
    int iFlags = 0;
    if(!pl_parse_args("sO|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &pcFmt, &iFlags))
        return NULL;
    float afValues[4] = {0};
    if(!pl__ui_floats_from_py(ptValues, afValues, 4, 4, NULL, "input_float4"))
        return NULL;
    const bool bChanged = gptUI->input_float4(pcLabel, afValues, pcFmt, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(afValues, 4));
}

static PyObject*
ui_input_int(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "flags", NULL};
    const char* pcLabel = NULL;
    int tValue = 0;
    int iFlags = 0;
    if(!pl_parse_args("si|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &iFlags))
        return NULL;
    const bool bChanged = gptUI->input_int(pcLabel, &tValue, iFlags);
    return Py_BuildValue("(Ni)", PyBool_FromLong(bChanged), tValue);
}

static PyObject*
ui_input_int_ex(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "step", "stepFast", "flags", NULL};
    const char* pcLabel = NULL;
    int tValue = 0;
    int tStep = 0;
    int tStepFast = 0;
    int iFlags = 0;
    if(!pl_parse_args("siii|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &tStep, &tStepFast, &iFlags))
        return NULL;
    const bool bChanged = gptUI->input_int_ex(pcLabel, &tValue, tStep, tStepFast, iFlags);
    return Py_BuildValue("(Ni)", PyBool_FromLong(bChanged), tValue);
}

static PyObject*
ui_input_int2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &iFlags))
        return NULL;
    int atValues[4] = {0};
    if(!pl__ui_ints_from_py(ptValues, atValues, 2, "input_int2"))
        return NULL;
    const bool bChanged = gptUI->input_int2(pcLabel, atValues, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_ints_to_py(atValues, 2));
}

static PyObject*
ui_input_int3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &iFlags))
        return NULL;
    int atValues[4] = {0};
    if(!pl__ui_ints_from_py(ptValues, atValues, 3, "input_int3"))
        return NULL;
    const bool bChanged = gptUI->input_int3(pcLabel, atValues, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_ints_to_py(atValues, 3));
}

static PyObject*
ui_input_int4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &iFlags))
        return NULL;
    int atValues[4] = {0};
    if(!pl__ui_ints_from_py(ptValues, atValues, 4, "input_int4"))
        return NULL;
    const bool bChanged = gptUI->input_int4(pcLabel, atValues, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_ints_to_py(atValues, 4));
}

static PyObject*
ui_input_uint(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "flags", NULL};
    const char* pcLabel = NULL;
    uint32_t tValue = 0;
    int iFlags = 0;
    if(!pl_parse_args("sI|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &iFlags))
        return NULL;
    const bool bChanged = gptUI->input_uint(pcLabel, &tValue, iFlags);
    return Py_BuildValue("(NI)", PyBool_FromLong(bChanged), (unsigned int)tValue);
}

static PyObject*
ui_input_uint_ex(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "step", "stepFast", "flags", NULL};
    const char* pcLabel = NULL;
    uint32_t tValue = 0;
    uint32_t tStep = 0;
    uint32_t tStepFast = 0;
    int iFlags = 0;
    if(!pl_parse_args("sIII|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &tValue, &tStep, &tStepFast, &iFlags))
        return NULL;
    const bool bChanged = gptUI->input_uint_ex(pcLabel, &tValue, tStep, tStepFast, iFlags);
    return Py_BuildValue("(NI)", PyBool_FromLong(bChanged), (unsigned int)tValue);
}

static PyObject*
ui_input_uint2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &iFlags))
        return NULL;
    uint32_t atValues[4] = {0};
    if(!pl__ui_uints_from_py(ptValues, atValues, 2, "input_uint2"))
        return NULL;
    const bool bChanged = gptUI->input_uint2(pcLabel, atValues, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_uints_to_py(atValues, 2));
}

static PyObject*
ui_input_uint3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &iFlags))
        return NULL;
    uint32_t atValues[4] = {0};
    if(!pl__ui_uints_from_py(ptValues, atValues, 3, "input_uint3"))
        return NULL;
    const bool bChanged = gptUI->input_uint3(pcLabel, atValues, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_uints_to_py(atValues, 3));
}

static PyObject*
ui_input_uint4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "values", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptValues = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptValues, &iFlags))
        return NULL;
    uint32_t atValues[4] = {0};
    if(!pl__ui_uints_from_py(ptValues, atValues, 4, "input_uint4"))
        return NULL;
    const bool bChanged = gptUI->input_uint4(pcLabel, atValues, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_uints_to_py(atValues, 4));
}

//-----------------------------------------------------------------------------
// [SECTION] colors
//-----------------------------------------------------------------------------

static PyObject*
ui_color_picker3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "colRGB", "flags", NULL};
    const char* pcLabel = NULL;
    PyObject* ptColor = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptColor, &iFlags))
        return NULL;
    float afColor[4] = {0};
    Py_ssize_t szCount = 0;
    if(!pl__ui_floats_from_py(ptColor, afColor, 3, 3, &szCount, "color_picker3"))
        return NULL;
    const bool bChanged = gptUI->color_picker3(pcLabel, afColor, iFlags);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(afColor, szCount));
}

static PyObject*
ui_color_picker4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    // refColRGBA: optional reference color (4 values)
    static const char* apcKeywords[] = {"label", "colRGBA", "flags", "refColRGBA", NULL};
    const char* pcLabel = NULL;
    PyObject* ptColor = NULL;
    int iFlags = 0;
    PyObject* ptRef = Py_None;
    if(!pl_parse_args("sO|iO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptColor, &iFlags, &ptRef))
        return NULL;
    float afColor[4] = {0};
    Py_ssize_t szCount = 0;
    if(!pl__ui_floats_from_py(ptColor, afColor, 4, 4, &szCount, "color_picker4"))
        return NULL;
    float afRef[4] = {0};
    const float* pfRef = NULL;
    if(ptRef != Py_None)
    {
        if(!pl__ui_floats_from_py(ptRef, afRef, 4, 4, NULL, "refColRGBA"))
            return NULL;
        pfRef = afRef;
    }
    const bool bChanged = gptUI->color_picker4(pcLabel, afColor, iFlags, pfRef);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(afColor, szCount));
}

static PyObject*
ui_color_edit3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    // color_edit3 also accepts 4 values (alpha is passed through)
    static const char* apcKeywords[] = {"label", "colRGBA", "flags", "refColRGBA", NULL};
    const char* pcLabel = NULL;
    PyObject* ptColor = NULL;
    int iFlags = 0;
    PyObject* ptRef = Py_None;
    if(!pl_parse_args("sO|iO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptColor, &iFlags, &ptRef))
        return NULL;
    float afColor[4] = {0};
    Py_ssize_t szCount = 0;
    if(!pl__ui_floats_from_py(ptColor, afColor, 3, 4, &szCount, "color_edit3"))
        return NULL;
    float afRef[4] = {0};
    const float* pfRef = NULL;
    if(ptRef != Py_None)
    {
        if(!pl__ui_floats_from_py(ptRef, afRef, 4, 4, NULL, "refColRGBA"))
            return NULL;
        pfRef = afRef;
    }
    const bool bChanged = gptUI->color_edit3(pcLabel, afColor, iFlags, pfRef);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(afColor, szCount));
}

static PyObject*
ui_color_edit4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    // refColRGBA: optional reference color (4 values)
    static const char* apcKeywords[] = {"label", "colRGBA", "flags", "refColRGBA", NULL};
    const char* pcLabel = NULL;
    PyObject* ptColor = NULL;
    int iFlags = 0;
    PyObject* ptRef = Py_None;
    if(!pl_parse_args("sO|iO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &ptColor, &iFlags, &ptRef))
        return NULL;
    float afColor[4] = {0};
    Py_ssize_t szCount = 0;
    if(!pl__ui_floats_from_py(ptColor, afColor, 4, 4, &szCount, "color_edit4"))
        return NULL;
    float afRef[4] = {0};
    const float* pfRef = NULL;
    if(ptRef != Py_None)
    {
        if(!pl__ui_floats_from_py(ptRef, afRef, 4, 4, NULL, "refColRGBA"))
            return NULL;
        pfRef = afRef;
    }
    const bool bChanged = gptUI->color_edit4(pcLabel, afColor, iFlags, pfRef);
    return Py_BuildValue("(NN)", PyBool_FromLong(bChanged), pl__ui_floats_to_py(afColor, szCount));
}

static PyObject*
ui_color_button(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"id", "colRGBA", "flags", "size", NULL};
    const char* pcId = NULL;
    PyObject* ptColor = NULL;
    int iFlags = 0;
    PyObject* ptSize = NULL;
    if(!pl_parse_args("sO|iO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcId, &ptColor, &iFlags, &ptSize))
        return NULL;
    float afColor[4] = {0};
    Py_ssize_t szCount = 0;
    if(!pl__ui_floats_from_py(ptColor, afColor, 3, 4, &szCount, "color_button"))
        return NULL;
    if(szCount == 3)
        afColor[3] = 1.0f;
    plVec2 tSize = {0};
    if(!pl__ui_vec2_arg(ptSize, tSize, &tSize))
        return NULL;
    return PyBool_FromLong(gptUI->color_button(pcId, afColor, iFlags, tSize));
}

//-----------------------------------------------------------------------------
// [SECTION] combo, trees, tabs & menus
//-----------------------------------------------------------------------------

static PyObject*
ui_begin_combo(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "preview", "flags", NULL};
    const char* pcLabel = NULL;
    const char* pcPreview = NULL;
    int iFlags = 0;
    if(!pl_parse_args("sz|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &pcPreview, &iFlags))
        return NULL;
    return PyBool_FromLong(gptUI->begin_combo(pcLabel, pcPreview, iFlags));
}

static PyObject*
ui_end_combo(PyObject* self, PyObject* unused)
{
    gptUI->end_combo();
    Py_RETURN_NONE;
}

static PyObject*
ui_collapsing_header(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"text", "flags", NULL};
    const char* pcText = NULL;
    int iFlags = 0;
    if(!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText, &iFlags))
        return NULL;
    return PyBool_FromLong(gptUI->collapsing_header(pcText, iFlags));
}

static PyObject*
ui_tree_node(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"text", "flags", NULL};
    const char* pcText = NULL;
    int iFlags = 0;
    if(!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText, &iFlags))
        return NULL;
    return PyBool_FromLong(gptUI->tree_node(pcText, iFlags));
}

static PyObject*
ui_begin_tab_bar(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"text", "flags", NULL};
    const char* pcText = NULL;
    int iFlags = 0;
    if(!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText, &iFlags))
        return NULL;
    return PyBool_FromLong(gptUI->begin_tab_bar(pcText, iFlags));
}

static PyObject*
ui_begin_tab(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"text", "flags", NULL};
    const char* pcText = NULL;
    int iFlags = 0;
    if(!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText, &iFlags))
        return NULL;
    return PyBool_FromLong(gptUI->begin_tab(pcText, iFlags));
}

static PyObject*
ui_tree_node_f(PyObject* self, PyObject* args, PyObject* kwargs)
{
    // python formats the text; same as tree_node
    static const char* apcKeywords[] = {"fmt", "flags", NULL};
    const char* pcText = NULL;
    int iFlags = 0;
    if(!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText, &iFlags))
        return NULL;
    return PyBool_FromLong(gptUI->tree_node_f("%s", iFlags, pcText));
}

static PyObject*
ui_tree_pop(PyObject* self, PyObject* unused)
{
    gptUI->tree_pop();
    Py_RETURN_NONE;
}

static PyObject*
ui_end_tab_bar(PyObject* self, PyObject* unused)
{
    gptUI->end_tab_bar();
    Py_RETURN_NONE;
}

static PyObject*
ui_end_tab(PyObject* self, PyObject* unused)
{
    gptUI->end_tab();
    Py_RETURN_NONE;
}

static PyObject*
ui_begin_menu(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "enabled", NULL};
    const char* pcLabel = NULL;
    int iEnabled = 1;
    if(!pl_parse_args("s|p", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &iEnabled))
        return NULL;
    return PyBool_FromLong(gptUI->begin_menu(pcLabel, iEnabled != 0));
}

static PyObject*
ui_end_menu(PyObject* self, PyObject* unused)
{
    gptUI->end_menu();
    Py_RETURN_NONE;
}

static PyObject*
ui_menu_item(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "shortcut", "selected", "enabled", NULL};
    const char* pcLabel = NULL;
    const char* pcShortcut = NULL;
    int iSelected = 0;
    int iEnabled = 1;
    if(!pl_parse_args("s|zpp", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &pcShortcut, &iSelected, &iEnabled))
        return NULL;
    return PyBool_FromLong(gptUI->menu_item(pcLabel, pcShortcut, iSelected != 0, iEnabled != 0));
}

static PyObject*
ui_menu_item_toggle(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "shortcut", "selected", "enabled", NULL};
    const char* pcLabel = NULL;
    const char* pcShortcut = NULL;
    int iSelected = 0;
    int iEnabled = 1;
    if(!pl_parse_args("s|zpp", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel, &pcShortcut, &iSelected, &iEnabled))
        return NULL;
    bool bSelected = iSelected != 0;
    const bool bPressed = gptUI->menu_item_toggle(pcLabel, pcShortcut, &bSelected, iEnabled != 0);
    return pl__ui_changed_and_bool(bPressed, &bSelected);
}

static PyObject*
ui_separator_text(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"text", NULL};
    const char* pcText = NULL;
    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText))
        return NULL;
    gptUI->separator_text(pcText);
    Py_RETURN_NONE;
}

static PyObject*
ui_separator(PyObject* self, PyObject* unused)
{
    gptUI->separator();
    Py_RETURN_NONE;
}

static PyObject*
ui_vertical_spacing(PyObject* self, PyObject* unused)
{
    gptUI->vertical_spacing();
    Py_RETURN_NONE;
}

static PyObject*
ui_indent(PyObject* self, PyObject* args, PyObject* kwargs)
{
    // 0.0 uses the style's indent size
    static const char* apcKeywords[] = {"amount", NULL};
    float fAmount = 0.0f;
    if(!pl_parse_args("|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fAmount))
        return NULL;
    gptUI->indent(fAmount);
    Py_RETURN_NONE;
}

static PyObject*
ui_unindent(PyObject* self, PyObject* args, PyObject* kwargs)
{
    // 0.0 uses the style's indent size
    static const char* apcKeywords[] = {"amount", NULL};
    float fAmount = 0.0f;
    if(!pl_parse_args("|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fAmount))
        return NULL;
    gptUI->unindent(fAmount);
    Py_RETURN_NONE;
}

static PyObject*
ui_end_menu_bar(PyObject* self, PyObject* unused)
{
    gptUI->end_menu_bar();
    Py_RETURN_NONE;
}

static PyObject*
ui_begin_menu_bar(PyObject* self, PyObject* unused)
{
    return PyBool_FromLong(gptUI->begin_menu_bar());
}

static PyObject*
ui_begin_main_menu_bar(PyObject* self, PyObject* unused)
{
    return PyBool_FromLong(gptUI->begin_main_menu_bar());
}

static PyObject*
ui_end_main_menu_bar(PyObject* self, PyObject* unused)
{
    gptUI->end_main_menu_bar();
    Py_RETURN_NONE;
}

//-----------------------------------------------------------------------------
// [SECTION] text filter & clipper
//-----------------------------------------------------------------------------

static PyObject*
ui_text_filter_cleanup(PyObject* self, PyObject* arg)
{
    plUiTextFilter* ptFilter = pl__ui_text_filter_from_py(arg);
    if(ptFilter == NULL)
        return NULL;
    gptUI->text_filter_cleanup(ptFilter);
    Py_RETURN_NONE;
}

static PyObject*
ui_text_filter_build(PyObject* self, PyObject* arg)
{
    plUiTextFilter* ptFilter = pl__ui_text_filter_from_py(arg);
    if(ptFilter == NULL)
        return NULL;
    gptUI->text_filter_build(ptFilter);
    Py_RETURN_NONE;
}

static PyObject*
ui_text_filter_active(PyObject* self, PyObject* arg)
{
    plUiTextFilter* ptFilter = pl__ui_text_filter_from_py(arg);
    if(ptFilter == NULL)
        return NULL;
    return PyBool_FromLong(gptUI->text_filter_active(ptFilter));
}

static PyObject*
ui_text_filter_pass(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"filter", "text", NULL};
    PyObject* ptFilterObj = NULL;
    const char* pcText = NULL;
    Py_ssize_t szLength = 0;
    if(!pl_parse_args("Os#", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptFilterObj, &pcText, &szLength))
        return NULL;
    plUiTextFilter* ptFilter = pl__ui_text_filter_from_py(ptFilterObj);
    if(ptFilter == NULL)
        return NULL;
    return PyBool_FromLong(gptUI->text_filter_pass(ptFilter, pcText, pcText + szLength));
}

static PyObject*
ui_step_clipper(PyObject* self, PyObject* arg)
{
    plUiClipper* ptClipper = pl__ui_clipper_from_py(arg);
    if(ptClipper == NULL)
        return NULL;
    return PyBool_FromLong(gptUI->step_clipper(ptClipper));
}

//-----------------------------------------------------------------------------
// [SECTION] layout systems
//-----------------------------------------------------------------------------

static PyObject*
ui_layout_dynamic(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"height", "widgetCount", NULL};
    float fHeight = 0.0f;
    unsigned int uCount = 1;
    if(!pl_parse_args("fI", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fHeight, &uCount))
        return NULL;
    gptUI->layout_dynamic(fHeight, uCount);
    Py_RETURN_NONE;
}

static PyObject*
ui_layout_static(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"height", "width", "widgetCount", NULL};
    float fHeight = 0.0f;
    float fWidth = 0.0f;
    unsigned int uCount = 1;
    if(!pl_parse_args("ffI", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fHeight, &fWidth, &uCount))
        return NULL;
    gptUI->layout_static(fHeight, fWidth, uCount);
    Py_RETURN_NONE;
}

static PyObject*
ui_layout_row_begin(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"type", "height", "widgetCount", NULL};
    int iType = 0;
    float fHeight = 0.0f;
    unsigned int uCount = 1;
    if(!pl_parse_args("ifI", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iType, &fHeight, &uCount))
        return NULL;
    gptUI->layout_row_begin(iType, fHeight, uCount);
    Py_RETURN_NONE;
}

static PyObject*
ui_layout_row_push(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"width", NULL};
    float fWidth = 0.0f;
    if(!pl_parse_args("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fWidth))
        return NULL;
    gptUI->layout_row_push(fWidth);
    Py_RETURN_NONE;
}

static PyObject*
ui_layout_row_end(PyObject* self, PyObject* unused)
{
    gptUI->layout_row_end();
    Py_RETURN_NONE;
}

static PyObject*
ui_layout_row(PyObject* self, PyObject* args, PyObject* kwargs)
{
    // sizesOrRatios must hold at least widgetCount values
    static const char* apcKeywords[] = {"type", "height", "widgetCount", "sizesOrRatios", NULL};
    int iType = 0;
    float fHeight = 0.0f;
    unsigned int uCount = 0;
    PyObject* ptSizes = NULL;
    if(!pl_parse_args("ifIO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iType, &fHeight, &uCount, &ptSizes))
        return NULL;
    PyObject* ptSeq = PySequence_Fast(ptSizes, "layout_row(): sizesOrRatios must be a sequence of floats");
    if(ptSeq == NULL)
        return NULL;
    const Py_ssize_t szCount = PySequence_Fast_GET_SIZE(ptSeq);
    if(uCount == 0 || szCount < (Py_ssize_t)uCount)
    {
        Py_DECREF(ptSeq);
        PyErr_Format(PyExc_ValueError, "layout_row(): expected at least %u sizes, got %zd", uCount, szCount);
        return NULL;
    }
    float* afSizes = PyMem_Malloc(sizeof(float) * uCount); // copied by the UI
    if(afSizes == NULL)
    {
        Py_DECREF(ptSeq);
        return PyErr_NoMemory();
    }
    for(uint32_t i = 0; i < uCount; i++)
        afSizes[i] = (float)PyFloat_AsDouble(PySequence_Fast_GET_ITEM(ptSeq, i));
    Py_DECREF(ptSeq);
    if(!PyErr_Occurred())
        gptUI->layout_row(iType, fHeight, uCount, afSizes);
    PyMem_Free(afSizes);
    if(PyErr_Occurred())
        return NULL;
    Py_RETURN_NONE;
}

static PyObject*
ui_layout_template_begin(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"height", NULL};
    float fHeight = 0.0f;
    if(!pl_parse_args("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fHeight))
        return NULL;
    gptUI->layout_template_begin(fHeight);
    Py_RETURN_NONE;
}

static PyObject*
ui_layout_template_push_dynamic(PyObject* self, PyObject* unused)
{
    gptUI->layout_template_push_dynamic();
    Py_RETURN_NONE;
}

static PyObject*
ui_layout_template_push_variable(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"width", NULL};
    float fWidth = 0.0f;
    if(!pl_parse_args("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fWidth))
        return NULL;
    gptUI->layout_template_push_variable(fWidth);
    Py_RETURN_NONE;
}

static PyObject*
ui_layout_template_push_static(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"width", NULL};
    float fWidth = 0.0f;
    if(!pl_parse_args("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fWidth))
        return NULL;
    gptUI->layout_template_push_static(fWidth);
    Py_RETURN_NONE;
}

static PyObject*
ui_layout_template_end(PyObject* self, PyObject* unused)
{
    gptUI->layout_template_end();
    Py_RETURN_NONE;
}

static PyObject*
ui_layout_space_begin(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"type", "height", "widgetCount", NULL};
    int iType = 0;
    float fHeight = 0.0f;
    unsigned int uCount = UINT32_MAX;
    if(!pl_parse_args("if|I", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iType, &fHeight, &uCount))
        return NULL;
    gptUI->layout_space_begin(iType, fHeight, uCount);
    Py_RETURN_NONE;
}

static PyObject*
ui_layout_space_push(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"x", "y", "width", "height", NULL};
    float fX = 0.0f, fY = 0.0f, fWidth = 0.0f, fHeight = 0.0f;
    if(!pl_parse_args("ffff", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fX, &fY, &fWidth, &fHeight))
        return NULL;
    gptUI->layout_space_push(fX, fY, fWidth, fHeight);
    Py_RETURN_NONE;
}

static PyObject*
ui_layout_space_end(PyObject* self, PyObject* unused)
{
    gptUI->layout_space_end();
    Py_RETURN_NONE;
}

static PyObject*
ui_stash_layout(PyObject* self, PyObject* unused)
{
    gptUI->stash_layout();
    Py_RETURN_NONE;
}

static PyObject*
ui_apply_layout(PyObject* self, PyObject* unused)
{
    gptUI->apply_layout();
    Py_RETURN_NONE;
}

//-----------------------------------------------------------------------------
// [SECTION] id stack, state & docking
//-----------------------------------------------------------------------------

static PyObject*
ui_push_id_string(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"id", NULL};
    const char* pcId = NULL;
    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcId))
        return NULL;
    gptUI->push_id_string(pcId);
    Py_RETURN_NONE;
}

static PyObject*
ui_push_id_pointer(PyObject* self, PyObject* arg)
{
    // uses the python object's address (keep the object alive while it is used as an id)
    gptUI->push_id_pointer((const void*)arg);
    Py_RETURN_NONE;
}

static PyObject*
ui_push_id_uint(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"id", NULL};
    unsigned int uId = 0;
    if(!pl_parse_args("I", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uId))
        return NULL;
    gptUI->push_id_uint(uId);
    Py_RETURN_NONE;
}

static PyObject*
ui_pop_id(PyObject* self, PyObject* unused)
{
    gptUI->pop_id();
    Py_RETURN_NONE;
}

static PyObject*
ui_get_id(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"name", NULL};
    const char* pcName = NULL;
    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcName))
        return NULL;
    return PyLong_FromUnsignedLong((unsigned long)gptUI->get_id(pcName));
}

static PyObject*
ui_was_last_item_hovered(PyObject* self, PyObject* unused)
{
    return PyBool_FromLong(gptUI->was_last_item_hovered());
}

static PyObject*
ui_was_last_item_active(PyObject* self, PyObject* unused)
{
    return PyBool_FromLong(gptUI->was_last_item_active());
}

static PyObject*
ui_set_keyboard_focus_last_item(PyObject* self, PyObject* unused)
{
    gptUI->set_keyboard_focus_last_item();
    Py_RETURN_NONE;
}

static PyObject*
ui_dock_space(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"name", "pos", "size", NULL};
    const char* pcName = NULL;
    PyObject* ptPos = NULL;
    PyObject* ptSize = NULL;
    if(!pl_parse_args("sOO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcName, &ptPos, &ptSize))
        return NULL;
    plVec2 tPos = {0};
    plVec2 tSize = {0};
    if(!pl__ui_vec2_arg(ptPos, tPos, &tPos) || !pl__ui_vec2_arg(ptSize, tSize, &tSize))
        return NULL;
    return PyLong_FromUnsignedLong((unsigned long)gptUI->dock_space(pcName, tPos, tSize));
}

static PyObject*
ui_dock_split(PyObject* self, PyObject* args, PyObject* kwargs)
{
    // direction: 0=left, 1=right, 2=up, 3=down; returns (node, opposite)
    static const char* apcKeywords[] = {"node", "direction", "ratio", NULL};
    unsigned int uNode = 0;
    int iDirection = 0;
    float fRatio = 0.5f;
    if(!pl_parse_args("Iif", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uNode, &iDirection, &fRatio))
        return NULL;
    uint32_t uOpposite = 0;
    const uint32_t uResult = gptUI->dock_split(uNode, iDirection, fRatio, &uOpposite);
    return Py_BuildValue("(II)", (unsigned int)uResult, (unsigned int)uOpposite);
}

static PyObject*
ui_set_next_window_dock(PyObject* self, PyObject* args, PyObject* kwargs)
{
    // node=0 undocks
    static const char* apcKeywords[] = {"node", "condition", NULL};
    unsigned int uNode = 0;
    int iCondition = PL_UI_COND_ONCE;
    if(!pl_parse_args("I|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uNode, &iCondition))
        return NULL;
    gptUI->set_next_window_dock(uNode, iCondition);
    Py_RETURN_NONE;
}

static PyObject*
ui_get_window_dock(PyObject* self, PyObject* unused)
{
    return PyLong_FromUnsignedLong((unsigned long)gptUI->get_window_dock());
}

static PyObject*
ui_set_docking_enabled(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"enabled", NULL};
    int iEnabled = 1;
    if(!pl_parse_args("p", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iEnabled))
        return NULL;
    gptUI->set_docking_enabled(iEnabled != 0);
    Py_RETURN_NONE;
}

static PyObject*
ui_set_settings_file(PyObject* self, PyObject* args, PyObject* kwargs)
{
    // None disables persistence; set before the first frame
    static const char* apcKeywords[] = {"path", NULL};
    const char* pcPath = NULL;
    if(!pl_parse_args("z", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcPath))
        return NULL;
    gptUI->set_settings_file(pcPath);
    Py_RETURN_NONE;
}

static PyObject*
ui_save_settings(PyObject* self, PyObject* unused)
{
    return PyBool_FromLong(gptUI->save_settings());
}

//-----------------------------------------------------------------------------
// [SECTION] method table
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplUiI[] =
{
    {"initialize",                            (PyCFunction)ui_initialize,                           METH_NOARGS | METH_STATIC, NULL},
    {"cleanup",                               (PyCFunction)ui_cleanup,                              METH_NOARGS | METH_STATIC, NULL},
    {"get_draw_lists",                        (PyCFunction)ui_get_draw_lists,                       METH_NOARGS | METH_STATIC, NULL},
    {"get_debug_draw_list",                   (PyCFunction)ui_get_debug_draw_list,                  METH_NOARGS | METH_STATIC, NULL},
    {"get_debug_draw_layer",                  (PyCFunction)ui_get_debug_draw_layer,                 METH_NOARGS | METH_STATIC, NULL},
    {"begin_frame",                           (PyCFunction)ui_begin_frame,                          METH_NOARGS | METH_STATIC, NULL},
    {"end_frame",                             (PyCFunction)ui_end_frame,                            METH_NOARGS | METH_STATIC, NULL},
    {"wants_mouse_capture",                   (PyCFunction)ui_wants_mouse_capture,                  METH_NOARGS | METH_STATIC, NULL},
    {"wants_keyboard_capture",                (PyCFunction)ui_wants_keyboard_capture,               METH_NOARGS | METH_STATIC, NULL},
    {"wants_text_input",                      (PyCFunction)ui_wants_text_input,                     METH_NOARGS | METH_STATIC, NULL},
    {"set_wants_mouse_capture_next_frame",    (PyCFunction)ui_set_wants_mouse_capture_next_frame,   PL_UI_KW, NULL},
    {"set_wants_keyboard_capture_next_frame", (PyCFunction)ui_set_wants_keyboard_capture_next_frame, PL_UI_KW, NULL},
    {"show_debug_window",                     (PyCFunction)ui_show_debug_window,                    PL_UI_KW, NULL},
    {"show_style_editor_window",              (PyCFunction)ui_show_style_editor_window,             PL_UI_KW, NULL},
    {"set_light_theme",                       (PyCFunction)ui_set_light_theme,                      METH_NOARGS | METH_STATIC, NULL},
    {"set_dark_theme",                        (PyCFunction)ui_set_dark_theme,                       METH_NOARGS | METH_STATIC, NULL},
    {"push_theme_color",                      (PyCFunction)ui_push_theme_color,                     PL_UI_KW, NULL},
    {"pop_theme_color",                       (PyCFunction)ui_pop_theme_color,                      PL_UI_KW, NULL},
    {"set_default_font",                      (PyCFunction)ui_set_default_font,                     METH_O | METH_STATIC, NULL},
    {"get_default_font",                      (PyCFunction)ui_get_default_font,                     METH_NOARGS | METH_STATIC, NULL},
    {"begin_window",                          (PyCFunction)ui_begin_window,                         PL_UI_KW, NULL},
    {"end_window",                            (PyCFunction)ui_end_window,                           METH_NOARGS | METH_STATIC, NULL},
    {"open_popup",                            (PyCFunction)ui_open_popup,                           PL_UI_KW, NULL},
    {"open_popup_id",                         (PyCFunction)ui_open_popup_id,                        PL_UI_KW, NULL},
    {"close_current_popup",                   (PyCFunction)ui_close_current_popup,                  METH_NOARGS | METH_STATIC, NULL},
    {"is_popup_open",                         (PyCFunction)ui_is_popup_open,                        PL_UI_KW, NULL},
    {"is_popup_open_id",                      (PyCFunction)ui_is_popup_open_id,                     PL_UI_KW, NULL},
    {"begin_popup",                           (PyCFunction)ui_begin_popup,                          PL_UI_KW, NULL},
    {"begin_popup_id",                        (PyCFunction)ui_begin_popup_id,                       PL_UI_KW, NULL},
    {"end_popup",                             (PyCFunction)ui_end_popup,                            METH_NOARGS | METH_STATIC, NULL},
    {"get_window_fg_drawlayer",               (PyCFunction)ui_get_window_fg_drawlayer,              METH_NOARGS | METH_STATIC, NULL},
    {"get_window_bg_drawlayer",               (PyCFunction)ui_get_window_bg_drawlayer,              METH_NOARGS | METH_STATIC, NULL},
    {"get_cursor_pos",                        (PyCFunction)ui_get_cursor_pos,                       METH_NOARGS | METH_STATIC, NULL},
    {"begin_child",                           (PyCFunction)ui_begin_child,                          PL_UI_KW, NULL},
    {"end_child",                             (PyCFunction)ui_end_child,                            METH_NOARGS | METH_STATIC, NULL},
    {"begin_tooltip",                         (PyCFunction)ui_begin_tooltip,                        METH_NOARGS | METH_STATIC, NULL},
    {"end_tooltip",                           (PyCFunction)ui_end_tooltip,                          METH_NOARGS | METH_STATIC, NULL},
    {"is_window_appearing",                   (PyCFunction)ui_is_window_appearing,                  METH_NOARGS | METH_STATIC, NULL},
    {"is_window_hovered",                     (PyCFunction)ui_is_window_hovered,                    METH_NOARGS | METH_STATIC, NULL},
    {"get_window_pos",                        (PyCFunction)ui_get_window_pos,                       METH_NOARGS | METH_STATIC, NULL},
    {"get_window_size",                       (PyCFunction)ui_get_window_size,                      METH_NOARGS | METH_STATIC, NULL},
    {"get_window_scroll",                     (PyCFunction)ui_get_window_scroll,                    METH_NOARGS | METH_STATIC, NULL},
    {"get_window_scroll_max",                 (PyCFunction)ui_get_window_scroll_max,                METH_NOARGS | METH_STATIC, NULL},
    {"get_content_region_available",          (PyCFunction)ui_get_content_region_available,         METH_NOARGS | METH_STATIC, NULL},
    {"set_window_scroll",                     (PyCFunction)ui_set_window_scroll,                    PL_UI_KW, NULL},
    {"set_next_window_pos",                   (PyCFunction)ui_set_next_window_pos,                  PL_UI_KW, NULL},
    {"set_next_window_size",                  (PyCFunction)ui_set_next_window_size,                 PL_UI_KW, NULL},
    {"set_next_window_collapse",              (PyCFunction)ui_set_next_window_collapse,             PL_UI_KW, NULL},
    {"button",                                (PyCFunction)ui_button,                               PL_UI_KW, NULL},
    {"selectable",                            (PyCFunction)ui_selectable,                           PL_UI_KW, NULL},
    {"checkbox",                              (PyCFunction)ui_checkbox,                             PL_UI_KW, NULL},
    {"checkbox_flags",                        (PyCFunction)ui_checkbox_flags,                       PL_UI_KW, NULL},
    {"radio_button",                          (PyCFunction)ui_radio_button,                         PL_UI_KW, NULL},
    {"image",                                 (PyCFunction)ui_image,                                PL_UI_KW, NULL},
    {"image_ex",                              (PyCFunction)ui_image_ex,                             PL_UI_KW, NULL},
    {"image_button",                          (PyCFunction)ui_image_button,                         PL_UI_KW, NULL},
    {"image_button_ex",                       (PyCFunction)ui_image_button_ex,                      PL_UI_KW, NULL},
    {"invisible_button",                      (PyCFunction)ui_invisible_button,                     PL_UI_KW, NULL},
    {"dummy",                                 (PyCFunction)ui_dummy,                                PL_UI_KW, NULL},
    {"progress_bar",                          (PyCFunction)ui_progress_bar,                         PL_UI_KW, NULL},
    {"text",                                  (PyCFunction)ui_text,                                 PL_UI_KW, NULL},
    {"color_text",                            (PyCFunction)ui_color_text,                           PL_UI_KW, NULL},
    {"labeled_text",                          (PyCFunction)ui_labeled_text,                         PL_UI_KW, NULL},
    {"input_text",                            (PyCFunction)ui_input_text,                           PL_UI_KW, NULL},
    {"input_text_hint",                       (PyCFunction)ui_input_text_hint,                      PL_UI_KW, NULL},
    {"input_text_multiline",                  (PyCFunction)ui_input_text_multiline,                 PL_UI_KW, NULL},
    {"slider_float",                          (PyCFunction)ui_slider_float,                         PL_UI_KW, NULL},
    {"slider_float_f",                        (PyCFunction)ui_slider_float_f,                       PL_UI_KW, NULL},
    {"slider_float2",                         (PyCFunction)ui_slider_float2,                        PL_UI_KW, NULL},
    {"slider_float2_f",                       (PyCFunction)ui_slider_float2_f,                      PL_UI_KW, NULL},
    {"slider_float3",                         (PyCFunction)ui_slider_float3,                        PL_UI_KW, NULL},
    {"slider_float3_f",                       (PyCFunction)ui_slider_float3_f,                      PL_UI_KW, NULL},
    {"slider_float4",                         (PyCFunction)ui_slider_float4,                        PL_UI_KW, NULL},
    {"slider_float4_f",                       (PyCFunction)ui_slider_float4_f,                      PL_UI_KW, NULL},
    {"slider_int",                            (PyCFunction)ui_slider_int,                           PL_UI_KW, NULL},
    {"slider_int_f",                          (PyCFunction)ui_slider_int_f,                         PL_UI_KW, NULL},
    {"slider_int2",                           (PyCFunction)ui_slider_int2,                          PL_UI_KW, NULL},
    {"slider_int2_f",                         (PyCFunction)ui_slider_int2_f,                        PL_UI_KW, NULL},
    {"slider_int3",                           (PyCFunction)ui_slider_int3,                          PL_UI_KW, NULL},
    {"slider_int3_f",                         (PyCFunction)ui_slider_int3_f,                        PL_UI_KW, NULL},
    {"slider_int4",                           (PyCFunction)ui_slider_int4,                          PL_UI_KW, NULL},
    {"slider_int4_f",                         (PyCFunction)ui_slider_int4_f,                        PL_UI_KW, NULL},
    {"slider_uint",                           (PyCFunction)ui_slider_uint,                          PL_UI_KW, NULL},
    {"slider_uint_f",                         (PyCFunction)ui_slider_uint_f,                        PL_UI_KW, NULL},
    {"slider_uint2",                          (PyCFunction)ui_slider_uint2,                         PL_UI_KW, NULL},
    {"slider_uint2_f",                        (PyCFunction)ui_slider_uint2_f,                       PL_UI_KW, NULL},
    {"slider_uint3",                          (PyCFunction)ui_slider_uint3,                         PL_UI_KW, NULL},
    {"slider_uint3_f",                        (PyCFunction)ui_slider_uint3_f,                       PL_UI_KW, NULL},
    {"slider_uint4",                          (PyCFunction)ui_slider_uint4,                         PL_UI_KW, NULL},
    {"slider_uint4_f",                        (PyCFunction)ui_slider_uint4_f,                       PL_UI_KW, NULL},
    {"drag_float",                            (PyCFunction)ui_drag_float,                           PL_UI_KW, NULL},
    {"drag_float_f",                          (PyCFunction)ui_drag_float_f,                         PL_UI_KW, NULL},
    {"drag_float2",                           (PyCFunction)ui_drag_float2,                          PL_UI_KW, NULL},
    {"drag_float2_f",                         (PyCFunction)ui_drag_float2_f,                        PL_UI_KW, NULL},
    {"drag_float3",                           (PyCFunction)ui_drag_float3,                          PL_UI_KW, NULL},
    {"drag_float3_f",                         (PyCFunction)ui_drag_float3_f,                        PL_UI_KW, NULL},
    {"drag_float4",                           (PyCFunction)ui_drag_float4,                          PL_UI_KW, NULL},
    {"drag_float4_f",                         (PyCFunction)ui_drag_float4_f,                        PL_UI_KW, NULL},
    {"drag_int",                              (PyCFunction)ui_drag_int,                             PL_UI_KW, NULL},
    {"drag_int_f",                            (PyCFunction)ui_drag_int_f,                           PL_UI_KW, NULL},
    {"drag_int2",                             (PyCFunction)ui_drag_int2,                            PL_UI_KW, NULL},
    {"drag_int2_f",                           (PyCFunction)ui_drag_int2_f,                          PL_UI_KW, NULL},
    {"drag_int3",                             (PyCFunction)ui_drag_int3,                            PL_UI_KW, NULL},
    {"drag_int3_f",                           (PyCFunction)ui_drag_int3_f,                          PL_UI_KW, NULL},
    {"drag_int4",                             (PyCFunction)ui_drag_int4,                            PL_UI_KW, NULL},
    {"drag_int4_f",                           (PyCFunction)ui_drag_int4_f,                          PL_UI_KW, NULL},
    {"drag_uint",                             (PyCFunction)ui_drag_uint,                            PL_UI_KW, NULL},
    {"drag_uint_f",                           (PyCFunction)ui_drag_uint_f,                          PL_UI_KW, NULL},
    {"drag_uint2",                            (PyCFunction)ui_drag_uint2,                           PL_UI_KW, NULL},
    {"drag_uint2_f",                          (PyCFunction)ui_drag_uint2_f,                         PL_UI_KW, NULL},
    {"drag_uint3",                            (PyCFunction)ui_drag_uint3,                           PL_UI_KW, NULL},
    {"drag_uint3_f",                          (PyCFunction)ui_drag_uint3_f,                         PL_UI_KW, NULL},
    {"drag_uint4",                            (PyCFunction)ui_drag_uint4,                           PL_UI_KW, NULL},
    {"drag_uint4_f",                          (PyCFunction)ui_drag_uint4_f,                         PL_UI_KW, NULL},
    {"slider_angle",                          (PyCFunction)ui_slider_angle,                         PL_UI_KW, NULL},
    {"input_float",                           (PyCFunction)ui_input_float,                          PL_UI_KW, NULL},
    {"input_float_ex",                        (PyCFunction)ui_input_float_ex,                       PL_UI_KW, NULL},
    {"input_float2",                          (PyCFunction)ui_input_float2,                         PL_UI_KW, NULL},
    {"input_float3",                          (PyCFunction)ui_input_float3,                         PL_UI_KW, NULL},
    {"input_float4",                          (PyCFunction)ui_input_float4,                         PL_UI_KW, NULL},
    {"input_int",                             (PyCFunction)ui_input_int,                            PL_UI_KW, NULL},
    {"input_int_ex",                          (PyCFunction)ui_input_int_ex,                         PL_UI_KW, NULL},
    {"input_int2",                            (PyCFunction)ui_input_int2,                           PL_UI_KW, NULL},
    {"input_int3",                            (PyCFunction)ui_input_int3,                           PL_UI_KW, NULL},
    {"input_int4",                            (PyCFunction)ui_input_int4,                           PL_UI_KW, NULL},
    {"input_uint",                            (PyCFunction)ui_input_uint,                           PL_UI_KW, NULL},
    {"input_uint_ex",                         (PyCFunction)ui_input_uint_ex,                        PL_UI_KW, NULL},
    {"input_uint2",                           (PyCFunction)ui_input_uint2,                          PL_UI_KW, NULL},
    {"input_uint3",                           (PyCFunction)ui_input_uint3,                          PL_UI_KW, NULL},
    {"input_uint4",                           (PyCFunction)ui_input_uint4,                          PL_UI_KW, NULL},
    {"color_picker3",                         (PyCFunction)ui_color_picker3,                        PL_UI_KW, NULL},
    {"color_picker4",                         (PyCFunction)ui_color_picker4,                        PL_UI_KW, NULL},
    {"color_edit3",                           (PyCFunction)ui_color_edit3,                          PL_UI_KW, NULL},
    {"color_edit4",                           (PyCFunction)ui_color_edit4,                          PL_UI_KW, NULL},
    {"color_button",                          (PyCFunction)ui_color_button,                         PL_UI_KW, NULL},
    {"begin_combo",                           (PyCFunction)ui_begin_combo,                          PL_UI_KW, NULL},
    {"end_combo",                             (PyCFunction)ui_end_combo,                            METH_NOARGS | METH_STATIC, NULL},
    {"collapsing_header",                     (PyCFunction)ui_collapsing_header,                    PL_UI_KW, NULL},
    {"tree_node",                             (PyCFunction)ui_tree_node,                            PL_UI_KW, NULL},
    {"begin_tab_bar",                         (PyCFunction)ui_begin_tab_bar,                        PL_UI_KW, NULL},
    {"begin_tab",                             (PyCFunction)ui_begin_tab,                            PL_UI_KW, NULL},
    {"tree_node_f",                           (PyCFunction)ui_tree_node_f,                          PL_UI_KW, NULL},
    {"tree_pop",                              (PyCFunction)ui_tree_pop,                             METH_NOARGS | METH_STATIC, NULL},
    {"end_tab_bar",                           (PyCFunction)ui_end_tab_bar,                          METH_NOARGS | METH_STATIC, NULL},
    {"end_tab",                               (PyCFunction)ui_end_tab,                              METH_NOARGS | METH_STATIC, NULL},
    {"begin_menu",                            (PyCFunction)ui_begin_menu,                           PL_UI_KW, NULL},
    {"end_menu",                              (PyCFunction)ui_end_menu,                             METH_NOARGS | METH_STATIC, NULL},
    {"menu_item",                             (PyCFunction)ui_menu_item,                            PL_UI_KW, NULL},
    {"menu_item_toggle",                      (PyCFunction)ui_menu_item_toggle,                     PL_UI_KW, NULL},
    {"separator_text",                        (PyCFunction)ui_separator_text,                       PL_UI_KW, NULL},
    {"separator",                             (PyCFunction)ui_separator,                            METH_NOARGS | METH_STATIC, NULL},
    {"vertical_spacing",                      (PyCFunction)ui_vertical_spacing,                     METH_NOARGS | METH_STATIC, NULL},
    {"indent",                                (PyCFunction)ui_indent,                               PL_UI_KW, NULL},
    {"unindent",                              (PyCFunction)ui_unindent,                             PL_UI_KW, NULL},
    {"end_menu_bar",                          (PyCFunction)ui_end_menu_bar,                         METH_NOARGS | METH_STATIC, NULL},
    {"begin_menu_bar",                        (PyCFunction)ui_begin_menu_bar,                       METH_NOARGS | METH_STATIC, NULL},
    {"begin_main_menu_bar",                   (PyCFunction)ui_begin_main_menu_bar,                  METH_NOARGS | METH_STATIC, NULL},
    {"end_main_menu_bar",                     (PyCFunction)ui_end_main_menu_bar,                    METH_NOARGS | METH_STATIC, NULL},
    {"text_filter_cleanup",                   (PyCFunction)ui_text_filter_cleanup,                  METH_O | METH_STATIC, NULL},
    {"text_filter_build",                     (PyCFunction)ui_text_filter_build,                    METH_O | METH_STATIC, NULL},
    {"text_filter_active",                    (PyCFunction)ui_text_filter_active,                   METH_O | METH_STATIC, NULL},
    {"text_filter_pass",                      (PyCFunction)ui_text_filter_pass,                     PL_UI_KW, NULL},
    {"step_clipper",                          (PyCFunction)ui_step_clipper,                         METH_O | METH_STATIC, NULL},
    {"layout_dynamic",                        (PyCFunction)ui_layout_dynamic,                       PL_UI_KW, NULL},
    {"layout_static",                         (PyCFunction)ui_layout_static,                        PL_UI_KW, NULL},
    {"layout_row_begin",                      (PyCFunction)ui_layout_row_begin,                     PL_UI_KW, NULL},
    {"layout_row_push",                       (PyCFunction)ui_layout_row_push,                      PL_UI_KW, NULL},
    {"layout_row_end",                        (PyCFunction)ui_layout_row_end,                       METH_NOARGS | METH_STATIC, NULL},
    {"layout_row",                            (PyCFunction)ui_layout_row,                           PL_UI_KW, NULL},
    {"layout_template_begin",                 (PyCFunction)ui_layout_template_begin,                PL_UI_KW, NULL},
    {"layout_template_push_dynamic",          (PyCFunction)ui_layout_template_push_dynamic,         METH_NOARGS | METH_STATIC, NULL},
    {"layout_template_push_variable",         (PyCFunction)ui_layout_template_push_variable,        PL_UI_KW, NULL},
    {"layout_template_push_static",           (PyCFunction)ui_layout_template_push_static,          PL_UI_KW, NULL},
    {"layout_template_end",                   (PyCFunction)ui_layout_template_end,                  METH_NOARGS | METH_STATIC, NULL},
    {"layout_space_begin",                    (PyCFunction)ui_layout_space_begin,                   PL_UI_KW, NULL},
    {"layout_space_push",                     (PyCFunction)ui_layout_space_push,                    PL_UI_KW, NULL},
    {"layout_space_end",                      (PyCFunction)ui_layout_space_end,                     METH_NOARGS | METH_STATIC, NULL},
    {"stash_layout",                          (PyCFunction)ui_stash_layout,                         METH_NOARGS | METH_STATIC, NULL},
    {"apply_layout",                          (PyCFunction)ui_apply_layout,                         METH_NOARGS | METH_STATIC, NULL},
    {"push_id_string",                        (PyCFunction)ui_push_id_string,                       PL_UI_KW, NULL},
    {"push_id_pointer",                       (PyCFunction)ui_push_id_pointer,                      METH_O | METH_STATIC, NULL},
    {"push_id_uint",                          (PyCFunction)ui_push_id_uint,                         PL_UI_KW, NULL},
    {"pop_id",                                (PyCFunction)ui_pop_id,                               METH_NOARGS | METH_STATIC, NULL},
    {"get_id",                                (PyCFunction)ui_get_id,                               PL_UI_KW, NULL},
    {"was_last_item_hovered",                 (PyCFunction)ui_was_last_item_hovered,                METH_NOARGS | METH_STATIC, NULL},
    {"was_last_item_active",                  (PyCFunction)ui_was_last_item_active,                 METH_NOARGS | METH_STATIC, NULL},
    {"set_keyboard_focus_last_item",          (PyCFunction)ui_set_keyboard_focus_last_item,         METH_NOARGS | METH_STATIC, NULL},
    {"dock_space",                            (PyCFunction)ui_dock_space,                           PL_UI_KW, NULL},
    {"dock_split",                            (PyCFunction)ui_dock_split,                           PL_UI_KW, NULL},
    {"set_next_window_dock",                  (PyCFunction)ui_set_next_window_dock,                 PL_UI_KW, NULL},
    {"get_window_dock",                       (PyCFunction)ui_get_window_dock,                      METH_NOARGS | METH_STATIC, NULL},
    {"set_docking_enabled",                   (PyCFunction)ui_set_docking_enabled,                  PL_UI_KW, NULL},
    {"set_settings_file",                     (PyCFunction)ui_set_settings_file,                    PL_UI_KW, NULL},
    {"save_settings",                         (PyCFunction)ui_save_settings,                        METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plUiI)

//-----------------------------------------------------------------------------
// [SECTION] constants
//-----------------------------------------------------------------------------

plPythonIntConstantPair gatUiIntPairs[] = {
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_TITLE_ACTIVE),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_TITLE_BG),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_TITLE_BG_COLLAPSED),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_WINDOW_BG),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_WINDOW_BORDER),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_CHILD_BG),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_BUTTON),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_BUTTON_HOVERED),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_BUTTON_ACTIVE),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_TEXT),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_TEXT_DISABLED),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_PROGRESS_BAR),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_CHECKMARK),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_FRAME_BG),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_FRAME_BG_HOVERED),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_FRAME_BG_ACTIVE),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_HEADER),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_HEADER_HOVERED),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_HEADER_ACTIVE),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_SCROLLBAR_BG),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_SCROLLBAR_HANDLE),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_SCROLLBAR_FRAME),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_SCROLLBAR_ACTIVE),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_SCROLLBAR_HOVERED),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_SEPARATOR),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_TAB),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_TAB_HOVERED),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_TAB_SELECTED),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_RESIZE_GRIP),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_RESIZE_GRIP_HOVERED),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_RESIZE_GRIP_ACTIVE),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_SLIDER),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_SLIDER_HOVERED),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_SLIDER_ACTIVE),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_POPUP_BG),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_TREE_LINES),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_COUNT),
    PL_ADD_INT_CONSTANT(PL_UI_WINDOW_FLAGS_NONE),
    PL_ADD_INT_CONSTANT(PL_UI_WINDOW_FLAGS_NO_TITLE_BAR),
    PL_ADD_INT_CONSTANT(PL_UI_WINDOW_FLAGS_NO_RESIZE),
    PL_ADD_INT_CONSTANT(PL_UI_WINDOW_FLAGS_NO_MOVE),
    PL_ADD_INT_CONSTANT(PL_UI_WINDOW_FLAGS_NO_COLLAPSE),
    PL_ADD_INT_CONSTANT(PL_UI_WINDOW_FLAGS_AUTO_SIZE),
    PL_ADD_INT_CONSTANT(PL_UI_WINDOW_FLAGS_NO_BACKGROUND),
    PL_ADD_INT_CONSTANT(PL_UI_WINDOW_FLAGS_NO_SCROLLBAR),
    PL_ADD_INT_CONSTANT(PL_UI_WINDOW_FLAGS_HORIZONTAL_SCROLLBAR),
    PL_ADD_INT_CONSTANT(PL_UI_WINDOW_FLAGS_NO_SAVED_SETTINGS),
    PL_ADD_INT_CONSTANT(PL_UI_WINDOW_FLAGS_NO_DOCKING),
    PL_ADD_INT_CONSTANT(PL_UI_WINDOW_FLAGS_NO_INPUT_CAPTURE),
    PL_ADD_INT_CONSTANT(PL_UI_TREE_NODE_FLAGS_NONE),
    PL_ADD_INT_CONSTANT(PL_UI_TREE_NODE_FLAGS_FRAMED),
    PL_ADD_INT_CONSTANT(PL_UI_TREE_NODE_FLAGS_NO_TREE_PUSH_ON_OPEN),
    PL_ADD_INT_CONSTANT(PL_UI_TREE_NODE_FLAGS_DEFAULT_OPEN),
    PL_ADD_INT_CONSTANT(PL_UI_TREE_NODE_FLAGS_OPEN_ON_ARROW),
    PL_ADD_INT_CONSTANT(PL_UI_TREE_NODE_FLAGS_LEAF),
    PL_ADD_INT_CONSTANT(PL_UI_TREE_NODE_FLAGS_BULLET),
    PL_ADD_INT_CONSTANT(PL_UI_TREE_NODE_FLAGS_COLLAPSING_HEADER),
    PL_ADD_INT_CONSTANT(PL_UI_TREE_NODE_FLAGS_DRAW_LINES_FULL),
    PL_ADD_INT_CONSTANT(PL_UI_TREE_NODE_FLAGS_DRAW_LINES_TO_NODES),
    PL_ADD_INT_CONSTANT(PL_UI_COMBO_FLAGS_NONE),
    PL_ADD_INT_CONSTANT(PL_UI_COMBO_FLAGS_HEIGHT_SMALL),
    PL_ADD_INT_CONSTANT(PL_UI_COMBO_FLAGS_HEIGHT_REGULAR),
    PL_ADD_INT_CONSTANT(PL_UI_COMBO_FLAGS_HEIGHT_LARGE),
    PL_ADD_INT_CONSTANT(PL_UI_COMBO_FLAGS_NO_ARROW_BUTTON),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_EDIT_FLAGS_NONE),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_EDIT_FLAGS_NO_ALPHA),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_EDIT_FLAGS_NO_LABEL),
    PL_ADD_INT_CONSTANT(PL_UI_COLOR_EDIT_FLAGS_NO_SIDE_PREVIEW),
    PL_ADD_INT_CONSTANT(PL_UI_COND_NONE),
    PL_ADD_INT_CONSTANT(PL_UI_COND_ALWAYS),
    PL_ADD_INT_CONSTANT(PL_UI_COND_ONCE),
    PL_ADD_INT_CONSTANT(PL_UI_COND_APPEARING),
    PL_ADD_INT_CONSTANT(PL_UI_LAYOUT_ROW_TYPE_NONE),
    PL_ADD_INT_CONSTANT(PL_UI_LAYOUT_ROW_TYPE_DYNAMIC),
    PL_ADD_INT_CONSTANT(PL_UI_LAYOUT_ROW_TYPE_STATIC),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_NONE),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_CHARS_DECIMAL),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_CHARS_HEXADECIMAL),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_CHARS_UPPERCASE),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_CHARS_NO_BLANK),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_AUTO_SELECT_ALL),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_ENTER_RETURNS_TRUE),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_CALLBACK_COMPLETION),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_CALLBACK_HISTORY),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_CALLBACK_ALWAYS),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_CALLBACK_CHAR_FILTER),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_ALLOW_TAB_INPUT),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_CTRL_ENTER_FOR_NEW_LINE),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_NO_HORIZONTAL_SCROLL),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_ALWAYS_OVERWRITE),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_READ_ONLY),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_PASSWORD),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_NO_UNDO_REDO),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_CHARS_SCIENTIFIC),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_CALLBACK_RESIZE),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_CALLBACK_EDIT),
    PL_ADD_INT_CONSTANT(PL_UI_INPUT_TEXT_FLAGS_ESCAPE_CLEARS_ALL),
};
