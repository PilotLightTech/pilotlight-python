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

static PyObject* pl_io_get_fDeltaTime(plPyIO* self, void *closure){ return PyFloat_FromDouble(self->ptIO->fDeltaTime);}
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
    PL_PYTHON_PROPERTY_GETTER_ONLY(fDeltaTime),
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

static PyObject* pl_camera_tProjMat(plPyCamera* self, void* closure){ return pl_mat4_new(self->tCamera.tProjMat);}
static PyObject* pl_camera_tViewMat(plPyCamera* self, void* closure){ return pl_mat4_new(self->tCamera.tViewMat);}
static PyObject* pl_camera_tViewProjMat(plPyCamera* self, void* closure){ return pl_mat4_new(self->tCamera.tViewProjMat);}
static PyObject* pl_camera_tInvViewMat(plPyCamera* self, void* closure){ return pl_mat4_new(self->tCamera.tInvViewMat);}
static PyObject* pl_camera_tInvProjMat(plPyCamera* self, void* closure){ return pl_mat4_new(self->tCamera.tInvProjMat);}
static PyObject* pl_camera_tInvViewProjMat(plPyCamera* self, void* closure){ return pl_mat4_new(self->tCamera.tInvViewProjMat);}
static PyObject* pl_camera_tViewMatNoTranslation(plPyCamera* self, void* closure){ return pl_mat4_new(self->tCamera.tViewMatNoTranslation);}
static PyObject* pl_camera_tInvViewMatNoTranslation(plPyCamera* self, void* closure){ return pl_mat4_new(self->tCamera.tInvViewMatNoTranslation);}

static PyObject* pl_camera_tPosition(plPyCamera* self, void* closure){ return pl_vec3_new(self->tCamera.tPositionF);}
static PyObject* pl_camera_tRotation(plPyCamera* self, void* closure){ return pl_vec4_new(self->tCamera.tRotation);}
static PyObject* pl_camera_tUpVec(plPyCamera* self, void* closure){ return pl_vec3_new(self->tCamera.tUpVec);}
static PyObject* pl_camera_tForwardVec(plPyCamera* self, void* closure){ return pl_vec3_new(self->tCamera.tForwardVec);}
static PyObject* pl_camera_tRightVec(plPyCamera* self, void* closure){ return pl_vec3_new(self->tCamera.tRightVec);}
static PyObject* pl_camera_fPitch(plPyCamera* self, void* closure){ return PyFloat_FromDouble((double)self->tCamera.fPitch);}
static PyObject* pl_camera_fYaw(plPyCamera* self, void* closure){ return PyFloat_FromDouble((double)self->tCamera.fYaw);}
static PyObject* pl_camera_fRoll(plPyCamera* self, void* closure){ return PyFloat_FromDouble((double)self->tCamera.fRoll);}
static PyObject* pl_camera_fNearZ(plPyCamera* self, void* closure){ return PyFloat_FromDouble((double)self->tCamera.fNearZ);}
static PyObject* pl_camera_fFarZ(plPyCamera* self, void* closure){ return PyFloat_FromDouble((double)self->tCamera.fFarZ);}
static PyObject* pl_camera_fYFov(plPyCamera* self, void* closure){ return PyFloat_FromDouble((double)self->tCamera.fYFov);}
static PyObject* pl_camera_fAspectRatio(plPyCamera* self, void* closure){ return PyFloat_FromDouble((double)self->tCamera.fAspectRatio);}
static PyObject* pl_camera_fWidth(plPyCamera* self, void* closure){ return PyFloat_FromDouble((double)self->tCamera.fWidth);}
static PyObject* pl_camera_fHeight(plPyCamera* self, void* closure){ return PyFloat_FromDouble((double)self->tCamera.fHeight);}
static PyObject* pl_camera_eProjectionType(plPyCamera* self, void* closure){ return PyLong_FromLong(self->tCamera.eProjectionType);}
static PyObject* pl_camera_eDepthMode(plPyCamera* self, void* closure){ return PyLong_FromLong(self->tCamera.eDepthMode);}

