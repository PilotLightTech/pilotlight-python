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

PyObject*
physics_register_ecs_system(PyObject* self)
{
 
    gptPhysics->register_ecs_system();
    Py_RETURN_NONE;
}

static PyMethodDef gatCommandsplPhysicsI[] =
{
    {"register_ecs_system", (PyCFunction)physics_register_ecs_system, METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plPhysicsI)