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
    tRenderSettings.ptSwap = PyCapsule_GetPointer(ptPySwapchain, "plSwapchain");

    gptRenderer->initialize(tRenderSettings);
    Py_RETURN_NONE;
}

PyObject*
renderer_cleanup(PyObject* self)
{
 
    gptRenderer->cleanup();
    Py_RETURN_NONE;
}

PyObject*
renderer_register_ecs_system(PyObject* self)
{
 
    gptRenderer->register_ecs_system();
    Py_RETURN_NONE;
}

PyObject*
renderer_create_directional_light(PyObject* self, PyObject* args, PyObject* kwargs)
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
    gptRenderer->create_directional_light(ptCompLibrary, pcName, pl_create_vec3(0.425f, -1.0f, -0.384f), &ptLight);
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