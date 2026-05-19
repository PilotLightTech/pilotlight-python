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
animation_register_ecs_system(PyObject* self)
{
 
    gptAnimation->register_ecs_system();
    Py_RETURN_NONE;
}

static PyMethodDef gatCommandsplAnimationI[] =
{
    {"register_ecs_system", (PyCFunction)animation_register_ecs_system, METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plAnimationI)
