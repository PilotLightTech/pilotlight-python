/*
   pilotlight.h
     - shared header file for all the modules (keep in mind we are actually
        utilizing unity builds for this project)
*/

/*
Index of this file:
// [SECTION] header mess
// [SECTION] includes
// [SECTION] helper macros
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

#define PL_UNITY_BUILD
#include "pl_unity_ext.c"

//-----------------------------------------------------------------------------
// [SECTION] helper macros
//-----------------------------------------------------------------------------

#define PL_ADD_INT_CONSTANT(X_ARG) {#X_ARG, X_ARG}
#define PL_ADD_UINT_CONSTANT(MODULE, X_ARG) pl_add_u32_constant(MODULE, #X_ARG, X_ARG)

// helps with some boilerplate code
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

// boilerplate for simple bindings

// void f(void)
#define PL_PY_BIND_VOID(FUNC_NAME, CALL) \
static PyObject* FUNC_NAME(PyObject* self, PyObject* unused) { (void)self; (void)unused; CALL; Py_RETURN_NONE; }

// key f(void)
#define PL_PY_BIND_KEY(FUNC_NAME, CALL) \
static PyObject* FUNC_NAME(PyObject* self, PyObject* unused) { (void)self; (void)unused; return PyLong_FromUnsignedLong((unsigned long)(CALL)); }

// void f(plComponentLibrary*) (use with METH_O, "ptLibrary" available to CALL)
// (pl_library_from_py is declared in pl_py_types.h)
#define PL_PY_BIND_LIBRARY(FUNC_NAME, CALL) \
static PyObject* FUNC_NAME(PyObject* self, PyObject* ptPyLibrary) \
{ \
    (void)self; \
    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary); \
    if(ptLibrary == NULL) return NULL; \
    CALL; \
    Py_RETURN_NONE; \
}

//-----------------------------------------------------------------------------
// [SECTION] public api
//-----------------------------------------------------------------------------


bool pl_parse_args(char* formatstring, const char** keywords, PyObject* args, PyObject* kwargs, const char* message, ...);

// defined in pl_main.c: false (and name of the missing library) if the graphics
// backend's runtime library can't be loaded (windows: vulkan-1.dll is delay loaded)
bool pl__python_graphics_runtime_available(const char** ppcMissingOut);

//-----------------------------------------------------------------------------
// [SECTION] structs
//-----------------------------------------------------------------------------

typedef struct _plPythonIntConstantPair
{
   const char* pcName;
   int         iValue;
} plPythonIntConstantPair;

#endif // PILOTLIGHT_H