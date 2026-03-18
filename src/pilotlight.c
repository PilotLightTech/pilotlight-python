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



//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

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
#include "pl_mesh_ext_m.c"
#include "pl_physics_ext_m.c"
#include "pl_renderer_ext_m.c"
#include "pl_script_ext_m.c"
#include "pl_core_m.c"


//-----------------------------------------------------------------------------
// [SECTION] forward declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] core module api
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// [SECTION] helper api implementation
//-----------------------------------------------------------------------------

bool
pl_parse(char* formatstring, const char** keywords, PyObject* args, PyObject* kwargs, const char* message, ...)
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

plPythonEntity
pl_get_entity_from_python(PyObject* ptValue)
{
    plPythonEntity tResult = {0};

    if (PyTuple_Check(ptValue))
    {
        tResult.tKey = PyLong_AsUnsignedLong(PyTuple_GetItem(ptValue, 0));
        tResult.tEntity.uIndex = PyLong_AsUnsignedLong(PyTuple_GetItem(ptValue, 1));
        tResult.tEntity.uGeneration = PyLong_AsUnsignedLong(PyTuple_GetItem(ptValue, 2));
    }
    return tResult;
}

plVec2
pl_get_vec2_from_python(PyObject* ptValue)
{

    plVec2 tResult = {0};

    if (PyTuple_Check(ptValue))
    {
        Py_ssize_t pySize = PyTuple_Size(ptValue);
        pySize = pl_min(pySize, 2);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            tResult.d[i] = (float)PyFloat_AsDouble(PyTuple_GetItem(ptValue, i));
        }
    }

    else if (PyList_Check(ptValue))
    {
        Py_ssize_t pySize = PyList_Size(ptValue);
        pySize = pl_min(pySize, 2);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            tResult.d[i] = (float)PyFloat_AsDouble(PyList_GetItem(ptValue, i));
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

plDVec3
pl_get_dvec3_from_python(PyObject* ptValue)
{
    plDVec3 tResult = {0};

    if (PyTuple_Check(ptValue))
    {
        Py_ssize_t pySize = PyTuple_Size(ptValue);
        pySize = pl_min(pySize, 3);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            tResult.d[i] = PyFloat_AsDouble(PyTuple_GetItem(ptValue, i));
        }
    }

    else if (PyList_Check(ptValue))
    {
        Py_ssize_t pySize = PyList_Size(ptValue);
        pySize = pl_min(pySize, 3);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            tResult.d[i] = PyFloat_AsDouble(PyList_GetItem(ptValue, i));
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

//-----------------------------------------------------------------------------
// [SECTION] python api registration
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
    ptWindows2 = pl_get_api_latest(ptApiRegistry, plWindowI);

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

#define PL_PYTHON_COMMAND(ARG, FLAGS, DOCS) {#ARG, (PyCFunction)ARG, FLAGS, DOCS}

static PyMethodDef gatCommands[] =
{

    {"run", (PyCFunction)pl_python_run, METH_O, NULL},

    // core API
    PL_PYTHON_COMMAND(set_pointer_value, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(get_pointer_value, METH_O, NULL),
    PL_PYTHON_COMMAND(create_bool_pointer, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(create_int_pointer, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(create_float_pointer, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(destroy_bool_pointer, METH_O, NULL),
    PL_PYTHON_COMMAND(destroy_int_pointer, METH_O, NULL),
    PL_PYTHON_COMMAND(destroy_float_pointer, METH_O, NULL),

    // window API
    PL_PYTHON_COMMAND(plWindowI_create, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plWindowI_show, METH_O, NULL),
    PL_PYTHON_COMMAND(plWindowI_destroy, METH_O, NULL),

    // vfs API
    PL_PYTHON_COMMAND(plVfsI_mount_directory, METH_VARARGS | METH_KEYWORDS, NULL),
    
    // io API
    PL_PYTHON_COMMAND(plIOI_get_version_string, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_get_io, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_new_frame, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_is_key_pressed, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_is_key_released, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_is_key_down, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_get_key_pressed_amount, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_is_mouse_down, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_is_mouse_released, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_is_mouse_double_clicked, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_is_mouse_clicked, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_is_mouse_dragging, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_reset_mouse_drag_delta, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_get_mouse_drag_delta, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_get_mouse_pos, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_get_mouse_wheel, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_is_mouse_pos_valid, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plIOI_set_mouse_cursor, METH_VARARGS, NULL),

    // draw API
    PL_PYTHON_COMMAND(plDrawI_add_triangle_filled, METH_VARARGS | METH_KEYWORDS, NULL),

    // ui API
    PL_PYTHON_COMMAND(plUiI_begin_window, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plUiI_end_window, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plUiI_button, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plUiI_checkbox, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plUiI_input_text, METH_VARARGS | METH_KEYWORDS, NULL),

    // graphics API
    PL_PYTHON_COMMAND(plGraphicsI_flush_device, METH_VARARGS | METH_KEYWORDS, NULL),

    // starter API
    PL_PYTHON_COMMAND(plStarterI_begin_frame, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plStarterI_initialize, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plStarterI_finalize, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plStarterI_cleanup, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plStarterI_resize, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plStarterI_end_frame, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plStarterI_get_foreground_layer, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plStarterI_get_background_layer, METH_VARARGS | METH_KEYWORDS, NULL),

    // shader API
    PL_PYTHON_COMMAND(plShaderI_initialize, METH_O, NULL),
    PL_PYTHON_COMMAND(plShaderI_cleanup, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plShaderI_load_glsl, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plShaderI_compile_glsl, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plShaderI_write_to_disk, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(plStarterI_get_device, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plStarterI_get_swapchain, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plStarterI_get_render_pass, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plStarterI_begin_main_pass, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plStarterI_end_main_pass, METH_VARARGS | METH_KEYWORDS, NULL),

    // pak API
    PL_PYTHON_COMMAND(plPakI_begin_packing, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(plPakI_add_from_disk, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(plPakI_end_packing, METH_O, NULL),

    // stats API
    PL_PYTHON_COMMAND(plStatsI_new_frame, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plStatsI_get_counter, METH_O, NULL),

    // screen log API
    PL_PYTHON_COMMAND(plScreenLogI_clear, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plScreenLogI_add_message, METH_VARARGS, NULL),

    // ecs API
    PL_PYTHON_COMMAND(plEcsI_initialize, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plEcsI_finalize, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plEcsI_cleanup, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plEcsI_get_default_library, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plEcsI_get_component, METH_VARARGS | METH_KEYWORDS, NULL),

    // animation API
    PL_PYTHON_COMMAND(plAnimationI_register_ecs_system, METH_NOARGS, NULL),

    // camera API
    PL_PYTHON_COMMAND(plCameraI_register_ecs_system, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plCameraI_create_perspective, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plCameraI_set_fov, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plCameraI_update, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plCameraI_get_ecs_type_key, METH_NOARGS, NULL),

    // material API
    PL_PYTHON_COMMAND(plMaterialI_register_ecs_system, METH_NOARGS, NULL),

    // mesh API
    PL_PYTHON_COMMAND(plMeshI_register_ecs_system, METH_NOARGS, NULL),

    // physics API
    PL_PYTHON_COMMAND(plPhysicsI_register_ecs_system, METH_NOARGS, NULL),

    // shader variant API
    PL_PYTHON_COMMAND(plShaderVariantI_initialize, METH_VARARGS | METH_KEYWORDS, NULL),

    // renderer API
    PL_PYTHON_COMMAND(plRendererI_initialize, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plRendererI_create_directional_light, METH_VARARGS | METH_KEYWORDS, NULL),
    PL_PYTHON_COMMAND(plRendererI_cleanup, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(plRendererI_register_ecs_system, METH_NOARGS, NULL),

    // script API
    PL_PYTHON_COMMAND(plScriptI_register_ecs_system, METH_NOARGS, NULL),

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

    // add constants
    for(uint32_t i = 0; i < PL_ARRAYSIZE(gatCoreIntPairs); i++)
        PyModule_AddIntConstant(ptModule, gatCoreIntPairs[i].pcName, gatCoreIntPairs[i].iValue);

    for(uint32_t i = 0; i < PL_ARRAYSIZE(gatStarterIntPairs); i++)
        PyModule_AddIntConstant(ptModule, gatStarterIntPairs[i].pcName, gatStarterIntPairs[i].iValue);

    for(uint32_t i = 0; i < PL_ARRAYSIZE(gatShaderIntPairs); i++)
        PyModule_AddIntConstant(ptModule, gatShaderIntPairs[i].pcName, gatShaderIntPairs[i].iValue);

    const plApiRegistryI* ptApiRegistry = pl__python_load();

    PyObject* ptApiRegistryCapsule = PyCapsule_New((void*)ptApiRegistry, "plApiRegistryI", NULL);

    if (PyModule_AddObject(ptModule, "plApiRegistryICapsule", ptApiRegistryCapsule) < 0)
    {
        return NULL;
    }

    return ptModule;
}