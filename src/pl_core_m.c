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

//-----------------------------------------------------------------------------
// [SECTION] implementations
//-----------------------------------------------------------------------------

PyObject*
set_pointer_value(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "pointer",
        "value",
        "index",
        NULL,
    };

    PyObject* ptPythonPointer = NULL;
    PyObject* ptPythonValue = NULL;
    int iIndex = 0;
	if (!pl_parse_args("OO|i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonPointer, &ptPythonValue, &iIndex))
		return NULL;

    const char* pcName = PyCapsule_GetName(ptPythonPointer);
    if(strcmp(pcName, "plDoublePointer") == 0)
    {
        double* ptValue = PyCapsule_GetPointer(ptPythonPointer, pcName);
        ptValue[iIndex] = PyFloat_AsDouble(ptPythonValue);
        return PyBool_FromLong(1);
    }
    return PyBool_FromLong(0);
}

PyObject*
get_pointer_value(PyObject* self, PyObject* args)
{

    static const char* apcKeywords[] = {
        "pointer",
        "index",
        NULL,
    };

    PyObject* ptPythonPointer = NULL;
    int iIndex = 0;
	if (!pl_parse_args("O|i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonPointer, &iIndex))
		return NULL;

    const char* pcName = PyCapsule_GetName(ptPythonPointer);
    if(strcmp(pcName, "plDoublePointer") == 0)
    {
        double* ptValue = PyCapsule_GetPointer(ptPythonPointer, pcName);
        return PyFloat_FromDouble(ptValue[iIndex]);
    }
    Py_RETURN_NONE;
}

PyObject*
io_is_key_pressed(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "key",
        "repeat",
        NULL,
    };

    int iKey = 0;
    int bRepeat = false;

	if (!pl_parse_args("i|p", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iKey, &bRepeat))
		return NULL;

    bool bResult = gptIOI->is_key_pressed(iKey, bRepeat);
    return PyBool_FromLong(bResult);
}

PyObject*
io_is_key_released(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "key",
        NULL,
    };

    int iKey = 0;

	if (!pl_parse_args("i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iKey))
		return NULL;

    bool bResult = gptIOI->is_key_released(iKey);
    return PyBool_FromLong(bResult);
}

PyObject*
io_is_key_down(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "key",
        NULL,
    };

    int iKey = 0;

	if (!pl_parse_args("i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iKey))
		return NULL;

    bool bResult = gptIOI->is_key_down(iKey);
    return PyBool_FromLong(bResult);
}

PyObject*
io_get_key_pressed_amount(PyObject* self, PyObject* args)
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

	if (!pl_parse_args("iff", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iKey, &fRepeatDelay, &fRate))
		return NULL;

    int iResult = gptIOI->get_key_pressed_amount(iKey, fRepeatDelay, fRate);
    return PyLong_FromInt32(iResult);
}

PyObject*
io_is_mouse_down(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "button",
        NULL,
    };

    int iButton = 0;

	if (!pl_parse_args("i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iButton))
		return NULL;

    bool bResult = gptIOI->is_mouse_down(iButton);
    return PyBool_FromLong(bResult);
}

PyObject*
io_is_mouse_released(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "button",
        NULL,
    };

    int iButton = 0;

	if (!pl_parse_args("i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iButton))
		return NULL;

    bool bResult = gptIOI->is_mouse_released(iButton);
    return PyBool_FromLong(bResult);
}

PyObject*
io_is_mouse_double_clicked(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "button",
        NULL,
    };

    int iButton = 0;

	if (!pl_parse_args("i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iButton))
		return NULL;

    bool bResult = gptIOI->is_mouse_double_clicked(iButton);
    return PyBool_FromLong(bResult);
}

PyObject*
io_is_mouse_clicked(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "button",
        "repeat",
        NULL,
    };

    int iButton = 0;
    int bRepeat = false;

	if (!pl_parse_args("ip", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iButton, &bRepeat))
		return NULL;

    bool bResult = gptIOI->is_mouse_clicked(iButton, bRepeat);
    return PyBool_FromLong(bResult);
}

PyObject*
io_is_mouse_dragging(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "button",
        "threshold",
        NULL,
    };

    int iButton = 0;
    float fThreshold = false;

	if (!pl_parse_args("if", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iButton, &fThreshold))
		return NULL;

    bool bResult = gptIOI->is_mouse_dragging(iButton, fThreshold);
    return PyBool_FromLong(bResult);
}

PyObject*
io_is_mouse_hovering_rect(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "minvec",
        "maxvec",
        NULL,
    };

    PyObject* ptPythonMinVec = NULL;
    PyObject* ptPythonMaxVec = NULL;

	if (!pl_parse_args("OO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonMinVec, &ptPythonMaxVec))
		return NULL;

    plVec2 tMinVec = {0};
    plVec2 tMaxVec = {0};
    pl_vec2_from_py(ptPythonMinVec, &tMinVec);
    pl_vec2_from_py(ptPythonMaxVec, &tMaxVec);
    bool bResult = gptIOI->is_mouse_hovering_rect(tMinVec, tMaxVec);
    return PyBool_FromLong(bResult);
}

PyObject*
io_reset_mouse_drag_delta(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "button",
        NULL,
    };

    int iButton = 0;

	if (!pl_parse_args("i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iButton))
		return NULL;

    gptIOI->reset_mouse_drag_delta(iButton);
    Py_RETURN_NONE;
}

PyObject*
io_get_mouse_drag_delta(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "button",
        "threshold",
        NULL,
    };

    int iButton = 0;
    float fThreshold = false;

	if (!pl_parse_args("if", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iButton, &fThreshold))
		return NULL;

    plVec2 tResult = gptIOI->get_mouse_drag_delta(iButton, fThreshold);
    return pl_vec2_to_py(tResult);
}

PyObject*
io_get_mouse_pos(PyObject* self)
{
    plVec2 tResult = gptIOI->get_mouse_pos();
    return Py_BuildValue("[ff]", tResult.x, tResult.y);
}

PyObject*
io_get_mouse_wheel(PyObject* self)
{
    return PyFloat_FromDouble((double)gptIOI->get_mouse_wheel());
}

PyObject*
io_is_mouse_pos_valid(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "pos",
        NULL,
    };

    PyObject* ptPython = NULL;

	if (!pl_parse_args("O", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPython))
		return NULL;

    plVec2 tPos = {0};
    pl_vec2_from_py(ptPython, &tPos);
    bool bResult = gptIOI->is_mouse_pos_valid(tPos);
    return PyBool_FromLong(bResult);
}

PyObject*
io_set_mouse_cursor(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "cursor",
        NULL,
    };

    int iCursor = 0;

	if (!pl_parse_args("i", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &iCursor))
		return NULL;

    gptIOI->set_mouse_cursor(iCursor);
    Py_RETURN_NONE;
}

PyObject*
io_get_version_string(PyObject* self)
{
    return PyUnicode_FromString(gptIOI->get_version_string());
}

PyObject*
io_new_frame(PyObject* self)
{
    gptIOI->new_frame(); // must be called once at the beginning of a frame
    Py_RETURN_NONE;
}

PyObject*
io_get_io(PyObject* self, PyObject* arg)
{

    pyplIO* obj = (pyplIO*)PyObject_CallObject(gptIOType, NULL);
    if(!obj)
        return NULL;
    return (PyObject*)obj;
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
    plWindowResult tResult = ptWindows2->create(tWindowDesc, &ptWindow);
    Py_DECREF(ptPythonObject); // must stay alive until window is created for title
    PyObject* ptCapsule = PyCapsule_New(ptWindow, "plWindow", NULL);
    return Py_BuildValue("(iO)", tResult, ptCapsule);
}

PyObject*
window_show(PyObject* self, PyObject* args)
{
    plWindow* ptWindowPtr = PyCapsule_GetPointer(args, "plWindow");
    ptWindows2->show(ptWindowPtr);
    Py_RETURN_NONE;
}

PyObject*
window_destroy(PyObject* self, PyObject* args)
{

    plWindow* ptWindowPtr = PyCapsule_GetPointer(args, "plWindow");
    ptWindows2->destroy(ptWindowPtr);
    Py_RETURN_NONE;
}

static PyMethodDef gatCommandsplIOI[] =
{
    {"get_version_string", (PyCFunction)io_get_version_string, METH_NOARGS | METH_STATIC, NULL},
    {"get_io", (PyCFunction)io_get_io, METH_NOARGS | METH_STATIC, NULL},
    {"new_frame", (PyCFunction)io_new_frame, METH_NOARGS | METH_STATIC, NULL},
    {"is_key_pressed", (PyCFunction)io_is_key_pressed, METH_VARARGS | METH_STATIC, NULL},
    {"is_key_released", (PyCFunction)io_is_key_released, METH_VARARGS | METH_STATIC, NULL},
    {"is_key_down", (PyCFunction)io_is_key_down, METH_VARARGS | METH_STATIC, NULL},
    {"get_key_pressed_amount", (PyCFunction)io_get_key_pressed_amount, METH_VARARGS | METH_STATIC, NULL},
    {"is_mouse_down", (PyCFunction)io_is_mouse_down, METH_VARARGS | METH_STATIC, NULL},
    {"is_mouse_released", (PyCFunction)io_is_mouse_released, METH_VARARGS | METH_STATIC, NULL},
    {"is_mouse_double_clicked", (PyCFunction)io_is_mouse_double_clicked, METH_VARARGS | METH_STATIC, NULL},
    {"is_mouse_clicked", (PyCFunction)io_is_mouse_clicked, METH_VARARGS | METH_STATIC, NULL},
    {"is_mouse_dragging", (PyCFunction)io_is_mouse_dragging, METH_VARARGS | METH_STATIC, NULL},
    {"reset_mouse_drag_delta", (PyCFunction)io_reset_mouse_drag_delta, METH_VARARGS | METH_STATIC, NULL},
    {"get_mouse_drag_delta", (PyCFunction)io_get_mouse_drag_delta, METH_VARARGS | METH_STATIC, NULL},
    {"get_mouse_pos", (PyCFunction)io_get_mouse_pos, METH_NOARGS | METH_STATIC, NULL},
    {"get_mouse_wheel", (PyCFunction)io_get_mouse_wheel, METH_NOARGS | METH_STATIC, NULL},
    {"is_mouse_pos_valid", (PyCFunction)io_is_mouse_pos_valid, METH_NOARGS | METH_STATIC, NULL},
    {"set_mouse_cursor", (PyCFunction)io_set_mouse_cursor, METH_VARARGS | METH_STATIC, NULL},

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
PL_NEW_PYTHON_API(plIOI)

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