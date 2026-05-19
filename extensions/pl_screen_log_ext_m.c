/*
   pl_stats_ext_m.c
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
screen_log_clear(PyObject* self)
{
    gptScreenLog->clear();
    Py_RETURN_NONE;
}

PyObject*
screen_log_add_message(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "time_to_display",
        "message",
        NULL,
    };

    double dTimeToDisplay = 0.0;
    const char* pcMessage = NULL;
	if (!pl_parse("ds", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &dTimeToDisplay, &pcMessage))
		return NULL;

    gptScreenLog->add_message(dTimeToDisplay, pcMessage);
    Py_RETURN_NONE;
}

static PyMethodDef gatCommandsplScreenLogI[] =
{
    {"clear", (PyCFunction)screen_log_clear, METH_NOARGS | METH_STATIC, NULL},
    {"add_message", (PyCFunction)screen_log_add_message, METH_VARARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plScreenLogI)