/*
   pl_draw_ext_m.c
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

typedef struct _pyplGraphicsI
{
    PyObject_HEAD
} pyplGraphicsI;

PyObject*
graphics_flush_device(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPythonDevice = NULL;

    static const char* apcKeywords[] = {
        "device",
        NULL,
    };

	if (!pl_parse("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonDevice))
		return NULL;

    plDevice* ptDevice = PyCapsule_GetPointer(ptPythonDevice, "plDevice");
    gptGfx->flush_device(ptDevice);
    Py_RETURN_NONE;
}

static PyMethodDef gatplGraphicsICommands[] =
{
    {"flush_device", (PyCFunction)graphics_flush_device, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot gatplGraphicsISlots[] = {
    {Py_tp_methods, (void*)gatplGraphicsICommands},
    {0, 0}
};

static PyType_Spec plGraphicsISpec = {
    "pilotlight.plGraphicsI",
    sizeof(pyplGraphicsI),
    0,
    Py_TPFLAGS_DEFAULT,
    gatplGraphicsISlots
};
