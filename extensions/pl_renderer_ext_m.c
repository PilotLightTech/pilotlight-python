/*
   pl_renderer_ext_m.c
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

    plRendererInit tRenderInit = {0};

    PyObject* ptPyDevice = NULL;
    PyObject* ptPySwapchain = NULL;
	if (!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyDevice, &ptPySwapchain))
		return NULL;

    tRenderInit.ptDevice = PyCapsule_GetPointer(ptPyDevice, "plDevice");
    tRenderInit.ptSwapchain = PyCapsule_GetPointer(ptPySwapchain, "plSwapchain");

    return PyBool_FromLong(gptRenderer->initialize(&tRenderInit));
}

PL_PY_BIND_VOID(renderer_cleanup,                 gptRenderer->cleanup())
PL_PY_BIND_VOID(renderer_register_asset_types,    gptRenderer->register_asset_types())
PL_PY_BIND_VOID(renderer_register_ecs_components, gptRenderer->register_ecs_components())

PL_PY_BIND_KEY(renderer_get_asset_type_key_environment,     gptRenderer->get_asset_type_key_environment())
PL_PY_BIND_KEY(renderer_get_asset_type_key_settings,        gptRenderer->get_asset_type_key_settings())
PL_PY_BIND_KEY(renderer_get_ecs_type_key_object,            gptRenderer->get_ecs_type_key_object())
PL_PY_BIND_KEY(renderer_get_ecs_type_key_light,             gptRenderer->get_ecs_type_key_light())
PL_PY_BIND_KEY(renderer_get_ecs_type_key_environment_probe, gptRenderer->get_ecs_type_key_environment_probe())
PL_PY_BIND_KEY(renderer_get_ecs_type_key_terrain,           gptRenderer->get_ecs_type_key_terrain())
PL_PY_BIND_KEY(renderer_get_ecs_type_key_environment,       gptRenderer->get_ecs_type_key_environment())
PL_PY_BIND_KEY(renderer_get_ecs_type_key_renderer,          gptRenderer->get_ecs_type_key_renderer())
PL_PY_BIND_KEY(renderer_get_ecs_type_key_decal,             gptRenderer->get_ecs_type_key_decal())

PL_PY_BIND_LIBRARY(renderer_run_light_update_system,             gptRenderer->run_light_update_system(ptLibrary))
PL_PY_BIND_LIBRARY(renderer_run_object_update_system,            gptRenderer->run_object_update_system(ptLibrary))
PL_PY_BIND_LIBRARY(renderer_run_environment_probe_update_system, gptRenderer->run_environment_probe_update_system(ptLibrary))

PyObject*
renderer_create_scene(PyObject* self, PyObject* unused)
{
    plSceneDesc tSceneDesc = {0};
    plScene* ptScene = gptRenderer->create_scene(&tSceneDesc);
    if(ptScene == NULL)
        Py_RETURN_NONE;
    return PyCapsule_New(ptScene, "plScene", NULL);
}

PyObject*
renderer_destroy_scene(PyObject* self, PyObject* ptPyScene)
{
    plScene* ptScene = PyCapsule_GetPointer(ptPyScene, "plScene");
    if(ptScene == NULL)
        return NULL;
    gptRenderer->destroy_scene(ptScene);
    Py_RETURN_NONE;
}

PyObject*
renderer_load_component_library(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "scene",
        "library",
        NULL,
    };

    PyObject* ptPyScene = NULL;
    PyObject* ptPyLibrary = NULL;
	if (!pl_parse_args("OO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPyScene, &ptPyLibrary))
		return NULL;

    plScene* ptScene = PyCapsule_GetPointer(ptPyScene, "plScene");
    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptScene == NULL || ptLibrary == NULL)
        return NULL;
    gptRenderer->load_component_library(ptScene, ptLibrary);
    Py_RETURN_NONE;
}

PyObject*
renderer_create_view(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "scene",
        "dims",
        NULL,
    };

    PyObject* ptPyScene = NULL;
    PyObject* ptPyDims = NULL;
	if (!pl_parse_args("OO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPyScene, &ptPyDims))
		return NULL;

    plScene* ptScene = PyCapsule_GetPointer(ptPyScene, "plScene");
    if(ptScene == NULL)
        return NULL;

    plVec2 tDims = {0};
    pl_vec2_from_py(ptPyDims, &tDims);

    plViewDesc tViewDesc = {
        .uWidth  = (uint32_t)tDims.x,
        .uHeight = (uint32_t)tDims.y
    };
    plView* ptView = gptRenderer->create_view(ptScene, &tViewDesc);
    if(ptView == NULL)
        Py_RETURN_NONE;
    return PyCapsule_New(ptView, "plView", NULL);
}

PyObject*
renderer_destroy_view(PyObject* self, PyObject* ptPyView)
{
    plView* ptView = PyCapsule_GetPointer(ptPyView, "plView");
    if(ptView == NULL)
        return NULL;
    gptRenderer->destroy_view(ptView);
    Py_RETURN_NONE;
}

PyObject*
renderer_get_drawlist(PyObject* self, PyObject* ptPyView)
{
    plView* ptView = PyCapsule_GetPointer(ptPyView, "plView");
    if(ptView == NULL)
        return NULL;
    return PyCapsule_New(gptRenderer->get_drawlist(ptView), "plDrawList3D", NULL);
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
        .ptCamera = ptCamera
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
    return PyBool_FromLong(gptRenderer->begin_frame());
}

static PyMethodDef gatCommandsplRendererI[] =
{
    {"initialize",                          (PyCFunction)renderer_initialize,                METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"cleanup",                             (PyCFunction)renderer_cleanup,                   METH_NOARGS | METH_STATIC, NULL},
    {"create_scene",                        (PyCFunction)renderer_create_scene,              METH_NOARGS | METH_STATIC, NULL},
    {"destroy_scene",                       (PyCFunction)renderer_destroy_scene,             METH_O | METH_STATIC, NULL},
    {"load_component_library",              (PyCFunction)renderer_load_component_library,    METH_VARARGS | METH_STATIC, NULL},
    {"create_view",                         (PyCFunction)renderer_create_view,               METH_VARARGS | METH_STATIC, NULL},
    {"destroy_view",                        (PyCFunction)renderer_destroy_view,              METH_O | METH_STATIC, NULL},
    {"resize_view",                         (PyCFunction)renderer_resize_view,               METH_VARARGS | METH_STATIC, NULL},
    {"get_view_color_bind_group",           (PyCFunction)renderer_get_view_color_bind_group, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_drawlist",                        (PyCFunction)renderer_get_drawlist,              METH_O | METH_STATIC, NULL},
    {"begin_frame",                         (PyCFunction)renderer_begin_frame,               METH_NOARGS | METH_STATIC, NULL},
    {"prepare_scene",                       (PyCFunction)renderer_prepare_scene,             METH_VARARGS | METH_STATIC, NULL},
    {"prepare_view",                        (PyCFunction)renderer_prepare_view,              METH_VARARGS | METH_STATIC, NULL},
    {"render_view",                         (PyCFunction)renderer_render_view,               METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"register_asset_types",                (PyCFunction)renderer_register_asset_types,      METH_NOARGS | METH_STATIC, NULL},
    {"register_ecs_components",             (PyCFunction)renderer_register_ecs_components,   METH_NOARGS | METH_STATIC, NULL},
    {"get_asset_type_key_environment",      (PyCFunction)renderer_get_asset_type_key_environment,     METH_NOARGS | METH_STATIC, NULL},
    {"get_asset_type_key_settings",         (PyCFunction)renderer_get_asset_type_key_settings,        METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_object",             (PyCFunction)renderer_get_ecs_type_key_object,            METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_light",              (PyCFunction)renderer_get_ecs_type_key_light,             METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_environment_probe",  (PyCFunction)renderer_get_ecs_type_key_environment_probe, METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_terrain",            (PyCFunction)renderer_get_ecs_type_key_terrain,           METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_environment",        (PyCFunction)renderer_get_ecs_type_key_environment,       METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_renderer",           (PyCFunction)renderer_get_ecs_type_key_renderer,          METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_decal",              (PyCFunction)renderer_get_ecs_type_key_decal,             METH_NOARGS | METH_STATIC, NULL},
    {"run_light_update_system",             (PyCFunction)renderer_run_light_update_system,             METH_O | METH_STATIC, NULL},
    {"run_object_update_system",            (PyCFunction)renderer_run_object_update_system,            METH_O | METH_STATIC, NULL},
    {"run_environment_probe_update_system", (PyCFunction)renderer_run_environment_probe_update_system, METH_O | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plRendererI)
