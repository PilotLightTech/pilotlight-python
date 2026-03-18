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
#define PL_MATH_INCLUDE_FUNCTIONS
#include "pl_math.h"

#include "pl_graphics_ext.h"

bool pl_parse(const char* formatstring, const char** keywords, PyObject* args, PyObject* kwargs, const char* message, ...);
static ImVec2 pl__get_vec2_from_python(PyObject* ptValue);

typedef struct _plPythonIntConstantPair
{
   const char* pcName;
   int         iValue;
} plPythonIntConstantPair;

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
plImgui_StyleColorsDark(PyObject* self)
{
    ImGui::StyleColorsDark();
    Py_RETURN_NONE;
}

PyObject*
plImgui_StyleColorsLight(PyObject* self)
{
    ImGui::StyleColorsLight();
    Py_RETURN_NONE;
}

PyObject*
plImgui_StyleColorsClassic(PyObject* self)
{
    ImGui::StyleColorsClassic();
    Py_RETURN_NONE;
}

PyObject*
plImgui_cleanup(PyObject* self, PyObject* args, PyObject* kwargs)
{
    pl_imgui_cleanup();
    Py_RETURN_NONE;
}

PyObject*
plImGui_ShowDemoWindow(PyObject* self, PyObject* arg)
{
    bool* ptShow = NULL;
    if(!Py_IsNone(arg))
    {
        ptShow = (bool*)PyCapsule_GetPointer(arg, "pb");
    }
    ImGui::ShowDemoWindow(ptShow);
    Py_RETURN_NONE;
}

PyObject*
plImPlot_ShowDemoWindow(PyObject* self, PyObject* arg)
{
    bool* ptShow = NULL;
    if(!Py_IsNone(arg))
    {
        ptShow = (bool*)PyCapsule_GetPointer(arg, "pb");
    }
    ImPlot::ShowDemoWindow(ptShow);
    Py_RETURN_NONE;
}

