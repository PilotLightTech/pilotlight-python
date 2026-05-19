#include "pl_py_math.h"
#include "structmember.h"
#define PL_INCLUDE_MATH_FUNCTION
#include "pl_math.h"

static PyObject* gptVec2Type = NULL;
static PyObject* gptVec3Type = NULL;
static PyObject* gptVec4Type = NULL;
static PyObject* gptMat4Type = NULL;

//-----------------------------------------------------------------------------
// [SECTION] vec2 functions
//-----------------------------------------------------------------------------

static int
pl_vec2_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plPyVec2* vec = (plPyVec2*)self;

    static const char* kwlist[] = {"x", "y", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|ff", (char**)kwlist, &vec->v.x, &vec->v.y))
        return -1;

    return 0;
}

static PyObject*
pl_vec2_repr(PyObject* self)
{
    plPyVec2* v = (plPyVec2*)self;
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

    plPyVec2* va = (plPyVec2*)a;
    plPyVec2* vb = (plPyVec2*)b;

    return pl_vec2_new(pl_add_vec2(va->v, vb->v));
}

static PyObject*
pl_vec2_subtract(PyObject* a, PyObject* b)
{
    if(!pl_vec2_check(a) || !pl_vec2_check(b))
        Py_RETURN_NOTIMPLEMENTED;

    plPyVec2* va = (plPyVec2*)a;
    plPyVec2* vb = (plPyVec2*)b;

    return pl_vec2_new(pl_sub_vec2(va->v, vb->v));
}

static PyObject*
pl_vec2_negative(PyObject* self)
{
    plPyVec2* v = (plPyVec2*)self;
    return pl_vec2_new((plVec2){-v->v.x, -v->v.y});
}

static PyMemberDef pl_vec2_members[] = {
    {"x", T_FLOAT, offsetof(plPyVec2, v.x), 0, ""},
    {"y", T_FLOAT, offsetof(plPyVec2, v.y), 0, ""},
    {NULL}
};

static PyType_Slot pl_vec2_slots[] = {
    {Py_tp_init, (void*)pl_vec2_init},
    {Py_tp_members, (void*)pl_vec2_members},
    {Py_tp_repr, (void*)pl_vec2_repr},
    {Py_nb_add, (void*)pl_vec2_add},
    {Py_nb_subtract, (void*)pl_vec2_subtract},
    // {Py_nb_multiply, (void*)pl_vec2_multiply},
    {Py_nb_negative, (void*)pl_vec2_negative},
    {0, 0}
};

static PyType_Spec pl_vec2_spec = {
    "pilotlight.plVec2",
    sizeof(plPyVec2),
    0,
    Py_TPFLAGS_DEFAULT,
    pl_vec2_slots
};

//-----------------------------------------------------------------------------
// [SECTION] vec3 functions
//-----------------------------------------------------------------------------

static int
pl_vec3_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plPyVec3* vec = (plPyVec3*)self;

    static const char* kwlist[] = {"x", "y", "z", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|fff", (char**)kwlist, &vec->v.x, &vec->v.y, &vec->v.z))
        return -1;

    return 0;
}

static PyObject*
pl_vec3_repr(PyObject* self)
{
    plPyVec3* v = (plPyVec3*)self;
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

    plPyVec3* va = (plPyVec3*)a;
    plPyVec3* vb = (plPyVec3*)b;

    return pl_vec3_new(pl_add_vec3(va->v, vb->v));
}

static PyObject*
pl_vec3_subtract(PyObject* a, PyObject* b)
{
    if(!pl_vec3_check(a) || !pl_vec3_check(b))
        Py_RETURN_NOTIMPLEMENTED;

    plPyVec3* va = (plPyVec3*)a;
    plPyVec3* vb = (plPyVec3*)b;

    return pl_vec3_new(pl_sub_vec3(va->v, vb->v));
}

static PyObject*
pl_vec3_negative(PyObject* self)
{
    plPyVec3* v = (plPyVec3*)self;
    return pl_vec3_new((plVec3){-v->v.x, -v->v.y, -v->v.z});
}

static PyMemberDef pl_vec3_members[] = {
    {"x", T_FLOAT, offsetof(plPyVec3, v.x), 0, ""},
    {"y", T_FLOAT, offsetof(plPyVec3, v.y), 0, ""},
    {"z", T_FLOAT, offsetof(plPyVec3, v.z), 0, ""},
    {NULL}
};

