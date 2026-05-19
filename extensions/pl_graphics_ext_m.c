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

static PyObject*
pl_swapchain_info_get_tSampleCount(pyplSwapchainInfo* self, void *closure)
{
    return PyLong_FromInt32(self->tInfo.tSampleCount);
}

static PyGetSetDef gatSwapchainInfoProps[] =
{
    {"tSampleCount", (getter)pl_swapchain_info_get_tSampleCount, (setter)NULL, "Property: tSampleCount"},
    {NULL, NULL, 0, NULL}
};

static int
pl_swapchain_info_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
    pyplSwapchainInfo* vec = (pyplSwapchainInfo*)self;

    static const char* kwlist[] = {"tSampleCount", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|i", (char**)kwlist, &vec->tInfo.tSampleCount))
        return -1;

    return 0;
}

static PyObject*
pl_swapchain_info_new(plSwapchainInfo tInfo)
{
    PyObject* args = Py_BuildValue("(i)", tInfo.tSampleCount);
    if(!args)
        return NULL;

    PyObject* obj = PyObject_CallObject(gptSwapchainInfoType, args);
    Py_DECREF(args);
    return obj;
}

static PyType_Slot pl_swapchain_info_slots[] = {
    {Py_tp_init, (void*)pl_swapchain_info_init},
    {Py_tp_getset, (void*)gatSwapchainInfoProps},
    // {Py_tp_methods, (void*)gatIOCommands},
    {0, 0}
};

static PyType_Spec pl_swapchain_info_spec = {
    "pilotlight.plSwapchainInfo",
    sizeof(pyplSwapchainInfo),
    0,
    Py_TPFLAGS_DEFAULT,
    pl_swapchain_info_slots
};


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
