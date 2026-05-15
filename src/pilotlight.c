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
#include "pl_py_math.c"


//-----------------------------------------------------------------------------
// [SECTION] forward declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] core module api
//-----------------------------------------------------------------------------


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

plVec2*
pl_get_vec2_list_from_python(PyObject* ptObject, uint32_t* puCountOut)
{
    static plVec2* sbtList = NULL;
    pl_sb_reset(sbtList);

    if (PyTuple_Check(ptObject))
    {
        Py_ssize_t pySize = PyTuple_Size(ptObject);
        *puCountOut = (uint32_t)pySize;
        pl_sb_resize(sbtList, *puCountOut);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            sbtList[i] = pl_get_vec2_from_python(PyTuple_GetItem(ptObject, i));
        }
    }

    else if (PyList_Check(ptObject))
    {
        Py_ssize_t pySize = PyList_Size(ptObject);
        *puCountOut = (uint32_t)pySize;
        pl_sb_resize(sbtList, *puCountOut);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            sbtList[i] = pl_get_vec2_from_python(PyList_GetItem(ptObject, i));
        }
    }

    return sbtList;
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

plVec3d
pl_get_dvec3_from_python(PyObject* ptValue)
{
    plVec3d tResult = {0};

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

static int
pl_io_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plPyIO* ptPyIO = (plPyIO*)self;
    ptPyIO->ptIO = gptIOI->get_io();
    return 0;
}

static PyObject* pl_io_get_bRunning(plPyIO* self, void *closure){ return PyBool_FromLong(self->ptIO->bRunning);}

static PyObject* pl_io_get_dTime(plPyIO* self, void *closure){ return PyFloat_FromDouble(self->ptIO->dTime);}
static PyObject* pl_io_get_fFrameRate(plPyIO* self, void *closure){ return PyFloat_FromDouble((double)self->ptIO->fFrameRate);}
static PyObject* pl_io_get_fMouseDragThreshold(plPyIO* self, void* closure){return PyFloat_FromDouble((double)self->ptIO->fMouseDragThreshold);}
static PyObject* pl_io_get_fMouseDoubleClickTime(plPyIO* self, void* closure){return PyFloat_FromDouble((double)self->ptIO->fMouseDoubleClickTime);}
static PyObject* pl_io_get_fMouseDoubleClickMaxDist(plPyIO* self, void* closure){return PyFloat_FromDouble((double)self->ptIO->fMouseDoubleClickMaxDist);}
static PyObject* pl_io_get_fKeyRepeatDelay(plPyIO* self, void* closure){return PyFloat_FromDouble((double)self->ptIO->fKeyRepeatDelay);}
static PyObject* pl_io_get_fKeyRepeatRate(plPyIO* self, void* closure){return PyFloat_FromDouble((double)self->ptIO->fKeyRepeatRate);}

static int pl_io_set_fMouseDragThreshold(plPyIO* self, PyObject *value, void *closure){ self->ptIO->fMouseDragThreshold = (float)PyFloat_AsDouble(value); return 0;}
static int pl_io_set_fMouseDoubleClickTime(plPyIO* self, PyObject* value, void* closure){ self->ptIO->fMouseDoubleClickTime = (float)PyFloat_AsDouble(value); return 0;}
static int pl_io_set_fMouseDoubleClickMaxDist(plPyIO* self, PyObject* value, void* closure){ self->ptIO->fMouseDoubleClickMaxDist = (float)PyFloat_AsDouble(value); return 0;}
static int pl_io_set_fKeyRepeatDelay(plPyIO* self, PyObject* value, void* closure){ self->ptIO->fKeyRepeatDelay = (float)PyFloat_AsDouble(value); return 0;}
static int pl_io_set_fKeyRepeatRate(plPyIO* self, PyObject* value, void* closure){ self->ptIO->fKeyRepeatRate = (float)PyFloat_AsDouble(value); return 0;}

static int
pl_io_set_bRunning(plPyIO* self, PyObject *value, void *closure)
{

    self->ptIO->bRunning = PyLong_AsLong(value);
    return 0;
}

static PyObject*
pl_io_get_tMainViewportSize(plPyIO* self, void *closure)
{
    return pl_vec2_to_py(self->ptIO->tMainViewportSize);
}

static PyObject*
pl_io_get_tMainFramebufferScale(plPyIO* self, void *closure)
{
    return pl_vec2_to_py(self->ptIO->tMainFramebufferScale);
}

#define PL_PYTHON_PROPERTY(ARG) {#ARG, (getter)pl_io_get_##ARG, (setter)pl_io_set_##ARG, "Property: " # ARG}
#define PL_PYTHON_PROPERTY_GETTER_ONLY(ARG) {#ARG, (getter)pl_io_get_##ARG, (setter)NULL, "Property: " # ARG}

static PyGetSetDef gatIOProps[] =
{
    PL_PYTHON_PROPERTY(bRunning),
    PL_PYTHON_PROPERTY(fMouseDragThreshold),
    PL_PYTHON_PROPERTY(fMouseDoubleClickTime),
    PL_PYTHON_PROPERTY(fMouseDoubleClickMaxDist),
    PL_PYTHON_PROPERTY(fKeyRepeatDelay),
    PL_PYTHON_PROPERTY(fKeyRepeatRate),
    PL_PYTHON_PROPERTY_GETTER_ONLY(fFrameRate),
    PL_PYTHON_PROPERTY_GETTER_ONLY(dTime),
    PL_PYTHON_PROPERTY_GETTER_ONLY(fFrameRate),
    PL_PYTHON_PROPERTY_GETTER_ONLY(tMainViewportSize),
    PL_PYTHON_PROPERTY_GETTER_ONLY(tMainFramebufferScale),
    {NULL, NULL, 0, NULL}
};

