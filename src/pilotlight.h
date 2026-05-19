/*
   pilotlight.h
     - shared header file for all the modules (keep in mind we are actually
        utilizing unity builds for this project)
*/

/*
Index of this file:
// [SECTION] header mess
// [SECTION] forward declarations
// [SECTION] public api
*/

//-----------------------------------------------------------------------------
// [SECTION] header mess
//-----------------------------------------------------------------------------

#ifndef PILOTLIGHT_H
#define PILOTLIGHT_H

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#define PL_EXPERIMENTAL
#define PL_UNITY_BUILD
#include "pl_unity_ext.c"

//-----------------------------------------------------------------------------
// [SECTION] custom types
//-----------------------------------------------------------------------------

typedef struct _plPyIO
{
    PyObject_HEAD
    plIO* ptIO;
} plPyIO;

typedef struct _pyplSwapchainInfo
{
    PyObject_HEAD
    plSwapchainInfo tInfo;
} pyplSwapchainInfo;

// custom types
static PyObject* gptIOType = NULL;
static PyObject* gptSwapchainInfoType = NULL;
static PyObject* gptCameraType = NULL;

//-----------------------------------------------------------------------------
// [SECTION] forward declarations
//-----------------------------------------------------------------------------

typedef struct _plPythonIntConstantPair
{
   const char* pcName;
   int         iValue;
} plPythonIntConstantPair;

typedef struct _plPythonEntity
{
   plEcsTypeKey tKey;
   plEntity     tEntity;
} plPythonEntity;

#define PL_ADD_INT_CONSTANT(X_ARG) {#X_ARG, X_ARG}
#define PL_ADD_UINT_CONSTANT(MODULE, X_ARG) pl_add_u32_constant(MODULE, #X_ARG, X_ARG)
#define PL_PYTHON_METHOD(ARG, FLAGS, DOCS) {#ARG, (PyCFunction) ARG, FLAGS, DOCS}

#define PL_NEW_PYTHON_API(ARG) \
typedef struct _py##ARG \
{ \
    PyObject_HEAD \
} py##ARG; \
static PyType_Slot gatSlots##ARG[] = { {Py_tp_methods, (void*)gatCommands##ARG}, {0, 0}}; \
static PyType_Spec gtSpec##ARG = {"pilotlight." # ARG, sizeof(py##ARG), 0, Py_TPFLAGS_DEFAULT, gatSlots##ARG};

#define PL_ADD_PYTHON_API(ARG) \
static PyObject* gpt##ARG = NULL; \
gpt##ARG = PyType_FromSpec(&gtSpec##ARG); \
PyModule_AddObject(ptModule, #ARG, gpt##ARG);

//-----------------------------------------------------------------------------
// [SECTION] public api
//-----------------------------------------------------------------------------

bool pl_parse(char* formatstring, const char** keywords, PyObject* args, PyObject* kwargs, const char* message, ...);

plVec2 pl_get_vec2_from_python(PyObject*);
plVec3d pl_get_dvec3_from_python(PyObject*);
plPythonEntity pl_get_entity_from_python(PyObject*);

// temporary
plVec2* pl_get_vec2_list_from_python(PyObject*, uint32_t*);

#endif // PILOTLIGHT_H