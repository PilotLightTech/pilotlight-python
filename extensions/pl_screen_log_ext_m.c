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
	if (!pl_parse_args("ds", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &dTimeToDisplay, &pcMessage))
		return NULL;

    gptScreenLog->add_message(dTimeToDisplay, pcMessage);
    Py_RETURN_NONE;
}

// key: messages with the same non-zero key replace each other; color 0 = white
static PyObject*
screen_log_add_message_ex(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"key", "timeToDisplay", "color", "textScale", "message", NULL};
    unsigned long long ulKey = 0;
    double dTimeToDisplay = 0.0;
    unsigned int uColor = 0;
    float fTextScale = 1.0f;
    const char* pcMessage = NULL;
    if(!pl_parse_args("KdIfs", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ulKey, &dTimeToDisplay, &uColor, &fTextScale, &pcMessage))
        return NULL;
    gptScreenLog->add_message_ex(ulKey, dTimeToDisplay, uColor, fTextScale, "%s", pcMessage);
    Py_RETURN_NONE;
}

static PyObject*
screen_log_set_flags(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"flags", NULL};
    int iFlags = 0;
    if(!pl_parse_args("i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iFlags))
        return NULL;
    gptScreenLog->set_flags(iFlags);
    Py_RETURN_NONE;
}

static PyObject*
screen_log_get_flags(PyObject* self, PyObject* unused)
{
    return PyLong_FromLong((long)gptScreenLog->get_flags());
}

static PyMethodDef gatCommandsplScreenLogI[] =
{
    {"add_message_ex", (PyCFunction)screen_log_add_message_ex, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_flags", (PyCFunction)screen_log_set_flags, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_flags", (PyCFunction)screen_log_get_flags, METH_NOARGS | METH_STATIC, NULL},
    {"clear", (PyCFunction)screen_log_clear, METH_NOARGS | METH_STATIC, NULL},
    {"add_message", (PyCFunction)screen_log_add_message, METH_VARARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plScreenLogI)
plPythonIntConstantPair gatScreenLogIntPairs[] = {
    PL_ADD_INT_CONSTANT(PL_SCREEN_LOG_FLAGS_NONE),
    PL_ADD_INT_CONSTANT(PL_SCREEN_LOG_FLAGS_HIDE_MESSAGES),
};
