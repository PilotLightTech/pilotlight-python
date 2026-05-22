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

static PyMethodDef gatCommandsplUiI[] =
{
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