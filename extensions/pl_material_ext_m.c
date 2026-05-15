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

typedef struct _pyplMaterialI
{
    PyObject_HEAD
} pyplMaterialI;

PyObject*
material_register_ecs_system(PyObject* self)
{
 
    gptMaterial->register_ecs_system();
    Py_RETURN_NONE;
}

static PyMethodDef gatplMaterialICommands[] =
{
    {"register_ecs_system", (PyCFunction)material_register_ecs_system, METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot gatplMaterialISlots[] = {
    {Py_tp_methods, (void*)gatplMaterialICommands},
    {0, 0}
};

static PyType_Spec plMaterialISpec = {
    "pilotlight.plMaterialI",
    sizeof(pyplMaterialI),
    0,
    Py_TPFLAGS_DEFAULT,
    gatplMaterialISlots
};