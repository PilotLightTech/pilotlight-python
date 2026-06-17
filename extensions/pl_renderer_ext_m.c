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
        NULL,
    };

    plRendererSettings tRenderSettings = {0};

    PyObject* ptPyDevice = NULL;
    PyObject* ptPySwapchain = NULL;
	if (!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyDevice, &ptPySwapchain))
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
renderer_prepare_scene(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "scene",
        "cameras",
        NULL,
    };

    PyObject* ptPyScene = NULL;
    PyObject* ptPyCameras = NULL;
	if (!pl_parse_args("OO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPyScene, &ptPyCameras))
		return NULL;

    plScene* ptScene = PyCapsule_GetPointer(ptPyScene, "plScene");

    const plCamera* atCameras[4] = {0};
    uint32_t uCameraCount = 0;

    if (PyTuple_Check(ptPyCameras))
    {
        Py_ssize_t pySize = PyTuple_Size(ptPyCameras);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            PyObject* ptPyCamera = PyTuple_GetItem(ptPyCameras, i);
            atCameras[i] = ((pyplCamera*)ptPyCamera)->ptCamera;
            uCameraCount++;
        }
    }

    else if (PyList_Check(ptPyCameras))
    {
        Py_ssize_t pySize = PyList_Size(ptPyCameras);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            PyObject* ptPyCamera = PyList_GetItem(ptPyCameras, i);
            atCameras[i] = ((pyplCamera*)ptPyCamera)->ptCamera;
            uCameraCount++;
        }
    }

    gptRenderer->prepare_scene(ptScene, atCameras, uCameraCount);
    Py_RETURN_NONE;
}

