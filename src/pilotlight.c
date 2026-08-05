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
#include "pl_shader_variant_ext_m.c"
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

const plApiRegistryI* pl__python_load(void);
void pl__python_setup(void);
int pl__python_pre_update(void);
int pl__python_shutdown(void);

void pl_load_platform_ext(plApiRegistryI* ptApiRegistry, bool bReload);

PyObject*
pl_python_run(PyObject* self, PyObject* arg)
{

    const plApiRegistryI* ptApiRegistry = pl__python_load();
    pl__python_setup();
    
    pl_load_ext((plApiRegistryI*)ptApiRegistry, false);
    pl_load_platform_ext((plApiRegistryI*)ptApiRegistry, false);
    plIO* ptIO = gptIOI->get_io();

    PyObject* ptResult = PyObject_CallMethod(arg, "pl_app_load", NULL);

    if(!ptResult)
    {
        PyErr_Print();   // <-- this will tell you why
        Py_RETURN_NONE;
    }

    Py_DECREF(ptResult);

    while (ptIO->bRunning)
    {
        int iResult = pl__python_pre_update();

        if(iResult == 1)
        {
            ptResult = PyObject_CallMethod(arg, "pl_app_resize", NULL);
            if(!ptResult)
            {
                PyErr_Print();   // <-- this will tell you why
                Py_RETURN_NONE;
            }

            Py_DECREF(ptResult);
        }

        if(!ptIO->bViewportMinimized)
        {
            ptResult = PyObject_CallMethod(arg, "pl_app_update", NULL);
            if(!ptResult)
            {
                PyErr_Print();   // <-- this will tell you why
                Py_RETURN_NONE;
            }

            Py_DECREF(ptResult);
        }
    }

    ptResult = PyObject_CallMethod(arg, "pl_app_shutdown", NULL);
    if(!ptResult)
    {
        PyErr_Print();   // <-- this will tell you why
        Py_RETURN_NONE;
    }

    pl__python_shutdown();
    Py_RETURN_NONE;
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
    PL_ADD_PYTHON_API(plShaderVariantI);
    PL_ADD_PYTHON_API(plWindowI);
    PL_ADD_PYTHON_API(plScreenLogI);
    PL_ADD_PYTHON_API(plEcsI);
    PL_ADD_PYTHON_API(plAnimationI);
    PL_ADD_PYTHON_API(plMaterialI);
    PL_ADD_PYTHON_API(plMeshI);
    PL_ADD_PYTHON_API(plPhysicsI);
    PL_ADD_PYTHON_API(plScriptI);
    PL_ADD_PYTHON_API(plRendererI);
    PL_ADD_PYTHON_API(plRendererEcsI);
    PL_ADD_PYTHON_API(plResourceI);
    PL_ADD_PYTHON_API(plJobI);

    return ptModule;
}