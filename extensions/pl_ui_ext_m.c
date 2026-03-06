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
plUiI_begin_window(PyObject* self, PyObject* args, PyObject* kwargs)
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
plUiI_end_window(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    gptUI->end_window();
    Py_RETURN_NONE;
}

PyObject*
plUiI_button(PyObject* self, PyObject* args, PyObject* kwargs)
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
plUiI_checkbox(PyObject* self, PyObject* args, PyObject* kwargs)
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
	if (!pl_parse("sp|$O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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