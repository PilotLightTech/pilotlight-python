/*
   pl_py_types.c
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] plVec2
// [SECTION] plVec3
// [SECTION] plVec4
// [SECTION] plMat4
// [SECTION] plCamera
// [SECTION] plIO
// [SECTION] plSwapchainInfo
// [SECTION] type conversion to python
// [SECTION] type conversion to c
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"
#include "pl_py_types.h"
#include "structmember.h"
#define PL_MATH_INCLUDE_FUNCTIONS
#include "pl_math.h"

//-----------------------------------------------------------------------------
// [SECTION] plVec2
//-----------------------------------------------------------------------------

static int
pl_vec2_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
    pyplVec2* vec = (pyplVec2*)self;

    static const char* kwlist[] = {"x", "y", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|ff", (char**)kwlist, &vec->v.x, &vec->v.y))
        return -1;

    return 0;
}

static PyObject*
pl_vec2_repr(PyObject* self)
{
    pyplVec2* v = (pyplVec2*)self;
    return PyUnicode_FromFormat("Vec2(%f, %f)", v->v.x, v->v.y);
}

static bool
pl_vec2_check(PyObject* obj)
{
    return PyObject_TypeCheck(obj, (PyTypeObject*)gptVec2Type);
}

static PyObject*
pl_vec2_new(plVec2 tV)
{
    PyObject* args = Py_BuildValue("(ff)", tV.x, tV.y);
    if(!args)
        return NULL;

    PyObject* obj = PyObject_CallObject(gptVec2Type, args);
    Py_DECREF(args);
    return obj;
}

static PyObject*
pl_vec2_add(PyObject* a, PyObject* b)
{
    if(!pl_vec2_check(a) || !pl_vec2_check(b))
        Py_RETURN_NOTIMPLEMENTED;

    pyplVec2* va = (pyplVec2*)a;
    pyplVec2* vb = (pyplVec2*)b;

    return pl_vec2_new(pl_add_vec2(va->v, vb->v));
}

static PyObject*
pl_vec2_subtract(PyObject* a, PyObject* b)
{
    if(!pl_vec2_check(a) || !pl_vec2_check(b))
        Py_RETURN_NOTIMPLEMENTED;

    pyplVec2* va = (pyplVec2*)a;
    pyplVec2* vb = (pyplVec2*)b;

    return pl_vec2_new(pl_sub_vec2(va->v, vb->v));
}

static PyObject*
pl_vec2_negative(PyObject* self)
{
    pyplVec2* v = (pyplVec2*)self;
    return pl_vec2_new((plVec2){-v->v.x, -v->v.y});
}

static PyMemberDef pl_vec2_members[] = {
    {"x", T_FLOAT, offsetof(pyplVec2, v.x), 0, ""},
    {"y", T_FLOAT, offsetof(pyplVec2, v.y), 0, ""},
    {NULL}
};

static PyType_Slot pl_vec2_slots[] = {
    {Py_tp_init,     (void*)pl_vec2_init},
    {Py_tp_members,  (void*)pl_vec2_members},
    {Py_tp_repr,     (void*)pl_vec2_repr},
    {Py_nb_add,      (void*)pl_vec2_add},
    {Py_nb_subtract, (void*)pl_vec2_subtract},
    {Py_nb_negative, (void*)pl_vec2_negative},
    // {Py_nb_multiply, (void*)pl_vec2_multiply},
    {0, 0}
};

static PyType_Spec pl_vec2_spec = {
    "pilotlight.plVec2",
    sizeof(pyplVec2),
    0,
    Py_TPFLAGS_DEFAULT,
    pl_vec2_slots
};

//-----------------------------------------------------------------------------
// [SECTION] plVec3
//-----------------------------------------------------------------------------

static int
pl_vec3_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
    pyplVec3* vec = (pyplVec3*)self;

    static const char* kwlist[] = {"x", "y", "z", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|fff", (char**)kwlist, &vec->v.x, &vec->v.y, &vec->v.z))
        return -1;

    return 0;
}

static PyObject*
pl_vec3_repr(PyObject* self)
{
    pyplVec3* v = (pyplVec3*)self;
    return PyUnicode_FromFormat("plVec3(%f, %f, %f)", v->v.x, v->v.y, v->v.z);
}

static bool
pl_vec3_check(PyObject* obj)
{
    return PyObject_TypeCheck(obj, (PyTypeObject*)gptVec3Type);
}

static PyObject*
pl_vec3_new(plVec3 tV)
{
    PyObject* args = Py_BuildValue("(fff)", tV.x, tV.y, tV.z);
    if(!args)
        return NULL;

    PyObject* obj = PyObject_CallObject(gptVec3Type, args);
    Py_DECREF(args);
    return obj;
}

static PyObject*
pl_vec3_add(PyObject* a, PyObject* b)
{
    if(!pl_vec3_check(a) || !pl_vec3_check(b))
        Py_RETURN_NOTIMPLEMENTED;

    pyplVec3* va = (pyplVec3*)a;
    pyplVec3* vb = (pyplVec3*)b;

    return pl_vec3_new(pl_add_vec3(va->v, vb->v));
}

static PyObject*
pl_vec3_subtract(PyObject* a, PyObject* b)
{
    if(!pl_vec3_check(a) || !pl_vec3_check(b))
        Py_RETURN_NOTIMPLEMENTED;

    pyplVec3* va = (pyplVec3*)a;
    pyplVec3* vb = (pyplVec3*)b;

    return pl_vec3_new(pl_sub_vec3(va->v, vb->v));
}

static PyObject*
pl_vec3_negative(PyObject* self)
{
    pyplVec3* v = (pyplVec3*)self;
    return pl_vec3_new((plVec3){-v->v.x, -v->v.y, -v->v.z});
}

static PyMemberDef pl_vec3_members[] = {
    {"x", T_FLOAT, offsetof(pyplVec3, v.x), 0, ""},
    {"y", T_FLOAT, offsetof(pyplVec3, v.y), 0, ""},
    {"z", T_FLOAT, offsetof(pyplVec3, v.z), 0, ""},
    {NULL}
};

static PyType_Slot pl_vec3_slots[] = {
    {Py_tp_init,     (void*)pl_vec3_init},
    {Py_tp_members,  (void*)pl_vec3_members},
    {Py_tp_repr,     (void*)pl_vec3_repr},
    {Py_nb_add,      (void*)pl_vec3_add},
    {Py_nb_subtract, (void*)pl_vec3_subtract},
    {Py_nb_negative, (void*)pl_vec3_negative},
    // {Py_nb_multiply, (void*)pl_vec2_multiply},
    {0, 0}
};

static PyType_Spec pl_vec3_spec = {
    "pilotlight.plVec3",
    sizeof(pyplVec3),
    0,
    Py_TPFLAGS_DEFAULT,
    pl_vec3_slots
};

//-----------------------------------------------------------------------------
// [SECTION] plVec4
//-----------------------------------------------------------------------------

static int
pl_vec4_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
    pyplVec4* vec = (pyplVec4*)self;

    static const char* kwlist[] = {"x", "y", "z", "w", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|ffff", (char**)kwlist, &vec->v.x, &vec->v.y, &vec->v.z, &vec->v.w))
        return -1;

    return 0;
}

static PyObject*
pl_vec4_repr(PyObject* self)
{
    pyplVec4* v = (pyplVec4*)self;
    return PyUnicode_FromFormat("plVec3(%f, %f, %f, %s)", v->v.x, v->v.y, v->v.z, v->v.w);
}

static bool
pl_vec4_check(PyObject* obj)
{
    return PyObject_TypeCheck(obj, (PyTypeObject*)gptVec4Type);
}

static PyObject*
pl_vec4_new(plVec4 tV)
{
    PyObject* args = Py_BuildValue("(ffff)", tV.x, tV.y, tV.z, tV.w);
    if(!args)
        return NULL;

    PyObject* obj = PyObject_CallObject(gptVec4Type, args);
    Py_DECREF(args);
    return obj;
}

static PyObject*
pl_vec4_add(PyObject* a, PyObject* b)
{
    if(!pl_vec4_check(a) || !pl_vec4_check(b))
        Py_RETURN_NOTIMPLEMENTED;

    pyplVec4* va = (pyplVec4*)a;
    pyplVec4* vb = (pyplVec4*)b;

    return pl_vec4_new(pl_add_vec4(va->v, vb->v));
}

static PyObject*
pl_vec4_subtract(PyObject* a, PyObject* b)
{
    if(!pl_vec4_check(a) || !pl_vec4_check(b))
        Py_RETURN_NOTIMPLEMENTED;

    pyplVec4* va = (pyplVec4*)a;
    pyplVec4* vb = (pyplVec4*)b;

    return pl_vec4_new(pl_sub_vec4(va->v, vb->v));
}

static PyObject*
pl_vec4_negative(PyObject* self)
{
    pyplVec4* v = (pyplVec4*)self;
    return pl_vec4_new((plVec4){-v->v.x, -v->v.y, -v->v.z, -v->v.w});
}

static PyMemberDef pl_vec4_members[] = {
    {"x", T_FLOAT, offsetof(pyplVec4, v.x), 0, ""},
    {"y", T_FLOAT, offsetof(pyplVec4, v.y), 0, ""},
    {"z", T_FLOAT, offsetof(pyplVec4, v.z), 0, ""},
    {"w", T_FLOAT, offsetof(pyplVec4, v.w), 0, ""},
    {NULL}
};

static PyType_Slot pl_vec4_slots[] = {
    {Py_tp_init,     (void*)pl_vec4_init},
    {Py_tp_members,  (void*)pl_vec4_members},
    {Py_tp_repr,     (void*)pl_vec4_repr},
    {Py_nb_add,      (void*)pl_vec4_add},
    {Py_nb_subtract, (void*)pl_vec4_subtract},
    {Py_nb_negative, (void*)pl_vec4_negative},
    // {Py_nb_multiply, (void*)pl_vec2_multiply},
    {0, 0}
};

static PyType_Spec pl_vec4_spec = {
    "pilotlight.plVec4",
    sizeof(pyplVec4),
    0,
    Py_TPFLAGS_DEFAULT,
    pl_vec4_slots
};

//-----------------------------------------------------------------------------
// [SECTION] plMat4
//-----------------------------------------------------------------------------

static bool
pl_mat4_check(PyObject* obj)
{
    return PyObject_TypeCheck(obj, (PyTypeObject*)gptMat4Type);
}

static PyObject*
pl_mat4_repr(PyObject* self)
{
    pyplMat4* v = (pyplMat4*)self;
    return PyUnicode_FromFormat("plMat4(%f, %f, %f, %f)", v->m.x11, v->m.x22, v->m.x33, v->m.x44);
}

static PyObject*
pl_mat4_new(plMat4 m)
{
    PyObject* args = Py_BuildValue("(ffffffffffffffff)",
        m.x11, m.x21, m.x31, m.x41,
        m.x12, m.x22, m.x32, m.x42,
        m.x13, m.x23, m.x33, m.x43,
        m.x14, m.x24, m.x34, m.x44);
    if(!args)
        return NULL;

    PyObject* obj = PyObject_CallObject(gptMat4Type, args);
    Py_DECREF(args);
    return obj;
}

static int
pl_mat4_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
    pyplMat4* mat = (pyplMat4*)self;

    mat->m.x11 = 1.0f;
    mat->m.x22 = 1.0f;
    mat->m.x33 = 1.0f;
    mat->m.x44 = 1.0f;

    static const char* kwlist[] = {
        "x11", "x21", "x31", "x41",
        "x12", "x22", "x32", "x42",
        "x13", "x23", "x33", "x43",
        "x14", "x24", "x34", "x44",
        NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|ffffffffffffffff", (char**)kwlist,
        &mat->m.x11, &mat->m.x21, &mat->m.x31, &mat->m.x41,
        &mat->m.x12, &mat->m.x22, &mat->m.x32, &mat->m.x42,
        &mat->m.x13, &mat->m.x23, &mat->m.x33, &mat->m.x43,
        &mat->m.x14, &mat->m.x24, &mat->m.x34, &mat->m.x44))
        return -1;
    return 0;
}

static PyObject*
pl_mat4_multiply(PyObject* a, PyObject* b)
{
    pyplMat4* ptLeft = NULL;
    pyplMat4* ptRight = NULL;

    if(pl_mat4_check(a))
    {
        ptLeft = (pyplMat4*)a;
        ptRight = (pyplMat4*)b;
    }
    else if(pl_mat4_check(b))
    {
        ptRight = (pyplMat4*)b;
        ptLeft = (pyplMat4*)a;
    }
    else
    {
        Py_RETURN_NOTIMPLEMENTED;
    }

    
    if(PyErr_Occurred())
        return NULL;

    plMat4 tResult = pl_mul_mat4(&ptLeft->m, &ptRight->m);

    return pl_mat4_new(tResult);
}

static PyMemberDef pl_mat4_members[] = {
    {"x11", T_FLOAT, offsetof(pyplMat4, m.x11), 0, ""},
    {"x21", T_FLOAT, offsetof(pyplMat4, m.x21), 0, ""},
    {"x31", T_FLOAT, offsetof(pyplMat4, m.x31), 0, ""},
    {"x41", T_FLOAT, offsetof(pyplMat4, m.x41), 0, ""},

    {"x12", T_FLOAT, offsetof(pyplMat4, m.x12), 0, ""},
    {"x22", T_FLOAT, offsetof(pyplMat4, m.x22), 0, ""},
    {"x32", T_FLOAT, offsetof(pyplMat4, m.x32), 0, ""},
    {"x42", T_FLOAT, offsetof(pyplMat4, m.x42), 0, ""},

    {"x13", T_FLOAT, offsetof(pyplMat4, m.x13), 0, ""},
    {"x23", T_FLOAT, offsetof(pyplMat4, m.x23), 0, ""},
    {"x33", T_FLOAT, offsetof(pyplMat4, m.x33), 0, ""},
    {"x43", T_FLOAT, offsetof(pyplMat4, m.x43), 0, ""},

    {"x14", T_FLOAT, offsetof(pyplMat4, m.x14), 0, ""},
    {"x24", T_FLOAT, offsetof(pyplMat4, m.x24), 0, ""},
    {"x34", T_FLOAT, offsetof(pyplMat4, m.x34), 0, ""},
    {"x44", T_FLOAT, offsetof(pyplMat4, m.x44), 0, ""},
    {NULL}
};

static PyType_Slot pl_mat4_slots[] = {
    {Py_tp_init,     (void*)pl_mat4_init},
    {Py_tp_members,  (void*)pl_mat4_members},
    {Py_tp_repr,     (void*)pl_mat4_repr},
    {Py_nb_multiply, (void*)pl_mat4_multiply},
    // {Py_nb_negative, (void*)pl_vec2_negative},
    // {Py_nb_add, (void*)pl_vec2_add},
    // {Py_nb_subtract, (void*)pl_vec2_subtract},
    {0, 0}
};

static PyType_Spec pl_mat4_spec = {
    "pilotlight.plMat4",
    sizeof(pyplMat4),
    0,
    Py_TPFLAGS_DEFAULT,
    pl_mat4_slots
};

//-----------------------------------------------------------------------------
// [SECTION] plCamera
//-----------------------------------------------------------------------------

static int
pl_type_camera_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
    pyplCamera* ptPyCamera = (pyplCamera*)self;
    ptPyCamera->ptCamera = malloc(sizeof(plCamera));
    memset(ptPyCamera->ptCamera, 0, sizeof(plCameraI));
    return 0;
}

static PyObject* pl_camera_tProjMat(pyplCamera* self, void* closure){ return pl_mat4_new(self->ptCamera->tProjMat);}
static PyObject* pl_camera_tViewMat(pyplCamera* self, void* closure){ return pl_mat4_new(self->ptCamera->tViewMat);}
static PyObject* pl_camera_tViewProjMat(pyplCamera* self, void* closure){ return pl_mat4_new(self->ptCamera->tViewProjMat);}
static PyObject* pl_camera_tInvViewMat(pyplCamera* self, void* closure){ return pl_mat4_new(self->ptCamera->tInvViewMat);}
static PyObject* pl_camera_tInvProjMat(pyplCamera* self, void* closure){ return pl_mat4_new(self->ptCamera->tInvProjMat);}
static PyObject* pl_camera_tInvViewProjMat(pyplCamera* self, void* closure){ return pl_mat4_new(self->ptCamera->tInvViewProjMat);}
static PyObject* pl_camera_tViewMatNoTranslation(pyplCamera* self, void* closure){ return pl_mat4_new(self->ptCamera->tViewMatNoTranslation);}
static PyObject* pl_camera_tInvViewMatNoTranslation(pyplCamera* self, void* closure){ return pl_mat4_new(self->ptCamera->tInvViewMatNoTranslation);}

static PyObject* pl_camera_tPosition(pyplCamera* self, void* closure){ return pl_vec3_new(self->ptCamera->tPositionF);}
static PyObject* pl_camera_tRotation(pyplCamera* self, void* closure){ return pl_vec4_new(self->ptCamera->tRotation);}
static PyObject* pl_camera_tUpVec(pyplCamera* self, void* closure){ return pl_vec3_new(self->ptCamera->tUpVec);}
static PyObject* pl_camera_tForwardVec(pyplCamera* self, void* closure){ return pl_vec3_new(self->ptCamera->tForwardVec);}
static PyObject* pl_camera_tRightVec(pyplCamera* self, void* closure){ return pl_vec3_new(self->ptCamera->tRightVec);}
static PyObject* pl_camera_fPitch(pyplCamera* self, void* closure){ return PyFloat_FromDouble((double)self->ptCamera->fPitch);}
static PyObject* pl_camera_fYaw(pyplCamera* self, void* closure){ return PyFloat_FromDouble((double)self->ptCamera->fYaw);}
static PyObject* pl_camera_fRoll(pyplCamera* self, void* closure){ return PyFloat_FromDouble((double)self->ptCamera->fRoll);}
static PyObject* pl_camera_fNearZ(pyplCamera* self, void* closure){ return PyFloat_FromDouble((double)self->ptCamera->fNearZ);}
static PyObject* pl_camera_fFarZ(pyplCamera* self, void* closure){ return PyFloat_FromDouble((double)self->ptCamera->fFarZ);}
static PyObject* pl_camera_fYFov(pyplCamera* self, void* closure){ return PyFloat_FromDouble((double)self->ptCamera->fYFov);}
static PyObject* pl_camera_fAspectRatio(pyplCamera* self, void* closure){ return PyFloat_FromDouble((double)self->ptCamera->fAspectRatio);}
static PyObject* pl_camera_fWidth(pyplCamera* self, void* closure){ return PyFloat_FromDouble((double)self->ptCamera->fWidth);}
static PyObject* pl_camera_fHeight(pyplCamera* self, void* closure){ return PyFloat_FromDouble((double)self->ptCamera->fHeight);}
static PyObject* pl_camera_eProjectionType(pyplCamera* self, void* closure){ return PyLong_FromLong(self->ptCamera->eProjectionType);}
static PyObject* pl_camera_eDepthMode(pyplCamera* self, void* closure){ return PyLong_FromLong(self->ptCamera->eDepthMode);}

static int
pl_camera_set_tPosition(pyplCamera* self, PyObject *value, void *closure)
{
    plVec3 tPosition = {0};
    pl_vec3_from_py(value, &tPosition);
    gptCamera->set_position(self->ptCamera, (plVec3d){(double)tPosition.x, (double)tPosition.y, (double)tPosition.z});
    return 0;
}

static PyGetSetDef gatCameraProps[] =
{
    {"tPosition",                (getter)pl_camera_tPosition,                (setter)pl_camera_set_tPosition, "Property: tPosition"},
    {"tProjMat",                 (getter)pl_camera_tProjMat,                 (setter)NULL,                     "Property: tProjMat"},
    {"tViewMat",                 (getter)pl_camera_tViewMat,                 (setter)NULL,                     "Property: tViewMat"},
    {"tViewProjMat",             (getter)pl_camera_tViewProjMat,             (setter)NULL,                     "Property: tViewProjMat"},
    {"tInvViewMat",              (getter)pl_camera_tInvViewMat,              (setter)NULL,                     "Property: tInvViewMat"},
    {"tInvProjMat",              (getter)pl_camera_tInvProjMat,              (setter)NULL,                     "Property: tInvProjMat"},
    {"tInvViewProjMat",          (getter)pl_camera_tInvViewProjMat,          (setter)NULL,                     "Property: tInvViewProjMat"},
    {"tViewMatNoTranslation",    (getter)pl_camera_tViewMatNoTranslation,    (setter)NULL,                     "Property: tViewMatNoTranslation"},
    {"tInvViewMatNoTranslation", (getter)pl_camera_tInvViewMatNoTranslation, (setter)NULL,                     "Property: tInvViewMatNoTranslation"},
    {"tRotation",                (getter)pl_camera_tRotation,                (setter)NULL,                     "Property: tRotation"},
    {"tUpVec",                   (getter)pl_camera_tUpVec,                   (setter)NULL,                     "Property: tUpVec"},
    {"tForwardVec",              (getter)pl_camera_tForwardVec,              (setter)NULL,                     "Property: tForwardVec"},
    {"tRightVec",                (getter)pl_camera_tRightVec,                (setter)NULL,                     "Property: tRightVec"},
    {"fPitch",                   (getter)pl_camera_fPitch,                   (setter)NULL,                     "Property: fPitch"},
    {"fYaw",                     (getter)pl_camera_fYaw,                     (setter)NULL,                     "Property: fYaw"},
    {"fRoll",                    (getter)pl_camera_fRoll,                    (setter)NULL,                     "Property: fRoll"},
    {"fNearZ",                   (getter)pl_camera_fNearZ,                   (setter)NULL,                     "Property: fNearZ"},
    {"fFarZ",                    (getter)pl_camera_fFarZ,                    (setter)NULL,                     "Property: fFarZ"},
    {"fYFov",                    (getter)pl_camera_fYFov,                    (setter)NULL,                     "Property: fYFov"},
    {"fAspectRatio",             (getter)pl_camera_fAspectRatio,             (setter)NULL,                     "Property: fAspectRatio"},
    {"fWidth",                   (getter)pl_camera_fWidth,                   (setter)NULL,                     "Property: fWidth"},
    {"fHeight",                  (getter)pl_camera_fHeight,                  (setter)NULL,                     "Property: fHeight"},
    {"eProjectionType",          (getter)pl_camera_eProjectionType,          (setter)NULL,                     "Property: eProjectionType"},
    {"eDepthMode",               (getter)pl_camera_eDepthMode,               (setter)NULL,                     "Property: eDepthMode"},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot pl_camera_slots[] = {
    {Py_tp_init, (void*)pl_type_camera_init},
    {Py_tp_getset, (void*)gatCameraProps},
    // {Py_tp_methods, (void*)gatIOCommands},
    {0, 0}
};

static PyType_Spec pl_camera_spec = {
    "pilotlight.plCamera",
    sizeof(pyplCamera),
    0,
    Py_TPFLAGS_DEFAULT,
    pl_camera_slots
};

//-----------------------------------------------------------------------------
// [SECTION] plIO
//-----------------------------------------------------------------------------

static int
pl_io_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
    pyplIO* ptPyIO = (pyplIO*)self;
    ptPyIO->ptIO = gptIOI->get_io();
    return 0;
}

static PyObject* pl_io_get_bRunning(pyplIO* self, void *closure){ return PyBool_FromLong(self->ptIO->bRunning);}

static PyObject* pl_io_get_fDeltaTime(pyplIO* self, void *closure){ return PyFloat_FromDouble(self->ptIO->fDeltaTime);}
static PyObject* pl_io_get_dTime(pyplIO* self, void *closure){ return PyFloat_FromDouble(self->ptIO->dTime);}
static PyObject* pl_io_get_fFrameRate(pyplIO* self, void *closure){ return PyFloat_FromDouble((double)self->ptIO->fFrameRate);}
static PyObject* pl_io_get_fMouseDragThreshold(pyplIO* self, void* closure){return PyFloat_FromDouble((double)self->ptIO->fMouseDragThreshold);}
static PyObject* pl_io_get_fMouseDoubleClickTime(pyplIO* self, void* closure){return PyFloat_FromDouble((double)self->ptIO->fMouseDoubleClickTime);}
static PyObject* pl_io_get_fMouseDoubleClickMaxDist(pyplIO* self, void* closure){return PyFloat_FromDouble((double)self->ptIO->fMouseDoubleClickMaxDist);}
static PyObject* pl_io_get_fKeyRepeatDelay(pyplIO* self, void* closure){return PyFloat_FromDouble((double)self->ptIO->fKeyRepeatDelay);}
static PyObject* pl_io_get_fKeyRepeatRate(pyplIO* self, void* closure){return PyFloat_FromDouble((double)self->ptIO->fKeyRepeatRate);}

static int pl_io_set_fMouseDragThreshold(pyplIO* self, PyObject *value, void *closure){ self->ptIO->fMouseDragThreshold = (float)PyFloat_AsDouble(value); return 0;}
static int pl_io_set_fMouseDoubleClickTime(pyplIO* self, PyObject* value, void* closure){ self->ptIO->fMouseDoubleClickTime = (float)PyFloat_AsDouble(value); return 0;}
static int pl_io_set_fMouseDoubleClickMaxDist(pyplIO* self, PyObject* value, void* closure){ self->ptIO->fMouseDoubleClickMaxDist = (float)PyFloat_AsDouble(value); return 0;}
static int pl_io_set_fKeyRepeatDelay(pyplIO* self, PyObject* value, void* closure){ self->ptIO->fKeyRepeatDelay = (float)PyFloat_AsDouble(value); return 0;}
static int pl_io_set_fKeyRepeatRate(pyplIO* self, PyObject* value, void* closure){ self->ptIO->fKeyRepeatRate = (float)PyFloat_AsDouble(value); return 0;}

static int
pl_io_set_bRunning(pyplIO* self, PyObject *value, void *closure)
{

    self->ptIO->bRunning = PyLong_AsLong(value);
    return 0;
}

static PyObject*
pl_io_get_tMainViewportSize(pyplIO* self, void *closure)
{
    return pl_vec2_to_py(self->ptIO->tMainViewportSize);
}

static PyObject*
pl_io_get_tMainFramebufferScale(pyplIO* self, void *closure)
{
    return pl_vec2_to_py(self->ptIO->tMainFramebufferScale);
}

static PyGetSetDef gatIOProps[] =
{
    {"bRunning",                 (getter)pl_io_get_bRunning,                 (setter)pl_io_set_bRunning,                 "Property: bRunning"},
    {"fMouseDragThreshold",      (getter)pl_io_get_fMouseDragThreshold,      (setter)pl_io_set_fMouseDragThreshold,      "Property: fMouseDragThreshold"},
    {"fMouseDoubleClickTime",    (getter)pl_io_get_fMouseDoubleClickTime,    (setter)pl_io_set_fMouseDoubleClickTime,    "Property: fMouseDoubleClickTime"},
    {"fMouseDoubleClickMaxDist", (getter)pl_io_get_fMouseDoubleClickMaxDist, (setter)pl_io_set_fMouseDoubleClickMaxDist, "Property: fMouseDoubleClickMaxDist"},
    {"fKeyRepeatDelay",          (getter)pl_io_get_fKeyRepeatDelay,          (setter)pl_io_set_fKeyRepeatDelay,           "Property: fKeyRepeatDelay"},
    {"fKeyRepeatRate",           (getter)pl_io_get_fKeyRepeatRate,           (setter)pl_io_set_fKeyRepeatRate,            "Property: fKeyRepeatRate"},
    {"fDeltaTime",               (getter)pl_io_get_fDeltaTime,               NULL,                                        "Property: fDeltaTime"},
    {"fFrameRate",               (getter)pl_io_get_fFrameRate,               NULL,                                        "Property: fFrameRate"},
    {"dTime",                    (getter)pl_io_get_dTime,                    NULL,                                        "Property: dTime"},
    {"fFrameRate",               (getter)pl_io_get_fFrameRate,               NULL,                                        "Property: fFrameRate"},
    {"tMainViewportSize",        (getter)pl_io_get_tMainViewportSize,        NULL,                                        "Property: tMainViewportSize"},
    {"tMainFramebufferScale",    (getter)pl_io_get_tMainFramebufferScale,    NULL,                                        "Property: tMainFramebufferScale"},
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
    sizeof(pyplIO),
    0,
    Py_TPFLAGS_DEFAULT,
    pl_io_slots
};

//-----------------------------------------------------------------------------
// [SECTION] plSwapchainInfo
//-----------------------------------------------------------------------------

static PyObject*
pl_swapchain_info_get_tSampleCount(pyplSwapchainInfo* self, void *closure)
{
    return PyLong_FromInt32(self->tInfo.tSampleCount);
}

static int
pl_swapchain_info_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
    pyplSwapchainInfo* vec = (pyplSwapchainInfo*)self;

    static const char* kwlist[] = {"tSampleCount", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|i", (char**)kwlist, &vec->tInfo.tSampleCount))
        return -1;

    return 0;
}

static PyObject*
pl_swapchain_info_new(plSwapchainInfo tInfo)
{
    PyObject* args = Py_BuildValue("(i)", tInfo.tSampleCount);
    if(!args)
        return NULL;

    PyObject* obj = PyObject_CallObject(gptSwapchainInfoType, args);
    Py_DECREF(args);
    return obj;
}

static PyGetSetDef gatSwapchainInfoProps[] =
{
    {"tSampleCount", (getter)pl_swapchain_info_get_tSampleCount, (setter)NULL, "Property: tSampleCount"},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot pl_swapchain_info_slots[] = {
    {Py_tp_init, (void*)pl_swapchain_info_init},
    {Py_tp_getset, (void*)gatSwapchainInfoProps},
    // {Py_tp_methods, (void*)gatIOCommands},
    {0, 0}
};

static PyType_Spec pl_swapchain_info_spec = {
    "pilotlight.plSwapchainInfo",
    sizeof(pyplSwapchainInfo),
    0,
    Py_TPFLAGS_DEFAULT,
    pl_swapchain_info_slots
};

//-----------------------------------------------------------------------------
// [SECTION] type conversion to python
//-----------------------------------------------------------------------------

static PyObject*
pl_vec2_to_py(plVec2 v)
{
    pyplVec2* obj = (pyplVec2*)PyObject_CallObject(gptVec2Type, NULL);
    if(!obj)
        return NULL;

    obj->v = v;
    return (PyObject*)obj;
}

static PyObject*
pl_vec3_to_py(plVec3 v)
{
    pyplVec3* obj = (pyplVec3*)PyObject_CallObject(gptVec2Type, NULL);
    if(!obj)
        return NULL;

    obj->v = v;
    return (PyObject*)obj;
}

static PyObject*
pl_vec4_to_py(plVec4 v)
{
    pyplVec4* obj = (pyplVec4*)PyObject_CallObject(gptVec4Type, NULL);
    if(!obj)
        return NULL;

    obj->v = v;
    return (PyObject*)obj;
}

static PyObject*
pl_mat4_to_py(plMat4 v)
{
    pyplMat4* obj = (pyplMat4*)PyObject_CallObject(gptMat4Type, NULL);
    if(!obj)
        return NULL;

    obj->m.x11 = v.x11;
    obj->m.x21 = v.x21;
    obj->m.x31 = v.x31;
    obj->m.x41 = v.x41;
    obj->m.x12 = v.x12;
    obj->m.x22 = v.x22;
    obj->m.x32 = v.x32;
    obj->m.x42 = v.x42;
    obj->m.x13 = v.x13;
    obj->m.x23 = v.x23;
    obj->m.x33 = v.x33;
    obj->m.x43 = v.x43;
    obj->m.x14 = v.x14;
    obj->m.x24 = v.x24;
    obj->m.x34 = v.x34;
    obj->m.x44 = v.x44;
    return (PyObject*)obj;
}

static PyObject*
pl_entity_to_py(plEcsTypeKey tKey, plEntity tEntity)
{
    return Py_BuildValue("(III)", tKey, tEntity.uIndex, tEntity.uGeneration);
}

//-----------------------------------------------------------------------------
// [SECTION] type conversion to c
//-----------------------------------------------------------------------------

static bool
pl_vec2_from_py(PyObject* obj, plVec2* out)
{
    if(PyObject_TypeCheck(obj, (PyTypeObject*)gptVec2Type))
    {
        pyplVec2* v = (pyplVec2*)obj;
        *out = v->v;
        return true;
    }

    // fallback: tuple
    if(PyTuple_Check(obj) && PyTuple_Size(obj) == 2)
    {
        out->x = (float)PyFloat_AsDouble(PyTuple_GetItem(obj, 0));
        out->y = (float)PyFloat_AsDouble(PyTuple_GetItem(obj, 1));
        return !PyErr_Occurred();
    }
    else if (PyList_Check(obj))
    {

        out->x = (float)PyFloat_AsDouble(PyList_GetItem(obj, 0));
        out->y = (float)PyFloat_AsDouble(PyList_GetItem(obj, 1));
        return !PyErr_Occurred();
    }

    PyErr_SetString(PyExc_TypeError, "Expected Vec2 or (x, y)");
    return false;
}

static bool
pl_vec3_from_py(PyObject* obj, plVec3* out)
{
    if(PyObject_TypeCheck(obj, (PyTypeObject*)gptVec3Type))
    {
        pyplVec3* v = (pyplVec3*)obj;
        *out = v->v;
        return true;
    }

    // fallback: tuple
    if(PyTuple_Check(obj) && PyTuple_Size(obj) == 3)
    {
        out->x = (float)PyFloat_AsDouble(PyTuple_GetItem(obj, 0));
        out->y = (float)PyFloat_AsDouble(PyTuple_GetItem(obj, 1));
        out->z = (float)PyFloat_AsDouble(PyTuple_GetItem(obj, 2));
        return !PyErr_Occurred();
    }
    else if (PyList_Check(obj))
    {

        out->x = (float)PyFloat_AsDouble(PyList_GetItem(obj, 0));
        out->y = (float)PyFloat_AsDouble(PyList_GetItem(obj, 1));
        out->z = (float)PyFloat_AsDouble(PyList_GetItem(obj, 2));
        return !PyErr_Occurred();
    }

    PyErr_SetString(PyExc_TypeError, "Expected Vec3 or (x, y, z)");
    return false;
}

static bool
pl_vec4_from_py(PyObject* obj, plVec4* out)
{
    if(PyObject_TypeCheck(obj, (PyTypeObject*)gptVec2Type))
    {
        pyplVec4* v = (pyplVec4*)obj;
        *out = v->v;
        return true;
    }

    // fallback: tuple
    if(PyTuple_Check(obj) && PyTuple_Size(obj) == 4)
    {
        out->x = (float)PyFloat_AsDouble(PyTuple_GetItem(obj, 0));
        out->y = (float)PyFloat_AsDouble(PyTuple_GetItem(obj, 1));
        out->z = (float)PyFloat_AsDouble(PyTuple_GetItem(obj, 2));
        out->w = (float)PyFloat_AsDouble(PyTuple_GetItem(obj, 3));
        return !PyErr_Occurred();
    }
    else if (PyList_Check(obj))
    {

        out->x = (float)PyFloat_AsDouble(PyList_GetItem(obj, 0));
        out->y = (float)PyFloat_AsDouble(PyList_GetItem(obj, 1));
        out->z = (float)PyFloat_AsDouble(PyList_GetItem(obj, 2));
        out->w = (float)PyFloat_AsDouble(PyList_GetItem(obj, 3));
        return !PyErr_Occurred();
    }

    PyErr_SetString(PyExc_TypeError, "Expected Vec4 or (x, y, z, w)");
    return false;
}

static bool
pl_mat4_from_py(PyObject* obj, plMat4* out)
{
    if(PyObject_TypeCheck(obj, (PyTypeObject*)gptMat4Type))
    {
        pyplMat4* v = (pyplMat4*)obj;
        out->x11 = v->m.x11;
        out->x21 = v->m.x21;
        out->x31 = v->m.x31;
        out->x41 = v->m.x41;
        out->x12 = v->m.x12;
        out->x22 = v->m.x22;
        out->x32 = v->m.x32;
        out->x42 = v->m.x42;
        out->x13 = v->m.x13;
        out->x23 = v->m.x23;
        out->x33 = v->m.x33;
        out->x43 = v->m.x43;
        out->x14 = v->m.x14;
        out->x24 = v->m.x24;
        out->x34 = v->m.x34;
        out->x44 = v->m.x44;
        return true;
    }

    // // fallback: tuple
    // if(PyTuple_Check(obj) && PyTuple_Size(obj) == 2)
    // {
    //     out->x = (float)PyFloat_AsDouble(PyTuple_GetItem(obj, 0));
    //     out->y = (float)PyFloat_AsDouble(PyTuple_GetItem(obj, 1));
    //     return !PyErr_Occurred();
    // }
    // else if (PyList_Check(obj))
    // {

    //     out->x = (float)PyFloat_AsDouble(PyList_GetItem(obj, 0));
    //     out->y = (float)PyFloat_AsDouble(PyList_GetItem(obj, 1));
    //     return !PyErr_Occurred();
    // }

    PyErr_SetString(PyExc_TypeError, "Expected plMat4");
    return false;
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
            pl_vec2_from_py(PyTuple_GetItem(ptObject, i) , &sbtList[i]);
        }
    }

    else if (PyList_Check(ptObject))
    {
        Py_ssize_t pySize = PyList_Size(ptObject);
        *puCountOut = (uint32_t)pySize;
        pl_sb_resize(sbtList, *puCountOut);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            pl_vec2_from_py(PyList_GetItem(ptObject, i) , &sbtList[i]);
        }
    }

    return sbtList;
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