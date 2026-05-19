#ifndef PL_PY_H
#define PL_PY_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdbool.h>
#include "pl_math.h"

typedef struct _plPyVec2
{
    PyObject_HEAD
    plVec2 v;
} plPyVec2;

typedef struct _plPyVec3
{
    PyObject_HEAD
    plVec3 v;
} plPyVec3;

typedef struct _plPyVec4
{
    PyObject_HEAD
    plVec4 v;
} plPyVec4;

typedef struct _plPyMat4
{
    PyObject_HEAD
    plMat4 m;
} plPyMat4;

static bool      pl_vec2_from_py(PyObject* obj, plVec2* out);
static PyObject* pl_vec2_to_py  (plVec2 v);

static bool      pl_vec3_from_py(PyObject* obj, plVec3* out);
static PyObject* pl_vec3_to_py  (plVec3 v);

static bool      pl_vec4_from_py(PyObject* obj, plVec4* out);
static PyObject* pl_vec4_to_py  (plVec4 v);

static bool      pl_mat4_from_py(PyObject* obj, plMat4* out);
static PyObject* pl_mat4_to_py  (plMat4 v);

#endif // PL_PY_H