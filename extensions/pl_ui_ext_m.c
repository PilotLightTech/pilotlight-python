/*
   pl_draw_ext_m.c
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
begin_window(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    const char* pcText = NULL;

    static const char* apcKeywords[] = {
        "name",
        NULL,
    };

	if (!pl_parse_args("s|", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText))
		return NULL;
    return PyBool_FromLong(gptUI->begin_window(pcText, NULL, 0));
}

PyObject*
end_window(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    gptUI->end_window();
    Py_RETURN_NONE;
}

PyObject*
wants_keyboard_capture(PyObject* self)
{
    return PyBool_FromLong(gptUI->wants_keyboard_capture());
}

PyObject*
wants_mouse_capture(PyObject* self)
{
    return PyBool_FromLong(gptUI->wants_mouse_capture());
}

PyObject*
button(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    const char* pcText = NULL;

    static const char* apcKeywords[] = {
        "name",
        NULL,
    };

	if (!pl_parse_args("s|", (const char**)apcKeywords,
        args, kwargs, __FUNCTION__, &pcText))
		return NULL;
        
    return PyBool_FromLong(gptUI->button(pcText));
}

PyObject*
checkbox(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "name",
        "value",
        NULL,
    };

    const char* pcName = NULL;
    int iValue = 0;
	if (!pl_parse_args("sp", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcName, &iValue))
		return NULL;

    bool bValue = iValue;
    bool bResult = gptUI->checkbox(pcName, &bValue);
    return Py_BuildValue("(pp)", bResult, bValue);
}

PyObject*
input_text(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "label",
        "value",
        NULL,
    };

    const char* pcName = NULL;
    const char* pcValue = NULL;
	if (!pl_parse_args("ss", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcName, &pcValue))
		return NULL;

    char acBuffer[256] = {0};
    strncpy(acBuffer, pcValue, 256);
    bool bResult = gptUI->input_text(pcName, acBuffer, 256, 0);
    return Py_BuildValue("(ps)", bResult, acBuffer);
}


PyObject*
ui_text(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"text", NULL};
    const char* pcText = NULL;
	if (!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText))
		return NULL;
    gptUI->text("%s", pcText);
    Py_RETURN_NONE;
}

PyObject*
ui_separator(PyObject* self, PyObject* unused)
{
    gptUI->separator();
    Py_RETURN_NONE;
}

// returns (changed, value)
PyObject*
ui_slider_float(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    float fValue = 0.0f;
    float fMin = 0.0f;
    float fMax = 1.0f;
    int iFlags = 0;
	if (!pl_parse_args("sfff|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &fValue, &fMin, &fMax, &iFlags))
		return NULL;
    bool bResult = gptUI->slider_float(pcLabel, &fValue, fMin, fMax, iFlags);
    return Py_BuildValue("(pf)", bResult, fValue);
}

// returns (changed, value)
PyObject*
ui_slider_int(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "value", "minValue", "maxValue", "flags", NULL};
    const char* pcLabel = NULL;
    int iValue = 0;
    int iMin = 0;
    int iMax = 1;
    int iFlags = 0;
	if (!pl_parse_args("siii|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &iValue, &iMin, &iMax, &iFlags))
		return NULL;
    bool bResult = gptUI->slider_int(pcLabel, &iValue, iMin, iMax, iFlags);
    return Py_BuildValue("(pi)", bResult, iValue);
}

// returns (pressed, current) where current is the selected value
PyObject*
ui_radio_button(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"text", "current", "value", NULL};
    const char* pcText = NULL;
    int iCurrent = 0;
    int iValue = 0;
	if (!pl_parse_args("sii", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcText, &iCurrent, &iValue))
		return NULL;
    bool bResult = gptUI->radio_button(pcText, &iCurrent, iValue);
    return Py_BuildValue("(pi)", bResult, iCurrent);
}

PyObject*
ui_set_next_window_pos(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"pos", "condition", NULL};
    PyObject* ptPythonPos = NULL;
    int iCondition = PL_UI_COND_ONCE;
	if (!pl_parse_args("O|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptPythonPos, &iCondition))
		return NULL;
    plVec2 tPos = {0};
    pl_vec2_from_py(ptPythonPos, &tPos);
    gptUI->set_next_window_pos(tPos, iCondition);
    Py_RETURN_NONE;
}

PyObject*
ui_set_next_window_size(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"size", "condition", NULL};
    PyObject* ptPythonSize = NULL;
    int iCondition = PL_UI_COND_ONCE;
	if (!pl_parse_args("O|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptPythonSize, &iCondition))
		return NULL;
    plVec2 tSize = {0};
    pl_vec2_from_py(ptPythonSize, &tSize);
    gptUI->set_next_window_size(tSize, iCondition);
    Py_RETURN_NONE;
}

static PyMethodDef gatCommandsplUiI[] =
{
    {"text", (PyCFunction)ui_text, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"separator", (PyCFunction)ui_separator, METH_NOARGS | METH_STATIC, NULL},
    {"slider_float", (PyCFunction)ui_slider_float, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"slider_int", (PyCFunction)ui_slider_int, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"radio_button", (PyCFunction)ui_radio_button, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_next_window_pos", (PyCFunction)ui_set_next_window_pos, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_next_window_size", (PyCFunction)ui_set_next_window_size, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"begin_window", (PyCFunction)begin_window, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"end_window", (PyCFunction)end_window, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"button", (PyCFunction)button, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"checkbox", (PyCFunction)checkbox, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"input_text", (PyCFunction)input_text, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"wants_keyboard_capture", (PyCFunction)wants_keyboard_capture, METH_NOARGS | METH_STATIC, NULL},
    {"wants_mouse_capture", (PyCFunction)wants_mouse_capture, METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plUiI)
plPythonIntConstantPair gatUiIntPairs[] = {
    PL_ADD_INT_CONSTANT(PL_UI_COND_NONE),
    PL_ADD_INT_CONSTANT(PL_UI_COND_ALWAYS),
    PL_ADD_INT_CONSTANT(PL_UI_COND_ONCE),
    PL_ADD_INT_CONSTANT(PL_UI_COND_APPEARING),
};