static PyType_Slot pl_vec3_slots[] = {
    {Py_tp_init, (void*)pl_vec3_init},
    {Py_tp_members, (void*)pl_vec3_members},
    {Py_tp_repr, (void*)pl_vec3_repr},
    {Py_nb_add, (void*)pl_vec3_add},
    {Py_nb_subtract, (void*)pl_vec3_subtract},
    // {Py_nb_multiply, (void*)pl_vec2_multiply},
    {Py_nb_negative, (void*)pl_vec3_negative},
    {0, 0}
};

static PyType_Spec pl_vec3_spec = {
    "pilotlight.plVec3",
    sizeof(plPyVec3),
    0,
    Py_TPFLAGS_DEFAULT,
    pl_vec3_slots
};

//-----------------------------------------------------------------------------
// [SECTION] vec4 functions
//-----------------------------------------------------------------------------

static int
pl_vec4_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plPyVec4* vec = (plPyVec4*)self;

    static const char* kwlist[] = {"x", "y", "z", "w", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|ffff", (char**)kwlist, &vec->v.x, &vec->v.y, &vec->v.z, &vec->v.w))
        return -1;

    return 0;
}

static PyObject*
pl_vec4_repr(PyObject* self)
{
    plPyVec4* v = (plPyVec4*)self;
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

    plPyVec4* va = (plPyVec4*)a;
    plPyVec4* vb = (plPyVec4*)b;

    return pl_vec4_new(pl_add_vec4(va->v, vb->v));
}

static PyObject*
pl_vec4_subtract(PyObject* a, PyObject* b)
{
    if(!pl_vec4_check(a) || !pl_vec4_check(b))
        Py_RETURN_NOTIMPLEMENTED;

    plPyVec4* va = (plPyVec4*)a;
    plPyVec4* vb = (plPyVec4*)b;

    return pl_vec4_new(pl_sub_vec4(va->v, vb->v));
}

static PyObject*
pl_vec4_negative(PyObject* self)
{
    plPyVec4* v = (plPyVec4*)self;
    return pl_vec4_new((plVec4){-v->v.x, -v->v.y, -v->v.z, -v->v.w});
}

static PyMemberDef pl_vec4_members[] = {
    {"x", T_FLOAT, offsetof(plPyVec4, v.x), 0, ""},
    {"y", T_FLOAT, offsetof(plPyVec4, v.y), 0, ""},
    {"z", T_FLOAT, offsetof(plPyVec4, v.z), 0, ""},
    {"w", T_FLOAT, offsetof(plPyVec4, v.w), 0, ""},
    {NULL}
};

static PyType_Slot pl_vec4_slots[] = {
    {Py_tp_init, (void*)pl_vec4_init},
    {Py_tp_members, (void*)pl_vec4_members},
    {Py_tp_repr, (void*)pl_vec4_repr},
    {Py_nb_add, (void*)pl_vec4_add},
    {Py_nb_subtract, (void*)pl_vec4_subtract},
    // {Py_nb_multiply, (void*)pl_vec2_multiply},
    {Py_nb_negative, (void*)pl_vec4_negative},
    {0, 0}
};

static PyType_Spec pl_vec4_spec = {
    "pilotlight.plVec4",
    sizeof(plPyVec4),
    0,
    Py_TPFLAGS_DEFAULT,
    pl_vec4_slots
};

//-----------------------------------------------------------------------------
// [SECTION] mat4 registration
//-----------------------------------------------------------------------------

static bool
pl_mat4_check(PyObject* obj)
{
    return PyObject_TypeCheck(obj, (PyTypeObject*)gptMat4Type);
}

