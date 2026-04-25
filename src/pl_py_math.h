#ifndef PL_PY_H
#define PL_PY_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdbool.h>
#include "pl_math.h"

typedef struct _plPyVec2
{
    PyObject_HEAD
    float x;
    float y;
} plPyVec2;

static bool      pl_vec2_from_py(PyObject* obj, plVec2* out);
static PyObject* pl_vec2_to_py  (plVec2 v);

#endif // PL_PY_H