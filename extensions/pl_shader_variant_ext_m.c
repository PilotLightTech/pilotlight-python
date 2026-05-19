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
shader_variant_initialize(PyObject* self, PyObject* args, PyObject* kwargs)
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

static PyMethodDef gatCommandsplShaderVariantI[] =
{
    {"initialize", (PyCFunction)shader_variant_initialize, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plShaderVariantI)