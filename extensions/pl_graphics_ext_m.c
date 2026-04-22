/*
   pl_draw_ext_m.c
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
graphics_flush_device(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPythonDevice = NULL;

    static const char* apcKeywords[] = {
        "device",
        NULL,
    };

	if (!pl_parse("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonDevice))
		return NULL;

    plDevice* ptDevice = PyCapsule_GetPointer(ptPythonDevice, "plDevice");
    gptGfx->flush_device(ptDevice);
    Py_RETURN_NONE;
}
