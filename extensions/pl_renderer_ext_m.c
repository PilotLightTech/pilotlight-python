/*
   pl_ecs_ext_m.c
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

typedef struct _pyplRendererI
{
    PyObject_HEAD
} pyplRendererI;

typedef struct _pyplRendererEcsI
{
    PyObject_HEAD
} pyplRendererEcsI;

PyObject*
renderer_initialize(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "ptDevice",
        "ptSwap",
        "uMaxTextureResolution",
        NULL,
    };

    plRendererSettings tRenderSettings = {
        .uMaxTextureResolution = 1024
    };

    PyObject* ptPyDevice = NULL;
    PyObject* ptPySwapchain = NULL;
	if (!pl_parse("OO|$I", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyDevice, &ptPySwapchain, &tRenderSettings.uMaxTextureResolution))
		return NULL;

    tRenderSettings.ptDevice = PyCapsule_GetPointer(ptPyDevice, "plDevice");
    tRenderSettings.ptSwapchain = PyCapsule_GetPointer(ptPySwapchain, "plSwapchain");

    gptRenderer->initialize(&tRenderSettings);
    Py_RETURN_NONE;
}

PyObject*
renderer_cleanup(PyObject* self)
{
 
    gptRenderer->cleanup();
    Py_RETURN_NONE;
}

PyObject*
renderer_ecs_register_system(PyObject* self)
{
 
    gptRendererEcs->register_system();
    Py_RETURN_NONE;
}

PyObject*
renderer_ecs_create_directional_light(PyObject* self, PyObject* args, PyObject* kwargs)
{

    PyObject* ptPyLibrary = NULL;

    static const char* apcKeywords[] = {
        "library",
        "name",
        NULL,
    };

    const char* pcName = NULL;
	if (!pl_parse("Os", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyLibrary, &pcName))
		return NULL;
 
    plComponentLibrary* ptCompLibrary = PyCapsule_GetPointer(ptPyLibrary, "plComponentLibrary");

    plLightComponent* ptLight = NULL;
    gptRendererEcs->create_directional_light(ptCompLibrary, pcName, pl_create_vec3(0.425f, -1.0f, -0.384f), &ptLight);
    ptLight->uCascadeCount = 4;
    ptLight->fIntensity = 20.0f;
    ptLight->uShadowResolution = 1024 * 2;
    ptLight->afCascadeSplits[0] = 0.05f;
    ptLight->afCascadeSplits[1] = 0.15f;
    ptLight->afCascadeSplits[2] = 0.25f;
    ptLight->afCascadeSplits[3] = 1.00f;
    ptLight->tFlags |= PL_LIGHT_FLAG_CAST_SHADOW | PL_LIGHT_FLAG_VISUALIZER;

    Py_RETURN_NONE;
}

static PyMethodDef gatplRendererICommands[] =
{
    {"initialize", (PyCFunction)renderer_initialize, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"cleanup", (PyCFunction)renderer_cleanup, METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot gatplRendererISlots[] = {
    {Py_tp_methods, (void*)gatplRendererICommands},
    {0, 0}
};

static PyType_Spec plRendererISpec = {
    "pilotlight.plRendererI",
    sizeof(pyplRendererI),
    0,
    Py_TPFLAGS_DEFAULT,
    gatplRendererISlots
};

static PyMethodDef gatplRendererEcsICommands[] =
{
    {"create_directional_light", (PyCFunction)renderer_ecs_create_directional_light, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"register_system", (PyCFunction)renderer_ecs_register_system, METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot gatplRendererEcsISlots[] = {
    {Py_tp_methods, (void*)gatplRendererEcsICommands},
    {0, 0}
};

static PyType_Spec plRendererEcsISpec = {
    "pilotlight.plRendererEcsI",
    sizeof(pyplRendererEcsI),
    0,
    Py_TPFLAGS_DEFAULT,
    gatplRendererEcsISlots
};