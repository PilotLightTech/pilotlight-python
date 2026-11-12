/*
   pilotlight.c
*/

/*
Index of this file:
// [SECTION] unity build
// [SECTION] helper api implementation
// [SECTION] core module api
// [SECTION] python module prep
*/

//-----------------------------------------------------------------------------
// [SECTION] unity build
//-----------------------------------------------------------------------------

#include "pl_py_types.c"
#include "pl_starter_ext_m.c"
#include "pl_draw_ext_m.c"
#include "pl_ui_ext_m.c"
#include "pl_vfs_ext_m.c"
#include "pl_graphics_ext_m.c"
#include "pl_shader_ext_m.c"
#include "pl_shader_library_ext_m.c"
#include "pl_pak_ext_m.c"
#include "pl_stats_ext_m.c"
#include "pl_screen_log_ext_m.c"
#include "pl_ecs_ext_m.c"
#include "pl_animation_ext_m.c"
#include "pl_camera_ext_m.c"
#include "pl_material_ext_m.c"
#include "pl_resource_ext_m.c"
#include "pl_mesh_ext_m.c"
#include "pl_physics_ext_m.c"
#include "pl_renderer_ext_m.c"
#include "pl_script_ext_m.c"
#include "pl_job_ext_m.c"
#include "pl_asset_ext_m.c"
#include "pl_terrain_ext_m.c"
#include "pl_platform_ext_m.c"
#include "pl_gltf_ext_m.c"
#include "pl_transform_ext_m.c"
#include "pl_skeleton_ext_m.c"
#include "pl_ik_ext_m.c"
#include "pl_texture_ext_m.c"
#include "pl_core_m.c"

//-----------------------------------------------------------------------------
// [SECTION] helper api implementation
//-----------------------------------------------------------------------------

static int
pl_add_u32_constant(PyObject* module, const char* name, uint32_t value)
{
    PyObject* obj = PyLong_FromUnsignedLong((unsigned long)value);
    if(!obj)
        return -1;

    int result = PyModule_AddObject(module, name, obj);
    if(result < 0)
    {
        Py_DECREF(obj);
        return -1;
    }

    return 0; // module steals reference on success
}

bool
pl_parse_args(char* formatstring, const char** keywords, PyObject* args, PyObject* kwargs, const char* message, ...)
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

//-----------------------------------------------------------------------------
// [SECTION] python module prep
//-----------------------------------------------------------------------------

// defined in pl_main.c
const plApiRegistryI* pl__python_load(void);
void                  pl__python_setup(void);
int                   pl__python_run(PyObject* ptApp);
void                  pl__python_shutdown(void);

PyObject*
pl_python_run(PyObject* self, PyObject* ptApp)
{
    // the app object must provide these methods
    static const char* apcRequired[] = {"pl_app_load", "pl_app_update", "pl_app_resize", "pl_app_shutdown"};
    for(uint32_t i = 0; i < PL_ARRAYSIZE(apcRequired); i++)
    {
        if(!PyObject_HasAttrString(ptApp, apcRequired[i]))
        {
            PyErr_Format(PyExc_AttributeError, "pl_run(): app object is missing required method '%s'", apcRequired[i]);
            return NULL;
        }
    }

    if(pl__python_run(ptApp) != 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "pl_run(): python app raised an exception (see traceback above)");
        return NULL;
    }
    Py_RETURN_NONE;
}

static void
pl_python_module_free(void* pUnused)
{
    pl__python_shutdown();
}

