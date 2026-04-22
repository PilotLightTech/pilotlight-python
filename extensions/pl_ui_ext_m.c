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
ui_begin_window(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    const char* pcText = NULL;

    static const char* apcKeywords[] = {
        "name",
        NULL,
    };

	if (!pl_parse("s|", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText))
		return NULL;
    return PyBool_FromLong(gptUI->begin_window(pcText, NULL, 0));
}

PyObject*
ui_end_window(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    gptUI->end_window();
    Py_RETURN_NONE;
}

PyObject*
ui_button(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    const char* pcText = NULL;

    static const char* apcKeywords[] = {
        "name",
        NULL,
    };

	if (!pl_parse("s|", (const char**)apcKeywords,
        args, kwargs, __FUNCTION__, &pcText))
		return NULL;
        
    return PyBool_FromLong(gptUI->button(pcText));
}

PyObject*
ui_checkbox(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "name",
        "value",
        "pointer",
        NULL,
    };

    const char* pcName = NULL;
    int iValue = 0;
    PyObject* ptPointer = NULL;
	if (!pl_parse("s|p$O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcName, &iValue, &ptPointer))
		return NULL;

    if(ptPointer)
    {
        return PyBool_FromLong(gptUI->checkbox(pcName, (bool*)PyCapsule_GetPointer(ptPointer, "pb")));
    }
    else
    {
        bool bValue = iValue;
        bool bResult = gptUI->checkbox(pcName, &bValue);
        return Py_BuildValue("(pp)", bResult, bValue);
    }
}

PyObject*
ui_input_text(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "label",
        "value",
        NULL,
    };

    const char* pcName = NULL;
    PyObject* ptByteObject = NULL;
	if (!pl_parse("sY", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcName, &ptByteObject))
		return NULL;


    char* acBuffer = PyByteArray_AsString(ptByteObject);
    Py_ssize_t szBufferSize = PyByteArray_Size(ptByteObject);
    bool bResult = gptUI->input_text(pcName, acBuffer, szBufferSize, 0);
    return Py_BuildValue("p", bResult);
}