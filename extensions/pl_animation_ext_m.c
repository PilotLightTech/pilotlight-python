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

typedef struct _pyplAnimationI
{
    PyObject_HEAD
} pyplAnimationI;

PyObject*
animation_register_ecs_system(PyObject* self)
{
 
    gptAnimation->register_ecs_system();
    Py_RETURN_NONE;
}

static PyMethodDef gatplAnimationICommands[] =
{
    {"register_ecs_system", (PyCFunction)animation_register_ecs_system, METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot gatplAnimationISlots[] = {
    {Py_tp_methods, (void*)gatplAnimationICommands},
    {0, 0}
};

static PyType_Spec plAnimationISpec = {
    "pilotlight.plAnimationI",
    sizeof(pyplAnimationI),
    0,
    Py_TPFLAGS_DEFAULT,
    gatplAnimationISlots
};