static PyMethodDef gatCommands[] =
{
    {"pl_run", (PyCFunction)pl_python_run, METH_O, NULL},
    {"pl_set_pointer_value", (PyCFunction)set_pointer_value, METH_VARARGS, NULL},
    {"pl_get_pointer_value", (PyCFunction)get_pointer_value, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
PyInit_pilotlight(void)
{

    static PyModuleDef tPilotLightModule = {0};
	tPilotLightModule = (PyModuleDef){
		PyModuleDef_HEAD_INIT,
        "pilotlight",
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

    PL_ADD_UINT_CONSTANT(ptModule, PL_COLOR_32_WHITE);
    PL_ADD_UINT_CONSTANT(ptModule, PL_COLOR_32_BLACK);
    PL_ADD_UINT_CONSTANT(ptModule, PL_COLOR_32_RED);
    PL_ADD_UINT_CONSTANT(ptModule, PL_COLOR_32_BLUE);
    PL_ADD_UINT_CONSTANT(ptModule, PL_COLOR_32_DARK_BLUE);
    PL_ADD_UINT_CONSTANT(ptModule, PL_COLOR_32_GREEN);
    PL_ADD_UINT_CONSTANT(ptModule, PL_COLOR_32_YELLOW);
    PL_ADD_UINT_CONSTANT(ptModule, PL_COLOR_32_ORANGE);
    PL_ADD_UINT_CONSTANT(ptModule, PL_COLOR_32_MAGENTA);
    PL_ADD_UINT_CONSTANT(ptModule, PL_COLOR_32_CYAN);
    PL_ADD_UINT_CONSTANT(ptModule, PL_COLOR_32_GREY);
    PL_ADD_UINT_CONSTANT(ptModule, PL_COLOR_32_LIGHT_GREY);

    // add constants
    for(uint32_t i = 0; i < PL_ARRAYSIZE(gatCoreIntPairs); i++)
        PyModule_AddIntConstant(ptModule, gatCoreIntPairs[i].pcName, gatCoreIntPairs[i].iValue);

    for(uint32_t i = 0; i < PL_ARRAYSIZE(gatStarterIntPairs); i++)
        PyModule_AddIntConstant(ptModule, gatStarterIntPairs[i].pcName, gatStarterIntPairs[i].iValue);

    for(uint32_t i = 0; i < PL_ARRAYSIZE(gatShaderIntPairs); i++)
        PyModule_AddIntConstant(ptModule, gatShaderIntPairs[i].pcName, gatShaderIntPairs[i].iValue);

    for(uint32_t i = 0; i < PL_ARRAYSIZE(gatDrawIntPairs); i++)
        PyModule_AddIntConstant(ptModule, gatDrawIntPairs[i].pcName, gatDrawIntPairs[i].iValue);

    for(uint32_t i = 0; i < PL_ARRAYSIZE(gatCameraIntPairs); i++)
        PyModule_AddIntConstant(ptModule, gatCameraIntPairs[i].pcName, gatCameraIntPairs[i].iValue);

    for(uint32_t i = 0; i < PL_ARRAYSIZE(gatAssetIntPairs); i++)
        PyModule_AddIntConstant(ptModule, gatAssetIntPairs[i].pcName, gatAssetIntPairs[i].iValue);

    for(uint32_t i = 0; i < PL_ARRAYSIZE(gatGltfIntPairs); i++)
        PyModule_AddIntConstant(ptModule, gatGltfIntPairs[i].pcName, gatGltfIntPairs[i].iValue);

    for(uint32_t i = 0; i < PL_ARRAYSIZE(gatScriptIntPairs); i++)
        PyModule_AddIntConstant(ptModule, gatScriptIntPairs[i].pcName, gatScriptIntPairs[i].iValue);

    for(uint32_t i = 0; i < PL_ARRAYSIZE(gatUiIntPairs); i++)
        PyModule_AddIntConstant(ptModule, gatUiIntPairs[i].pcName, gatUiIntPairs[i].iValue);

    for(uint32_t i = 0; i < PL_ARRAYSIZE(gatPlatformIntPairs); i++)
        PyModule_AddIntConstant(ptModule, gatPlatformIntPairs[i].pcName, gatPlatformIntPairs[i].iValue);

    // load core apis & extensions so APIs can be used before pl_run()
    pl__python_setup();
    const plApiRegistryI* ptApiRegistry = pl__python_load();

    PyObject* ptApiRegistryCapsule = PyCapsule_New((void*)ptApiRegistry, "plApiRegistryI", NULL);

    if (PyModule_AddObject(ptModule, "plApiRegistryICapsule", ptApiRegistryCapsule) < 0)
    {
        return NULL;
    }

    gptVec2Type          = PyType_FromSpec(&pl_vec2_spec);
    gptVec3Type          = PyType_FromSpec(&pl_vec3_spec);
    gptVec4Type          = PyType_FromSpec(&pl_vec4_spec);
    gptMat4Type          = PyType_FromSpec(&pl_mat4_spec);
    gptIOType            = PyType_FromSpec(&pl_io_spec);
    gptSwapchainInfoType = PyType_FromSpec(&pl_swapchain_info_spec);
    gptCameraType        = PyType_FromSpec(&pl_camera_spec);
    
    PyModule_AddObject(ptModule, "plVec2", gptVec2Type);
    PyModule_AddObject(ptModule, "plVec3", gptVec3Type);
    PyModule_AddObject(ptModule, "plVec4", gptVec4Type);
    PyModule_AddObject(ptModule, "plMat4", gptMat4Type);
    PyModule_AddObject(ptModule, "plCamera", gptCameraType);
    PyModule_AddObject(ptModule, "plIO", gptIOType);
    PyModule_AddObject(ptModule, "plSwapchainInfo", gptSwapchainInfoType);

    PL_ADD_PYTHON_API(plCameraI);
    PL_ADD_PYTHON_API(plCameraEcsI);
    PL_ADD_PYTHON_API(plUiI);
    PL_ADD_PYTHON_API(plVfsI);
    PL_ADD_PYTHON_API(plPakI);
    PL_ADD_PYTHON_API(plStatsI);
    PL_ADD_PYTHON_API(plDrawI);
    PL_ADD_PYTHON_API(plGraphicsI);
    PL_ADD_PYTHON_API(plStarterI);
    PL_ADD_PYTHON_API(plIOI);
    PL_ADD_PYTHON_API(plShaderI);
    PL_ADD_PYTHON_API(plShaderLibraryI);
    PL_ADD_PYTHON_API(plWindowI);
    PL_ADD_PYTHON_API(plScreenLogI);
    PL_ADD_PYTHON_API(plEcsI);
    PL_ADD_PYTHON_API(plAnimationI);
    PL_ADD_PYTHON_API(plMaterialI);
    PL_ADD_PYTHON_API(plMeshI);
    PL_ADD_PYTHON_API(plPhysicsI);
    PL_ADD_PYTHON_API(plScriptI);
    PL_ADD_PYTHON_API(plRendererI);
    PL_ADD_PYTHON_API(plResourceI);
    PL_ADD_PYTHON_API(plJobI);
    PL_ADD_PYTHON_API(plTransformI);
    PL_ADD_PYTHON_API(plSkeletonI);
    PL_ADD_PYTHON_API(plIkI);
    PL_ADD_PYTHON_API(plTextureI);
    PL_ADD_PYTHON_API(plAssetI);
    PL_ADD_PYTHON_API(plGltfI);
    PL_ADD_PYTHON_API(plTerrainI);
    PL_ADD_PYTHON_API(plFileI);

    return ptModule;
}