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

typedef struct _pyplPhysicsI
{
    PyObject_HEAD
} pyplPhysicsI;

PyObject*
physics_register_ecs_system(PyObject* self)
{
 
    gptPhysics->register_ecs_system();
    Py_RETURN_NONE;
}

static PyMethodDef gatplPhysicsICommands[] =
{
    {"register_ecs_system", (PyCFunction)physics_register_ecs_system, METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot gatplPhysicsISlots[] = {
    {Py_tp_methods, (void*)gatplPhysicsICommands},
    {0, 0}
};

static PyType_Spec plPhysicsISpec = {
    "pilotlight.plPhysicsI",
    sizeof(pyplPhysicsI),
    0,
    Py_TPFLAGS_DEFAULT,
    gatplPhysicsISlots
};