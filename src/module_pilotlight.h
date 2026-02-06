/*
   module_pilotlight.h
*/

/*
Index of this file:
// [SECTION] header mess
// [SECTION] includes
// [SECTION] forward declarations
// [SECTION] global data
// [SECTION] public api
*/

//-----------------------------------------------------------------------------
// [SECTION] header mess
//-----------------------------------------------------------------------------

#ifndef PL_MODULE_PILOTLIGHT_H
#define PL_MODULE_PILOTLIGHT_H

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#define PY_SSIZE_T_CLEAN
#include <Python.h>

//-----------------------------------------------------------------------------
// [SECTION] forward declarations
//-----------------------------------------------------------------------------

// external
typedef struct _plPythonArgumentPack plPythonArgumentPack; // pl_python.h

//-----------------------------------------------------------------------------
// [SECTION] global data
//-----------------------------------------------------------------------------

extern plPythonArgumentPack* sbtParserPacks;

//-----------------------------------------------------------------------------
// [SECTION] public api
//-----------------------------------------------------------------------------

PyMODINIT_FUNC PyInit__pilotlight(void);

#endif // PL_MODULE_PILOTLIGHT_H