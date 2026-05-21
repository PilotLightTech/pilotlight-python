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
job_initialize(PyObject* self)
{
    gptJob->initialize((plJobSystemInit){0});
    Py_RETURN_NONE;
}

static PyMethodDef gatCommandsplJobI[] =
{
    {"initialize", (PyCFunction)job_initialize, METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plJobI)