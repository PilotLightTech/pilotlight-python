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

typedef struct _pyplStatsI
{
    PyObject_HEAD
} pyplStatsI;

PyObject*
stats_new_frame(PyObject* self)
{
    gptStats->new_frame();
    Py_RETURN_NONE;
}

PyObject*
stats_get_counter(PyObject* self, PyObject* arg)
{
    const char* pcName = PyUnicode_AsUTF8(arg);
    double* pdCounter = gptStats->get_counter(pcName);
    return PyCapsule_New(pdCounter, "plFloatPointer", NULL);
}

static PyMethodDef gatplStatsICommands[] =
{
    {"new_frame", (PyCFunction)stats_new_frame, METH_NOARGS | METH_STATIC, NULL},
    {"get_counter", (PyCFunction)stats_get_counter, METH_O | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot gatplStatsISlots[] = {
    {Py_tp_methods, (void*)gatplStatsICommands},
    {0, 0}
};

static PyType_Spec plStatsISpec = {
    "pilotlight.plStatsI",
    sizeof(pyplStatsI),
    0,
    Py_TPFLAGS_DEFAULT,
    gatplStatsISlots
};