PyObject*
plImgui_Begin(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "name",
        "open",
        "flags",
        nullptr,
    };
    const char* pcText = nullptr;
    PyObject* ptPointer = nullptr;
    int iFlags = 0;
	if (!pl_parse("s|Oi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcText, &ptPointer, &iFlags))
		return nullptr;
    
    bool* pbOpen = nullptr;
    if(!Py_IsNone(ptPointer))
        pbOpen = (bool*)PyCapsule_GetPointer(ptPointer, "pb");

    return PyBool_FromLong(ImGui::Begin(pcText, pbOpen, iFlags));
}

PyObject*
plImgui_End(PyObject* self)
{
    ImGui::End();
    Py_RETURN_NONE;
}

PyObject*
plImgui_Button(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "label",
        "size",
        nullptr,
    };
    const char* pcLabel = nullptr;
    PyObject* ptSize = nullptr;
	if (!pl_parse("s|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptSize))
		return nullptr;
    
    ImVec2 tSize = {};
    if(!Py_IsNone(ptSize))
        tSize = pl__get_vec2_from_python(ptSize);

    return PyBool_FromLong(ImGui::Button(pcLabel, tSize));
}

PyObject*
plImgui_BeginMenuBar(PyObject* self)
{
    return PyBool_FromLong(ImGui::BeginMenuBar());
}

PyObject*
plImgui_BeginMainMenuBar(PyObject* self)
{
    return PyBool_FromLong(ImGui::BeginMainMenuBar());
}

PyObject*
plImgui_EndMenuBar(PyObject* self)
{
    ImGui::EndMenuBar();
    Py_RETURN_NONE;
}

PyObject*
plImgui_EndMainMenuBar(PyObject* self)
{
    ImGui::EndMainMenuBar();
    Py_RETURN_NONE;
}

PyObject*
plImgui_EndMenu(PyObject* self)
{
    ImGui::EndMenu();
    Py_RETURN_NONE;
}

PyObject*
plImgui_BeginMenu(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "label",
        "enabled",
        nullptr,
    };
    const char* pcLabel = nullptr;
    int bEnabled = true;
	if (!pl_parse("s|p", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &bEnabled))
		return nullptr;
    
    return PyBool_FromLong(ImGui::BeginMenu(pcLabel, bEnabled));
}

PyObject*
plImgui_MenuItem(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "label",
        "shortcut",
        "selected",
        "enabled",
        "selected_pointer",
        nullptr,
    };
    const char* pcLabel = nullptr;
    const char* pcShortcut = nullptr;
    int bEnabled = false;
    int bSelected = true;
    PyObject* ptPointer = nullptr;
	if (!pl_parse("s|spp$O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &pcShortcut, &bSelected, &bEnabled, &ptPointer))
		return nullptr;
    
    bool* pbSelected = nullptr;
    if(!Py_IsNone(ptPointer))
        pbSelected = (bool*)PyCapsule_GetPointer(ptPointer, "pb");

    if(pbSelected)
        return PyBool_FromLong(ImGui::MenuItem(pcLabel, pcShortcut, pbSelected, bEnabled));
    return PyBool_FromLong(ImGui::MenuItem(pcLabel, pcShortcut, bSelected, bEnabled));
}

PyObject*
plImgui_Checkbox(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "label",
        "value_pointer",
        nullptr,
    };
    const char* pcLabel = nullptr;
    PyObject* ptPointer = nullptr;
	if (!pl_parse("sO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer))
		return nullptr;
    
    bool* pbSelected = (bool*)PyCapsule_GetPointer(ptPointer, "pb");

    return PyBool_FromLong(ImGui::Checkbox(pcLabel, pbSelected));
}


#define PL_PYTHON_COMMAND(ARG, FLAGS, DOCS) {#ARG, (PyCFunction)ARG, FLAGS, DOCS}
#define PL_ADD_INT_CONSTANT(X_ARG) {#X_ARG, X_ARG}

static PyMethodDef gatCommands[] =
{
    // imgui
    PL_PYTHON_COMMAND(plImgui_initialize, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plImgui_new_frame, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plImgui_render, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plImgui_cleanup, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plImGui_ShowDemoWindow, METH_O, NULL),
    PL_PYTHON_COMMAND(plImgui_Begin, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plImgui_End, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plImgui_StyleColorsDark, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plImgui_StyleColorsLight, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plImgui_StyleColorsClassic, METH_NOARGS, NULL),

    // imgui widgets
    PL_PYTHON_COMMAND(plImgui_Button, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui menus
    PL_PYTHON_COMMAND(plImgui_BeginMenuBar, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plImgui_BeginMainMenuBar, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plImgui_EndMenuBar, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plImgui_EndMainMenuBar, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plImgui_EndMenu, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plImgui_BeginMenu, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plImgui_MenuItem, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plImgui_Checkbox, METH_VARARGS | METH_KEYWORDS, NULL),

    // implot
    PL_PYTHON_COMMAND(plImPlot_ShowDemoWindow, METH_O, NULL),

    {NULL, NULL, 0, NULL}
};

static plPythonIntConstantPair gatImguiIntPairs[] = {
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_None),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoTitleBar),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoResize),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoMove),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoScrollbar),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoScrollWithMouse),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoCollapse),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_AlwaysAutoResize),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoBackground),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoSavedSettings),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoMouseInputs),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_MenuBar),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_HorizontalScrollbar),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoFocusOnAppearing),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoBringToFrontOnFocus),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_AlwaysVerticalScrollbar),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_AlwaysHorizontalScrollbar),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoNavInputs),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoNavFocus),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_UnsavedDocument),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoDocking),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoNav),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoDecoration),
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoInputs)

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

    for(uint32_t i = 0; i < PL_ARRAYSIZE(gatImguiIntPairs); i++)
        PyModule_AddIntConstant(ptModule, gatImguiIntPairs[i].pcName, gatImguiIntPairs[i].iValue);

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

static ImVec2
pl__get_vec2_from_python(PyObject* ptValue)
{
    ImVec2 tResult = {};

    if (PyTuple_Check(ptValue))
    {
        Py_ssize_t pySize = PyTuple_Size(ptValue);
        pySize = pl_min(pySize, 2);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            tResult[i] = (float)PyFloat_AsDouble(PyTuple_GetItem(ptValue, i));
        }
    }

    else if (PyList_Check(ptValue))
    {
        Py_ssize_t pySize = PyList_Size(ptValue);
        pySize = pl_min(pySize, 2);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            tResult[i] = (float)PyFloat_AsDouble(PyList_GetItem(ptValue, i));
        }
    }

    // else if (PyObject_CheckBuffer(ptValue))
    // {
    //     Py_buffer buffer_info;

    //     if (!PyObject_GetBuffer(ptValue, &buffer_info,
    //                             PyBUF_CONTIG_RO | PyBUF_FORMAT))
    //     {

    //         auto BufferViewer = BufferViewFunctionsFloat(buffer_info);
    //         items.reserve(buffer_info.len / buffer_info.itemsize);

    //         for (Py_ssize_t i = 0; i < buffer_info.len / buffer_info.itemsize; ++i)
    //         {
    //             items.emplace_back(BufferViewer(buffer_info, i));
    //         }
    //     }
    //     PyBuffer_Release(&buffer_info);
    // }
    // else
    //     mvThrowPythonError(mvErrorCode::mvWrongType, "Python value error. Must be List[float].");

    return tResult;
}