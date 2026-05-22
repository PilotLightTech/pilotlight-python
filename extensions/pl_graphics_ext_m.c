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

PyObject*
graphics_flush_device(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPythonDevice = NULL;

    static const char* apcKeywords[] = {
        "device",
        NULL,
    };

	if (!pl_parse_args("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonDevice))
		return NULL;

    plDevice* ptDevice = PyCapsule_GetPointer(ptPythonDevice, "plDevice");
    gptGfx->flush_device(ptDevice);
    Py_RETURN_NONE;
}

PyObject*
graphics_get_swapchain_info(PyObject* self, PyObject* arg)
{
    plSwapchain* ptSwapchain = PyCapsule_GetPointer(arg, "plSwapchain");
    return pl_swapchain_info_new(gptGfx->get_swapchain_info(ptSwapchain));
}

static PyMethodDef gatCommandsplGraphicsI[] =
{
    {"flush_device", (PyCFunction)graphics_flush_device, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_swapchain_info", (PyCFunction)graphics_get_swapchain_info, METH_O | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plGraphicsI)