static PyType_Slot pl_io_slots[] = {
    {Py_tp_init, (void*)pl_io_init},
    {Py_tp_getset, (void*)gatIOProps},
    // {Py_tp_methods, (void*)gatIOCommands},
    {0, 0}
};

static PyType_Spec pl_io_spec = {
    "pilotlight.IO",
    sizeof(plPyIO),
    0,
    Py_TPFLAGS_DEFAULT,
    pl_io_slots
};

#define PL_PYTHON_COMMAND(ARG, FLAGS, DOCS) {"pl_" # ARG, (PyCFunction)ARG, FLAGS, DOCS}

static PyMethodDef gatCommands[] =
{

    {"pl_run", (PyCFunction)pl_python_run, METH_O, NULL},

    // core API
    PL_PYTHON_COMMAND(set_pointer_value, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(get_pointer_value, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(create_bool_pointer, METH_NOARGS, NULL),
    PL_PYTHON_COMMAND(create_int_pointer, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(create_float_pointer, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(create_double_pointer, METH_VARARGS, NULL),
    PL_PYTHON_COMMAND(destroy_pointer, METH_O, NULL),

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
        
    const plApiRegistryI* ptApiRegistry = pl__python_load();

    PyObject* ptApiRegistryCapsule = PyCapsule_New((void*)ptApiRegistry, "plApiRegistryI", NULL);

    if (PyModule_AddObject(ptModule, "plApiRegistryICapsule", ptApiRegistryCapsule) < 0)
    {
        return NULL;
    }

    gptVec2Type = PyType_FromSpec(&pl_vec2_spec);
    gptIOType = PyType_FromSpec(&pl_io_spec);
    gptplUiI = PyType_FromSpec(&plUiISpec);
    gptplVfsI = PyType_FromSpec(&plVfsISpec);
    gptplPakI = PyType_FromSpec(&plPakISpec);
    gptplStatsI = PyType_FromSpec(&plStatsISpec);
    gptplDrawI = PyType_FromSpec(&plDrawISpec);
    gptplGraphicsI = PyType_FromSpec(&plGraphicsISpec);
    gptplStarterI = PyType_FromSpec(&plStarterISpec);
    gptplIOI = PyType_FromSpec(&plIOISpec);
    gptplShaderI = PyType_FromSpec(&plShaderISpec);
    gptplShaderVariantI = PyType_FromSpec(&plShaderVariantISpec);
    gptplWindowI = PyType_FromSpec(&plWindowISpec);
    gptplScreenLogI = PyType_FromSpec(&plScreenLogISpec);
    gptplEcsI = PyType_FromSpec(&plEcsISpec);
    gptplAnimationI = PyType_FromSpec(&plAnimationISpec);
    gptplMaterialI = PyType_FromSpec(&plMaterialISpec);
    gptplMeshI = PyType_FromSpec(&plMeshISpec);
    gptplPhysicsI = PyType_FromSpec(&plPhysicsISpec);
    gptplScriptI = PyType_FromSpec(&plScriptISpec);
    gptplCameraI = PyType_FromSpec(&plCameraISpec);
    gptplRendererI = PyType_FromSpec(&plRendererISpec);
    gptplRendererEcsI = PyType_FromSpec(&plRendererEcsISpec);

    PyModule_AddObject(ptModule, "plUiI", gptplUiI);
    PyModule_AddObject(ptModule, "plVec2", gptVec2Type);
    PyModule_AddObject(ptModule, "plIO", gptIOType);
    PyModule_AddObject(ptModule, "plVfsI", gptplVfsI);
    PyModule_AddObject(ptModule, "plPakI", gptplPakI);
    PyModule_AddObject(ptModule, "plStatsI", gptplStatsI);
    PyModule_AddObject(ptModule, "plDrawI", gptplDrawI);
    PyModule_AddObject(ptModule, "plGraphicsI", gptplGraphicsI);
    PyModule_AddObject(ptModule, "plStarterI", gptplStarterI);
    PyModule_AddObject(ptModule, "plIOI", gptplIOI);
    PyModule_AddObject(ptModule, "plShaderI", gptplShaderI);
    PyModule_AddObject(ptModule, "plShaderVariantI", gptplShaderVariantI);
    PyModule_AddObject(ptModule, "plWindowI", gptplWindowI);
    PyModule_AddObject(ptModule, "plScreenLogI", gptplScreenLogI);
    PyModule_AddObject(ptModule, "plEcsI", gptplEcsI);
    PyModule_AddObject(ptModule, "plAnimationI", gptplAnimationI);
    PyModule_AddObject(ptModule, "plMaterialI", gptplMaterialI);
    PyModule_AddObject(ptModule, "plMeshI", gptplMeshI);
    PyModule_AddObject(ptModule, "plPhysicsI", gptplPhysicsI);
    PyModule_AddObject(ptModule, "plScriptI", gptplScriptI);
    PyModule_AddObject(ptModule, "plCameraI", gptplCameraI);
    PyModule_AddObject(ptModule, "plRendererI", gptplRendererI);
    PyModule_AddObject(ptModule, "plRendererEcsI", gptplRendererEcsI);

    return ptModule;
}