PyObject*
renderer_resize_view(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "view",
        "dims",
        NULL,
    };

    PyObject* ptPyView = NULL;
    PyObject* ptPyDims = NULL;
	if (!pl_parse_args("OO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPyView, &ptPyDims))
		return NULL;

    plView* ptView = PyCapsule_GetPointer(ptPyView, "plView");

    plVec2 tDims = {0};
    pl_vec2_from_py(ptPyDims, &tDims);

    gptRenderer->resize_view(ptView, tDims);
    Py_RETURN_NONE;
}

PyObject*
renderer_prepare_view(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "view",
        "cameras",
        NULL,
    };

    PyObject* ptPyView = NULL;
    PyObject* ptPyCamera = NULL;
	if (!pl_parse_args("OO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPyView, &ptPyCamera))
		return NULL;

    plView* ptView = PyCapsule_GetPointer(ptPyView, "plView");
    plCamera* ptCamera = ((pyplCamera*)ptPyCamera)->ptCamera;

    gptRenderer->prepare_view(ptView, ptCamera);
    Py_RETURN_NONE;
}

PyObject*
renderer_load_test_world(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "path",
        "library",
        NULL,
    };

    const char* pcPath = NULL;
    PyObject* ptPyLibrary = NULL;
	if (!pl_parse_args("sO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcPath, &ptPyLibrary))
		return NULL;

    plTestWorldData tTestData = {0};
    plComponentLibrary* ptLibrary = PyCapsule_GetPointer(ptPyLibrary, "plComponentLibrary");

    bool bResult = gptRenderer->load_test_world(pcPath, ptLibrary, &tTestData);

    PyObject* ptCapsuleScene = PyCapsule_New(tTestData.ptScene, "plScene", NULL);
    PyObject* ptCapsuleView = PyCapsule_New(tTestData.ptView, "plView", NULL);

    PyObject* ptCameraEntity = pl_entity_to_py(gptCameraEcs->get_ecs_type_key(), tTestData.tMainCamera);
    return Py_BuildValue("(pOOO)", bResult, ptCameraEntity, ptCapsuleScene, ptCapsuleView);
}

PyObject*
renderer_render_view(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "view",
        "camera",
        NULL,
    };

    PyObject* ptPyView = NULL;
    PyObject* ptPyCamera = NULL;
	if (!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyView, &ptPyCamera))
		return NULL;

    plView* ptView = PyCapsule_GetPointer(ptPyView, "plView");
    plCamera* ptCamera = ((pyplCamera*)ptPyCamera)->ptCamera;

    plRenderViewDesc tViewDesc0 = {
        .ptCamera = ptCamera,
        .ptCullCamera = ptCamera
    };
    gptRenderer->render_view(ptView, &tViewDesc0);
    Py_RETURN_NONE;
}

PyObject*
renderer_get_view_color_bind_group(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "view",
        NULL,
    };

    PyObject* ptPyView = NULL;
	if (!pl_parse_args("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyView))
		return NULL;

    plView* ptView = PyCapsule_GetPointer(ptPyView, "plView");
    plVec2 tUV = {0};
    plBindGroupHandle tHandle = gptRenderer->get_view_color_bind_group(ptView, &tUV);
    return Py_BuildValue("(iff)", tHandle.uData, tUV.x, tUV.y);
}

PyObject*
renderer_begin_frame(PyObject* self, PyObject* args)
{
    bool bResult = gptRenderer->begin_frame();
    Py_RETURN_NONE;
}

PyObject*
renderer_ecs_run_light_update_system(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "library",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
	if (!pl_parse_args("O", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPyLibrary))
		return NULL;

    plComponentLibrary* ptCompLibrary = PyCapsule_GetPointer(ptPyLibrary, "plComponentLibrary");

    gptRendererEcs->run_light_update_system(ptCompLibrary);
    Py_RETURN_NONE;
}

PyObject*
renderer_ecs_run_skin_update_system(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "library",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
	if (!pl_parse_args("O", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPyLibrary))
		return NULL;

    plComponentLibrary* ptCompLibrary = PyCapsule_GetPointer(ptPyLibrary, "plComponentLibrary");

    gptRendererEcs->run_skin_update_system(ptCompLibrary);
    Py_RETURN_NONE;
}

PyObject*
renderer_ecs_run_object_update_system(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "library",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
	if (!pl_parse_args("O", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPyLibrary))
		return NULL;

    plComponentLibrary* ptCompLibrary = PyCapsule_GetPointer(ptPyLibrary, "plComponentLibrary");

    gptRendererEcs->run_object_update_system(ptCompLibrary);
    Py_RETURN_NONE;
}

PyObject*
renderer_ecs_run_environment_probe_update_system(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "library",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
	if (!pl_parse_args("O", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPyLibrary))
		return NULL;

    plComponentLibrary* ptCompLibrary = PyCapsule_GetPointer(ptPyLibrary, "plComponentLibrary");

    gptRendererEcs->run_environment_probe_update_system(ptCompLibrary);
    Py_RETURN_NONE;
}

static PyMethodDef gatCommandsplRendererI[] =
{
    {"initialize", (PyCFunction)renderer_initialize, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"load_test_world", (PyCFunction)renderer_load_test_world, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"resize_view", (PyCFunction)renderer_resize_view, METH_VARARGS | METH_STATIC, NULL},
    {"prepare_scene", (PyCFunction)renderer_prepare_scene, METH_VARARGS | METH_STATIC, NULL},
    {"get_view_color_bind_group", (PyCFunction)renderer_get_view_color_bind_group, METH_VARARGS | METH_STATIC, NULL},
    {"prepare_view", (PyCFunction)renderer_prepare_view, METH_VARARGS | METH_STATIC, NULL},
    {"render_view", (PyCFunction)renderer_render_view, METH_VARARGS | METH_STATIC, NULL},
    {"begin_frame", (PyCFunction)renderer_begin_frame, METH_NOARGS | METH_STATIC, NULL},
    {"cleanup", (PyCFunction)renderer_cleanup, METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyMethodDef gatCommandsplRendererEcsI[] =
{
    {"register_system", (PyCFunction)renderer_ecs_register_system, METH_NOARGS | METH_STATIC, NULL},
    {"run_light_update_system", (PyCFunction)renderer_ecs_run_light_update_system, METH_VARARGS | METH_STATIC, NULL},
    {"run_skin_update_system", (PyCFunction)renderer_ecs_run_skin_update_system, METH_VARARGS | METH_STATIC, NULL},
    {"run_object_update_system", (PyCFunction)renderer_ecs_run_object_update_system, METH_VARARGS | METH_STATIC, NULL},
    {"run_environment_probe_update_system", (PyCFunction)renderer_ecs_run_environment_probe_update_system, METH_VARARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plRendererI)
PL_NEW_PYTHON_API(plRendererEcsI)