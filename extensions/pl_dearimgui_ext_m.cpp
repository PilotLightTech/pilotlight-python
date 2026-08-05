/*
   pl_dearimgui_ext_m.cpp
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] forward declarations
// [SECTION] custom type stuff
// [SECTION] public api implementations
// [SECTION] helper implementations
// [SECTION] python module prep
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pl_dear_imgui_ext.cpp"

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "imgui.h"
#define PL_MATH_INCLUDE_FUNCTIONS
#include "pl_math.h"

#include "pl_graphics_ext.h"
#include "pl_ds.h"

//-----------------------------------------------------------------------------
// [SECTION] forward declarations
//-----------------------------------------------------------------------------

void pl_fill_int_array_from_python(PyObject* ptPyObject, int* atArray, uint32_t uArraySize);
void pl_fill_int_array_from_c(PyObject* ptPyObject, int* atArray, uint32_t uArraySize);
void pl_fill_float_array_from_python(PyObject* ptPyObject, float* atArray, uint32_t uArraySize);
void pl_fill_float_array_from_c(PyObject* ptPyObject, float* atArray, uint32_t uArraySize);
bool pl_parse_args(const char* formatstring, const char** keywords, PyObject* args, PyObject* kwargs, const char* message, ...);

typedef struct _plPythonIntConstantPair
{
   const char* pcName;
   int         iValue;
} plPythonIntConstantPair;

//-----------------------------------------------------------------------------
// [SECTION] custom type stuff
//-----------------------------------------------------------------------------

static PyObject* gptplDearImGuiIType = nullptr;
static PyObject* gptImGuiType = nullptr;
static PyObject* gptImPlotType = nullptr;

static ImVec2 pl__get_vec2_from_python(PyObject* ptValue);
static ImVec4 pl__get_vec4_from_python(PyObject* ptValue);

//-----------------------------------------------------------------------------
// [SECTION] public api implementations
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
    PyObject* ptPythonAttachmentInfo = NULL;

    static const char* apcKeywords[] = {
        "device",
        "swapchain",
        "attachmentInfo",
        NULL,
    };

	if (!pl_parse_args("OOO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonDevice, &ptPythonSwapchain, &ptPythonAttachmentInfo))
		return NULL;

    plDevice* ptDevice = (plDevice*)PyCapsule_GetPointer(ptPythonDevice, "plDevice");
    plSwapchain* ptSwapchain = (plSwapchain*)PyCapsule_GetPointer(ptPythonSwapchain, "plSwapchain");

    plRenderAttachmentInfo tInfo = {0};
    PyObject* ptPythonAttachmentDepth = PyObject_GetAttrString(ptPythonAttachmentInfo, "eDepthFormat");
    PyLong_AsInt32(ptPythonAttachmentDepth, &tInfo.eDepthFormat);
    Py_DECREF(ptPythonAttachmentDepth);

    ptPythonAttachmentDepth = PyObject_GetAttrString(ptPythonAttachmentInfo, "eStencilFormat");
    PyLong_AsInt32(ptPythonAttachmentDepth, &tInfo.eStencilFormat);
    Py_DECREF(ptPythonAttachmentDepth);

    ptPythonAttachmentDepth = PyObject_GetAttrString(ptPythonAttachmentInfo, "aeColorFormats");
    Py_ssize_t pySize = PyList_Size(ptPythonAttachmentDepth);
    for (Py_ssize_t i = 0; i < pySize; ++i)
    {
        PyObject* ptColor = PyList_GetItem(ptPythonAttachmentDepth, i);
        PyLong_AsInt32(ptColor, &tInfo.aeColorFormats[i]);
    }
    Py_DECREF(ptPythonAttachmentDepth);

    pl_dear_imgui_initialize(nullptr);

    // ImPlot::SetCurrentContext((ImPlotContext*)ptDataRegistry->get_data("implot"));
    ImGuiIO& tImGuiIO = ImGui::GetIO();
    tImGuiIO.IniFilename = nullptr;

    Py_RETURN_NONE;
}

PyObject*
dear_imgui_new_frame(PyObject* self, PyObject* args, PyObject* kwargs)
{

    PyObject* ptPythonDevice = nullptr;

    static const char* apcKeywords[] = {
        "device",
        NULL,
    };

	if (!pl_parse_args("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonDevice))
		return NULL;

    plDevice* ptDevice = (plDevice*)PyCapsule_GetPointer(ptPythonDevice, "plDevice");

    pl_dear_imgui_new_frame(ptDevice);

    Py_RETURN_NONE;
}

PyObject*
dear_imgui_render(PyObject* self, PyObject* args, PyObject* kwargs)
{

    PyObject* ptPythonRenderEncoder = nullptr;

    static const char* apcKeywords[] = {
        "commandBuffer",
        NULL,
    };

	if (!pl_parse_args("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonRenderEncoder))
		return NULL;

    plCommandBuffer* ptCmdBuffer = (plCommandBuffer*)PyCapsule_GetPointer(ptPythonRenderEncoder, "plCommandBuffer");

    pl_dear_imgui_render(ptCmdBuffer);

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

    bool bShow = true;
    if(!Py_IsNone(arg))
    {
        bShow = PyLong_AsLong(arg);
        ImGui::ShowDemoWindow(&bShow);
        return PyBool_FromLong(bShow);
    }
    ImGui::ShowDemoWindow(nullptr);
    Py_RETURN_NONE;
}

PyObject*
ImPlot_ShowDemoWindow(PyObject* self, PyObject* arg)
{

    bool bShow = true;
    if(!Py_IsNone(arg))
    {
        bShow = PyLong_AsLong(arg);
        ImPlot::ShowDemoWindow(&bShow);
        return PyBool_FromLong(bShow);
    }
    ImPlot::ShowDemoWindow(nullptr);
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
    PyObject* ptOpen = Py_None;
    int iFlags = 0;
	if (!pl_parse_args("s|Oi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcText, &ptOpen, &iFlags))
		return nullptr;
    
    bool* pbOpen = nullptr;
    bool bShow = true;
    if(!Py_IsNone(ptOpen))
    {
        bShow = PyLong_AsLong(ptOpen);
        pbOpen = &bShow;
    }

    bool bResult = ImGui::Begin(pcText, pbOpen, iFlags);
    if(pbOpen)
        return Py_BuildValue("(pp)", bResult, bShow);
    return PyBool_FromLong(bResult);
}

PyObject*
End(PyObject* self)
{
    ImGui::End();
    Py_RETURN_NONE;
}

PyObject*
Image(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "texture",
        "size",
        "uv0",
        "uv1",
        nullptr,
    };
    PyObject* ptPyTexture = nullptr;
    PyObject* ptSize = Py_None;
    PyObject* ptUV0 = Py_None;
    PyObject* ptUV1 = Py_None;
	if (!pl_parse_args("OO|OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyTexture, &ptSize, &ptUV0, &ptUV1))
		return nullptr;
    
    ImVec2 tSize = pl__get_vec2_from_python(ptSize);

    ImVec2 tUV0 = {};
    if(!Py_IsNone(ptUV0))
        tUV0 = pl__get_vec2_from_python(ptUV0);

    ImVec2 tUV1 = {1.0f, 1.0f};
    if(!Py_IsNone(ptUV1))
        tUV1 = pl__get_vec2_from_python(ptUV1);

    void* ptTexture = (plDevice*)PyCapsule_GetPointer(ptPyTexture, "void");

    // ImTextureRef tTexture = ImTextureRef(ptTexture);

    // ImGui::Image(tTexture, tSize, tUV0, tUV1);
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
	if (!pl_parse_args("s|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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
	if (!pl_parse_args("s|p", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &bEnabled))
		return nullptr;
    
    return PyBool_FromLong(ImGui::BeginMenu(pcLabel, bEnabled));
}

PyObject*
MenuItemSimple(PyObject* self, PyObject* args, PyObject* kwargs)
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
    const char* pcShortcut = "";
    int iSelected = false;
    int bEnabled = true;
	if (!pl_parse_args("ssp|p", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &pcShortcut, &iSelected, &bEnabled))
		return nullptr;
    
    bool bSelected = iSelected;

    bool bActivated = ImGui::MenuItem(pcLabel, pcShortcut, bSelected, bEnabled);
    return Py_BuildValue("(pp)", bActivated, bSelected);
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
    const char* pcShortcut = "";
    int iSelected = false;
    int bEnabled = true;
	if (!pl_parse_args("ssp|p", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &pcShortcut, &iSelected, &bEnabled))
		return nullptr;
    
    bool bSelected = iSelected;
    bool* pbSelected = &bSelected;

    bool bActivated = ImGui::MenuItem(pcLabel, pcShortcut, pbSelected, bEnabled);
    return Py_BuildValue("(pp)", bActivated, bSelected);
}

PyObject*
Checkbox(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "label",
        "value",
        nullptr,
    };
    const char* pcLabel = nullptr;
    int iValue = 0;
	if (!pl_parse_args("sp", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &iValue))
		return nullptr;
    
    bool bValue = iValue;
    bool bResult = ImGui::Checkbox(pcLabel, &bValue);
    return Py_BuildValue("(pp)", bResult, bValue);
}

PyObject*
DragFloat(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "speed", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    float fValue = 0.0f;
    float fSpeed = 1.0f;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse_args("sf|fffsi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &fValue, &fSpeed, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    bool bResult = ImGui::DragFloat(pcLabel, &fValue, fSpeed, fMin, fMax, pcFormat, iFlags);
    return Py_BuildValue("(pf)", bResult, fValue);
}

PyObject*
DragFloat2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "speed", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fSpeed = 1.0f;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse_args("sO|fffsi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fSpeed, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    float atValues[4] = {};
    pl_fill_float_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::DragFloat2(pcLabel, atValues, fSpeed, fMin, fMax, pcFormat, iFlags);
    if(bResult)
        pl_fill_float_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
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

    if(!pl_parse_args("sO|fffsi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fSpeed, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    float atValues[4] = {};
    pl_fill_float_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::DragFloat3(pcLabel, atValues, fSpeed, fMin, fMax, pcFormat, iFlags);
    if(bResult)
        pl_fill_float_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
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

    if(!pl_parse_args("sO|fffsi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fSpeed, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    float atValues[4] = {};
    pl_fill_float_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::DragFloat4(pcLabel, atValues, fSpeed, fMin, fMax, pcFormat, iFlags);
    if(bResult)
        pl_fill_float_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
}

PyObject*
DragFloatRange2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "min_value", "max_value", "speed", "min", "max",
        "format", "format_max", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    float fMinValue = 0.0f;
    float fMaxValue = 0.0f;
    float fSpeed = 1.0f;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    const char* pcFormatMax = nullptr;
    int iFlags = 0;

    if(!pl_parse_args("sff|fffzzi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &fMinValue, &fMaxValue, &fSpeed, &fMin, &fMax,
        &pcFormat, &pcFormatMax, &iFlags))
        return nullptr;

    bool bResult = ImGui::DragFloatRange2(pcLabel, &fMinValue, &fMaxValue, fSpeed, fMin, fMax, pcFormat, pcFormatMax, iFlags);
    return Py_BuildValue("(pff)", bResult, fMinValue, fMaxValue);
}

PyObject*
DragInt(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value_pointer", "speed", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    int iValue = 0;
    float fSpeed = 1.0f;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    int iFlags = 0;

    if(!pl_parse_args("si|fiisi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &iValue, &fSpeed, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    bool bResult = ImGui::DragInt(pcLabel, &iValue, fSpeed, iMin, iMax, pcFormat, iFlags);
    return Py_BuildValue("(pi)", bResult, iValue);
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

    if(!pl_parse_args("sO|fiisi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fSpeed, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    int atValues[4] = {};
    pl_fill_int_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::DragInt2(pcLabel, atValues, fSpeed, iMin, iMax, pcFormat, iFlags);
    if(bResult)
        pl_fill_int_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
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

    if(!pl_parse_args("sO|fiisi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fSpeed, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    int atValues[4] = {};
    pl_fill_int_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::DragInt3(pcLabel, atValues, fSpeed, iMin, iMax, pcFormat, iFlags);
    if(bResult)
        pl_fill_int_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
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

    if(!pl_parse_args("sO|fiisi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fSpeed, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    int atValues[4] = {};
    pl_fill_int_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::DragInt4(pcLabel, atValues, fSpeed, iMin, iMax, pcFormat, iFlags);
    if(bResult)
        pl_fill_int_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
}

PyObject*
DragIntRange2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "min_value", "max_value", "speed", "min", "max",
        "format", "format_max", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    int iMinValue = 0;
    int iMaxValue = 0;
    float fSpeed = 1.0f;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    const char* pcFormatMax = nullptr;
    int iFlags = 0;

    if(!pl_parse_args("sii|fiizzi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &iMinValue, &iMaxValue, &fSpeed, &iMin, &iMax,
        &pcFormat, &pcFormatMax, &iFlags))
        return nullptr;

    bool bResult = ImGui::DragIntRange2(pcLabel, &iMinValue, &iMaxValue, fSpeed, iMin, iMax, pcFormat, pcFormatMax, iFlags);

    return Py_BuildValue("(pii)", bResult, iMinValue, iMaxValue);
}

PyObject*
SliderFloat(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    float fValue = 0.0f;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse_args("sfff|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &fValue, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;


    bool bResult = ImGui::SliderFloat(pcLabel, &fValue, fMin, fMax, pcFormat, iFlags);
    return Py_BuildValue("(pf)", bResult, fValue);
}

PyObject*
SliderFloat2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse_args("sOff|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    float atValues[4] = {};
    pl_fill_float_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::SliderFloat2(pcLabel, atValues, fMin, fMax, pcFormat, iFlags);
    if(bResult)
        pl_fill_float_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
}

PyObject*
SliderFloat3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse_args("sOff|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    float atValues[4] = {};
    pl_fill_float_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::SliderFloat3(pcLabel, atValues, fMin, fMax, pcFormat, iFlags);
    if(bResult)
        pl_fill_float_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
}

PyObject*
SliderFloat4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse_args("sOff|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    float atValues[4] = {};
    pl_fill_float_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::SliderFloat4(pcLabel, atValues, fMin, fMax, pcFormat, iFlags);
    if(bResult)
        pl_fill_float_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
}

PyObject*
SliderAngle(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "radians", "degrees_min", "degrees_max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    float fRadians = 0.0f;
    float fDegreesMin = -360.0f;
    float fDegreesMax = 360.0f;
    const char* pcFormat = "%.0f deg";
    int iFlags = 0;

    if(!pl_parse_args("sf|ffsi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &fRadians, &fDegreesMin, &fDegreesMax, &pcFormat, &iFlags))
        return nullptr;

    bool bResult = ImGui::SliderAngle(pcLabel, &fRadians, fDegreesMin, fDegreesMax, pcFormat, iFlags);
    return Py_BuildValue("(pf)", bResult, fRadians);
}

PyObject*
SliderInt(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    int iValue = 0;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    int iFlags = 0;

    if(!pl_parse_args("siii|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &iValue, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    bool bResult = ImGui::SliderInt(pcLabel, &iValue, iMin, iMax, pcFormat, iFlags);
    return Py_BuildValue("(pi)", bResult, iValue);
}

PyObject*
SliderInt2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    int iFlags = 0;

    if(!pl_parse_args("sOii|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    int atValues[4] = {};
    pl_fill_int_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::SliderInt2(pcLabel, atValues, iMin, iMax, pcFormat, iFlags);
    if(bResult)
        pl_fill_int_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
}

PyObject*
SliderInt3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    int iFlags = 0;

    if(!pl_parse_args("sOii|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    int atValues[4] = {};
    pl_fill_int_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::SliderInt3(pcLabel, atValues, iMin, iMax, pcFormat, iFlags);
    if(bResult)
        pl_fill_int_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
}

PyObject*
SliderInt4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    int iFlags = 0;

    if(!pl_parse_args("sOii|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    int atValues[4] = {};
    pl_fill_int_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::SliderInt4(pcLabel, atValues, iMin, iMax, pcFormat, iFlags);
    if(bResult)
        pl_fill_int_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
}

PyObject*
VSliderFloat(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "size", "value", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptSize = Py_None;
    float fValue = 0.0f;
    float fMin = 0.0f;
    float fMax = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse_args("sOfff|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptSize, &fValue, &fMin, &fMax, &pcFormat, &iFlags))
        return nullptr;

    ImVec2 tSize = pl__get_vec2_from_python(ptSize);

    bool bResult = ImGui::VSliderFloat(pcLabel, tSize, &fValue, fMin, fMax, pcFormat, iFlags);
    return Py_BuildValue("(pf)", bResult, fValue);
}

PyObject*
VSliderInt(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "size", "value", "min", "max", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptSize = Py_None;
    int iValue = 0;
    int iMin = 0;
    int iMax = 0;
    const char* pcFormat = "%d";
    int iFlags = 0;

    if(!pl_parse_args("sOiii|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptSize, &iValue, &iMin, &iMax, &pcFormat, &iFlags))
        return nullptr;

    ImVec2 tSize = pl__get_vec2_from_python(ptSize);

    bool bResult = ImGui::VSliderInt(pcLabel, tSize, &iValue, iMin, iMax, pcFormat, iFlags);
    return Py_BuildValue("(pi)", bResult, iValue);
}

PyObject*
InputText(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    const char* pcString = nullptr;
    int iFlags = 0;

    if(!pl_parse_args("ss|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &pcString, &iFlags))
        return nullptr;

    char acBuffer[256] = {0};
    strncpy(acBuffer, pcString, 256);

    bool bResult = ImGui::InputText(pcLabel, acBuffer, 256, iFlags);
    return Py_BuildValue("(ps)", bResult, acBuffer);
}

PyObject*
InputTextMultiline(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "size", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    const char* pcString = nullptr;
    PyObject* ptSize = Py_None;
    int iFlags = 0;

    if(!pl_parse_args("ss|Oi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &pcString, &ptSize, &iFlags))
        return nullptr;

    ImVec2 tSize = {};
    if(!Py_IsNone(ptSize))
        tSize = pl__get_vec2_from_python(ptSize);


    char acBuffer[256] = {0};
    strncpy(acBuffer, pcString, 256);

    bool bResult = ImGui::InputTextMultiline(pcLabel, acBuffer, 256, tSize, iFlags);
    return Py_BuildValue("(ps)", bResult, acBuffer);
}

PyObject*
InputTextWithHint(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "hint", "value", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    const char* pcHint = nullptr;
    const char* pcString = nullptr;
    int iFlags = 0;

    if(!pl_parse_args("sss|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &pcHint, &pcString, &iFlags))
        return nullptr;

    char acBuffer[256] = {0};
    strncpy(acBuffer, pcString, 256);
    bool bResult = ImGui::InputTextWithHint(pcLabel, pcHint, acBuffer, 256, iFlags);
    return Py_BuildValue("(ps)", bResult, acBuffer);
}

PyObject*
InputFloat(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "step", "step_fast", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    float fValue = 0.0f;
    float fStep = 0.0f;
    float fStepFast = 0.0f;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse_args("sf|ffsi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &fValue, &fStep, &fStepFast, &pcFormat, &iFlags))
        return nullptr;

    bool bResult = ImGui::InputFloat(pcLabel, &fValue, fStep, fStepFast, pcFormat, iFlags);

    return Py_BuildValue("(pf)", bResult, fValue);
}

PyObject*
InputFloat2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse_args("sO|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &pcFormat, &iFlags))
        return nullptr;

    float atValues[4] = {};
    pl_fill_float_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::InputFloat2(pcLabel, atValues, pcFormat, iFlags);
    if(bResult)
        pl_fill_float_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
}

PyObject*
InputFloat3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse_args("sO|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &pcFormat, &iFlags))
        return nullptr;

    float atValues[4] = {};
    pl_fill_float_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::InputFloat3(pcLabel, atValues, pcFormat, iFlags);
    if(bResult)
        pl_fill_float_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
}

PyObject*
InputFloat4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    const char* pcFormat = "%.3f";
    int iFlags = 0;

    if(!pl_parse_args("sO|si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &pcFormat, &iFlags))
        return nullptr;

    float atValues[4] = {};
    pl_fill_float_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::InputFloat4(pcLabel, atValues, pcFormat, iFlags);
    if(bResult)
        pl_fill_float_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
}

PyObject*
InputInt(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "step", "step_fast", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    int iValue = 0;
    int iStep = 1;
    int iStepFast = 100;
    int iFlags = 0;

    if(!pl_parse_args("si|iii", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &iValue, &iStep, &iStepFast, &iFlags))
        return nullptr;

    bool bResult = ImGui::InputInt(pcLabel, &iValue, iStep, iStepFast, iFlags);
    return Py_BuildValue("(pi)", bResult, iValue);
}

PyObject*
InputInt2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    int iFlags = 0;

    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &iFlags))
        return nullptr;

    int atValues[4] = {};
    pl_fill_int_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::InputInt2(pcLabel, atValues, iFlags);
    if(bResult)
        pl_fill_int_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
}

PyObject*
InputInt3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    int iFlags = 0;

    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &iFlags))
        return nullptr;

    int atValues[4] = {};
    pl_fill_int_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::InputInt3(pcLabel, atValues, iFlags);
    if(bResult)
        pl_fill_int_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
}

PyObject*
InputInt4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptPointer = Py_None;
    int iFlags = 0;

    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptPointer, &iFlags))
        return nullptr;

    int atValues[4] = {};
    pl_fill_int_array_from_python(ptPointer, atValues, 4);
    bool bResult = ImGui::InputInt4(pcLabel, atValues, iFlags);
    if(bResult)
        pl_fill_int_array_from_c(ptPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptPointer);
}

PyObject*
InputDouble(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "label", "value", "step", "step_fast", "format", "flags", nullptr,
    };

    const char* pcLabel = nullptr;
    double dValue = 0.0;
    double dStep = 0.0;
    double dStepFast = 0.0;
    const char* pcFormat = "%.6f";
    int iFlags = 0;

    if(!pl_parse_args("sd|ddsi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &dValue, &dStep, &dStepFast, &pcFormat, &iFlags))
        return nullptr;

    bool bResult = ImGui::InputDouble(pcLabel, &dValue, dStep, dStepFast, pcFormat, iFlags);
    return Py_BuildValue("(pd)", bResult, dValue);
}

PyObject*
TextUnformatted(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "text",
        nullptr,
    };

    const char* pcText = nullptr;

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("Os", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("ss", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("|ff", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fScrollX))
        return nullptr;

    ImGui::SetScrollX(fScrollX);
    Py_RETURN_NONE;
}

PyObject*
SetScrollY(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"scroll_y", nullptr};

    float fScrollY = 0.0f;

    if(!pl_parse_args("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fScrollY))
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

    if(!pl_parse_args("|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fCenterXRatio))
        return nullptr;

    ImGui::SetScrollHereX(fCenterXRatio);
    Py_RETURN_NONE;
}

PyObject*
SetScrollHereY(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"center_y_ratio", nullptr};

    float fCenterYRatio = 0.5f;

    if(!pl_parse_args("|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fCenterYRatio))
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

    if(!pl_parse_args("f|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("f|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fItemWidth))
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

    if(!pl_parse_args("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fItemWidth))
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

    if(!pl_parse_args("|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fWrapLocalPosX))
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

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel))
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

    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("si", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcStrId, &iDir))
        return nullptr;

    return PyBool_FromLong(ImGui::ArrowButton(pcStrId, (ImGuiDir)iDir));
}

PyObject*
CheckboxFlags(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "flags", "flags_value", nullptr};

    const char* pcLabel = nullptr;
    int iFlags = 0;
    int iFlagsValue = 0;

    if(!pl_parse_args("sii", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &iFlags, &iFlagsValue))
        return nullptr;

    bool bResult = ImGui::CheckboxFlags(pcLabel, &iFlags, iFlagsValue);
    return Py_BuildValue("(pi)", bResult, iFlags);
}

PyObject*
RadioButton(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "active", "value_pointer", "button_value", nullptr};

    const char* pcLabel = nullptr;
    PyObject* ptSecondParam = nullptr;
    int iButtonValue = 0;

    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptSecondParam, &iButtonValue))
        return nullptr;

    if(PyBool_Check(ptSecondParam))
    {
        bool bActive = PyLong_AsLong(ptSecondParam);
        return PyBool_FromLong(ImGui::RadioButton(pcLabel, bActive));
    }

    int iValue = PyLong_AsLong(ptSecondParam);
    bool bResult = ImGui::RadioButton(pcLabel, &iValue, iButtonValue);
    return Py_BuildValue("(pi)", bResult, iValue);
}

PyObject*
ProgressBar(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"fraction", "size", "overlay", nullptr};

    float fFraction = 0.0f;
    PyObject* ptSize = Py_None;
    const char* pcOverlay = nullptr;

    if(!pl_parse_args("f|Oz", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel))
        return nullptr;

    return PyBool_FromLong(ImGui::TextLink(pcLabel));
}

PyObject*
TextLinkOpenURL(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "url", nullptr};

    const char* pcLabel = nullptr;
    const char* pcUrl = nullptr;

    if(!pl_parse_args("s|z", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &pcUrl))
        return nullptr;

    return PyBool_FromLong(ImGui::TextLinkOpenURL(pcLabel, pcUrl));
}

PyObject*
ColorEdit3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "color", "flags", nullptr};

    const char* pcLabel = nullptr;
    PyObject* ptColorPointer = Py_None;
    int iFlags = 0;

    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptColorPointer, &iFlags))
        return nullptr;

    float atValues[4] = {};
    pl_fill_float_array_from_python(ptColorPointer, atValues, 4);
    bool bResult = ImGui::ColorEdit3(pcLabel, atValues, iFlags);
    if(bResult)
        pl_fill_float_array_from_c(ptColorPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptColorPointer);
}

PyObject*
ColorEdit4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "color", "flags", nullptr};

    const char* pcLabel = nullptr;
    PyObject* ptColorPointer = Py_None;
    int iFlags = 0;

    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptColorPointer, &iFlags))
        return nullptr;

    float atValues[4] = {};
    pl_fill_float_array_from_python(ptColorPointer, atValues, 4);
    bool bResult = ImGui::ColorEdit4(pcLabel, atValues, iFlags);
    if(bResult)
        pl_fill_float_array_from_c(ptColorPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptColorPointer);
}

PyObject*
ColorPicker3(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", " color", "flags", nullptr};

    const char* pcLabel = nullptr;
    PyObject* ptColorPointer = Py_None;
    int iFlags = 0;

    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptColorPointer, &iFlags))
        return nullptr;

    float atValues[4] = {};
    pl_fill_float_array_from_python(ptColorPointer, atValues, 4);
    bool bResult = ImGui::ColorPicker3(pcLabel, atValues, iFlags);
    if(bResult)
        pl_fill_float_array_from_c(ptColorPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptColorPointer);
}

PyObject*
ColorPicker4(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "color", "flags", "ref_color", nullptr};

    const char* pcLabel = nullptr;
    PyObject* ptColorPointer = Py_None;
    int iFlags = 0;
    PyObject* ptRefColorPointer = Py_None;

    if(!pl_parse_args("sO|iO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptColorPointer, &iFlags, &ptRefColorPointer))
        return nullptr;

    float atValues[4] = {};
    pl_fill_float_array_from_python(ptColorPointer, atValues, 4);
    float atRefValues[4] = {};
    pl_fill_float_array_from_python(ptRefColorPointer, atRefValues, 4);
    bool bResult = false;
    if(Py_IsNone(ptRefColorPointer))
        ImGui::ColorPicker4(pcLabel, atValues, iFlags);
    else
        ImGui::ColorPicker4(pcLabel, atValues, iFlags, atRefValues);
    if(bResult)
        pl_fill_float_array_from_c(ptColorPointer, atValues, 4);

    return Py_BuildValue("(pO)", bResult, ptColorPointer);
}

PyObject*
ColorButton(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"desc_id", "color", "flags", "size", nullptr};

    const char* pcDescId = nullptr;
    PyObject* ptColor = Py_None;
    int iFlags = 0;
    PyObject* ptSize = Py_None;

    if(!pl_parse_args("sO|iO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iFlags))
        return nullptr;

    ImGui::SetColorEditOptions(iFlags);
    Py_RETURN_NONE;
}

PyObject*
TreeNode(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", nullptr};

    const char* pcLabel = nullptr;

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel))
        return nullptr;

    return PyBool_FromLong(ImGui::TreeNode(pcLabel));
}

PyObject*
TreeNodeEx(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "flags", nullptr};

    const char* pcLabel = nullptr;
    int iFlags = 0;

    if(!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &iFlags))
        return nullptr;

    return PyBool_FromLong(ImGui::TreeNodeEx(pcLabel, iFlags));
}

PyObject*
TreePush(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"str_id", nullptr};

    const char* pcStrId = nullptr;

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcStrId))
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
        "visible",
        "flags",
        nullptr,
    };

    const char* pcLabel = nullptr;
    PyObject* ptSecondParam = Py_None;
    int iFlags = 0;

    if(!pl_parse_args("sO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptSecondParam, &iFlags))
        return nullptr;

    if(PyBool_Check(ptSecondParam))
    {
        bool bVisible = PyLong_AsLong(ptSecondParam);
        bool bResult = ImGui::CollapsingHeader(pcLabel, &bVisible, iFlags);
        return Py_BuildValue("(pp)", bResult, bVisible);
    }

    return PyBool_FromLong(ImGui::CollapsingHeader(pcLabel, iFlags));
}

PyObject*
SetNextItemOpen(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"is_open", "cond", nullptr};

    int bIsOpen = false;
    int iCond = 0;

    if(!pl_parse_args("p|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("I", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("I", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iFlags))
        return nullptr;

    return PyBool_FromLong(ImGui::IsWindowFocused(iFlags));
}

PyObject*
IsWindowHovered(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"flags", nullptr};

    int iFlags = 0;

    if(!pl_parse_args("|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iFlags))
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

    if(!pl_parse_args("O|iO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("O|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptSize))
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

    if(!pl_parse_args("p|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptScroll))
        return nullptr;

    ImGui::SetNextWindowScroll(pl__get_vec2_from_python(ptScroll));
    Py_RETURN_NONE;
}

PyObject*
SetNextWindowBgAlpha(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"alpha", nullptr};

    float fAlpha = 0.0f;

    if(!pl_parse_args("f", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &fAlpha))
        return nullptr;

    ImGui::SetNextWindowBgAlpha(fAlpha);
    Py_RETURN_NONE;
}

PyObject*
SetNextWindowViewport(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"viewport_id", nullptr};

    uint32_t uViewportId = 0;

    if(!pl_parse_args("I", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uViewportId))
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

    if(!pl_parse_args("O|iz", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("O|iz", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("p|iz", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("|z", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcName))
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
        nullptr,
    };

    const char* pcLabel = nullptr;
    int bSelected = false;
    int iFlags = 0;
    PyObject* ptSize = Py_None;

    if(!pl_parse_args("s|piO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &bSelected, &iFlags, &ptSize))
        return nullptr;

    ImVec2 tSize = {};
    if(!Py_IsNone(ptSize))
        tSize = pl__get_vec2_from_python(ptSize);

    bool bResult = ImGui::Selectable(pcLabel, &bSelected, iFlags, tSize);

    return Py_BuildValue("(pp)", bResult, bSelected);
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

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText))
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

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText))
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

    if(!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("s|Oi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcName, &ptOpen, &iFlags))
        return nullptr;

    bool bValue = true;
    bool* pbOpen = nullptr;
    if(!Py_IsNone(ptOpen))
    {
        bValue = PyLong_AsLong(ptOpen);
        pbOpen = &bValue;
    }

    bool bResult = ImGui::BeginPopupModal(pcName, pbOpen, iFlags);

    if(pbOpen)
        return Py_BuildValue("(pp)", bResult, bValue);
    return Py_BuildValue("(pO)", bResult, Py_None);
    
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

    if(!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("|zi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("|zi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("|zi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("|zi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("s|Oi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &ptOpen, &iFlags))
        return nullptr;

    bool bValue = true;
    bool* pbOpen = nullptr;
    if(!Py_IsNone(ptOpen))
    {
        bValue = PyLong_AsLong(ptOpen);
        pbOpen = &bValue;
    }

    bool bResult = ImGui::BeginTabItem(pcLabel, pbOpen, iFlags);

    if(pbOpen)
        return Py_BuildValue("(pp)", bResult, bValue);
    return Py_BuildValue("(pO)", bResult, Py_None);
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

    if(!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &iFlags))
        return nullptr;

    return PyBool_FromLong(ImGui::TabItemButton(pcLabel, iFlags));
}

PyObject*
SetTabItemClosed(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"tab_or_docked_window_label", nullptr};

    const char* pcLabel = nullptr;

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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
    if(!pl_parse_args("|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iFlags))
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
    if(!pl_parse_args("|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iMouseButton))
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
    if(!pl_parse_args("|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iOffset))
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

    if(!pl_parse_args("s|Oii", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptId))
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

    if(!pl_parse_args("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptId))
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
    bool bShow = true;
    if(!Py_IsNone(arg))
    {
        bShow = PyLong_AsLong(arg);
        ImGui::ShowMetricsWindow(&bShow);
        return PyBool_FromLong(bShow);
    }

    ImGui::ShowMetricsWindow(nullptr);
    Py_RETURN_NONE;
}

PyObject*
ShowDebugLogWindow(PyObject* self, PyObject* arg)
{
    bool bShow = true;
    if(!Py_IsNone(arg))
    {
        bShow = PyLong_AsLong(arg);
        ImGui::ShowDebugLogWindow(&bShow);
        return PyBool_FromLong(bShow);
    }

    ImGui::ShowDebugLogWindow(nullptr);
    Py_RETURN_NONE;
}

PyObject*
ShowIDStackToolWindow(PyObject* self, PyObject* arg)
{
    bool bShow = true;
    if(!Py_IsNone(arg))
    {
        bShow = PyLong_AsLong(arg);
        ImGui::ShowIDStackToolWindow(&bShow);
        return PyBool_FromLong(bShow);
    }

    ImGui::ShowIDStackToolWindow(nullptr);
    Py_RETURN_NONE;
}

PyObject*
ShowAboutWindow(PyObject* self, PyObject* arg)
{
    bool bShow = true;
    if(!Py_IsNone(arg))
    {
        bShow = PyLong_AsLong(arg);
        ImGui::ShowAboutWindow(&bShow);
        return PyBool_FromLong(bShow);
    }

    ImGui::ShowAboutWindow(nullptr);
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

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel))
        return nullptr;

    return PyBool_FromLong(ImGui::ShowStyleSelector(pcLabel));
}

PyObject*
ShowFontSelector(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", nullptr};

    const char* pcLabel = nullptr;

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcLabel))
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

    if(!pl_parse_args("ss|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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
    int iCurrentItem = 0;
    PyObject* ptItems = nullptr;
    int popup_max_height_in_items = -1;
	if (!pl_parse_args("siO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcLabel, &iCurrentItem, &ptItems, &popup_max_height_in_items))
		return nullptr;
    
    bool bActivated = false;
    if(PyList_Check(ptItems))
    {
        Py_ssize_t pySize = PyList_Size(ptItems);
        pl_sb_resize(sbtEntries, (uint32_t)pySize);
        for(Py_ssize_t i = 0; i < pySize; i++)
        {
            PyObject* item = PyList_GetItem(ptItems, i);
            sbtEntries[i] = PyUnicode_AsUTF8(item);
        }

        bActivated = ImGui::Combo(pcLabel, &iCurrentItem, sbtEntries, (int)pySize, popup_max_height_in_items);
        pl_sb_reset(sbtEntries);
    }
    else
    {
        const char* pcItems = PyUnicode_AsUTF8(ptItems);
        bActivated = ImGui::Combo(pcLabel, &iCurrentItem, pcItems, popup_max_height_in_items);
    }
    return Py_BuildValue("(pi)", bActivated, iCurrentItem);
}


PyObject*
BeginListBox(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"label", "size", nullptr};

    const char* pcLabel = nullptr;
    PyObject* ptSize = Py_None;

    if(!pl_parse_args("s|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iButton))
        return nullptr;

    return PyBool_FromLong(ImGui::IsMouseDown(iButton));
}

PyObject*
IsMouseClicked(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"button", "repeat", nullptr};

    int iButton = 0;
    int bRepeat = false;

    if(!pl_parse_args("i|p", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &iButton, &bRepeat))
        return nullptr;

    return PyBool_FromLong(ImGui::IsMouseClicked(iButton, bRepeat));
}

PyObject*
IsMouseReleased(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"button", nullptr};

    int iButton = 0;

    if(!pl_parse_args("i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iButton))
        return nullptr;

    return PyBool_FromLong(ImGui::IsMouseReleased(iButton));
}

PyObject*
IsMouseDoubleClicked(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"button", nullptr};

    int iButton = 0;

    if(!pl_parse_args("i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iButton))
        return nullptr;

    return PyBool_FromLong(ImGui::IsMouseDoubleClicked(iButton));
}

PyObject*
GetMouseClickedCount(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"button", nullptr};

    int iButton = 0;

    if(!pl_parse_args("i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iButton))
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

    if(!pl_parse_args("OO|p", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("i|f", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("|if", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iButton))
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

    if(!pl_parse_args("i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &iCursorType))
        return nullptr;

    ImGui::SetMouseCursor(iCursorType);
    Py_RETURN_NONE;
}

PyObject*
SetNextFrameWantCaptureMouse(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"want_capture_mouse", nullptr};

    int bWantCaptureMouse = false;

    if(!pl_parse_args("p", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
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

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcText))
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

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcIniFilename))
        return nullptr;

    ImGui::LoadIniSettingsFromDisk(pcIniFilename);
    Py_RETURN_NONE;
}

PyObject*
LoadIniSettingsFromMemory(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"ini_data", nullptr};

    const char* pcIniData = nullptr;

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcIniData))
        return nullptr;

    ImGui::LoadIniSettingsFromMemory(pcIniData);
    Py_RETURN_NONE;
}

PyObject*
SaveIniSettingsToDisk(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"ini_filename", nullptr};

    const char* pcIniFilename = nullptr;

    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcIniFilename))
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

//-----------------------------------------------------------------------------
// [SECTION] helper implementations
//-----------------------------------------------------------------------------

bool
pl_parse_args(const char* formatstring, const char** keywords, PyObject* args, PyObject* kwargs, const char* message, ...)
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

void
pl_fill_float_array_from_python(PyObject* ptPyObject, float* atArray, uint32_t uArraySize)
{
    if (PyTuple_Check(ptPyObject))
    {
        Py_ssize_t pySize = pl_min(uArraySize, PyTuple_Size(ptPyObject));
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            atArray[i] = (float)PyFloat_AsDouble(PyTuple_GetItem(ptPyObject, i));
        }
    }

    else if (PyList_Check(ptPyObject))
    {
        Py_ssize_t pySize = pl_min(uArraySize, PyList_Size(ptPyObject));
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            atArray[i] = (float)PyFloat_AsDouble(PyList_GetItem(ptPyObject, i));
        }
    }
}

void
pl_fill_float_array_from_c(PyObject* ptPyObject, float* atArray, uint32_t uArraySize)
{
    if (PyTuple_Check(ptPyObject))
    {
        Py_ssize_t pySize = pl_min(uArraySize, PyTuple_Size(ptPyObject));
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            PyTuple_SetItem(ptPyObject, i, PyFloat_FromDouble((double)atArray[i]));
        }
    }

    else if (PyList_Check(ptPyObject))
    {
        Py_ssize_t pySize = pl_min(uArraySize, PyList_Size(ptPyObject));
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            PyList_SetItem(ptPyObject, i, PyFloat_FromDouble((double)atArray[i]));
        }
    }
}

void
pl_fill_int_array_from_python(PyObject* ptPyObject, int* atArray, uint32_t uArraySize)
{
    if (PyTuple_Check(ptPyObject))
    {
        Py_ssize_t pySize = pl_min(uArraySize, PyTuple_Size(ptPyObject));
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            atArray[i] = PyLong_AsLong(PyTuple_GetItem(ptPyObject, i));
        }
    }

    else if (PyList_Check(ptPyObject))
    {
        Py_ssize_t pySize = pl_min(uArraySize, PyList_Size(ptPyObject));
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            atArray[i] = PyLong_AsLong(PyList_GetItem(ptPyObject, i));
        }
    }
}

void
pl_fill_int_array_from_c(PyObject* ptPyObject, int* atArray, uint32_t uArraySize)
{
    if (PyTuple_Check(ptPyObject))
    {
        Py_ssize_t pySize = pl_min(uArraySize, PyTuple_Size(ptPyObject));
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            PyTuple_SetItem(ptPyObject, i, PyLong_FromInt32(atArray[i]));
        }
    }

    else if (PyList_Check(ptPyObject))
    {
        Py_ssize_t pySize = pl_min(uArraySize, PyList_Size(ptPyObject));
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            PyList_SetItem(ptPyObject, i, PyLong_FromInt32(atArray[i]));
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] python module prep
//-----------------------------------------------------------------------------

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
    PL_PYTHON_COMMAND(MenuItemSimple, METH_VARARGS | METH_KEYWORDS, NULL),

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
    
    // image widgets
    PL_PYTHON_COMMAND(Image, METH_VARARGS | METH_KEYWORDS, NULL),

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