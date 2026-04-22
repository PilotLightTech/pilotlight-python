/*
   pl_stats_ext_m.c
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] binding apis
// [SECTION] implementations
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] implementations
//-----------------------------------------------------------------------------

PyObject*
stats_new_frame(PyObject* self)
{
    gptStats->new_frame();
    Py_RETURN_NONE;
}

PyObject*
stats_get_counter(PyObject* self, PyObject* arg)
{
    const char* pcName = PyUnicode_AsUTF8(arg);
    double* pdCounter = gptStats->get_counter(pcName);
    return PyCapsule_New(pdCounter, "pd", NULL);
}
