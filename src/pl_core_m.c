/*
   pl_core_m.c
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
// [SECTION] binding apis
//-----------------------------------------------------------------------------

const plWindowI* ptWindows2;
static PyObject* ptpyIO = NULL;

//-----------------------------------------------------------------------------
// [SECTION] implementations
//-----------------------------------------------------------------------------

PyObject*
plIOI_is_key_pressed(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "key",
        "repeat",
        NULL,
    };

    int iKey = 0;
    int bRepeat = false;

	if (!pl_parse("ip", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iKey, &bRepeat))
		return NULL;

    bool bResult = gptIOI->is_key_pressed(iKey, bRepeat);
    return PyBool_FromLong(bResult);
}

PyObject*
plIOI_is_key_released(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "key",
        NULL,
    };

    int iKey = 0;

	if (!pl_parse("i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iKey))
		return NULL;

    bool bResult = gptIOI->is_key_released(iKey);
    return PyBool_FromLong(bResult);
}

PyObject*
plIOI_is_key_down(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "key",
        NULL,
    };

    int iKey = 0;

	if (!pl_parse("i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iKey))
		return NULL;

    bool bResult = gptIOI->is_key_down(iKey);
    return PyBool_FromLong(bResult);
}

PyObject*
plIOI_get_key_pressed_amount(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "key",
        "fRepeatDelay",
        "fRate",
        NULL,
    };

    int iKey = 0;
    float fRepeatDelay = 0.0f;
    float fRate = 0.0f;

	if (!pl_parse("iff", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iKey, &fRepeatDelay, &fRate))
		return NULL;

    int iResult = gptIOI->get_key_pressed_amount(iKey, fRepeatDelay, fRate);
    return PyLong_FromInt32(iResult);
}

PyObject*
plIOI_is_mouse_down(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "button",
        NULL,
    };

    int iButton = 0;

	if (!pl_parse("i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iButton))
		return NULL;

    bool bResult = gptIOI->is_mouse_down(iButton);
    return PyBool_FromLong(bResult);
}

PyObject*
plIOI_is_mouse_released(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "button",
        NULL,
    };

    int iButton = 0;

	if (!pl_parse("i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iButton))
		return NULL;

    bool bResult = gptIOI->is_mouse_released(iButton);
    return PyBool_FromLong(bResult);
}

PyObject*
plIOI_is_mouse_double_clicked(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "button",
        NULL,
    };

    int iButton = 0;

	if (!pl_parse("i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iButton))
		return NULL;

    bool bResult = gptIOI->is_mouse_double_clicked(iButton);
    return PyBool_FromLong(bResult);
}

PyObject*
plIOI_is_mouse_clicked(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "button",
        "repeat",
        NULL,
    };

    int iButton = 0;
    int bRepeat = false;

	if (!pl_parse("ip", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iButton, &bRepeat))
		return NULL;

    bool bResult = gptIOI->is_mouse_clicked(iButton, bRepeat);
    return PyBool_FromLong(bResult);
}

PyObject*
plIOI_is_mouse_dragging(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "button",
        "threshold",
        NULL,
    };

    int iButton = 0;
    float fThreshold = false;

	if (!pl_parse("if", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iButton, &fThreshold))
		return NULL;

    bool bResult = gptIOI->is_mouse_dragging(iButton, fThreshold);
    return PyBool_FromLong(bResult);
}

PyObject*
plIOI_is_mouse_hovering_rect(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "minvec",
        "maxvec",
        NULL,
    };

    PyObject* ptPythonMinVec = NULL;
    PyObject* ptPythonMaxVec = NULL;

	if (!pl_parse("OO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonMinVec, &ptPythonMaxVec))
		return NULL;

    bool bResult = gptIOI->is_mouse_hovering_rect(pl_get_vec2_from_python(ptPythonMinVec), pl_get_vec2_from_python(ptPythonMaxVec));
    return PyBool_FromLong(bResult);
}

PyObject*
plIOI_reset_mouse_drag_delta(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "button",
        NULL,
    };

    int iButton = 0;

	if (!pl_parse("i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iButton))
		return NULL;

    gptIOI->reset_mouse_drag_delta(iButton);
    Py_RETURN_NONE;
}

PyObject*
plIOI_get_mouse_drag_delta(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "button",
        "threshold",
        NULL,
    };

    int iButton = 0;
    float fThreshold = false;

	if (!pl_parse("if", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iButton, &fThreshold))
		return NULL;

    plVec2 tResult = gptIOI->get_mouse_drag_delta(iButton, fThreshold);
    return Py_BuildValue("[ff]", tResult.x, tResult.y);
}

PyObject*
plIOI_get_mouse_pos(PyObject* self)
{
    plVec2 tResult = gptIOI->get_mouse_pos();
    return Py_BuildValue("[ff]", tResult.x, tResult.y);
}

PyObject*
plIOI_get_mouse_wheel(PyObject* self)
{
    return PyFloat_FromDouble((double)gptIOI->get_mouse_wheel());
}

PyObject*
plIOI_is_mouse_pos_valid(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "pos",
        NULL,
    };

    PyObject* ptPython = NULL;

	if (!pl_parse("O", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPython))
		return NULL;

    bool bResult = gptIOI->is_mouse_pos_valid(pl_get_vec2_from_python(ptPython));
    return PyBool_FromLong(bResult);
}

PyObject*
plIOI_set_mouse_cursor(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "cursor",
        NULL,
    };

    int iCursor = 0;

	if (!pl_parse("i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iCursor))
		return NULL;

    gptIOI->set_mouse_cursor(iCursor);
    Py_RETURN_NONE;
}

PyObject*
plIOI_get_version_string(PyObject* self)
{
    return PyUnicode_FromString(gptIOI->get_version_string());
}

PyObject*
plIOI_new_frame(PyObject* self)
{
    gptIOI->new_frame(); // must be called once at the beginning of a frame
    Py_RETURN_NONE;
}

PyObject*
plIOI_get_io(PyObject* self, PyObject* arg)
{

    if(ptpyIO == NULL)
    {
        ptpyIO = PyDict_New();
    }

    plIO* ptIO = gptIOI->get_io();

    PyDict_SetItemString(ptpyIO, "fDeltaTime", PyFloat_FromDouble((double)ptIO->fDeltaTime));
    PyDict_SetItemString(ptpyIO, "fMouseDragThreshold", PyFloat_FromDouble((double)ptIO->fMouseDragThreshold));
    PyDict_SetItemString(ptpyIO, "fMouseDoubleClickTime", PyFloat_FromDouble((double)ptIO->fMouseDoubleClickTime));
    PyDict_SetItemString(ptpyIO, "fMouseDoubleClickMaxDist", PyFloat_FromDouble((double)ptIO->fMouseDoubleClickMaxDist));
    PyDict_SetItemString(ptpyIO, "fKeyRepeatDelay", PyFloat_FromDouble((double)ptIO->fKeyRepeatDelay));
    PyDict_SetItemString(ptpyIO, "fKeyRepeatRate", PyFloat_FromDouble((double)ptIO->fKeyRepeatRate));
    PyDict_SetItemString(ptpyIO, "fFrameRate", PyFloat_FromDouble((double)ptIO->fFrameRate));
    PyDict_SetItemString(ptpyIO, "ulFrameCount", PyLong_FromInt64(ptIO->ulFrameCount));
    PyDict_SetItemString(ptpyIO, "dTime", PyFloat_FromDouble(ptIO->dTime));
    PyDict_SetItemString(ptpyIO, "bKeyCtrl", PyBool_FromLong(ptIO->bKeyCtrl));
    PyDict_SetItemString(ptpyIO, "bKeyShift", PyBool_FromLong(ptIO->bKeyShift));
    PyDict_SetItemString(ptpyIO, "bKeyAlt", PyBool_FromLong(ptIO->bKeyAlt));
    PyDict_SetItemString(ptpyIO, "bKeySuper", PyBool_FromLong(ptIO->bKeySuper));
    PyDict_SetItemString(ptpyIO, "bRunning", PyBool_FromLong(ptIO->bRunning));
    PyDict_SetItemString(ptpyIO, "tMainViewportSize", Py_BuildValue("[ii]", (int)ptIO->tMainViewportSize.x, (int)ptIO->tMainViewportSize.y));
    PyDict_SetItemString(ptpyIO, "tMainFramebufferScale", Py_BuildValue("[ff]", ptIO->tMainFramebufferScale.x, ptIO->tMainFramebufferScale.y));

    Py_XINCREF(ptpyIO);
    return ptpyIO;
}

PyObject*
plWindowI_create(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "pcTitle",
        "iXPos",
        "iYPos",
        "uWidth",
        "uHeight",
        "tFlags",
        NULL,
    };

    plWindowDesc tWindowDesc = {0};

	if (!pl_parse("siiIIi", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &tWindowDesc.pcTitle, &tWindowDesc.iXPos, &tWindowDesc.iYPos, &tWindowDesc.uWidth, &tWindowDesc.uHeight,
        &tWindowDesc.tFlags))
		return NULL;

    plWindow* ptWindow = NULL;

    ptWindows2->create(tWindowDesc, &ptWindow);
    return PyCapsule_New(ptWindow, "plWindow", NULL);
}

PyObject*
plWindowI_show(PyObject* self, PyObject* args)
{
    plWindow* ptWindowPtr = PyCapsule_GetPointer(args, "plWindow");
    ptWindows2->show(ptWindowPtr);
    Py_RETURN_NONE;
}

PyObject*
plWindowI_destroy(PyObject* self, PyObject* args)
{

    plWindow* ptWindowPtr = PyCapsule_GetPointer(args, "plWindow");
    ptWindows2->destroy(ptWindowPtr);
    Py_RETURN_NONE;
}

plPythonIntConstantPair gatCoreIntPairs[] = {
    PL_ADD_INT_CONSTANT(PL_KEY_NONE),
    PL_ADD_INT_CONSTANT(PL_KEY_NAMED_KEY_BEGIN),
    PL_ADD_INT_CONSTANT(PL_KEY_TAB),
    PL_ADD_INT_CONSTANT(PL_KEY_LEFT_ARROW),
    PL_ADD_INT_CONSTANT(PL_KEY_RIGHT_ARROW),
    PL_ADD_INT_CONSTANT(PL_KEY_UP_ARROW),
    PL_ADD_INT_CONSTANT(PL_KEY_DOWN_ARROW),
    PL_ADD_INT_CONSTANT(PL_KEY_PAGE_UP),
    PL_ADD_INT_CONSTANT(PL_KEY_PAGE_DOWN),
    PL_ADD_INT_CONSTANT(PL_KEY_HOME),
    PL_ADD_INT_CONSTANT(PL_KEY_END),
    PL_ADD_INT_CONSTANT(PL_KEY_INSERT),
    PL_ADD_INT_CONSTANT(PL_KEY_DELETE),
    PL_ADD_INT_CONSTANT(PL_KEY_BACKSPACE),
    PL_ADD_INT_CONSTANT(PL_KEY_SPACE),
    PL_ADD_INT_CONSTANT(PL_KEY_ENTER),
    PL_ADD_INT_CONSTANT(PL_KEY_ESCAPE),
    PL_ADD_INT_CONSTANT(PL_KEY_LEFT_CTRL),
    PL_ADD_INT_CONSTANT(PL_KEY_LEFT_SHIFT),
    PL_ADD_INT_CONSTANT(PL_KEY_LEFT_ALT),
    PL_ADD_INT_CONSTANT(PL_KEY_LEFT_SUPER),
    PL_ADD_INT_CONSTANT(PL_KEY_RIGHT_CTRL),
    PL_ADD_INT_CONSTANT(PL_KEY_RIGHT_SHIFT),
    PL_ADD_INT_CONSTANT(PL_KEY_RIGHT_ALT),
    PL_ADD_INT_CONSTANT(PL_KEY_RIGHT_SUPER),
    PL_ADD_INT_CONSTANT(PL_KEY_MENU),
    PL_ADD_INT_CONSTANT(PL_KEY_0),
    PL_ADD_INT_CONSTANT(PL_KEY_1),
    PL_ADD_INT_CONSTANT(PL_KEY_2),
    PL_ADD_INT_CONSTANT(PL_KEY_3),
    PL_ADD_INT_CONSTANT(PL_KEY_4),
    PL_ADD_INT_CONSTANT(PL_KEY_5),
    PL_ADD_INT_CONSTANT(PL_KEY_6),
    PL_ADD_INT_CONSTANT(PL_KEY_7),
    PL_ADD_INT_CONSTANT(PL_KEY_8),
    PL_ADD_INT_CONSTANT(PL_KEY_9),
    PL_ADD_INT_CONSTANT(PL_KEY_A),
    PL_ADD_INT_CONSTANT(PL_KEY_B),
    PL_ADD_INT_CONSTANT(PL_KEY_C),
    PL_ADD_INT_CONSTANT(PL_KEY_D),
    PL_ADD_INT_CONSTANT(PL_KEY_E),
    PL_ADD_INT_CONSTANT(PL_KEY_F),
    PL_ADD_INT_CONSTANT(PL_KEY_G),
    PL_ADD_INT_CONSTANT(PL_KEY_H),
    PL_ADD_INT_CONSTANT(PL_KEY_I),
    PL_ADD_INT_CONSTANT(PL_KEY_J),
    PL_ADD_INT_CONSTANT(PL_KEY_K),
    PL_ADD_INT_CONSTANT(PL_KEY_L),
    PL_ADD_INT_CONSTANT(PL_KEY_M),
    PL_ADD_INT_CONSTANT(PL_KEY_N),
    PL_ADD_INT_CONSTANT(PL_KEY_O),
    PL_ADD_INT_CONSTANT(PL_KEY_P),
    PL_ADD_INT_CONSTANT(PL_KEY_Q),
    PL_ADD_INT_CONSTANT(PL_KEY_R),
    PL_ADD_INT_CONSTANT(PL_KEY_S),
    PL_ADD_INT_CONSTANT(PL_KEY_T),
    PL_ADD_INT_CONSTANT(PL_KEY_U),
    PL_ADD_INT_CONSTANT(PL_KEY_V),
    PL_ADD_INT_CONSTANT(PL_KEY_W),
    PL_ADD_INT_CONSTANT(PL_KEY_X),
    PL_ADD_INT_CONSTANT(PL_KEY_Y),
    PL_ADD_INT_CONSTANT(PL_KEY_Z),
    PL_ADD_INT_CONSTANT(PL_KEY_F1),
    PL_ADD_INT_CONSTANT(PL_KEY_F2),
    PL_ADD_INT_CONSTANT(PL_KEY_F3),
    PL_ADD_INT_CONSTANT(PL_KEY_F4),
    PL_ADD_INT_CONSTANT(PL_KEY_F5),
    PL_ADD_INT_CONSTANT(PL_KEY_F6),
    PL_ADD_INT_CONSTANT(PL_KEY_F7),
    PL_ADD_INT_CONSTANT(PL_KEY_F8),
    PL_ADD_INT_CONSTANT(PL_KEY_F9),
    PL_ADD_INT_CONSTANT(PL_KEY_F10),
    PL_ADD_INT_CONSTANT(PL_KEY_F11),
    PL_ADD_INT_CONSTANT(PL_KEY_F12),
    PL_ADD_INT_CONSTANT(PL_KEY_F13),
    PL_ADD_INT_CONSTANT(PL_KEY_F14),
    PL_ADD_INT_CONSTANT(PL_KEY_F15),
    PL_ADD_INT_CONSTANT(PL_KEY_F16),
    PL_ADD_INT_CONSTANT(PL_KEY_F17),
    PL_ADD_INT_CONSTANT(PL_KEY_F18),
    PL_ADD_INT_CONSTANT(PL_KEY_F19),
    PL_ADD_INT_CONSTANT(PL_KEY_F20),
    PL_ADD_INT_CONSTANT(PL_KEY_F21),
    PL_ADD_INT_CONSTANT(PL_KEY_F22),
    PL_ADD_INT_CONSTANT(PL_KEY_F23),
    PL_ADD_INT_CONSTANT(PL_KEY_F24),
    PL_ADD_INT_CONSTANT(PL_KEY_APOSTROPHE),
    PL_ADD_INT_CONSTANT(PL_KEY_COMMA),
    PL_ADD_INT_CONSTANT(PL_KEY_MINUS),
    PL_ADD_INT_CONSTANT(PL_KEY_PERIOD),
    PL_ADD_INT_CONSTANT(PL_KEY_SLASH),
    PL_ADD_INT_CONSTANT(PL_KEY_SEMICOLON),
    PL_ADD_INT_CONSTANT(PL_KEY_EQUAL),
    PL_ADD_INT_CONSTANT(PL_KEY_LEFT_BRACKET),
    PL_ADD_INT_CONSTANT(PL_KEY_BACKSLASH),
    PL_ADD_INT_CONSTANT(PL_KEY_RIGHT_BRACKET),
    PL_ADD_INT_CONSTANT(PL_KEY_GRAVE_ACCENT),
    PL_ADD_INT_CONSTANT(PL_KEY_CAPS_LOCK),
    PL_ADD_INT_CONSTANT(PL_KEY_SCROLL_LOCK),
    PL_ADD_INT_CONSTANT(PL_KEY_NUM_LOCK),
    PL_ADD_INT_CONSTANT(PL_KEY_PRINT_SCREEN),
    PL_ADD_INT_CONSTANT(PL_KEY_PAUSE),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_0),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_1),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_2),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_3),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_4),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_5),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_6),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_7),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_8),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_9),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_DECIMAL),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_DIVIDE),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_MULTIPLY),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_SUBTRACT),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_ADD),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_ENTER),
    PL_ADD_INT_CONSTANT(PL_KEY_KEYPAD_EQUAL),
    PL_ADD_INT_CONSTANT(PL_KEY_RESERVED_MOD_CTRL),
    PL_ADD_INT_CONSTANT(PL_KEY_RESERVED_MOD_SHIFT),
    PL_ADD_INT_CONSTANT(PL_KEY_RESERVED_MOD_ALT),
    PL_ADD_INT_CONSTANT(PL_RESERVED_KEY_MOD_SUPER),
    PL_ADD_INT_CONSTANT(PL_KEY_NAMED_KEY_END),
    PL_ADD_INT_CONSTANT(PL_KEY_COUNT),
    PL_ADD_INT_CONSTANT(PL_KEY_MOD_NONE),
    PL_ADD_INT_CONSTANT(PL_KEY_MOD_CTRL),
    PL_ADD_INT_CONSTANT(PL_KEY_MOD_SHIFT),
    PL_ADD_INT_CONSTANT(PL_KEY_MOD_ALT),
    PL_ADD_INT_CONSTANT(PL_KEY_MOD_SUPER),
    PL_ADD_INT_CONSTANT(PL_KEY_MOD_SHORTCUT),

    // mouse buttons
    PL_ADD_INT_CONSTANT(PL_MOUSE_BUTTON_LEFT),
    PL_ADD_INT_CONSTANT(PL_MOUSE_BUTTON_RIGHT),
    PL_ADD_INT_CONSTANT(PL_MOUSE_BUTTON_MIDDLE),
    PL_ADD_INT_CONSTANT(PL_MOUSE_BUTTON_COUNT),

    // mouse cursors
    PL_ADD_INT_CONSTANT(PL_MOUSE_CURSOR_NONE),
    PL_ADD_INT_CONSTANT(PL_MOUSE_CURSOR_ARROW),
    PL_ADD_INT_CONSTANT(PL_MOUSE_CURSOR_TEXT_INPUT),
    PL_ADD_INT_CONSTANT(PL_MOUSE_CURSOR_RESIZE_ALL),
    PL_ADD_INT_CONSTANT(PL_MOUSE_CURSOR_RESIZE_NS),
    PL_ADD_INT_CONSTANT(PL_MOUSE_CURSOR_RESIZE_EW),
    PL_ADD_INT_CONSTANT(PL_MOUSE_CURSOR_RESIZE_NESW),
    PL_ADD_INT_CONSTANT(PL_MOUSE_CURSOR_RESIZE_NWSE),
    PL_ADD_INT_CONSTANT(PL_MOUSE_CURSOR_HAND),
    PL_ADD_INT_CONSTANT(PL_MOUSE_CURSOR_WAIT),
    PL_ADD_INT_CONSTANT(PL_MOUSE_CURSOR_PROGRESS),
    PL_ADD_INT_CONSTANT(PL_MOUSE_CURSOR_NOT_ALLOWED),
    PL_ADD_INT_CONSTANT(PL_MOUSE_CURSOR_COUNT)
};