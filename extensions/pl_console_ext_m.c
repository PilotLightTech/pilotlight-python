/*
   pl_console_ext_m.c
     * plConsoleI (developer console, F1 when started through plStarterI)
     * partial binding: opening/closing the console and reading/writing
       existing bool & toggle variables (i.e. the "t.*" tool windows)
     * not bound yet: add_*_variable & add_command (the console keeps a
       pointer to caller storage, which needs a python-side design),
       initialize/cleanup/update (plStarterI drives them)
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] implementations
// [SECTION] method table
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] implementations
//-----------------------------------------------------------------------------

// call during a UI frame (between plStarterI.begin_frame & end_frame)
static PyObject*
console_open(PyObject* self, PyObject* unused)
{
    gptConsole->open();
    Py_RETURN_NONE;
}

static PyObject*
console_close(PyObject* self, PyObject* unused)
{
    gptConsole->close();
    Py_RETURN_NONE;
}

// returns the value of a bool/toggle variable, or None if it doesn't exist
// (like C's get_variable, the variable must be a bool or toggle)
static PyObject*
console_get_bool(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"name", NULL};
    const char* pcName = NULL;
    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcName))
        return NULL;
    const bool* pbValue = (const bool*)gptConsole->get_variable(pcName, NULL, NULL);
    if(pbValue == NULL)
        Py_RETURN_NONE;
    return PyBool_FromLong(*pbValue);
}

// sets a bool/toggle variable; returns False if it doesn't exist
// (writes the variable directly like the C sandbox; callbacks don't run)
static PyObject*
console_set_bool(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"name", "value", NULL};
    const char* pcName = NULL;
    int iValue = 0;
    if(!pl_parse_args("sp", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcName, &iValue))
        return NULL;
    bool* pbValue = (bool*)gptConsole->get_variable(pcName, NULL, NULL);
    if(pbValue == NULL)
        Py_RETURN_FALSE;
    *pbValue = iValue != 0;
    Py_RETURN_TRUE;
}

static PyObject*
console_remove_variable(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"name", NULL};
    const char* pcName = NULL;
    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcName))
        return NULL;
    return PyBool_FromLong(gptConsole->remove_variable(pcName));
}

//-----------------------------------------------------------------------------
// [SECTION] method table
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplConsoleI[] =
{
    {"open",            (PyCFunction)console_open,            METH_NOARGS | METH_STATIC, NULL},
    {"close",           (PyCFunction)console_close,           METH_NOARGS | METH_STATIC, NULL},
    {"get_bool",        (PyCFunction)console_get_bool,        METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_bool",        (PyCFunction)console_set_bool,        METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"remove_variable", (PyCFunction)console_remove_variable, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plConsoleI)
