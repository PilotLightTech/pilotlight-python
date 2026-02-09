/*
   pilotlight.c
*/

/*
Index of this file:
// [SECTION] notes
// [SECTION] header mess
// [SECTION] includes
// [SECTION] forward declarations
// [SECTION] helper api implementation
// [SECTION] core module api
// [SECTION] python api registration
*/

//-----------------------------------------------------------------------------
// [SECTION] notes
//-----------------------------------------------------------------------------

/*
    Python binding functions are registered in the "gatCommands" array at the
    bottom of this file. Constants are registered at the bottom of the
    "PyInit_pilotlight" function.
*/

//-----------------------------------------------------------------------------
// [SECTION] header mess
//-----------------------------------------------------------------------------

#include "pl_dear_imgui_ext.cpp"

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "imgui.h"

#include "pl_graphics_ext.h"

bool pl_parse(const char* formatstring, const char** keywords, PyObject* args, PyObject* kwargs, const char* message, ...);

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

PyObject*
plImgui_initialize(PyObject* self, PyObject* args, PyObject* kwargs)
{

    // const plApiRegistryI* ptApiRegistry = pl__python_setup();
    // PyObject* ptApiRegistryCapsule = PyCapsule_GetPointer(ptApiRegistry, "plApiRegistryI", NULL);

    PyObject *pymodule = PyImport_ImportModule("pilotlight.pilotlight");
    if (!pymodule) {
        /* error handling here */
    }

    PyObject* ptCapsule = PyObject_GetAttrString(pymodule, "plApiRegistryICapsule");
    const plApiRegistryI* ptApiRegistry = (const plApiRegistryI*)PyCapsule_GetPointer(ptCapsule, "plApiRegistryI");
    pl_load_dear_imgui_ext(ptApiRegistry, false);

    PyObject* ptPythonDevice = nullptr;
    PyObject* ptPythonSwapchain = nullptr;
    uint32_t  uRenderPassHandle = 0;

    static const char* apcKeywords[] = {
        "device",
        "swapchain",
        "renderpass_handle",
        NULL,
    };

	if (!pl_parse("OOI", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonDevice, &ptPythonSwapchain, &uRenderPassHandle))
		return NULL;

    plDevice* ptDevice = (plDevice*)PyCapsule_GetPointer(ptPythonDevice, "plDevice");
    plSwapchain* ptSwapchain = (plSwapchain*)PyCapsule_GetPointer(ptPythonSwapchain, "plSwapchain");
    plRenderPassHandle tHandle = {};
    tHandle.uData = uRenderPassHandle;

    pl_imgui_initialize(ptDevice, ptSwapchain, tHandle);

    // ImPlot::SetCurrentContext((ImPlotContext*)ptDataRegistry->get_data("implot"));
    ImGuiIO& tImGuiIO = ImGui::GetIO();
    tImGuiIO.IniFilename = nullptr;

    Py_RETURN_NONE;
}

PyObject*
plImgui_new_frame(PyObject* self, PyObject* args, PyObject* kwargs)
{

    PyObject* ptPythonDevice = nullptr;
    uint32_t  uRenderPassHandle = 0;

    static const char* apcKeywords[] = {
        "device",
        "renderpass_handle",
        NULL,
    };

	if (!pl_parse("OI", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonDevice, &uRenderPassHandle))
		return NULL;

    plDevice* ptDevice = (plDevice*)PyCapsule_GetPointer(ptPythonDevice, "plDevice");
    plRenderPassHandle tHandle = {};
    tHandle.uData = uRenderPassHandle;

    pl_imgui_new_frame(ptDevice, tHandle);

    Py_RETURN_NONE;
}

PyObject*
plImgui_render(PyObject* self, PyObject* args, PyObject* kwargs)
{

    PyObject* ptPythonRenderEncoder = nullptr;

    static const char* apcKeywords[] = {
        "render_encoder",
        NULL,
    };

	if (!pl_parse("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonRenderEncoder))
		return NULL;

    plRenderEncoder* ptRenderEncoder = (plRenderEncoder*)PyCapsule_GetPointer(ptPythonRenderEncoder, "plRenderEncoder");

    pl_imgui_render(ptRenderEncoder, gptGfx->get_encoder_command_buffer(ptRenderEncoder));

    Py_RETURN_NONE;
}

PyObject*
plImgui_cleanup(PyObject* self, PyObject* args, PyObject* kwargs)
{
    pl_imgui_cleanup();
    Py_RETURN_NONE;
}

PyObject*
plImgui_test(PyObject* self, PyObject* args, PyObject* kwargs)
{
    ImGui::ShowDemoWindow();
    ImPlot::ShowDemoWindow();
    Py_RETURN_NONE;
}


#define PL_PYTHON_COMMAND(ARG, FLAGS, DOCS) {#ARG, (PyCFunction)ARG, FLAGS, DOCS}

static PyMethodDef gatCommands[] =
{
    PL_PYTHON_COMMAND(plImgui_initialize, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plImgui_new_frame, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plImgui_render, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plImgui_cleanup, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plImgui_test, METH_VARARGS | METH_KEYWORDS, NULL),


    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
PyInit_imgui(void)
{

    static PyModuleDef tPilotLightModule = {0};
	tPilotLightModule = {
		PyModuleDef_HEAD_INIT,
        "imgui",
        NULL,
        -1,
        gatCommands,
		NULL,
        NULL,
        NULL,
        NULL
	};

	PyObject* ptModule = PyModule_Create(&tPilotLightModule);
	if (ptModule == NULL)
    {
		return NULL;
    }
    return ptModule;
}

bool
pl_parse(const char* formatstring, const char** keywords, PyObject* args, PyObject* kwargs, const char* message, ...)
{

    bool check = true;

    va_list arguments;
    va_start(arguments, message);
    if (!PyArg_VaParseTupleAndKeywords(args, kwargs, formatstring, (char * const *)keywords, arguments))
    {
        check = false;
    }

    va_end(arguments);

    // if (!check)
    //     mvThrowPythonError(mvErrorCode::mvNone, "Error parsing Dear PyGui command: " + std::string(message));

    return check;
}