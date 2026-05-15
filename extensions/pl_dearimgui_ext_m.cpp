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
#include "pl_ds.h"

bool pl_parse(const char* formatstring, const char** keywords, PyObject* args, PyObject* kwargs, const char* message, ...);
static ImVec2 pl__get_vec2_from_python(PyObject* ptValue);
static ImVec4 pl__get_vec4_from_python(PyObject* ptValue);

typedef struct _plPythonIntConstantPair
{
   const char* pcName;
   int         iValue;
} plPythonIntConstantPair;


static PyObject* gptplDearImGuiIType = nullptr;
static PyObject* gptImGuiType = nullptr;
static PyObject* gptImPlotType = nullptr;

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

PyObject*
dear_imgui_initialize(PyObject* self, PyObject* args, PyObject* kwargs)
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

    pl_dear_imgui_initialize(ptDevice, ptSwapchain, tHandle);

    // ImPlot::SetCurrentContext((ImPlotContext*)ptDataRegistry->get_data("implot"));
    ImGuiIO& tImGuiIO = ImGui::GetIO();
    tImGuiIO.IniFilename = nullptr;

    Py_RETURN_NONE;
}

PyObject*
dear_imgui_new_frame(PyObject* self, PyObject* args, PyObject* kwargs)
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

    pl_dear_imgui_new_frame(ptDevice, tHandle);

    Py_RETURN_NONE;
}

PyObject*
dear_imgui_render(PyObject* self, PyObject* args, PyObject* kwargs)
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

    pl_dear_imgui_render(ptRenderEncoder, gptGfx->get_encoder_command_buffer(ptRenderEncoder));

    Py_RETURN_NONE;
}

PyObject*
StyleColorsDark(PyObject* self)
{
    ImGui::StyleColorsDark();
    Py_RETURN_NONE;
}

PyObject*
StyleColorsLight(PyObject* self)
{
    ImGui::StyleColorsLight();
    Py_RETURN_NONE;
}

PyObject*
StyleColorsClassic(PyObject* self)
{
    ImGui::StyleColorsClassic();
    Py_RETURN_NONE;
}

PyObject*
dear_imgui_cleanup(PyObject* self, PyObject* args, PyObject* kwargs)
{
    pl_dear_imgui_cleanup();
    Py_RETURN_NONE;
}

PyObject*
ShowDemoWindow(PyObject* self, PyObject* arg)
{
    bool* ptShow = NULL;
    if(!Py_IsNone(arg))
    {
        ptShow = (bool*)PyCapsule_GetPointer(arg, "plBoolPointer");
    }
    ImGui::ShowDemoWindow(ptShow);
    Py_RETURN_NONE;
}

PyObject*
ImPlot_ShowDemoWindow(PyObject* self, PyObject* arg)
{
    bool* ptShow = NULL;
    if(!Py_IsNone(arg))
    {
        ptShow = (bool*)PyCapsule_GetPointer(arg, "plBoolPointer");
    }
    ImPlot::ShowDemoWindow(ptShow);
    Py_RETURN_NONE;
}

