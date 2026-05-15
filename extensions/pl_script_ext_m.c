/*
   pl_ecs_ext_m.c
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

typedef struct _pyplScriptI
{
    PyObject_HEAD
} pyplScriptI;

PyObject*
script_register_ecs_system(PyObject* self)
{
 
    gptScript->register_ecs_system();
    Py_RETURN_NONE;
}

static PyMethodDef gatplScriptICommands[] =
{
    {"register_ecs_system", (PyCFunction)script_register_ecs_system, METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot gatplScriptISlots[] = {
    {Py_tp_methods, (void*)gatplScriptICommands},
    {0, 0}
};

static PyType_Spec plScriptISpec = {
    "pilotlight.plScriptI",
    sizeof(pyplScriptI),
    0,
    Py_TPFLAGS_DEFAULT,
    gatplScriptISlots
};