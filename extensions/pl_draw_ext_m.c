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
plDrawI_add_triangle_filled(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonP2 = NULL;
    uint32_t uColor = PL_COLOR_32_WHITE;

    static const char* apcKeywords[] = {
        "layer",
        "p0",
        "p1",
        "p2",
        "uColor",
        NULL,
    };

	if (!pl_parse("OOOO|$I", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &ptPythonP2, &uColor))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    gptDraw->add_triangle_filled(ptLayer,
        pl_get_vec2_from_python(ptPythonP0),
        pl_get_vec2_from_python(ptPythonP1),
        pl_get_vec2_from_python(ptPythonP2),
        (plDrawSolidOptions){.uColor = uColor});

    Py_RETURN_NONE;
}
