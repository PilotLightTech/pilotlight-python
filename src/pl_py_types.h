/*
   pl_py_types.h
*/

/*
Index of this file:
// [SECTION] header mess
// [SECTION] includes
// [SECTION] global data
// [SECTION] types
// [SECTION] type conversion to python
// [SECTION] type conversion to c
*/

//-----------------------------------------------------------------------------
// [SECTION] header mess
//-----------------------------------------------------------------------------

#ifndef PL_PY_TYPES_H
#define PL_PY_TYPES_H

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdbool.h>
#include "pl_math.h"

#include "pl.h"
#include "pl_graphics_ext.h"
#include "pl_camera_ext.h"

//-----------------------------------------------------------------------------
// [SECTION] global data
//-----------------------------------------------------------------------------

static PyObject* gptVec2Type          = NULL;
static PyObject* gptVec3Type          = NULL;
static PyObject* gptVec4Type          = NULL;
static PyObject* gptMat4Type          = NULL;
static PyObject* gptIOType            = NULL;
static PyObject* gptSwapchainInfoType = NULL;
static PyObject* gptCameraType        = NULL;

//-----------------------------------------------------------------------------
// [SECTION] types
//-----------------------------------------------------------------------------

typedef struct _pyplIO
{
    PyObject_HEAD
    plIO* ptIO;
} pyplIO;

typedef struct _pyplSwapchainInfo
{
    PyObject_HEAD
    plSwapchainInfo tInfo;
} pyplSwapchainInfo;

typedef struct _pyplCamera
{
    PyObject_HEAD
    plCamera* ptCamera;
} pyplCamera;

typedef struct _pyplVec2
{
    PyObject_HEAD
    plVec2 v;
} pyplVec2;

typedef struct _pyplVec3
{
    PyObject_HEAD
    plVec3 v;
} pyplVec3;

typedef struct _pyplVec4
{
    PyObject_HEAD
    plVec4 v;
} pyplVec4;

typedef struct _pyplMat4
{
    PyObject_HEAD
    plMat4 m;
} pyplMat4;

typedef struct _plPythonEntity
{
   plEcsTypeKey tKey;
   plEntity     tEntity;
} plPythonEntity;

//-----------------------------------------------------------------------------
// [SECTION] type conversions to python
//-----------------------------------------------------------------------------

static PyObject* pl_vec2_to_py(plVec2);
static PyObject* pl_vec3_to_py(plVec3);
static PyObject* pl_vec4_to_py(plVec4);
static PyObject* pl_mat4_to_py(plMat4);

static PyObject* pl_entity_to_py(plEcsTypeKey, plEntity);

//-----------------------------------------------------------------------------
// [SECTION] type conversions to c
//-----------------------------------------------------------------------------

static bool pl_vec2_from_py(PyObject*, plVec2* out);
static bool pl_vec3_from_py(PyObject*, plVec3* out);
static bool pl_vec4_from_py(PyObject*, plVec4* out);
static bool pl_mat4_from_py(PyObject*, plMat4* out);

plPythonEntity pl_get_entity_from_python(PyObject*);

// temporary arrays
plVec2* pl_get_vec2_list_from_python(PyObject*, uint32_t*);

#endif // PL_PY_TYPES_H