static PyGetSetDef gatCameraProps[] =
{
    {"tProjMat", (getter)pl_camera_tProjMat, (setter)NULL, "Property: tProjMat"},
    {"tViewMat", (getter)pl_camera_tViewMat, (setter)NULL, "Property: tViewMat"},
    {"tViewProjMat", (getter)pl_camera_tViewProjMat, (setter)NULL, "Property: tViewProjMat"},
    {"tInvViewMat", (getter)pl_camera_tInvViewMat, (setter)NULL, "Property: tInvViewMat"},
    {"tInvProjMat", (getter)pl_camera_tInvProjMat, (setter)NULL, "Property: tInvProjMat"},
    {"tInvViewProjMat", (getter)pl_camera_tInvViewProjMat, (setter)NULL, "Property: tInvViewProjMat"},
    {"tViewMatNoTranslation", (getter)pl_camera_tViewMatNoTranslation, (setter)NULL, "Property: tViewMatNoTranslation"},
    {"tInvViewMatNoTranslation", (getter)pl_camera_tInvViewMatNoTranslation, (setter)NULL, "Property: tInvViewMatNoTranslation"},
    {"tPosition", (getter)pl_camera_tPosition, (setter)NULL, "Property: tPosition"},
    {"tRotation", (getter)pl_camera_tRotation, (setter)NULL, "Property: tRotation"},
    {"tUpVec", (getter)pl_camera_tUpVec, (setter)NULL, "Property: tUpVec"},
    {"tForwardVec", (getter)pl_camera_tForwardVec, (setter)NULL, "Property: tForwardVec"},
    {"tRightVec", (getter)pl_camera_tRightVec, (setter)NULL, "Property: tRightVec"},
    {"fPitch", (getter)pl_camera_fPitch, (setter)NULL, "Property: fPitch"},
    {"fYaw", (getter)pl_camera_fYaw, (setter)NULL, "Property: fYaw"},
    {"fRoll", (getter)pl_camera_fRoll, (setter)NULL, "Property: fRoll"},
    {"fNearZ", (getter)pl_camera_fNearZ, (setter)NULL, "Property: fNearZ"},
    {"fFarZ", (getter)pl_camera_fFarZ, (setter)NULL, "Property: fFarZ"},
    {"fYFov", (getter)pl_camera_fYFov, (setter)NULL, "Property: fYFov"},
    {"fAspectRatio", (getter)pl_camera_fAspectRatio, (setter)NULL, "Property: fAspectRatio"},
    {"fWidth", (getter)pl_camera_fWidth, (setter)NULL, "Property: fWidth"},
    {"fHeight", (getter)pl_camera_fHeight, (setter)NULL, "Property: fHeight"},
    {"eProjectionType", (getter)pl_camera_eProjectionType, (setter)NULL, "Property: eProjectionType"},
    {"eDepthMode", (getter)pl_camera_eDepthMode, (setter)NULL, "Property: eDepthMode"},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot pl_camera_slots[] = {
    // {Py_tp_init, (void*)pl_io_init},
    {Py_tp_getset, (void*)gatCameraProps},
    // {Py_tp_methods, (void*)gatIOCommands},
    {0, 0}
};

static PyType_Spec pl_camera_spec = {
    "pilotlight.plCamera",
    sizeof(plPyCamera),
    0,
    Py_TPFLAGS_DEFAULT,
    pl_camera_slots
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

    for(uint32_t i = 0; i < PL_ARRAYSIZE(gatCameraIntPairs); i++)
        PyModule_AddIntConstant(ptModule, gatCameraIntPairs[i].pcName, gatCameraIntPairs[i].iValue);
        
    const plApiRegistryI* ptApiRegistry = pl__python_load();

    PyObject* ptApiRegistryCapsule = PyCapsule_New((void*)ptApiRegistry, "plApiRegistryI", NULL);

    if (PyModule_AddObject(ptModule, "plApiRegistryICapsule", ptApiRegistryCapsule) < 0)
    {
        return NULL;
    }

    gptVec2Type = PyType_FromSpec(&pl_vec2_spec);
    gptVec3Type = PyType_FromSpec(&pl_vec3_spec);
    gptVec4Type = PyType_FromSpec(&pl_vec4_spec);
    gptMat4Type = PyType_FromSpec(&pl_mat4_spec);
    gptIOType = PyType_FromSpec(&pl_io_spec);
    gptSwapchainInfoType = PyType_FromSpec(&pl_swapchain_info_spec);
    gptCameraType = PyType_FromSpec(&pl_camera_spec);
    
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

    return ptModule;
}