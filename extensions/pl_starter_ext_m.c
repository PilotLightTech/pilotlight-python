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

typedef struct _pyplStarterI
{
    PyObject_HEAD
} pyplStarterI;

PyObject*
starter_initialize(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyObject* ptWindow = NULL;

    static const char* apcKeywords[] = {
        "window",
        "flags",
        NULL,
    };

    int iFlags = 0;
	if (!pl_parse("Oi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptWindow, &iFlags))
		return NULL;

    plWindow* ptWindowPtr = PyCapsule_GetPointer(ptWindow, "plWindow");

    // initialize the starter API (handles alot of boilerplate)
    plStarterInit tStarterInit = {
        .tFlags   = iFlags,
        .ptWindow = ptWindowPtr
    };
    gptStarter->initialize(tStarterInit);
    Py_RETURN_NONE;
}

PyObject*
starter_finalize(PyObject* self)
{
    gptStarter->finalize();
    Py_RETURN_NONE;
}

PyObject*
starter_cleanup(PyObject* self)
{
    gptStarter->cleanup();
    Py_RETURN_NONE;
}

PyObject*
starter_resize(PyObject* self)
{
    gptStarter->resize();
    Py_RETURN_NONE;
}

PyObject*
starter_begin_frame(PyObject* self)
{
    bool bResult = gptStarter->begin_frame(); // must be called once at the beginning of a frame
    return PyBool_FromLong(bResult);
}

PyObject*
starter_end_frame(PyObject* self)
{
    gptStarter->end_frame();
    Py_RETURN_NONE;
}

PyObject*
starter_get_foreground_layer(PyObject* self)
{
    plDrawLayer2D* ptFGLayer = gptStarter->get_foreground_layer();

    return PyCapsule_New(ptFGLayer, "plDrawLayer2D", NULL);
}

PyObject*
starter_get_background_layer(PyObject* self)
{
    plDrawLayer2D* ptBGLayer = gptStarter->get_background_layer();

    return PyCapsule_New(ptBGLayer, "plDrawLayer2D", NULL);
}

PyObject*
starter_get_device(PyObject* self)
{
    plDevice* ptDevice = gptStarter->get_device();
    return PyCapsule_New(ptDevice, "plDevice", NULL);
}

PyObject*
starter_get_swapchain(PyObject* self)
{
    plSwapchain* ptSwapchain = gptStarter->get_swapchain();
    return PyCapsule_New(ptSwapchain, "plSwapchain", NULL);
}

PyObject*
starter_get_render_pass(PyObject* self)
{
    plRenderPassHandle tHandle = gptStarter->get_render_pass();
    return Py_BuildValue("K", tHandle.uData);
}

PyObject*
starter_begin_main_pass(PyObject* self)
{
    plRenderEncoder* ptEncoder = gptStarter->begin_main_pass();
    return PyCapsule_New(ptEncoder, "plRenderEncoder", NULL);
}

PyObject*
starter_end_main_pass(PyObject* self)
{
    gptStarter->end_main_pass();
    Py_RETURN_NONE;
}

PyObject*
starter_get_command_buffer(PyObject* self)
{
    plCommandBuffer* ptCommandBuffer = gptStarter->get_command_buffer();
    return PyCapsule_New(ptCommandBuffer, "plCommandBuffer", NULL);
}

PyObject*
starter_get_temporary_command_buffer(PyObject* self)
{
    plCommandBuffer* ptCommandBuffer = gptStarter->get_temporary_command_buffer();
    return PyCapsule_New(ptCommandBuffer, "plCommandBuffer", NULL);
}

PyObject*
starter_get_raw_command_buffer(PyObject* self)
{
    plCommandBuffer* ptCommandBuffer = gptStarter->get_raw_command_buffer();
    return PyCapsule_New(ptCommandBuffer, "plCommandBuffer", NULL);
}

PyObject*
starter_submit_command_buffer(PyObject* self, PyObject* arg)
{
    plCommandBuffer* ptCommandBuffer = PyCapsule_GetPointer(arg, "plCommandBuffer");
    gptStarter->submit_command_buffer(ptCommandBuffer);
    Py_RETURN_NONE;
}

PyObject*
starter_submit_temporary_command_buffer(PyObject* self, PyObject* arg)
{
    plCommandBuffer* ptCommandBuffer = PyCapsule_GetPointer(arg, "plCommandBuffer");
    gptStarter->submit_temporary_command_buffer(ptCommandBuffer);
    Py_RETURN_NONE;
}

PyObject*
starter_return_raw_command_buffer(PyObject* self, PyObject* arg)
{
    plCommandBuffer* ptCommandBuffer = PyCapsule_GetPointer(arg, "plCommandBuffer");
    gptStarter->return_raw_command_buffer(ptCommandBuffer);
    Py_RETURN_NONE;
}

plPythonIntConstantPair gatStarterIntPairs[] = {
    PL_ADD_INT_CONSTANT(PL_STARTER_FLAGS_NONE),
    PL_ADD_INT_CONSTANT(PL_STARTER_FLAGS_DRAW_EXT),
    PL_ADD_INT_CONSTANT(PL_STARTER_FLAGS_UI_EXT),
    PL_ADD_INT_CONSTANT(PL_STARTER_FLAGS_CONSOLE_EXT),
    PL_ADD_INT_CONSTANT(PL_STARTER_FLAGS_PROFILE_EXT),
    PL_ADD_INT_CONSTANT(PL_STARTER_FLAGS_STATS_EXT),
    PL_ADD_INT_CONSTANT(PL_STARTER_FLAGS_SHADER_EXT),
    PL_ADD_INT_CONSTANT(PL_STARTER_FLAGS_SCREEN_LOG_EXT),
    PL_ADD_INT_CONSTANT(PL_STARTER_FLAGS_GRAPHICS_EXT),
    PL_ADD_INT_CONSTANT(PL_STARTER_FLAGS_TOOLS_EXT),
    PL_ADD_INT_CONSTANT(PL_STARTER_FLAGS_DEPTH_BUFFER),
    PL_ADD_INT_CONSTANT(PL_STARTER_FLAGS_MSAA),
    PL_ADD_INT_CONSTANT(PL_STARTER_FLAGS_VSYNC_OFF),
    PL_ADD_INT_CONSTANT(PL_STARTER_FLAGS_REVERSE_Z),
    PL_ADD_INT_CONSTANT(PL_STARTER_FLAGS_ALL_EXTENSIONS)
};

static PyMethodDef gatplStarterICommands[] =
{
    {"begin_frame", (PyCFunction)starter_begin_frame, METH_NOARGS | METH_STATIC, NULL},
    {"initialize", (PyCFunction)starter_initialize, METH_VARARGS | METH_STATIC, NULL},
    {"finalize", (PyCFunction)starter_finalize, METH_NOARGS | METH_STATIC, NULL},
    {"cleanup", (PyCFunction)starter_cleanup, METH_NOARGS | METH_STATIC, NULL},
    {"resize", (PyCFunction)starter_resize, METH_NOARGS | METH_STATIC, NULL},
    {"end_frame", (PyCFunction)starter_end_frame, METH_NOARGS | METH_STATIC, NULL},
    {"get_foreground_layer", (PyCFunction)starter_get_foreground_layer, METH_NOARGS | METH_STATIC, NULL},
    {"get_background_layer", (PyCFunction)starter_get_background_layer, METH_NOARGS | METH_STATIC, NULL},
    {"get_device", (PyCFunction)starter_get_device, METH_NOARGS | METH_STATIC, NULL},
    {"get_swapchain", (PyCFunction)starter_get_swapchain, METH_NOARGS | METH_STATIC, NULL},
    {"get_render_pass", (PyCFunction)starter_get_render_pass, METH_NOARGS | METH_STATIC, NULL},
    {"begin_main_pass", (PyCFunction)starter_begin_main_pass, METH_NOARGS | METH_STATIC, NULL},
    {"end_main_pass", (PyCFunction)starter_end_main_pass, METH_NOARGS | METH_STATIC, NULL},
    {"get_command_buffer", (PyCFunction)starter_get_command_buffer, METH_NOARGS | METH_STATIC, NULL},
    {"get_temporary_command_buffer", (PyCFunction)starter_get_temporary_command_buffer, METH_NOARGS | METH_STATIC, NULL},
    {"get_raw_command_buffer", (PyCFunction)starter_get_raw_command_buffer, METH_NOARGS | METH_STATIC, NULL},
    {"submit_command_buffer", (PyCFunction)starter_submit_command_buffer, METH_O | METH_STATIC, NULL},
    {"submit_temporary_command_buffer", (PyCFunction)starter_submit_temporary_command_buffer, METH_O | METH_STATIC, NULL},
    {"return_raw_command_buffer", (PyCFunction)starter_return_raw_command_buffer, METH_O | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot gatplStarterISlots[] = {
    {Py_tp_methods, (void*)gatplStarterICommands},
    {0, 0}
};

static PyType_Spec plStarterISpec = {
    "pilotlight.plStarterI",
    sizeof(pyplStarterI),
    0,
    Py_TPFLAGS_DEFAULT,
    gatplStarterISlots
};