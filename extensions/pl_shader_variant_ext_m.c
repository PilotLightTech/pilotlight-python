/*
   pl_ecs_ext_m.c
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
plShaderVariantI_initialize(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "ptDevice",
        NULL,
    };

    PyObject* ptPyDevice = NULL;
	if (!pl_parse("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyDevice))
		return NULL;


    const plShaderVariantInit tShaderVariantInit = {
        .ptDevice = PyCapsule_GetPointer(ptPyDevice, "plDevice")
    };
    gptShaderVariant->initialize(tShaderVariantInit);
    Py_RETURN_NONE;
}