static PyObject*
pl_mat4_repr(PyObject* self)
{
    plPyMat4* v = (plPyMat4*)self;
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
    plPyMat4* mat = (plPyMat4*)self;

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
    plPyMat4* ptLeft = NULL;
    plPyMat4* ptRight = NULL;

    if(pl_mat4_check(a))
    {
        ptLeft = (plPyMat4*)a;
        ptRight = (plPyMat4*)b;
    }
    else if(pl_mat4_check(b))
    {
        ptRight = (plPyMat4*)b;
        ptLeft = (plPyMat4*)a;
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
    {"x11", T_FLOAT, offsetof(plPyMat4, m.x11), 0, ""},
    {"x21", T_FLOAT, offsetof(plPyMat4, m.x21), 0, ""},
    {"x31", T_FLOAT, offsetof(plPyMat4, m.x31), 0, ""},
    {"x41", T_FLOAT, offsetof(plPyMat4, m.x41), 0, ""},

    {"x12", T_FLOAT, offsetof(plPyMat4, m.x12), 0, ""},
    {"x22", T_FLOAT, offsetof(plPyMat4, m.x22), 0, ""},
    {"x32", T_FLOAT, offsetof(plPyMat4, m.x32), 0, ""},
    {"x42", T_FLOAT, offsetof(plPyMat4, m.x42), 0, ""},

    {"x13", T_FLOAT, offsetof(plPyMat4, m.x13), 0, ""},
    {"x23", T_FLOAT, offsetof(plPyMat4, m.x23), 0, ""},
    {"x33", T_FLOAT, offsetof(plPyMat4, m.x33), 0, ""},
    {"x43", T_FLOAT, offsetof(plPyMat4, m.x43), 0, ""},

    {"x14", T_FLOAT, offsetof(plPyMat4, m.x14), 0, ""},
    {"x24", T_FLOAT, offsetof(plPyMat4, m.x24), 0, ""},
    {"x34", T_FLOAT, offsetof(plPyMat4, m.x34), 0, ""},
    {"x44", T_FLOAT, offsetof(plPyMat4, m.x44), 0, ""},
    {NULL}
};

static PyType_Slot pl_mat4_slots[] = {
    {Py_tp_init, (void*)pl_mat4_init},
    {Py_tp_members, (void*)pl_mat4_members},
    {Py_tp_repr, (void*)pl_mat4_repr},
    // {Py_nb_add, (void*)pl_vec2_add},
    // {Py_nb_subtract, (void*)pl_vec2_subtract},
    {Py_nb_multiply, (void*)pl_mat4_multiply},
    // {Py_nb_negative, (void*)pl_vec2_negative},
    {0, 0}
};

static PyType_Spec pl_mat4_spec = {
    "pilotlight.plMat4",
    sizeof(plPyMat4),
    0,
    Py_TPFLAGS_DEFAULT,
    pl_mat4_slots
};

//-----------------------------------------------------------------------------
// [SECTION] vec2 helpers
//-----------------------------------------------------------------------------

static bool
pl_vec2_from_py(PyObject* obj, plVec2* out)
{
    if(PyObject_TypeCheck(obj, (PyTypeObject*)gptVec2Type))
    {
        plPyVec2* v = (plPyVec2*)obj;
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

static PyObject*
pl_vec2_to_py(plVec2 v)
{
    plPyVec2* obj = (plPyVec2*)PyObject_CallObject(gptVec2Type, NULL);
    if(!obj)
        return NULL;

    obj->v = v;
    return (PyObject*)obj;
}

static bool
pl_vec3_from_py(PyObject* obj, plVec3* out)
{
    if(PyObject_TypeCheck(obj, (PyTypeObject*)gptVec3Type))
    {
        plPyVec3* v = (plPyVec3*)obj;
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

static PyObject*
pl_vec3_to_py(plVec3 v)
{
    plPyVec3* obj = (plPyVec3*)PyObject_CallObject(gptVec2Type, NULL);
    if(!obj)
        return NULL;

    obj->v = v;
    return (PyObject*)obj;
}

static bool
pl_vec4_from_py(PyObject* obj, plVec4* out)
{
    if(PyObject_TypeCheck(obj, (PyTypeObject*)gptVec2Type))
    {
        plPyVec4* v = (plPyVec4*)obj;
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

static PyObject*
pl_vec4_to_py(plVec4 v)
{
    plPyVec4* obj = (plPyVec4*)PyObject_CallObject(gptVec4Type, NULL);
    if(!obj)
        return NULL;

    obj->v = v;
    return (PyObject*)obj;
}

static bool
pl_mat4_from_py(PyObject* obj, plMat4* out)
{
    if(PyObject_TypeCheck(obj, (PyTypeObject*)gptMat4Type))
    {
        plPyMat4* v = (plPyMat4*)obj;
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

static PyObject*
pl_mat4_to_py(plMat4 v)
{
    plPyMat4* obj = (plPyMat4*)PyObject_CallObject(gptMat4Type, NULL);
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