/*
   pl_shader_ext_m.c
*/

/*
Index of this file:
// [SECTION] includes
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
plPakI_begin_packing(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "pcFile",
        "uContentVersion",
        NULL,
    };

    const char* pcFile = NULL;
    uint32_t uContentVersion = 0;
	if (!pl_parse("sI", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &pcFile, &uContentVersion))
		return NULL;

    // plWindow* ptWindowPtr = PyCapsule_GetPointer(ptWindow, "plWindow");

    plPakFile* ptPakFile = NULL;
    bool bResult = gptPak->begin_packing(pcFile, uContentVersion, &ptPakFile);

    PyObject* ptCapsule = PyCapsule_New(ptPakFile, "plPakFile", NULL);
    return Py_BuildValue("(pO)", bResult, ptCapsule);
}

PyObject*
plPakI_add_from_disk(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "ptPakFile",
        "pcPakPath",
        "pcFilePath",
        "bCompress",
        NULL,
    };

    PyObject* ptCapsule = NULL;
    const char* pcPakPath = NULL;
    const char* pcFilePath = NULL;
    int bCompress = false;
	if (!pl_parse("Ossp", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptCapsule, &pcPakPath, &pcFilePath, &bCompress))
		return NULL;

    plPakFile* ptPakPtr = PyCapsule_GetPointer(ptCapsule, "plPakFile");

    bool bResult = gptPak->add_from_disk(ptPakPtr, pcPakPath, pcFilePath, bCompress);
    return PyBool_FromLong(bResult);
}

PyObject*
plPakI_end_packing(PyObject* self, PyObject* arg)
{
    plPakFile* ptPakPtr = PyCapsule_GetPointer(arg, "plPakFile");

    gptPak->end_packing(&ptPakPtr);
    Py_RETURN_NONE;
}