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
plStarterI_initialize(PyObject* self, PyObject* args, PyObject* kwargs)
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
plStarterI_finalize(PyObject* self, PyObject* args, PyObject* kwargs)
{
    gptStarter->finalize();
    Py_RETURN_NONE;
}

PyObject*
plStarterI_cleanup(PyObject* self, PyObject* args, PyObject* kwargs)
{
    gptStarter->cleanup();
    Py_RETURN_NONE;
}

PyObject*
plStarterI_resize(PyObject* self, PyObject* args, PyObject* kwargs)
{
    gptStarter->resize();
    Py_RETURN_NONE;
}

PyObject*
plStarterI_begin_frame(PyObject* self, PyObject* args, PyObject* kwargs)
{
    bool bResult = gptStarter->begin_frame(); // must be called once at the beginning of a frame
    return PyBool_FromLong(bResult);
}

PyObject*
plStarterI_end_frame(PyObject* self, PyObject* args, PyObject* kwargs)
{
    gptStarter->end_frame();
    Py_RETURN_NONE;
}

PyObject*
plStarterI_get_foreground_layer(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plDrawLayer2D* ptFGLayer = gptStarter->get_foreground_layer();

    return PyCapsule_New(ptFGLayer, "plDrawLayer2D", NULL);
}

PyObject*
plStarterI_get_background_layer(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plDrawLayer2D* ptBGLayer = gptStarter->get_background_layer();

    return PyCapsule_New(ptBGLayer, "plDrawLayer2D", NULL);
}

PyObject*
plStarterI_get_device(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plDevice* ptDevice = gptStarter->get_device();
    return PyCapsule_New(ptDevice, "plDevice", NULL);
}

PyObject*
plStarterI_get_swapchain(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plSwapchain* ptSwapchain = gptStarter->get_swapchain();
    return PyCapsule_New(ptSwapchain, "plSwapchain", NULL);
}

PyObject*
plStarterI_get_render_pass(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plRenderPassHandle tHandle = gptStarter->get_render_pass();
    return Py_BuildValue("K", tHandle.uData);
}

PyObject*
plStarterI_begin_main_pass(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plRenderEncoder* ptEncoder = gptStarter->begin_main_pass();
    return PyCapsule_New(ptEncoder, "plRenderEncoder", NULL);
}

PyObject*
plStarterI_end_main_pass(PyObject* self, PyObject* args, PyObject* kwargs)
{
    gptStarter->end_main_pass();
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