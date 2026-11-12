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

PyObject*
graphics_get_backend_string(PyObject* self, PyObject* unused)
{
    return PyUnicode_FromString(gptGfx->get_backend_string());
}

PyObject*
graphics_set_gpu_timing_enabled(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "device",
        "enabled",
        NULL,
    };

    PyObject* ptPythonDevice = NULL;
    int bEnabled = 0;
	if (!pl_parse_args("Op", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonDevice, &bEnabled))
		return NULL;

    plDevice* ptDevice = PyCapsule_GetPointer(ptPythonDevice, "plDevice");
    if(ptDevice == NULL)
        return NULL;
    gptGfx->set_gpu_timing_enabled(ptDevice, bEnabled);
    Py_RETURN_NONE;
}

// returns dict (results are a copy, so they can be kept)
PyObject*
graphics_get_gpu_timings(PyObject* self, PyObject* ptPythonDevice)
{
    plDevice* ptDevice = PyCapsule_GetPointer(ptPythonDevice, "plDevice");
    if(ptDevice == NULL)
        return NULL;

    const plGpuTimingFrame* ptFrame = gptGfx->get_gpu_timings(ptDevice);
    if(ptFrame == NULL)
        Py_RETURN_NONE;

    PyObject* ptSamples = PyList_New(ptFrame->uSampleCount);
    for(uint32_t i = 0; i < ptFrame->uSampleCount; i++)
    {
        const plGpuTimingSample* ptSample = &ptFrame->ptSamples[i];
        PyList_SetItem(ptSamples, i, Py_BuildValue("{s:s,s:d,s:d,s:I}",
            "name",      ptSample->acName,
            "start",     ptSample->dStartTime,
            "duration",  ptSample->dDuration,
            "depth",     ptSample->uDepth));
    }

    return Py_BuildValue("{s:N,s:K,s:d,s:I,s:O,s:O,s:O}",
        "samples",         ptSamples,
        "frame_index",     (unsigned long long)ptFrame->ulFrameIndex,
        "duration",        ptFrame->dDuration,
        "dropped_samples", ptFrame->uDroppedSamples,
        "supported",       ptFrame->bSupported ? Py_True : Py_False,
        "enabled",         ptFrame->bEnabled ? Py_True : Py_False,
        "cpu",             ptFrame->bCpu ? Py_True : Py_False);
}

static PyMethodDef gatCommandsplGraphicsI[] =
{
    {"flush_device", (PyCFunction)graphics_flush_device, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_swapchain_info", (PyCFunction)graphics_get_swapchain_info, METH_O | METH_STATIC, NULL},
    {"get_backend_string", (PyCFunction)graphics_get_backend_string, METH_NOARGS | METH_STATIC, NULL},
    {"set_gpu_timing_enabled", (PyCFunction)graphics_set_gpu_timing_enabled, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_gpu_timings", (PyCFunction)graphics_get_gpu_timings, METH_O | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plGraphicsI)
