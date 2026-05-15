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

typedef struct _pyplMeshI
{
    PyObject_HEAD
} pyplMeshI;

PyObject*
mesh_register_ecs_system(PyObject* self)
{
 
    gptMesh->register_ecs_system();
    Py_RETURN_NONE;
}

static PyMethodDef gatplMeshICommands[] =
{
    {"register_ecs_system", (PyCFunction)mesh_register_ecs_system, METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot gatplMeshISlots[] = {
    {Py_tp_methods, (void*)gatplMeshICommands},
    {0, 0}
};

static PyType_Spec plMeshISpec = {
    "pilotlight.plMeshI",
    sizeof(pyplMeshI),
    0,
    Py_TPFLAGS_DEFAULT,
    gatplMeshISlots
};