PyObject*
Begin(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "name",
        "open",
        "flags",
        nullptr,
    };
    const char* pcText = nullptr;
    PyObject* ptPointer = Py_None;
    int iFlags = 0;
	if (!pl_parse("s|Oi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcText, &ptPointer, &iFlags))
		return nullptr;
    
    bool* pbOpen = nullptr;
    if(!Py_IsNone(ptPointer))
        pbOpen = (bool*)PyCapsule_GetPointer(ptPointer, "plBoolPointer");

    return PyBool_FromLong(ImGui::Begin(pcText, pbOpen, iFlags));
}

PyObject*
End(PyObject* self)
{
    ImGui::End();
    Py_RETURN_NONE;
}

PyObject*
Button(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "label",
        "size",
        nullptr,
    };
    const char* pcLabel = nullptr;
    PyObject* ptSize = Py_None;
	if (!pl_parse("s|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptSize))
		return nullptr;
    
    ImVec2 tSize = {};
    if(!Py_IsNone(ptSize))
        tSize = pl__get_vec2_from_python(ptSize);

    return PyBool_FromLong(ImGui::Button(pcLabel, tSize));
}

PyObject*
BeginMenuBar(PyObject* self)
{
    return PyBool_FromLong(ImGui::BeginMenuBar());
}

PyObject*
BeginMainMenuBar(PyObject* self)
{
    return PyBool_FromLong(ImGui::BeginMainMenuBar());
}

PyObject*
EndMenuBar(PyObject* self)
{
    ImGui::EndMenuBar();
    Py_RETURN_NONE;
}

PyObject*
EndMainMenuBar(PyObject* self)
{
    ImGui::EndMainMenuBar();
    Py_RETURN_NONE;
}

PyObject*
EndMenu(PyObject* self)
{
    ImGui::EndMenu();
    Py_RETURN_NONE;
}

PyObject*
BeginMenu(PyObject* self, PyObject* args, PyObject* kwargs)
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
MenuItem(PyObject* self, PyObject* args, PyObject* kwargs)
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
    int bEnabled = true;
    int bSelected = false;
    PyObject* ptPointer = Py_None;
	if (!pl_parse("s|spp$O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &pcShortcut, &bSelected, &bEnabled, &ptPointer))
		return nullptr;
    
    bool bbSelected = (bool)bSelected;
    bool* pbSelected = &bbSelected;
    if(!Py_IsNone(ptPointer))
        pbSelected = (bool*)PyCapsule_GetPointer(ptPointer, "plBoolPointer");

    bool bActivated = ImGui::MenuItem(pcLabel, pcShortcut, pbSelected, bEnabled);
    return Py_BuildValue("(pp)", bActivated, bbSelected);
}

PyObject*
Checkbox(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "label",
        "value_pointer",
        nullptr,
    };
    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
	if (!pl_parse("sO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer))
		return nullptr;
    
    bool* pbSelected = (bool*)PyCapsule_GetPointer(ptPointer, "plBoolPointer");

    return PyBool_FromLong(ImGui::Checkbox(pcLabel, pbSelected));
}

PyObject*
DragFloat(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "speed", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fSpeed = 1.0f;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse("sO|fffsi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fSpeed, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    float* pfValue = (float*)PyCapsule_GetPointer(ptPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::DragFloat(pcLabel, pfValue, fSpeed, fMin, fMax, pcFormat, iFlags));
}

PyObject*
DragFloat2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "speed", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fSpeed = 1.0f;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse("sO|fffsi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fSpeed, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    float* pfValue = (float*)PyCapsule_GetPointer(ptPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::DragFloat2(pcLabel, pfValue, fSpeed, fMin, fMax, pcFormat, iFlags));
}

PyObject*
DragFloat3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "speed", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fSpeed = 1.0f;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse("sO|fffsi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fSpeed, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    float* pfValue = (float*)PyCapsule_GetPointer(ptPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::DragFloat3(pcLabel, pfValue, fSpeed, fMin, fMax, pcFormat, iFlags));
}

PyObject*
DragFloat4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "speed", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fSpeed = 1.0f;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse("sO|fffsi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fSpeed, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    float* pfValue = (float*)PyCapsule_GetPointer(ptPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::DragFloat4(pcLabel, pfValue, fSpeed, fMin, fMax, pcFormat, iFlags));
}

PyObject*
DragFloatRange2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "current_min_pointer", "current_max_pointer", "speed", "min", "max",
        "format", "format_max", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptMinPointer = Py_None;
    PyObject* ptMaxPointer = Py_None;
    float fSpeed = 1.0f;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    const char* pcFormatMax = nullptr;
    int iFlags = 0;

    if(!pl_parse("sOO|fffzzi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptMinPointer, &ptMaxPointer, &fSpeed, &fMin, &fMax,
        &pcFormat, &pcFormatMax, &iFlags))
        return nullptr;

    float* pfCurrentMin = (float*)PyCapsule_GetPointer(ptMinPointer, "plFloatPointer");
    float* pfCurrentMax = (float*)PyCapsule_GetPointer(ptMaxPointer, "plFloatPointer");

    return PyBool_FromLong(ImGui::DragFloatRange2(
        pcLabel, pfCurrentMin, pfCurrentMax, fSpeed, fMin, fMax, pcFormat, pcFormatMax, iFlags));
}

PyObject*
DragInt(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "speed", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fSpeed = 1.0f;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    int iFlags = 0;

    if(!pl_parse("sO|fiisi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fSpeed, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    int* piValue = (int*)PyCapsule_GetPointer(ptPointer, "plIntPointer");
    return PyBool_FromLong(ImGui::DragInt(pcLabel, piValue, fSpeed, iMin, iMax, pcFormat, iFlags));
}

PyObject*
DragInt2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "speed", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fSpeed = 1.0f;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    int iFlags = 0;

    if(!pl_parse("sO|fiisi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fSpeed, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    int* piValue = (int*)PyCapsule_GetPointer(ptPointer, "plIntPointer");
    return PyBool_FromLong(ImGui::DragInt2(pcLabel, piValue, fSpeed, iMin, iMax, pcFormat, iFlags));
}

PyObject*
DragInt3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "speed", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fSpeed = 1.0f;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    int iFlags = 0;

    if(!pl_parse("sO|fiisi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fSpeed, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    int* piValue = (int*)PyCapsule_GetPointer(ptPointer, "plIntPointer");
    return PyBool_FromLong(ImGui::DragInt3(pcLabel, piValue, fSpeed, iMin, iMax, pcFormat, iFlags));
}

PyObject*
DragInt4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "speed", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fSpeed = 1.0f;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    int iFlags = 0;

    if(!pl_parse("sO|fiisi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fSpeed, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    int* piValue = (int*)PyCapsule_GetPointer(ptPointer, "plIntPointer");
    return PyBool_FromLong(ImGui::DragInt4(pcLabel, piValue, fSpeed, iMin, iMax, pcFormat, iFlags));
}

PyObject*
DragIntRange2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "current_min_pointer", "current_max_pointer", "speed", "min", "max",
        "format", "format_max", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptMinPointer = Py_None;
    PyObject* ptMaxPointer = Py_None;
    float fSpeed = 1.0f;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    const char* pcFormatMax = nullptr;
    int iFlags = 0;

    if(!pl_parse("sOO|fiizzi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptMinPointer, &ptMaxPointer, &fSpeed, &iMin, &iMax,
        &pcFormat, &pcFormatMax, &iFlags))
        return nullptr;

    int* piCurrentMin = (int*)PyCapsule_GetPointer(ptMinPointer, "plIntPointer");
    int* piCurrentMax = (int*)PyCapsule_GetPointer(ptMaxPointer, "plIntPointer");

    return PyBool_FromLong(ImGui::DragIntRange2(
        pcLabel, piCurrentMin, piCurrentMax, fSpeed, iMin, iMax, pcFormat, pcFormatMax, iFlags));
}

PyObject*
SliderFloat(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse("sOff|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    float* pfValue = (float*)PyCapsule_GetPointer(ptPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::SliderFloat(pcLabel, pfValue, fMin, fMax, pcFormat, iFlags));
}

PyObject*
SliderFloat2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse("sOff|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    float* pfValue = (float*)PyCapsule_GetPointer(ptPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::SliderFloat2(pcLabel, pfValue, fMin, fMax, pcFormat, iFlags));
}

PyObject*
SliderFloat3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse("sOff|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    float* pfValue = (float*)PyCapsule_GetPointer(ptPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::SliderFloat3(pcLabel, pfValue, fMin, fMax, pcFormat, iFlags));
}

PyObject*
SliderFloat4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse("sOff|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    float* pfValue = (float*)PyCapsule_GetPointer(ptPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::SliderFloat4(pcLabel, pfValue, fMin, fMax, pcFormat, iFlags));
}

PyObject*
SliderAngle(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "rad_pointer", "degrees_min", "degrees_max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fDegreesMin = -360.0f;
    float fDegreesMax = 360.0f;
    const char* pcFormat = "%.0f deg";
    int iFlags = 0;

    if(!pl_parse("sO|ffsi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fDegreesMin, &fDegreesMax, &pcFormat, &iFlags))
        return nullptr;

    float* pfRad = (float*)PyCapsule_GetPointer(ptPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::SliderAngle(pcLabel, pfRad, fDegreesMin, fDegreesMax, pcFormat, iFlags));
}

PyObject*
SliderInt(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    int iFlags = 0;

    if(!pl_parse("sOii|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    int* piValue = (int*)PyCapsule_GetPointer(ptPointer, "plIntPointer");
    return PyBool_FromLong(ImGui::SliderInt(pcLabel, piValue, iMin, iMax, pcFormat, iFlags));
}

PyObject*
SliderInt2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    int iFlags = 0;

    if(!pl_parse("sOii|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    int* piValue = (int*)PyCapsule_GetPointer(ptPointer, "plIntPointer");
    return PyBool_FromLong(ImGui::SliderInt2(pcLabel, piValue, iMin, iMax, pcFormat, iFlags));
}

PyObject*
SliderInt3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    int iFlags = 0;

    if(!pl_parse("sOii|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    int* piValue = (int*)PyCapsule_GetPointer(ptPointer, "plIntPointer");
    return PyBool_FromLong(ImGui::SliderInt3(pcLabel, piValue, iMin, iMax, pcFormat, iFlags));
}

PyObject*
SliderInt4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    int iFlags = 0;

    if(!pl_parse("sOii|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    int* piValue = (int*)PyCapsule_GetPointer(ptPointer, "plIntPointer");
    return PyBool_FromLong(ImGui::SliderInt4(pcLabel, piValue, iMin, iMax, pcFormat, iFlags));
}

PyObject*
VSliderFloat(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "size", "value_pointer", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptSize = Py_None;
    PyObject* ptPointer = Py_None;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse("sOOff|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptSize, &ptPointer, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    ImVec2 tSize = pl__get_vec2_from_python(ptSize);
    float* pfValue = (float*)PyCapsule_GetPointer(ptPointer, "plFloatPointer");

    return PyBool_FromLong(ImGui::VSliderFloat(pcLabel, tSize, pfValue, fMin, fMax, pcFormat, iFlags));
}

PyObject*
VSliderInt(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "size", "value_pointer", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptSize = Py_None;
    PyObject* ptPointer = Py_None;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    int iFlags = 0;

    if(!pl_parse("sOOii|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptSize, &ptPointer, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    ImVec2 tSize = pl__get_vec2_from_python(ptSize);
    int* piValue = (int*)PyCapsule_GetPointer(ptPointer, "plIntPointer");

    return PyBool_FromLong(ImGui::VSliderInt(pcLabel, tSize, piValue, iMin, iMax, pcFormat, iFlags));
}

PyObject*
InputText(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptByteObject = nullptr;
    int iFlags = 0;

    if(!pl_parse("sY|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptByteObject, &iFlags))
        return nullptr;

    char* pcBuffer = PyByteArray_AsString(ptByteObject);
    Py_ssize_t szBufferSize = PyByteArray_Size(ptByteObject);

    if(!pcBuffer || szBufferSize <= 0)
        return nullptr;

    return PyBool_FromLong(ImGui::InputText(pcLabel, pcBuffer, (size_t)szBufferSize, iFlags));
}

PyObject*
InputTextMultiline(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "size", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptByteObject = nullptr;
    PyObject* ptSize = Py_None;
    int iFlags = 0;

    if(!pl_parse("sY|Oi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptByteObject, &ptSize, &iFlags))
        return nullptr;

    ImVec2 tSize = {};
    if(!Py_IsNone(ptSize))
        tSize = pl__get_vec2_from_python(ptSize);

    char* pcBuffer = PyByteArray_AsString(ptByteObject);
    Py_ssize_t szBufferSize = PyByteArray_Size(ptByteObject);

    if(!pcBuffer || szBufferSize <= 0)
        return nullptr;

    return PyBool_FromLong(ImGui::InputTextMultiline(pcLabel, pcBuffer, (size_t)szBufferSize, tSize, iFlags));
}

PyObject*
InputTextWithHint(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "hint", "value", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    const char* pcHint = nullptr;
    PyObject* ptByteObject = nullptr;
    int iFlags = 0;

    if(!pl_parse("ssY|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &pcHint, &ptByteObject, &iFlags))
        return nullptr;

    char* pcBuffer = PyByteArray_AsString(ptByteObject);
    Py_ssize_t szBufferSize = PyByteArray_Size(ptByteObject);

    if(!pcBuffer || szBufferSize <= 0)
        return nullptr;

    return PyBool_FromLong(ImGui::InputTextWithHint(pcLabel, pcHint, pcBuffer, (size_t)szBufferSize, iFlags));
}

PyObject*
InputFloat(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "step", "step_fast", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fStep = 0.0f;
    float fStepFast = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse("sO|ffsi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fStep, &fStepFast, &pcFormat, &iFlags))
        return nullptr;

    float* pfValue = (float*)PyCapsule_GetPointer(ptPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::InputFloat(pcLabel, pfValue, fStep, fStepFast, pcFormat, iFlags));
}

PyObject*
InputFloat2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse("sO|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &pcFormat, &iFlags))
        return nullptr;

    float* pfValue = (float*)PyCapsule_GetPointer(ptPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::InputFloat2(pcLabel, pfValue, pcFormat, iFlags));
}

PyObject*
InputFloat3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse("sO|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &pcFormat, &iFlags))
        return nullptr;

    float* pfValue = (float*)PyCapsule_GetPointer(ptPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::InputFloat3(pcLabel, pfValue, pcFormat, iFlags));
}

PyObject*
InputFloat4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse("sO|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &pcFormat, &iFlags))
        return nullptr;

    float* pfValue = (float*)PyCapsule_GetPointer(ptPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::InputFloat4(pcLabel, pfValue, pcFormat, iFlags));
}

PyObject*
InputInt(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "step", "step_fast", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    int iStep = 1;
    int iStepFast = 100;
    int iFlags = 0;

    if(!pl_parse("sO|iii", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &iStep, &iStepFast, &iFlags))
        return nullptr;

    int* piValue = (int*)PyCapsule_GetPointer(ptPointer, "plIntPointer");
    return PyBool_FromLong(ImGui::InputInt(pcLabel, piValue, iStep, iStepFast, iFlags));
}

PyObject*
InputInt2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    int iFlags = 0;

    if(!pl_parse("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &iFlags))
        return nullptr;

    int* piValue = (int*)PyCapsule_GetPointer(ptPointer, "plIntPointer");
    return PyBool_FromLong(ImGui::InputInt2(pcLabel, piValue, iFlags));
}

PyObject*
InputInt3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    int iFlags = 0;

    if(!pl_parse("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &iFlags))
        return nullptr;

    int* piValue = (int*)PyCapsule_GetPointer(ptPointer, "plIntPointer");
    return PyBool_FromLong(ImGui::InputInt3(pcLabel, piValue, iFlags));
}

PyObject*
InputInt4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    int iFlags = 0;

    if(!pl_parse("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &iFlags))
        return nullptr;

    int* piValue = (int*)PyCapsule_GetPointer(ptPointer, "plIntPointer");
    return PyBool_FromLong(ImGui::InputInt4(pcLabel, piValue, iFlags));
}

PyObject*
InputDouble(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "step", "step_fast", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    double dStep = 0.0;
    double dStepFast = 0.0;
    const char* pcFormat = "%.6f";
    int iFlags = 0;

    if(!pl_parse("sO|ddsi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &dStep, &dStepFast, &pcFormat, &iFlags))
        return nullptr;

    double* pdValue = (double*)PyCapsule_GetPointer(ptPointer, "plDoublePointer");
    return PyBool_FromLong(ImGui::InputDouble(pcLabel, pdValue, dStep, dStepFast, pcFormat, iFlags));
}

PyObject*
TextUnformatted(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "text",
        nullptr,
    };

    const char* pcText = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcText))
        return nullptr;

    ImGui::TextUnformatted(pcText);
    Py_RETURN_NONE;
}

PyObject*
Text(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "text",
        nullptr,
    };

    const char* pcText = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcText))
        return nullptr;

    ImGui::TextUnformatted(pcText);
    Py_RETURN_NONE;
}

PyObject*
TextColored(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "color",
        "text",
        nullptr,
    };

    PyObject* ptColor = Py_None;
    const char* pcText = nullptr;

    if(!pl_parse("Os", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptColor, &pcText))
        return nullptr;

    ImVec4 tColor = {};
    if(!Py_IsNone(ptColor))
    {
        // assumes you add this helper like your Vec2 helper
        tColor = pl__get_vec4_from_python(ptColor);
    }

    ImGui::TextColored(tColor, "%s", pcText);
    Py_RETURN_NONE;
}

PyObject*
TextDisabled(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "text",
        nullptr,
    };

    const char* pcText = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcText))
        return nullptr;

    ImGui::TextDisabled("%s", pcText);
    Py_RETURN_NONE;
}

PyObject*
TextWrapped(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "text",
        nullptr,
    };

    const char* pcText = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcText))
        return nullptr;

    ImGui::TextWrapped("%s", pcText);
    Py_RETURN_NONE;
}

PyObject*
LabelText(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label",
        "text",
        nullptr,
    };

    const char* pcLabel = nullptr;
    const char* pcText = nullptr;

    if(!pl_parse("ss", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &pcText))
        return nullptr;

    ImGui::LabelText(pcLabel, "%s", pcText);
    Py_RETURN_NONE;
}

PyObject*
BulletText(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "text",
        nullptr,
    };

    const char* pcText = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcText))
        return nullptr;

    ImGui::BulletText("%s", pcText);
    Py_RETURN_NONE;
}

PyObject*
SeparatorText(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label",
        nullptr,
    };

    const char* pcLabel = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel))
        return nullptr;

    ImGui::SeparatorText(pcLabel);
    Py_RETURN_NONE;
}

PyObject*
Separator(PyObject* self)
{
    ImGui::Separator();
    Py_RETURN_NONE;
}

PyObject*
SameLine(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "offset_from_start_x",
        "spacing",
        nullptr,
    };

    float fOffset = 0.0f;
    float fSpacing = -1.0f;

    if(!pl_parse("|ff", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &fOffset, &fSpacing))
        return nullptr;

    ImGui::SameLine(fOffset, fSpacing);
    Py_RETURN_NONE;
}

PyObject*
NewLine(PyObject* self)
{
    ImGui::NewLine();
    Py_RETURN_NONE;
}

PyObject*
Spacing(PyObject* self)
{
    ImGui::Spacing();
    Py_RETURN_NONE;
}

PyObject*
Dummy(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "size",
        nullptr,
    };

    PyObject* ptSize = Py_None;

    if(!pl_parse("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptSize))
        return nullptr;

    ImVec2 tSize = {};
    if(!Py_IsNone(ptSize))
        tSize = pl__get_vec2_from_python(ptSize);

    ImGui::Dummy(tSize);
    Py_RETURN_NONE;
}

PyObject*
Indent(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "indent_w",
        nullptr,
    };

    float fIndent = 0.0f;

    if(!pl_parse("|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &fIndent))
        return nullptr;

    ImGui::Indent(fIndent);
    Py_RETURN_NONE;
}

PyObject*
Unindent(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "indent_w",
        nullptr,
    };

    float fIndent = 0.0f;

    if(!pl_parse("|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &fIndent))
        return nullptr;

    ImGui::Unindent(fIndent);
    Py_RETURN_NONE;
}

PyObject*
BeginGroup(PyObject* self)
{
    ImGui::BeginGroup();
    Py_RETURN_NONE;
}

PyObject*
EndGroup(PyObject* self)
{
    ImGui::EndGroup();
    Py_RETURN_NONE;
}

PyObject*
AlignTextToFramePadding(PyObject* self)
{
    ImGui::AlignTextToFramePadding();
    Py_RETURN_NONE;
}

PyObject*
GetTextLineHeight(PyObject* self)
{
    return PyFloat_FromDouble(ImGui::GetTextLineHeight());
}

PyObject*
GetTextLineHeightWithSpacing(PyObject* self)
{
    return PyFloat_FromDouble(ImGui::GetTextLineHeightWithSpacing());
}

PyObject*
GetFrameHeight(PyObject* self)
{
    return PyFloat_FromDouble(ImGui::GetFrameHeight());
}

PyObject*
GetFrameHeightWithSpacing(PyObject* self)
{
    return PyFloat_FromDouble(ImGui::GetFrameHeightWithSpacing());
}

PyObject*
GetCursorScreenPos(PyObject* self)
{
    ImVec2 tPos = ImGui::GetCursorScreenPos();
    return Py_BuildValue("(ff)", tPos.x, tPos.y);
}

PyObject*
SetCursorScreenPos(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "pos",
        nullptr,
    };

    PyObject* ptPos = Py_None;

    if(!pl_parse("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPos))
        return nullptr;

    ImGui::SetCursorScreenPos(pl__get_vec2_from_python(ptPos));
    Py_RETURN_NONE;
}

PyObject*
GetContentRegionAvail(PyObject* self)
{
    ImVec2 tSize = ImGui::GetContentRegionAvail();
    return Py_BuildValue("(ff)", tSize.x, tSize.y);
}

PyObject*
GetCursorPos(PyObject* self)
{
    ImVec2 tPos = ImGui::GetCursorPos();
    return Py_BuildValue("(ff)", tPos.x, tPos.y);
}

PyObject*
GetCursorPosX(PyObject* self)
{
    return PyFloat_FromDouble(ImGui::GetCursorPosX());
}

PyObject*
GetCursorPosY(PyObject* self)
{
    return PyFloat_FromDouble(ImGui::GetCursorPosY());
}

PyObject*
SetCursorPos(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "local_pos",
        nullptr,
    };

    PyObject* ptPos = Py_None;

    if(!pl_parse("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPos))
        return nullptr;

    ImGui::SetCursorPos(pl__get_vec2_from_python(ptPos));
    Py_RETURN_NONE;
}

PyObject*
SetCursorPosX(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "local_x",
        nullptr,
    };

    float fLocalX = 0.0f;

    if(!pl_parse("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &fLocalX))
        return nullptr;

    ImGui::SetCursorPosX(fLocalX);
    Py_RETURN_NONE;
}

PyObject*
SetCursorPosY(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "local_y",
        nullptr,
    };

    float fLocalY = 0.0f;

    if(!pl_parse("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &fLocalY))
        return nullptr;

    ImGui::SetCursorPosY(fLocalY);
    Py_RETURN_NONE;
}

PyObject*
GetCursorStartPos(PyObject* self)
{
    ImVec2 tPos = ImGui::GetCursorStartPos();
    return Py_BuildValue("(ff)", tPos.x, tPos.y);
}

PyObject*
GetScrollX(PyObject* self)
{
    return PyFloat_FromDouble(ImGui::GetScrollX());
}

PyObject*
GetScrollY(PyObject* self)
{
    return PyFloat_FromDouble(ImGui::GetScrollY());
}

PyObject*
SetScrollX(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"scroll_x", nullptr};

    float fScrollX = 0.0f;

    if(!pl_parse("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fScrollX))
        return nullptr;

    ImGui::SetScrollX(fScrollX);
    Py_RETURN_NONE;
}

PyObject*
SetScrollY(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"scroll_y", nullptr};

    float fScrollY = 0.0f;

    if(!pl_parse("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fScrollY))
        return nullptr;

    ImGui::SetScrollY(fScrollY);
    Py_RETURN_NONE;
}

PyObject*
GetScrollMaxX(PyObject* self)
{
    return PyFloat_FromDouble(ImGui::GetScrollMaxX());
}

PyObject*
GetScrollMaxY(PyObject* self)
{
    return PyFloat_FromDouble(ImGui::GetScrollMaxY());
}

PyObject*
SetScrollHereX(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"center_x_ratio", nullptr};

    float fCenterXRatio = 0.5f;

    if(!pl_parse("|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fCenterXRatio))
        return nullptr;

    ImGui::SetScrollHereX(fCenterXRatio);
    Py_RETURN_NONE;
}

PyObject*
SetScrollHereY(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"center_y_ratio", nullptr};

    float fCenterYRatio = 0.5f;

    if(!pl_parse("|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fCenterYRatio))
        return nullptr;

    ImGui::SetScrollHereY(fCenterYRatio);
    Py_RETURN_NONE;
}

PyObject*
SetScrollFromPosX(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"local_x", "center_x_ratio", nullptr};

    float fLocalX = 0.0f;
    float fCenterXRatio = 0.5f;

    if(!pl_parse("f|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &fLocalX, &fCenterXRatio))
        return nullptr;

    ImGui::SetScrollFromPosX(fLocalX, fCenterXRatio);
    Py_RETURN_NONE;
}

PyObject*
SetScrollFromPosY(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"local_y", "center_y_ratio", nullptr};

    float fLocalY = 0.0f;
    float fCenterYRatio = 0.5f;

    if(!pl_parse("f|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &fLocalY, &fCenterYRatio))
        return nullptr;

    ImGui::SetScrollFromPosY(fLocalY, fCenterYRatio);
    Py_RETURN_NONE;
}

PyObject*
PushItemWidth(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"item_width", nullptr};

    float fItemWidth = 0.0f;

    if(!pl_parse("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fItemWidth))
        return nullptr;

    ImGui::PushItemWidth(fItemWidth);
    Py_RETURN_NONE;
}

PyObject*
PopItemWidth(PyObject* self)
{
    ImGui::PopItemWidth();
    Py_RETURN_NONE;
}

PyObject*
SetNextItemWidth(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"item_width", nullptr};

    float fItemWidth = 0.0f;

    if(!pl_parse("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fItemWidth))
        return nullptr;

    ImGui::SetNextItemWidth(fItemWidth);
    Py_RETURN_NONE;
}

PyObject*
CalcItemWidth(PyObject* self)
{
    return PyFloat_FromDouble(ImGui::CalcItemWidth());
}

PyObject*
PushTextWrapPos(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"wrap_local_pos_x", nullptr};

    float fWrapLocalPosX = 0.0f;

    if(!pl_parse("|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fWrapLocalPosX))
        return nullptr;

    ImGui::PushTextWrapPos(fWrapLocalPosX);
    Py_RETURN_NONE;
}

PyObject*
PopTextWrapPos(PyObject* self)
{
    ImGui::PopTextWrapPos();
    Py_RETURN_NONE;
}

PyObject*
SmallButton(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", nullptr};

    const char* pcLabel = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel))
        return nullptr;

    return PyBool_FromLong(ImGui::SmallButton(pcLabel));
}

PyObject*
InvisibleButton(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"str_id", "size", "flags", nullptr};

    const char* pcStrId = nullptr;
    PyObject* ptSize = Py_None;
    int iFlags = 0;

    if(!pl_parse("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcStrId, &ptSize, &iFlags))
        return nullptr;

    ImVec2 tSize = pl__get_vec2_from_python(ptSize);
    return PyBool_FromLong(ImGui::InvisibleButton(pcStrId, tSize, iFlags));
}

PyObject*
ArrowButton(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"str_id", "dir", nullptr};

    const char* pcStrId = nullptr;
    int iDir = 0;

    if(!pl_parse("si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcStrId, &iDir))
        return nullptr;

    return PyBool_FromLong(ImGui::ArrowButton(pcStrId, (ImGuiDir)iDir));
}

PyObject*
CheckboxFlags(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "flags_pointer", "flags_value", nullptr};

    const char* pcLabel = nullptr;
    PyObject* ptFlagsPointer = Py_None;
    int iFlagsValue = 0;

    if(!pl_parse("sOi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptFlagsPointer, &iFlagsValue))
        return nullptr;

    int* piFlags = (int*)PyCapsule_GetPointer(ptFlagsPointer, "plIntPointer");
    return PyBool_FromLong(ImGui::CheckboxFlags(pcLabel, piFlags, iFlagsValue));
}

PyObject*
RadioButton(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "active", "value_pointer", "button_value", nullptr};

    const char* pcLabel = nullptr;
    int bActive = false;
    PyObject* ptValuePointer = Py_None;
    int iButtonValue = 0;

    if(!pl_parse("s|pOi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &bActive, &ptValuePointer, &iButtonValue))
        return nullptr;

    if(!Py_IsNone(ptValuePointer))
    {
        int* piValue = (int*)PyCapsule_GetPointer(ptValuePointer, "plIntPointer");
        return PyBool_FromLong(ImGui::RadioButton(pcLabel, piValue, iButtonValue));
    }

    return PyBool_FromLong(ImGui::RadioButton(pcLabel, bActive));
}

PyObject*
ProgressBar(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"fraction", "size", "overlay", nullptr};

    float fFraction = 0.0f;
    PyObject* ptSize = Py_None;
    const char* pcOverlay = nullptr;

    if(!pl_parse("f|Oz", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &fFraction, &ptSize, &pcOverlay))
        return nullptr;

    ImVec2 tSize = ImVec2(-FLT_MIN, 0.0f);
    if(!Py_IsNone(ptSize))
        tSize = pl__get_vec2_from_python(ptSize);

    ImGui::ProgressBar(fFraction, tSize, pcOverlay);
    Py_RETURN_NONE;
}

PyObject*
Bullet(PyObject* self)
{
    ImGui::Bullet();
    Py_RETURN_NONE;
}

PyObject*
TextLink(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", nullptr};

    const char* pcLabel = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel))
        return nullptr;

    return PyBool_FromLong(ImGui::TextLink(pcLabel));
}

PyObject*
TextLinkOpenURL(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "url", nullptr};

    const char* pcLabel = nullptr;
    const char* pcUrl = nullptr;

    if(!pl_parse("s|z", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &pcUrl))
        return nullptr;

    return PyBool_FromLong(ImGui::TextLinkOpenURL(pcLabel, pcUrl));
}

PyObject*
ColorEdit3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "color_pointer", "flags", nullptr};

    const char* pcLabel = nullptr;
    PyObject* ptColorPointer = Py_None;
    int iFlags = 0;

    if(!pl_parse("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptColorPointer, &iFlags))
        return nullptr;

    float* pfColor = (float*)PyCapsule_GetPointer(ptColorPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::ColorEdit3(pcLabel, pfColor, iFlags));
}

PyObject*
ColorEdit4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "color_pointer", "flags", nullptr};

    const char* pcLabel = nullptr;
    PyObject* ptColorPointer = Py_None;
    int iFlags = 0;

    if(!pl_parse("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptColorPointer, &iFlags))
        return nullptr;

    float* pfColor = (float*)PyCapsule_GetPointer(ptColorPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::ColorEdit4(pcLabel, pfColor, iFlags));
}

PyObject*
ColorPicker3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "color_pointer", "flags", nullptr};

    const char* pcLabel = nullptr;
    PyObject* ptColorPointer = Py_None;
    int iFlags = 0;

    if(!pl_parse("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptColorPointer, &iFlags))
        return nullptr;

    float* pfColor = (float*)PyCapsule_GetPointer(ptColorPointer, "plFloatPointer");
    return PyBool_FromLong(ImGui::ColorPicker3(pcLabel, pfColor, iFlags));
}

PyObject*
ColorPicker4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "color_pointer", "flags", "ref_color", nullptr};

    const char* pcLabel = nullptr;
    PyObject* ptColorPointer = Py_None;
    int iFlags = 0;
    PyObject* ptRefColorPointer = Py_None;

    if(!pl_parse("sO|iO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptColorPointer, &iFlags, &ptRefColorPointer))
        return nullptr;

    float* pfColor = (float*)PyCapsule_GetPointer(ptColorPointer, "plFloatPointer");

    const float* pfRefColor = nullptr;
    if(!Py_IsNone(ptRefColorPointer))
        pfRefColor = (const float*)PyCapsule_GetPointer(ptRefColorPointer, "plFloatPointer");

    return PyBool_FromLong(ImGui::ColorPicker4(pcLabel, pfColor, iFlags, pfRefColor));
}

PyObject*
ColorButton(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"desc_id", "color", "flags", "size", nullptr};

    const char* pcDescId = nullptr;
    PyObject* ptColor = Py_None;
    int iFlags = 0;
    PyObject* ptSize = Py_None;

    if(!pl_parse("sO|iO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcDescId, &ptColor, &iFlags, &ptSize))
        return nullptr;

    ImVec4 tColor = pl__get_vec4_from_python(ptColor);

    ImVec2 tSize = {};
    if(!Py_IsNone(ptSize))
        tSize = pl__get_vec2_from_python(ptSize);

    return PyBool_FromLong(ImGui::ColorButton(pcDescId, tColor, iFlags, tSize));
}

PyObject*
SetColorEditOptions(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"flags", nullptr};

    int iFlags = 0;

    if(!pl_parse("i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iFlags))
        return nullptr;

    ImGui::SetColorEditOptions(iFlags);
    Py_RETURN_NONE;
}

PyObject*
TreeNode(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", nullptr};

    const char* pcLabel = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel))
        return nullptr;

    return PyBool_FromLong(ImGui::TreeNode(pcLabel));
}

PyObject*
TreeNodeEx(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "flags", nullptr};

    const char* pcLabel = nullptr;
    int iFlags = 0;

    if(!pl_parse("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &iFlags))
        return nullptr;

    return PyBool_FromLong(ImGui::TreeNodeEx(pcLabel, iFlags));
}

PyObject*
TreePush(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"str_id", nullptr};

    const char* pcStrId = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcStrId))
        return nullptr;

    ImGui::TreePush(pcStrId);
    Py_RETURN_NONE;
}

PyObject*
TreePop(PyObject* self)
{
    ImGui::TreePop();
    Py_RETURN_NONE;
}

PyObject*
GetTreeNodeToLabelSpacing(PyObject* self)
{
    return PyFloat_FromDouble(ImGui::GetTreeNodeToLabelSpacing());
}

PyObject*
CollapsingHeader(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label",
        "visible_pointer",
        "flags",
        nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptVisiblePointer = Py_None;
    int iFlags = 0;

    if(!pl_parse("s|Oi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptVisiblePointer, &iFlags))
        return nullptr;

    if(!Py_IsNone(ptVisiblePointer))
    {
        bool* pbVisible = (bool*)PyCapsule_GetPointer(ptVisiblePointer, "plBoolPointer");
        return PyBool_FromLong(ImGui::CollapsingHeader(pcLabel, pbVisible, iFlags));
    }

    return PyBool_FromLong(ImGui::CollapsingHeader(pcLabel, iFlags));
}

PyObject*
SetNextItemOpen(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"is_open", "cond", nullptr};

    int bIsOpen = false;
    int iCond = 0;

    if(!pl_parse("p|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &bIsOpen, &iCond))
        return nullptr;

    ImGui::SetNextItemOpen(bIsOpen, iCond);
    Py_RETURN_NONE;
}

PyObject*
SetNextItemStorageID(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"storage_id", nullptr};

    uint32_t uStorageId = 0;

    if(!pl_parse("I", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &uStorageId))
        return nullptr;

    ImGui::SetNextItemStorageID((ImGuiID)uStorageId);
    Py_RETURN_NONE;
}

PyObject*
TreeNodeGetOpen(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"storage_id", nullptr};

    uint32_t uStorageId = 0;

    if(!pl_parse("I", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &uStorageId))
        return nullptr;

    return PyBool_FromLong(ImGui::TreeNodeGetOpen((ImGuiID)uStorageId));
}

//-----------------------------------------------------------------------------
// [SECTION] Window Utilities
//-----------------------------------------------------------------------------

PyObject*
IsWindowAppearing(PyObject* self)
{
    return PyBool_FromLong(ImGui::IsWindowAppearing());
}

PyObject*
IsWindowCollapsed(PyObject* self)
{
    return PyBool_FromLong(ImGui::IsWindowCollapsed());
}

PyObject*
IsWindowFocused(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"flags", nullptr};

    int iFlags = 0;

    if(!pl_parse("|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iFlags))
        return nullptr;

    return PyBool_FromLong(ImGui::IsWindowFocused(iFlags));
}

PyObject*
IsWindowHovered(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"flags", nullptr};

    int iFlags = 0;

    if(!pl_parse("|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iFlags))
        return nullptr;

    return PyBool_FromLong(ImGui::IsWindowHovered(iFlags));
}

PyObject*
GetWindowDpiScale(PyObject* self)
{
    return PyFloat_FromDouble(ImGui::GetWindowDpiScale());
}

PyObject*
GetWindowPos(PyObject* self)
{
    ImVec2 tPos = ImGui::GetWindowPos();
    return Py_BuildValue("(ff)", tPos.x, tPos.y);
}

PyObject*
GetWindowSize(PyObject* self)
{
    ImVec2 tSize = ImGui::GetWindowSize();
    return Py_BuildValue("(ff)", tSize.x, tSize.y);
}

PyObject*
GetWindowWidth(PyObject* self)
{
    return PyFloat_FromDouble(ImGui::GetWindowWidth());
}

PyObject*
GetWindowHeight(PyObject* self)
{
    return PyFloat_FromDouble(ImGui::GetWindowHeight());
}

PyObject*
SetNextWindowPos(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"pos", "cond", "pivot", nullptr};

    PyObject* ptPos = Py_None;
    int iCond = 0;
    PyObject* ptPivot = Py_None;

    if(!pl_parse("O|iO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPos, &iCond, &ptPivot))
        return nullptr;

    ImVec2 tPivot = {};
    if(!Py_IsNone(ptPivot))
        tPivot = pl__get_vec2_from_python(ptPivot);

    ImGui::SetNextWindowPos(pl__get_vec2_from_python(ptPos), iCond, tPivot);
    Py_RETURN_NONE;
}

PyObject*
SetNextWindowSize(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"size", "cond", nullptr};

    PyObject* ptSize = Py_None;
    int iCond = 0;

    if(!pl_parse("O|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptSize, &iCond))
        return nullptr;

    ImGui::SetNextWindowSize(pl__get_vec2_from_python(ptSize), iCond);
    Py_RETURN_NONE;
}

PyObject*
SetNextWindowContentSize(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"size", nullptr};

    PyObject* ptSize = Py_None;

    if(!pl_parse("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptSize))
        return nullptr;

    ImGui::SetNextWindowContentSize(pl__get_vec2_from_python(ptSize));
    Py_RETURN_NONE;
}

PyObject*
SetNextWindowCollapsed(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"collapsed", "cond", nullptr};

    int bCollapsed = false;
    int iCond = 0;

    if(!pl_parse("p|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &bCollapsed, &iCond))
        return nullptr;

    ImGui::SetNextWindowCollapsed(bCollapsed, iCond);
    Py_RETURN_NONE;
}

PyObject*
SetNextWindowFocus(PyObject* self)
{
    ImGui::SetNextWindowFocus();
    Py_RETURN_NONE;
}

PyObject*
SetNextWindowScroll(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"scroll", nullptr};

    PyObject* ptScroll = Py_None;

    if(!pl_parse("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptScroll))
        return nullptr;

    ImGui::SetNextWindowScroll(pl__get_vec2_from_python(ptScroll));
    Py_RETURN_NONE;
}

PyObject*
SetNextWindowBgAlpha(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"alpha", nullptr};

    float fAlpha = 0.0f;

    if(!pl_parse("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fAlpha))
        return nullptr;

    ImGui::SetNextWindowBgAlpha(fAlpha);
    Py_RETURN_NONE;
}

PyObject*
SetNextWindowViewport(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"viewport_id", nullptr};

    uint32_t uViewportId = 0;

    if(!pl_parse("I", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uViewportId))
        return nullptr;

    ImGui::SetNextWindowViewport((ImGuiID)uViewportId);
    Py_RETURN_NONE;
}

PyObject*
SetWindowPos(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"pos", "cond", "name", nullptr};

    PyObject* ptPos = Py_None;
    int iCond = 0;
    const char* pcName = nullptr;

    if(!pl_parse("O|iz", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPos, &iCond, &pcName))
        return nullptr;

    if(pcName)
        ImGui::SetWindowPos(pcName, pl__get_vec2_from_python(ptPos), iCond);
    else
        ImGui::SetWindowPos(pl__get_vec2_from_python(ptPos), iCond);

    Py_RETURN_NONE;
}

PyObject*
SetWindowSize(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"size", "cond", "name", nullptr};

    PyObject* ptSize = Py_None;
    int iCond = 0;
    const char* pcName = nullptr;

    if(!pl_parse("O|iz", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptSize, &iCond, &pcName))
        return nullptr;

    if(pcName)
        ImGui::SetWindowSize(pcName, pl__get_vec2_from_python(ptSize), iCond);
    else
        ImGui::SetWindowSize(pl__get_vec2_from_python(ptSize), iCond);

    Py_RETURN_NONE;
}

PyObject*
SetWindowCollapsed(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"collapsed", "cond", "name", nullptr};

    int bCollapsed = false;
    int iCond = 0;
    const char* pcName = nullptr;

    if(!pl_parse("p|iz", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &bCollapsed, &iCond, &pcName))
        return nullptr;

    if(pcName)
        ImGui::SetWindowCollapsed(pcName, bCollapsed, iCond);
    else
        ImGui::SetWindowCollapsed(bCollapsed, iCond);

    Py_RETURN_NONE;
}

PyObject*
SetWindowFocus(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"name", nullptr};

    const char* pcName = nullptr;

    if(!pl_parse("|z", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcName))
        return nullptr;

    if(pcName)
        ImGui::SetWindowFocus(pcName);
    else
        ImGui::SetWindowFocus();

    Py_RETURN_NONE;
}

PyObject*
Selectable(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label",
        "selected",
        "flags",
        "size",
        "selected_pointer",
        nullptr,
    };

    const char* pcLabel = nullptr;
    int bSelected = false;
    int iFlags = 0;
    PyObject* ptSize = Py_None;
    PyObject* ptSelectedPointer = Py_None;

    if(!pl_parse("s|piO$O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &bSelected, &iFlags, &ptSize, &ptSelectedPointer))
        return nullptr;

    ImVec2 tSize = {};
    if(!Py_IsNone(ptSize))
        tSize = pl__get_vec2_from_python(ptSize);

    if(!Py_IsNone(ptSelectedPointer))
    {
        bool* pbSelected = (bool*)PyCapsule_GetPointer(ptSelectedPointer, "plBoolPointer");
        return PyBool_FromLong(ImGui::Selectable(pcLabel, pbSelected, iFlags, tSize));
    }

    return PyBool_FromLong(ImGui::Selectable(pcLabel, bSelected, iFlags, tSize));
}

PyObject*
BeginTooltip(PyObject* self)
{
    return PyBool_FromLong(ImGui::BeginTooltip());
}

PyObject*
EndTooltip(PyObject* self)
{
    ImGui::EndTooltip();
    Py_RETURN_NONE;
}

PyObject*
SetTooltip(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"text", nullptr};

    const char* pcText = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText))
        return nullptr;

    ImGui::SetTooltip("%s", pcText);
    Py_RETURN_NONE;
}

PyObject*
BeginItemTooltip(PyObject* self)
{
    return PyBool_FromLong(ImGui::BeginItemTooltip());
}

PyObject*
SetItemTooltip(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"text", nullptr};

    const char* pcText = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText))
        return nullptr;

    ImGui::SetItemTooltip("%s", pcText);
    Py_RETURN_NONE;
}

PyObject*
BeginPopup(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"str_id", "flags", nullptr};

    const char* pcStrId = nullptr;
    int iFlags = 0;

    if(!pl_parse("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcStrId, &iFlags))
        return nullptr;

    return PyBool_FromLong(ImGui::BeginPopup(pcStrId, iFlags));
}

PyObject*
BeginPopupModal(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"name", "open", "flags", nullptr};

    const char* pcName = nullptr;
    PyObject* ptOpen = Py_None;
    int iFlags = 0;

    if(!pl_parse("s|Oi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcName, &ptOpen, &iFlags))
        return nullptr;

    bool* pbOpen = nullptr;
    if(!Py_IsNone(ptOpen))
        pbOpen = (bool*)PyCapsule_GetPointer(ptOpen, "plBoolPointer");

    return PyBool_FromLong(ImGui::BeginPopupModal(pcName, pbOpen, iFlags));
}

PyObject*
EndPopup(PyObject* self)
{
    ImGui::EndPopup();
    Py_RETURN_NONE;
}

PyObject*
OpenPopup(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"str_id", "popup_flags", nullptr};

    const char* pcStrId = nullptr;
    int iPopupFlags = 0;

    if(!pl_parse("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcStrId, &iPopupFlags))
        return nullptr;

    ImGui::OpenPopup(pcStrId, iPopupFlags);
    Py_RETURN_NONE;
}

PyObject*
OpenPopupOnItemClick(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"str_id", "popup_flags", nullptr};

    const char* pcStrId = nullptr;
    int iPopupFlags = 0;

    if(!pl_parse("|zi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcStrId, &iPopupFlags))
        return nullptr;

    ImGui::OpenPopupOnItemClick(pcStrId, iPopupFlags);
    Py_RETURN_NONE;
}

PyObject*
CloseCurrentPopup(PyObject* self)
{
    ImGui::CloseCurrentPopup();
    Py_RETURN_NONE;
}

PyObject*
BeginPopupContextItem(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"str_id", "popup_flags", nullptr};

    const char* pcStrId = nullptr;
    int iPopupFlags = 0;

    if(!pl_parse("|zi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcStrId, &iPopupFlags))
        return nullptr;

    return PyBool_FromLong(ImGui::BeginPopupContextItem(pcStrId, iPopupFlags));
}

PyObject*
BeginPopupContextWindow(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"str_id", "popup_flags", nullptr};

    const char* pcStrId = nullptr;
    int iPopupFlags = 0;

    if(!pl_parse("|zi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcStrId, &iPopupFlags))
        return nullptr;

    return PyBool_FromLong(ImGui::BeginPopupContextWindow(pcStrId, iPopupFlags));
}

PyObject*
BeginPopupContextVoid(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"str_id", "popup_flags", nullptr};

    const char* pcStrId = nullptr;
    int iPopupFlags = 0;

    if(!pl_parse("|zi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcStrId, &iPopupFlags))
        return nullptr;

    return PyBool_FromLong(ImGui::BeginPopupContextVoid(pcStrId, iPopupFlags));
}

PyObject*
IsPopupOpen(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"str_id", "flags", nullptr};

    const char* pcStrId = nullptr;
    int iFlags = 0;

    if(!pl_parse("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcStrId, &iFlags))
        return nullptr;

    return PyBool_FromLong(ImGui::IsPopupOpen(pcStrId, iFlags));
}

PyObject*
BeginTabBar(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"str_id", "flags", nullptr};

    const char* pcStrId = nullptr;
    int iFlags = 0;

    if(!pl_parse("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcStrId, &iFlags))
        return nullptr;

    return PyBool_FromLong(ImGui::BeginTabBar(pcStrId, iFlags));
}

PyObject*
EndTabBar(PyObject* self)
{
    ImGui::EndTabBar();
    Py_RETURN_NONE;
}

PyObject*
BeginTabItem(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "open", "flags", nullptr};

    const char* pcLabel = nullptr;
    PyObject* ptOpen = Py_None;
    int iFlags = 0;

    if(!pl_parse("s|Oi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptOpen, &iFlags))
        return nullptr;

    bool* pbOpen = nullptr;
    if(!Py_IsNone(ptOpen))
        pbOpen = (bool*)PyCapsule_GetPointer(ptOpen, "plBoolPointer");

    return PyBool_FromLong(ImGui::BeginTabItem(pcLabel, pbOpen, iFlags));
}

PyObject*
EndTabItem(PyObject* self)
{
    ImGui::EndTabItem();
    Py_RETURN_NONE;
}

PyObject*
TabItemButton(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "flags", nullptr};

    const char* pcLabel = nullptr;
    int iFlags = 0;

    if(!pl_parse("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &iFlags))
        return nullptr;

    return PyBool_FromLong(ImGui::TabItemButton(pcLabel, iFlags));
}

PyObject*
SetTabItemClosed(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"tab_or_docked_window_label", nullptr};

    const char* pcLabel = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel))
        return nullptr;

    ImGui::SetTabItemClosed(pcLabel);
    Py_RETURN_NONE;
}

PyObject*
IsItemHovered(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"flags", nullptr};
    int iFlags = 0;
    if(!pl_parse("|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iFlags))
        return nullptr;
    return PyBool_FromLong(ImGui::IsItemHovered(iFlags));
}

PyObject*
IsItemActive(PyObject* self)
{
    return PyBool_FromLong(ImGui::IsItemActive());
}

PyObject*
IsItemFocused(PyObject* self)
{
    return PyBool_FromLong(ImGui::IsItemFocused());
}

PyObject*
IsItemClicked(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"mouse_button", nullptr};
    int iMouseButton = 0;
    if(!pl_parse("|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iMouseButton))
        return nullptr;
    return PyBool_FromLong(ImGui::IsItemClicked(iMouseButton));
}

PyObject*
IsItemVisible(PyObject* self)
{
    return PyBool_FromLong(ImGui::IsItemVisible());
}

PyObject*
IsItemEdited(PyObject* self)
{
    return PyBool_FromLong(ImGui::IsItemEdited());
}

PyObject*
IsItemActivated(PyObject* self)
{
    return PyBool_FromLong(ImGui::IsItemActivated());
}

PyObject*
IsItemDeactivated(PyObject* self)
{
    return PyBool_FromLong(ImGui::IsItemDeactivated());
}

PyObject*
IsItemDeactivatedAfterEdit(PyObject* self)
{
    return PyBool_FromLong(ImGui::IsItemDeactivatedAfterEdit());
}

PyObject*
IsItemToggledOpen(PyObject* self)
{
    return PyBool_FromLong(ImGui::IsItemToggledOpen());
}

PyObject*
IsAnyItemHovered(PyObject* self)
{
    return PyBool_FromLong(ImGui::IsAnyItemHovered());
}

PyObject*
IsAnyItemActive(PyObject* self)
{
    return PyBool_FromLong(ImGui::IsAnyItemActive());
}

PyObject*
IsAnyItemFocused(PyObject* self)
{
    return PyBool_FromLong(ImGui::IsAnyItemFocused());
}

PyObject*
GetItemID(PyObject* self)
{
    return PyLong_FromUnsignedLong((unsigned long)ImGui::GetItemID());
}

PyObject*
GetItemRectMin(PyObject* self)
{
    ImVec2 tValue = ImGui::GetItemRectMin();
    return Py_BuildValue("(ff)", tValue.x, tValue.y);
}

PyObject*
GetItemRectMax(PyObject* self)
{
    ImVec2 tValue = ImGui::GetItemRectMax();
    return Py_BuildValue("(ff)", tValue.x, tValue.y);
}

PyObject*
GetItemRectSize(PyObject* self)
{
    ImVec2 tValue = ImGui::GetItemRectSize();
    return Py_BuildValue("(ff)", tValue.x, tValue.y);
}

PyObject*
SetNextItemAllowOverlap(PyObject* self)
{
    ImGui::SetNextItemAllowOverlap();
    Py_RETURN_NONE;
}

PyObject*
SetItemDefaultFocus(PyObject* self)
{
    ImGui::SetItemDefaultFocus();
    Py_RETURN_NONE;
}

PyObject*
SetKeyboardFocusHere(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"offset", nullptr};
    int iOffset = 0;
    if(!pl_parse("|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iOffset))
        return nullptr;
    ImGui::SetKeyboardFocusHere(iOffset);
    Py_RETURN_NONE;
}

PyObject*
BeginChild(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "str_id",
        "size",
        "child_flags",
        "window_flags",
        nullptr,
    };

    const char* pcStrId = nullptr;
    PyObject* ptSize = Py_None;
    int iChildFlags = 0;
    int iWindowFlags = 0;

    if(!pl_parse("s|Oii", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcStrId, &ptSize, &iChildFlags, &iWindowFlags))
        return nullptr;

    ImVec2 tSize = {};
    if(!Py_IsNone(ptSize))
        tSize = pl__get_vec2_from_python(ptSize);

    return PyBool_FromLong(ImGui::BeginChild(pcStrId, tSize, iChildFlags, iWindowFlags));
}

PyObject*
EndChild(PyObject* self)
{
    ImGui::EndChild();
    Py_RETURN_NONE;
}

PyObject*
PushID(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"id", nullptr};

    PyObject* ptId = Py_None;

    if(!pl_parse("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptId))
        return nullptr;

    if(PyLong_Check(ptId))
    {
        int iId = (int)PyLong_AsLong(ptId);
        if(PyErr_Occurred())
            return nullptr;

        ImGui::PushID(iId);
        Py_RETURN_NONE;
    }

    const char* pcId = PyUnicode_AsUTF8(ptId);
    if(!pcId)
        return nullptr;

    ImGui::PushID(pcId);
    Py_RETURN_NONE;
}

PyObject*
PopID(PyObject* self)
{
    ImGui::PopID();
    Py_RETURN_NONE;
}

PyObject*
GetID(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"id", nullptr};

    PyObject* ptId = Py_None;

    if(!pl_parse("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptId))
        return nullptr;

    ImGuiID tId = 0;

    if(PyLong_Check(ptId))
    {
        int iId = (int)PyLong_AsLong(ptId);
        if(PyErr_Occurred())
            return nullptr;

        tId = ImGui::GetID(iId);
    }
    else
    {
        const char* pcId = PyUnicode_AsUTF8(ptId);
        if(!pcId)
            return nullptr;

        tId = ImGui::GetID(pcId);
    }

    return PyLong_FromUnsignedLong((unsigned long)tId);
}

PyObject*
ShowMetricsWindow(PyObject* self, PyObject* arg)
{
    bool* ptShow = NULL;
    if(!Py_IsNone(arg))
        ptShow = (bool*)PyCapsule_GetPointer(arg, "plBoolPointer");

    ImGui::ShowMetricsWindow(ptShow);
    Py_RETURN_NONE;
}

PyObject*
ShowDebugLogWindow(PyObject* self, PyObject* arg)
{
    bool* ptShow = NULL;
    if(!Py_IsNone(arg))
        ptShow = (bool*)PyCapsule_GetPointer(arg, "plBoolPointer");

    ImGui::ShowDebugLogWindow(ptShow);
    Py_RETURN_NONE;
}

PyObject*
ShowIDStackToolWindow(PyObject* self, PyObject* arg)
{
    bool* ptShow = NULL;
    if(!Py_IsNone(arg))
        ptShow = (bool*)PyCapsule_GetPointer(arg, "plBoolPointer");

    ImGui::ShowIDStackToolWindow(ptShow);
    Py_RETURN_NONE;
}

PyObject*
ShowAboutWindow(PyObject* self, PyObject* arg)
{
    bool* ptShow = NULL;
    if(!Py_IsNone(arg))
        ptShow = (bool*)PyCapsule_GetPointer(arg, "plBoolPointer");

    ImGui::ShowAboutWindow(ptShow);
    Py_RETURN_NONE;
}

PyObject*
ShowStyleEditor(PyObject* self)
{
    ImGui::ShowStyleEditor();
    Py_RETURN_NONE;
}

PyObject*
ShowStyleSelector(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", nullptr};

    const char* pcLabel = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel))
        return nullptr;

    return PyBool_FromLong(ImGui::ShowStyleSelector(pcLabel));
}

PyObject*
ShowFontSelector(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", nullptr};

    const char* pcLabel = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel))
        return nullptr;

    ImGui::ShowFontSelector(pcLabel);
    Py_RETURN_NONE;
}

PyObject*
ShowUserGuide(PyObject* self)
{
    ImGui::ShowUserGuide();
    Py_RETURN_NONE;
}

PyObject*
GetVersion(PyObject* self)
{
    return PyUnicode_FromString(ImGui::GetVersion());
}

PyObject*
BeginCombo(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "preview_value", "flags", nullptr};

    const char* pcLabel = nullptr;
    const char* pcPreviewValue = nullptr;
    int iFlags = 0;

    if(!pl_parse("ss|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &pcPreviewValue, &iFlags))
        return nullptr;

    return PyBool_FromLong(ImGui::BeginCombo(pcLabel, pcPreviewValue, iFlags));
}

PyObject*
EndCombo(PyObject* self)
{
    ImGui::EndCombo();
    Py_RETURN_NONE;
}

PyObject*
Combo(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label",
        "current_item",
        "items",
        "popup_max_height_in_items",
        nullptr,
    };
    static const char** sbtEntries = nullptr;

    const char* pcLabel = nullptr;
    PyObject* ptCurrentItem = nullptr;
    PyObject* ptItems = nullptr;
    int popup_max_height_in_items = -1;
	if (!pl_parse("sOO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptCurrentItem, &ptItems, &popup_max_height_in_items))
		return nullptr;
    
    int* ptCurrentItemInt = (int*)PyCapsule_GetPointer(ptCurrentItem, "plIntPointer");

    Py_ssize_t pySize = PyList_Size(ptItems);
    pl_sb_resize(sbtEntries, (uint32_t)pySize);
    for(Py_ssize_t i = 0; i < pySize; i++)
    {
        PyObject* item = PyList_GetItem(ptItems, i);
        sbtEntries[i] = PyUnicode_AsUTF8(item);
    }

    bool bActivated = ImGui::Combo(pcLabel, ptCurrentItemInt, sbtEntries, (int)pySize, popup_max_height_in_items);
    pl_sb_reset(sbtEntries);
    return Py_BuildValue("p", bActivated);
}


PyObject*
BeginListBox(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "size", nullptr};

    const char* pcLabel = nullptr;
    PyObject* ptSize = Py_None;

    if(!pl_parse("s|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptSize))
        return nullptr;

    ImVec2 tSize = {};
    if(!Py_IsNone(ptSize))
        tSize = pl__get_vec2_from_python(ptSize);

    return PyBool_FromLong(ImGui::BeginListBox(pcLabel, tSize));
}

PyObject*
EndListBox(PyObject* self)
{
    ImGui::EndListBox();
    Py_RETURN_NONE;
}

PyObject*
IsMouseDown(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"button", nullptr};

    int iButton = 0;

    if(!pl_parse("i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iButton))
        return nullptr;

    return PyBool_FromLong(ImGui::IsMouseDown(iButton));
}

PyObject*
IsMouseClicked(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"button", "repeat", nullptr};

    int iButton = 0;
    int bRepeat = false;

    if(!pl_parse("i|p", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &iButton, &bRepeat))
        return nullptr;

    return PyBool_FromLong(ImGui::IsMouseClicked(iButton, bRepeat));
}

PyObject*
IsMouseReleased(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"button", nullptr};

    int iButton = 0;

    if(!pl_parse("i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iButton))
        return nullptr;

    return PyBool_FromLong(ImGui::IsMouseReleased(iButton));
}

PyObject*
IsMouseDoubleClicked(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"button", nullptr};

    int iButton = 0;

    if(!pl_parse("i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iButton))
        return nullptr;

    return PyBool_FromLong(ImGui::IsMouseDoubleClicked(iButton));
}

PyObject*
GetMouseClickedCount(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"button", nullptr};

    int iButton = 0;

    if(!pl_parse("i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iButton))
        return nullptr;

    return PyLong_FromLong(ImGui::GetMouseClickedCount(iButton));
}

PyObject*
IsMouseHoveringRect(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"min", "max", "clip", nullptr};

    PyObject* ptMin = Py_None;
    PyObject* ptMax = Py_None;
    int bClip = true;

    if(!pl_parse("OO|p", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptMin, &ptMax, &bClip))
        return nullptr;

    return PyBool_FromLong(ImGui::IsMouseHoveringRect(
        pl__get_vec2_from_python(ptMin),
        pl__get_vec2_from_python(ptMax),
        bClip));
}

PyObject*
IsMousePosValid(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"mouse_pos", nullptr};

    PyObject* ptMousePos = Py_None;

    if(!pl_parse("|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptMousePos))
        return nullptr;

    if(Py_IsNone(ptMousePos))
        return PyBool_FromLong(ImGui::IsMousePosValid());

    ImVec2 tMousePos = pl__get_vec2_from_python(ptMousePos);
    return PyBool_FromLong(ImGui::IsMousePosValid(&tMousePos));
}

PyObject*
IsAnyMouseDown(PyObject* self)
{
    return PyBool_FromLong(ImGui::IsAnyMouseDown());
}

PyObject*
GetMousePos(PyObject* self)
{
    ImVec2 tPos = ImGui::GetMousePos();
    return Py_BuildValue("(ff)", tPos.x, tPos.y);
}

PyObject*
GetMousePosOnOpeningCurrentPopup(PyObject* self)
{
    ImVec2 tPos = ImGui::GetMousePosOnOpeningCurrentPopup();
    return Py_BuildValue("(ff)", tPos.x, tPos.y);
}

PyObject*
IsMouseDragging(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"button", "lock_threshold", nullptr};

    int iButton = 0;
    float fLockThreshold = -1.0f;

    if(!pl_parse("i|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &iButton, &fLockThreshold))
        return nullptr;

    return PyBool_FromLong(ImGui::IsMouseDragging(iButton, fLockThreshold));
}

PyObject*
GetMouseDragDelta(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"button", "lock_threshold", nullptr};

    int iButton = 0;
    float fLockThreshold = -1.0f;

    if(!pl_parse("|if", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &iButton, &fLockThreshold))
        return nullptr;

    ImVec2 tDelta = ImGui::GetMouseDragDelta(iButton, fLockThreshold);
    return Py_BuildValue("(ff)", tDelta.x, tDelta.y);
}

PyObject*
ResetMouseDragDelta(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"button", nullptr};

    int iButton = 0;

    if(!pl_parse("|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iButton))
        return nullptr;

    ImGui::ResetMouseDragDelta(iButton);
    Py_RETURN_NONE;
}

PyObject*
GetMouseCursor(PyObject* self)
{
    return PyLong_FromLong(ImGui::GetMouseCursor());
}

PyObject*
SetMouseCursor(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"cursor_type", nullptr};

    int iCursorType = 0;

    if(!pl_parse("i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iCursorType))
        return nullptr;

    ImGui::SetMouseCursor(iCursorType);
    Py_RETURN_NONE;
}

PyObject*
SetNextFrameWantCaptureMouse(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"want_capture_mouse", nullptr};

    int bWantCaptureMouse = false;

    if(!pl_parse("p", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &bWantCaptureMouse))
        return nullptr;

    ImGui::SetNextFrameWantCaptureMouse(bWantCaptureMouse);
    Py_RETURN_NONE;
}

//-----------------------------------------------------------------------------
// [SECTION] Clipboard Utilities
//-----------------------------------------------------------------------------

PyObject*
GetClipboardText(PyObject* self)
{
    const char* pcText = ImGui::GetClipboardText();
    if(!pcText)
        Py_RETURN_NONE;

    return PyUnicode_FromString(pcText);
}

PyObject*
SetClipboardText(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"text", nullptr};

    const char* pcText = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText))
        return nullptr;

    ImGui::SetClipboardText(pcText);
    Py_RETURN_NONE;
}


//-----------------------------------------------------------------------------
// [SECTION] Settings/.Ini Utilities
//-----------------------------------------------------------------------------

PyObject*
LoadIniSettingsFromDisk(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"ini_filename", nullptr};

    const char* pcIniFilename = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcIniFilename))
        return nullptr;

    ImGui::LoadIniSettingsFromDisk(pcIniFilename);
    Py_RETURN_NONE;
}

PyObject*
LoadIniSettingsFromMemory(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"ini_data", nullptr};

    const char* pcIniData = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcIniData))
        return nullptr;

    ImGui::LoadIniSettingsFromMemory(pcIniData);
    Py_RETURN_NONE;
}

PyObject*
SaveIniSettingsToDisk(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"ini_filename", nullptr};

    const char* pcIniFilename = nullptr;

    if(!pl_parse("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcIniFilename))
        return nullptr;

    ImGui::SaveIniSettingsToDisk(pcIniFilename);
    Py_RETURN_NONE;
}

PyObject*
SaveIniSettingsToMemory(PyObject* self)
{
    const char* pcIniData = ImGui::SaveIniSettingsToMemory();

    if(!pcIniData)
        Py_RETURN_NONE;

    return PyUnicode_FromString(pcIniData);
}

#define PL_ADD_INT_CONSTANT(X_ARG) {#X_ARG, X_ARG}
#define PL_PYTHON_COMMAND(ARG, FLAGS, DOCS) {#ARG, (PyCFunction)ARG, FLAGS | METH_STATIC, DOCS}

typedef struct _pyplDearImGuiI
{
    PyObject_HEAD
} pyplDearImGuiI;

typedef struct _pyImGui
{
    PyObject_HEAD
} pyImGui;

typedef struct _pyImPlot
{
    PyObject_HEAD
} pyImPlot;

static PyMethodDef gatImGuiCommands[] =
{
    PL_PYTHON_COMMAND(Begin, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(End, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(StyleColorsDark, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(StyleColorsLight, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(StyleColorsClassic, METH_NOARGS, NULL),

    // imgui demo/debug/info
    PL_PYTHON_COMMAND(ShowMetricsWindow, METH_O, NULL),
    PL_PYTHON_COMMAND(ShowDebugLogWindow, METH_O, NULL),
    PL_PYTHON_COMMAND(ShowIDStackToolWindow, METH_O, NULL),
    PL_PYTHON_COMMAND(ShowAboutWindow, METH_O, NULL),
    PL_PYTHON_COMMAND(ShowStyleEditor, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(ShowStyleSelector, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(ShowFontSelector, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(ShowUserGuide, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetVersion, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(ShowDemoWindow, METH_O, NULL),
    PL_PYTHON_COMMAND(IsMouseDown, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(IsMouseClicked, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(IsMouseReleased, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(IsMouseDoubleClicked, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(GetMouseClickedCount, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(IsMouseHoveringRect, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(IsMousePosValid, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(IsAnyMouseDown, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetMousePos, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetMousePosOnOpeningCurrentPopup, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(IsMouseDragging, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(GetMouseDragDelta, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(ResetMouseDragDelta, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(GetMouseCursor, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(SetMouseCursor, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetNextFrameWantCaptureMouse, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui clipboard utilities
    PL_PYTHON_COMMAND(GetClipboardText, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(SetClipboardText, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui settings / ini utilities
    PL_PYTHON_COMMAND(LoadIniSettingsFromDisk, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(LoadIniSettingsFromMemory, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SaveIniSettingsToDisk, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SaveIniSettingsToMemory, METH_NOARGS, NULL),

    // imgui id stack/scopes
    PL_PYTHON_COMMAND(PushID, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(PopID, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetID, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui child windows
    PL_PYTHON_COMMAND(BeginChild, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(EndChild, METH_NOARGS, NULL),

    // imgui item/widget utilities and query functions
    PL_PYTHON_COMMAND(IsItemHovered, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(IsItemActive, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(IsItemFocused, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(IsItemClicked, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(IsItemVisible, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(IsItemEdited, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(IsItemActivated, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(IsItemDeactivated, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(IsItemDeactivatedAfterEdit, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(IsItemToggledOpen, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(IsAnyItemHovered, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(IsAnyItemActive, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(IsAnyItemFocused, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetItemID, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetItemRectMin, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetItemRectMax, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetItemRectSize, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(SetNextItemAllowOverlap, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(SetItemDefaultFocus, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(SetKeyboardFocusHere, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui combo box/dropdown widgets
    PL_PYTHON_COMMAND(Combo, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(BeginCombo, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(EndCombo, METH_NOARGS, NULL),

    // imgui list boxes
    PL_PYTHON_COMMAND(BeginListBox, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(EndListBox, METH_NOARGS, NULL),

    // imgui tooltips
    PL_PYTHON_COMMAND(BeginTooltip, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(EndTooltip, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(SetTooltip, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(BeginItemTooltip, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(SetItemTooltip, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui main widgets
    PL_PYTHON_COMMAND(Button, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(Checkbox, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SmallButton, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(InvisibleButton, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(ArrowButton, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(CheckboxFlags, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(RadioButton, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(ProgressBar, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(Bullet, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(TextLink, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(TextLinkOpenURL, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui drag widgets
    PL_PYTHON_COMMAND(DragFloat, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(DragFloat2, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(DragFloat3, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(DragFloat4, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(DragFloatRange2, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(DragInt, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(DragInt2, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(DragInt3, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(DragInt4, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(DragIntRange2, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui slider widgets
    PL_PYTHON_COMMAND(SliderFloat, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SliderFloat2, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SliderFloat3, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SliderFloat4, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SliderAngle, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SliderInt, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SliderInt2, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SliderInt3, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SliderInt4, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(VSliderFloat, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(VSliderInt, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui menus
    PL_PYTHON_COMMAND(BeginMenuBar, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(BeginMainMenuBar, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(EndMenuBar, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(EndMainMenuBar, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(EndMenu, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(BeginMenu, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(MenuItem, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui input widgets
    PL_PYTHON_COMMAND(InputText, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(InputTextMultiline, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(InputTextWithHint, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(InputFloat, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(InputFloat2, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(InputFloat3, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(InputFloat4, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(InputInt, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(InputInt2, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(InputInt3, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(InputInt4, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(InputDouble, METH_VARARGS | METH_KEYWORDS, NULL),
    
    // imgui text widgets
    PL_PYTHON_COMMAND(TextUnformatted, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(Text, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(TextColored, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(TextDisabled, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(TextWrapped, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(LabelText, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(BulletText, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SeparatorText, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui layout
    PL_PYTHON_COMMAND(Separator, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(SameLine, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(NewLine, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(Spacing, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(Dummy, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(Indent, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(Unindent, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(BeginGroup, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(EndGroup, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(AlignTextToFramePadding, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetTextLineHeight, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetTextLineHeightWithSpacing, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetFrameHeight, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetFrameHeightWithSpacing, METH_NOARGS, NULL),

    // imgui layout cursor positioning
    PL_PYTHON_COMMAND(GetCursorScreenPos, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(SetCursorScreenPos, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(GetContentRegionAvail, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetCursorPos, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetCursorPosX, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetCursorPosY, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(SetCursorPos, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetCursorPosX, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetCursorPosY, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(GetCursorStartPos, METH_NOARGS, NULL),

    // imgui window scrolling
    PL_PYTHON_COMMAND(GetScrollX, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetScrollY, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(SetScrollX, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetScrollY, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(GetScrollMaxX, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetScrollMaxY, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(SetScrollHereX, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetScrollHereY, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetScrollFromPosX, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetScrollFromPosY, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui parameters stacks (current window)
    PL_PYTHON_COMMAND(PushItemWidth, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(PopItemWidth, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(SetNextItemWidth, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(CalcItemWidth, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(PushTextWrapPos, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(PopTextWrapPos, METH_NOARGS, NULL),

    // imgui color editor/picker widgets
    PL_PYTHON_COMMAND(ColorEdit3, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(ColorEdit4, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(ColorPicker3, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(ColorPicker4, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(ColorButton, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetColorEditOptions, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui tree widgets
    PL_PYTHON_COMMAND(TreeNode, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(TreeNodeEx, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(TreePush, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(TreePop, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetTreeNodeToLabelSpacing, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(CollapsingHeader, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetNextItemOpen, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetNextItemStorageID, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(TreeNodeGetOpen, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui window utilities
    PL_PYTHON_COMMAND(IsWindowAppearing, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(IsWindowCollapsed, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(IsWindowFocused, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(IsWindowHovered, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(GetWindowDpiScale, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetWindowPos, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetWindowSize, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetWindowWidth, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(GetWindowHeight, METH_NOARGS, NULL),

    // imgui window manipulation
    PL_PYTHON_COMMAND(SetNextWindowPos, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetNextWindowSize, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetNextWindowContentSize, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetNextWindowCollapsed, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetNextWindowFocus, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(SetNextWindowScroll, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetNextWindowBgAlpha, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetNextWindowViewport, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetWindowPos, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetWindowSize, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetWindowCollapsed, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetWindowFocus, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui selectables
    PL_PYTHON_COMMAND(Selectable, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui popups
    PL_PYTHON_COMMAND(BeginPopup, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(BeginPopupModal, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(EndPopup, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(OpenPopup, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(OpenPopupOnItemClick, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(CloseCurrentPopup, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(BeginPopupContextItem, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(BeginPopupContextWindow, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(BeginPopupContextVoid, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(IsPopupOpen, METH_VARARGS | METH_KEYWORDS, NULL),

    // imgui tab bars / tabs
    PL_PYTHON_COMMAND(BeginTabBar, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(EndTabBar, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(BeginTabItem, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(EndTabItem, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(TabItemButton, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(SetTabItemClosed, METH_VARARGS | METH_KEYWORDS, NULL),
    {NULL, NULL, 0, NULL}
};

static PyMethodDef gatPlotCommands[] =
{
    {"ShowDemoWindow", (PyCFunction)ImPlot_ShowDemoWindow, METH_O | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyMethodDef gatplDearImGuiICommands[] =
{
    {"initialize", (PyCFunction)dear_imgui_initialize, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"new_frame", (PyCFunction)dear_imgui_new_frame, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"render", (PyCFunction)dear_imgui_render, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"cleanup", (PyCFunction)dear_imgui_cleanup, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot gatImPlotSlots[] = {
    {Py_tp_methods, (void*)gatPlotCommands},
    {0, 0}
};

static PyType_Slot gatImGuiSlots[] = {
    {Py_tp_methods, (void*)gatImGuiCommands},
    {0, 0}
};

static PyType_Slot gatplDearImGuiISlots[] = {
    {Py_tp_methods, (void*)gatplDearImGuiICommands},
    {0, 0}
};

static PyType_Spec ImPlotSpec = {
    "imgui.ImPlot",
    sizeof(pyImPlot),
    0,
    Py_TPFLAGS_DEFAULT,
    gatImPlotSlots
};

static PyType_Spec ImGuiSpec = {
    "imgui.ImGui",
    sizeof(pyImGui),
    0,
    Py_TPFLAGS_DEFAULT,
    gatImGuiSlots
};

static PyType_Spec plDearImGuiISpec = {
    "imgui.plDearImGuiI",
    sizeof(pyplDearImGuiI),
    0,
    Py_TPFLAGS_DEFAULT,
    gatplDearImGuiISlots
};

static PyMethodDef gatCommands[] =
{
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
    PL_ADD_INT_CONSTANT(ImGuiWindowFlags_NoInputs),

    PL_ADD_INT_CONSTANT(ImGuiSliderFlags_None),
    PL_ADD_INT_CONSTANT(ImGuiSliderFlags_Logarithmic),
    PL_ADD_INT_CONSTANT(ImGuiSliderFlags_NoRoundToFormat),
    PL_ADD_INT_CONSTANT(ImGuiSliderFlags_NoInput),
    PL_ADD_INT_CONSTANT(ImGuiSliderFlags_WrapAround),
    PL_ADD_INT_CONSTANT(ImGuiSliderFlags_ClampOnInput),
    PL_ADD_INT_CONSTANT(ImGuiSliderFlags_ClampZeroRange),
    PL_ADD_INT_CONSTANT(ImGuiSliderFlags_NoSpeedTweaks),
    PL_ADD_INT_CONSTANT(ImGuiSliderFlags_ColorMarkers),
    PL_ADD_INT_CONSTANT(ImGuiSliderFlags_AlwaysClamp),

    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_None),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_CharsDecimal),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_CharsHexadecimal),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_CharsScientific),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_CharsUppercase),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_CharsNoBlank),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_AllowTabInput),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_EnterReturnsTrue),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_EscapeClearsAll),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_CtrlEnterForNewLine),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_ReadOnly),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_Password),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_AlwaysOverwrite),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_AutoSelectAll),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_ParseEmptyRefVal),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_DisplayEmptyRefVal),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_NoHorizontalScroll),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_NoUndoRedo),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_ElideLeft),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_CallbackCompletion),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_CallbackHistory),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_CallbackAlways),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_CallbackCharFilter),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_CallbackResize),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_CallbackEdit),
    PL_ADD_INT_CONSTANT(ImGuiInputTextFlags_WordWrap),

    PL_ADD_INT_CONSTANT(ImGuiButtonFlags_None),
    PL_ADD_INT_CONSTANT(ImGuiButtonFlags_MouseButtonLeft),
    PL_ADD_INT_CONSTANT(ImGuiButtonFlags_MouseButtonRight),
    PL_ADD_INT_CONSTANT(ImGuiButtonFlags_MouseButtonMiddle),
    PL_ADD_INT_CONSTANT(ImGuiButtonFlags_EnableNav),
    PL_ADD_INT_CONSTANT(ImGuiButtonFlags_AllowOverlap),

    PL_ADD_INT_CONSTANT(ImGuiDir_None),
    PL_ADD_INT_CONSTANT(ImGuiDir_Left),
    PL_ADD_INT_CONSTANT(ImGuiDir_Right),
    PL_ADD_INT_CONSTANT(ImGuiDir_Up),
    PL_ADD_INT_CONSTANT(ImGuiDir_Down),
    PL_ADD_INT_CONSTANT(ImGuiDir_COUNT),

    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_None),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_NoAlpha),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_NoPicker),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_NoOptions),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_NoSmallPreview),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_NoInputs),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_NoTooltip),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_NoLabel),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_NoSidePreview),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_NoDragDrop),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_NoBorder),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_NoColorMarkers),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_AlphaOpaque),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_AlphaNoBg),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_AlphaPreviewHalf),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_AlphaBar),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_HDR),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_DisplayRGB),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_DisplayHSV),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_DisplayHex),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_Uint8),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_Float),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_PickerHueBar),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_PickerHueWheel),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_InputRGB),
    PL_ADD_INT_CONSTANT(ImGuiColorEditFlags_InputHSV),
    
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_None),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_Selected),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_Framed),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_AllowOverlap),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_NoTreePushOnOpen),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_NoAutoOpenOnLog),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_DefaultOpen),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_OpenOnDoubleClick),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_OpenOnArrow),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_Leaf),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_Bullet),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_FramePadding),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_SpanAvailWidth),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_SpanFullWidth),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_SpanLabelWidth),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_SpanAllColumns),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_LabelSpanAllColumns),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_NavLeftJumpsToParent),
    PL_ADD_INT_CONSTANT(ImGuiTreeNodeFlags_CollapsingHeader),

    PL_ADD_INT_CONSTANT(ImGuiCond_None),
    PL_ADD_INT_CONSTANT(ImGuiCond_Always),
    PL_ADD_INT_CONSTANT(ImGuiCond_Once),
    PL_ADD_INT_CONSTANT(ImGuiCond_FirstUseEver),
    PL_ADD_INT_CONSTANT(ImGuiCond_Appearing),

    PL_ADD_INT_CONSTANT(ImGuiFocusedFlags_None),
    PL_ADD_INT_CONSTANT(ImGuiFocusedFlags_ChildWindows),
    PL_ADD_INT_CONSTANT(ImGuiFocusedFlags_RootWindow),
    PL_ADD_INT_CONSTANT(ImGuiFocusedFlags_AnyWindow),
    PL_ADD_INT_CONSTANT(ImGuiFocusedFlags_NoPopupHierarchy),
    PL_ADD_INT_CONSTANT(ImGuiFocusedFlags_DockHierarchy),
    PL_ADD_INT_CONSTANT(ImGuiFocusedFlags_RootAndChildWindows),

    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_None),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_ChildWindows),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_RootWindow),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_AnyWindow),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_NoPopupHierarchy),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_DockHierarchy),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_AllowWhenBlockedByPopup),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_AllowWhenOverlappedByItem),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_AllowWhenOverlappedByWindow),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_AllowWhenDisabled),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_NoNavOverride),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_AllowWhenOverlapped),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_RectOnly),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_RootAndChildWindows),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_ForTooltip),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_Stationary),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_DelayNone),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_DelayShort),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_DelayNormal),
    PL_ADD_INT_CONSTANT(ImGuiHoveredFlags_NoSharedDelay),

    PL_ADD_INT_CONSTANT(ImGuiSelectableFlags_None),
    PL_ADD_INT_CONSTANT(ImGuiSelectableFlags_NoAutoClosePopups),
    PL_ADD_INT_CONSTANT(ImGuiSelectableFlags_SpanAllColumns),
    PL_ADD_INT_CONSTANT(ImGuiSelectableFlags_AllowDoubleClick),
    PL_ADD_INT_CONSTANT(ImGuiSelectableFlags_Disabled),
    PL_ADD_INT_CONSTANT(ImGuiSelectableFlags_AllowOverlap),
    PL_ADD_INT_CONSTANT(ImGuiSelectableFlags_Highlight),
    PL_ADD_INT_CONSTANT(ImGuiSelectableFlags_SelectOnNav),

    PL_ADD_INT_CONSTANT(ImGuiPopupFlags_None),
    PL_ADD_INT_CONSTANT(ImGuiPopupFlags_MouseButtonLeft),
    PL_ADD_INT_CONSTANT(ImGuiPopupFlags_MouseButtonRight),
    PL_ADD_INT_CONSTANT(ImGuiPopupFlags_MouseButtonMiddle),
    PL_ADD_INT_CONSTANT(ImGuiPopupFlags_NoReopen),
    PL_ADD_INT_CONSTANT(ImGuiPopupFlags_NoOpenOverExistingPopup),
    PL_ADD_INT_CONSTANT(ImGuiPopupFlags_NoOpenOverItems),
    PL_ADD_INT_CONSTANT(ImGuiPopupFlags_AnyPopupId),
    PL_ADD_INT_CONSTANT(ImGuiPopupFlags_AnyPopupLevel),
    PL_ADD_INT_CONSTANT(ImGuiPopupFlags_AnyPopup),

    PL_ADD_INT_CONSTANT(ImGuiTabBarFlags_None),
    PL_ADD_INT_CONSTANT(ImGuiTabBarFlags_Reorderable),
    PL_ADD_INT_CONSTANT(ImGuiTabBarFlags_AutoSelectNewTabs),
    PL_ADD_INT_CONSTANT(ImGuiTabBarFlags_TabListPopupButton),
    PL_ADD_INT_CONSTANT(ImGuiTabBarFlags_NoCloseWithMiddleMouseButton),
    PL_ADD_INT_CONSTANT(ImGuiTabBarFlags_NoTabListScrollingButtons),
    PL_ADD_INT_CONSTANT(ImGuiTabBarFlags_NoTooltip),
    PL_ADD_INT_CONSTANT(ImGuiTabBarFlags_DrawSelectedOverline),
    PL_ADD_INT_CONSTANT(ImGuiTabBarFlags_FittingPolicyMixed),
    PL_ADD_INT_CONSTANT(ImGuiTabBarFlags_FittingPolicyShrink),
    PL_ADD_INT_CONSTANT(ImGuiTabBarFlags_FittingPolicyScroll),

    PL_ADD_INT_CONSTANT(ImGuiTabItemFlags_None),
    PL_ADD_INT_CONSTANT(ImGuiTabItemFlags_UnsavedDocument),
    PL_ADD_INT_CONSTANT(ImGuiTabItemFlags_SetSelected),
    PL_ADD_INT_CONSTANT(ImGuiTabItemFlags_NoCloseWithMiddleMouseButton),
    PL_ADD_INT_CONSTANT(ImGuiTabItemFlags_NoPushId),
    PL_ADD_INT_CONSTANT(ImGuiTabItemFlags_NoTooltip),
    PL_ADD_INT_CONSTANT(ImGuiTabItemFlags_NoReorder),
    PL_ADD_INT_CONSTANT(ImGuiTabItemFlags_Leading),
    PL_ADD_INT_CONSTANT(ImGuiTabItemFlags_Trailing),
    PL_ADD_INT_CONSTANT(ImGuiTabItemFlags_NoAssumedClosure),

    PL_ADD_INT_CONSTANT(ImGuiItemFlags_None),
    PL_ADD_INT_CONSTANT(ImGuiItemFlags_NoTabStop),
    PL_ADD_INT_CONSTANT(ImGuiItemFlags_NoNav),
    PL_ADD_INT_CONSTANT(ImGuiItemFlags_NoNavDefaultFocus),
    PL_ADD_INT_CONSTANT(ImGuiItemFlags_ButtonRepeat),
    PL_ADD_INT_CONSTANT(ImGuiItemFlags_AutoClosePopups),
    PL_ADD_INT_CONSTANT(ImGuiItemFlags_AllowDuplicateId),
    PL_ADD_INT_CONSTANT(ImGuiItemFlags_Disabled),

    PL_ADD_INT_CONSTANT(ImGuiChildFlags_None),
    PL_ADD_INT_CONSTANT(ImGuiChildFlags_Borders),
    PL_ADD_INT_CONSTANT(ImGuiChildFlags_AlwaysUseWindowPadding),
    PL_ADD_INT_CONSTANT(ImGuiChildFlags_ResizeX),
    PL_ADD_INT_CONSTANT(ImGuiChildFlags_ResizeY),
    PL_ADD_INT_CONSTANT(ImGuiChildFlags_AutoResizeX),
    PL_ADD_INT_CONSTANT(ImGuiChildFlags_AutoResizeY),
    PL_ADD_INT_CONSTANT(ImGuiChildFlags_AlwaysAutoResize),
    PL_ADD_INT_CONSTANT(ImGuiChildFlags_FrameStyle),
    PL_ADD_INT_CONSTANT(ImGuiChildFlags_NavFlattened),

    PL_ADD_INT_CONSTANT(ImGuiComboFlags_None),
    PL_ADD_INT_CONSTANT(ImGuiComboFlags_PopupAlignLeft),
    PL_ADD_INT_CONSTANT(ImGuiComboFlags_HeightSmall),
    PL_ADD_INT_CONSTANT(ImGuiComboFlags_HeightRegular),
    PL_ADD_INT_CONSTANT(ImGuiComboFlags_HeightLarge),
    PL_ADD_INT_CONSTANT(ImGuiComboFlags_HeightLargest),
    PL_ADD_INT_CONSTANT(ImGuiComboFlags_NoArrowButton),
    PL_ADD_INT_CONSTANT(ImGuiComboFlags_NoPreview),
    PL_ADD_INT_CONSTANT(ImGuiComboFlags_WidthFitPreview),

    PL_ADD_INT_CONSTANT(ImGuiMouseButton_Left),
    PL_ADD_INT_CONSTANT(ImGuiMouseButton_Right),
    PL_ADD_INT_CONSTANT(ImGuiMouseButton_Middle),
    PL_ADD_INT_CONSTANT(ImGuiMouseButton_COUNT),

    PL_ADD_INT_CONSTANT(ImGuiMouseCursor_None),
    PL_ADD_INT_CONSTANT(ImGuiMouseCursor_Arrow),
    PL_ADD_INT_CONSTANT(ImGuiMouseCursor_TextInput),
    PL_ADD_INT_CONSTANT(ImGuiMouseCursor_ResizeAll),
    PL_ADD_INT_CONSTANT(ImGuiMouseCursor_ResizeNS),
    PL_ADD_INT_CONSTANT(ImGuiMouseCursor_ResizeEW),
    PL_ADD_INT_CONSTANT(ImGuiMouseCursor_ResizeNESW),
    PL_ADD_INT_CONSTANT(ImGuiMouseCursor_ResizeNWSE),
    PL_ADD_INT_CONSTANT(ImGuiMouseCursor_Hand),
    PL_ADD_INT_CONSTANT(ImGuiMouseCursor_Wait),
    PL_ADD_INT_CONSTANT(ImGuiMouseCursor_Progress),
    PL_ADD_INT_CONSTANT(ImGuiMouseCursor_NotAllowed),
    PL_ADD_INT_CONSTANT(ImGuiMouseCursor_COUNT),
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

    gptImPlotType = PyType_FromSpec(&ImPlotSpec);
    gptImGuiType = PyType_FromSpec(&ImGuiSpec);
    gptplDearImGuiIType = PyType_FromSpec(&plDearImGuiISpec);

    PyModule_AddObject(ptModule, "ImPlot", gptImPlotType);
    PyModule_AddObject(ptModule, "ImGui", gptImGuiType);
    PyModule_AddObject(ptModule, "plDearImGuiI", gptplDearImGuiIType);

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

static ImVec4
pl__get_vec4_from_python(PyObject* ptValue)
{
    ImVec4 tResult = {};

    if (PyTuple_Check(ptValue))
    {
        tResult.x = (float)PyFloat_AsDouble(PyTuple_GetItem(ptValue, 0));
        tResult.y = (float)PyFloat_AsDouble(PyTuple_GetItem(ptValue, 1));
        tResult.z = (float)PyFloat_AsDouble(PyTuple_GetItem(ptValue, 2));
        tResult.w = (float)PyFloat_AsDouble(PyTuple_GetItem(ptValue, 3));
    }

    else if (PyList_Check(ptValue))
    {
        tResult.x = (float)PyFloat_AsDouble(PyList_GetItem(ptValue, 0));
        tResult.y = (float)PyFloat_AsDouble(PyList_GetItem(ptValue, 1));
        tResult.z = (float)PyFloat_AsDouble(PyList_GetItem(ptValue, 2));
        tResult.w = (float)PyFloat_AsDouble(PyList_GetItem(ptValue, 3));
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