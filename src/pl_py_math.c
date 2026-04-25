#include "pl_py_math.h"
#include "structmember.h"

static PyObject* gptVec2Type = NULL;

static int
pl_vec2_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plPyVec2* vec = (plPyVec2*)self;

    float x = 0.0f;
    float y = 0.0f;

    static const char* kwlist[] = {"x", "y", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|ff", (char**)kwlist, &x, &y))
        return -1;

    vec->x = x;
    vec->y = y;
    return 0;
}

static PyObject*
pl_vec2_repr(PyObject* self)
{
    plPyVec2* v = (plPyVec2*)self;
    return PyUnicode_FromFormat("Vec2(%f, %f)", v->x, v->y);
}

static bool
pl_vec2_check(PyObject* obj)
{
    return PyObject_TypeCheck(obj, (PyTypeObject*)gptVec2Type);
}

static PyObject*
pl_vec2_new(float x, float y)
{
    PyObject* args = Py_BuildValue("(ff)", x, y);
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

    return pl_vec2_new(va->x + vb->x, va->y + vb->y);
}

static PyObject*
pl_vec2_subtract(PyObject* a, PyObject* b)
{
    if(!pl_vec2_check(a) || !pl_vec2_check(b))
        Py_RETURN_NOTIMPLEMENTED;

    plPyVec2* va = (plPyVec2*)a;
    plPyVec2* vb = (plPyVec2*)b;

    return pl_vec2_new(va->x - vb->x, va->y - vb->y);
}

static PyObject*
pl_vec2_multiply(PyObject* a, PyObject* b)
{
    plPyVec2* v = NULL;
    PyObject* scalar_obj = NULL;

    if(pl_vec2_check(a))
    {
        v = (plPyVec2*)a;
        scalar_obj = b;
    }
    else if(pl_vec2_check(b))
    {
        v = (plPyVec2*)b;
        scalar_obj = a;
    }
    else
    {
        Py_RETURN_NOTIMPLEMENTED;
    }

    const double scalar = PyFloat_AsDouble(scalar_obj);
    if(PyErr_Occurred())
        return NULL;

    return pl_vec2_new(v->x * (float)scalar, v->y * (float)scalar);
}

static PyObject*
pl_vec2_negative(PyObject* self)
{
    plPyVec2* v = (plPyVec2*)self;
    return pl_vec2_new(-v->x, -v->y);
}

static PyMemberDef pl_vec2_members[] = {
    {"x", T_FLOAT, offsetof(plPyVec2, x), 0, ""},
    {"y", T_FLOAT, offsetof(plPyVec2, y), 0, ""},
    {NULL}
};

static PyType_Slot pl_vec2_slots[] = {
    {Py_tp_init, (void*)pl_vec2_init},
    {Py_tp_members, (void*)pl_vec2_members},
    {Py_tp_repr, (void*)pl_vec2_repr},
    {Py_nb_add, (void*)pl_vec2_add},
    {Py_nb_subtract, (void*)pl_vec2_subtract},
    {Py_nb_multiply, (void*)pl_vec2_multiply},
    {Py_nb_negative, (void*)pl_vec2_negative},
    {0, 0}
};

static PyType_Spec pl_vec2_spec = {
    .name = "pilotlight.Vec2",
    .basicsize = sizeof(plPyVec2),
    .flags = Py_TPFLAGS_DEFAULT,
    .slots = pl_vec2_slots
};

static bool
pl_vec2_from_py(PyObject* obj, plVec2* out)
{
    if(PyObject_TypeCheck(obj, (PyTypeObject*)gptVec2Type))
    {
        plPyVec2* v = (plPyVec2*)obj;
        out->x = v->x;
        out->y = v->y;
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

    obj->x = v.x;
    obj->y = v.y;
    return (PyObject*)obj;
}