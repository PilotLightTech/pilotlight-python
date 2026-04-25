/*
   pl_draw_ext_m.c
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] enums
// [SECTION] implementations
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"
#include "pl_py_math.h"

//-----------------------------------------------------------------------------
// [SECTION] enums
//-----------------------------------------------------------------------------

plPythonIntConstantPair gatDrawIntPairs[] = {

    // plDrawFlags
    PL_ADD_INT_CONSTANT(PL_DRAW_FLAG_NONE),
    PL_ADD_INT_CONSTANT(PL_DRAW_FLAG_DEPTH_TEST),
    PL_ADD_INT_CONSTANT(PL_DRAW_FLAG_DEPTH_WRITE),
    PL_ADD_INT_CONSTANT(PL_DRAW_FLAG_CULL_FRONT),
    PL_ADD_INT_CONSTANT(PL_DRAW_FLAG_CULL_BACK),
    PL_ADD_INT_CONSTANT(PL_DRAW_FLAG_FRONT_FACE_CW),
    PL_ADD_INT_CONSTANT(PL_DRAW_FLAG_REVERSE_Z_DEPTH),

    // plDrawRectFlags
    PL_ADD_INT_CONSTANT(PL_DRAW_RECT_FLAG_NONE),
    PL_ADD_INT_CONSTANT(PL_DRAW_RECT_FLAG_ROUND_CORNERS_TOP_LEFT),
    PL_ADD_INT_CONSTANT(PL_DRAW_RECT_FLAG_ROUND_CORNERS_TOP_RIGHT),
    PL_ADD_INT_CONSTANT(PL_DRAW_RECT_FLAG_ROUND_CORNERS_BOTTOM_LEFT),
    PL_ADD_INT_CONSTANT(PL_DRAW_RECT_FLAG_ROUND_CORNERS_BOTTOM_RIGHT),
    PL_ADD_INT_CONSTANT(PL_DRAW_RECT_FLAG_ROUND_CORNERS_NONE),
    PL_ADD_INT_CONSTANT(PL_DRAW_RECT_FLAG_ROUND_CORNERS_TOP),
    PL_ADD_INT_CONSTANT(PL_DRAW_RECT_FLAG_ROUND_CORNERS_BOTTOM),
    PL_ADD_INT_CONSTANT(PL_DRAW_RECT_FLAG_ROUND_CORNERS_LEFT),
    PL_ADD_INT_CONSTANT(PL_DRAW_RECT_FLAG_ROUND_CORNERS_RIGHT),
    PL_ADD_INT_CONSTANT(PL_DRAW_RECT_FLAG_ROUND_CORNERS_All)
};

static plVec2* gsbtTempVec2Vec = NULL;

static inline void
pl__get_draw_solid_options(PyObject* ptPythonOptions, plDrawSolidOptions* ptOptionsOut)
{
    PyObject* ptPythonOptionColor = PyObject_GetAttrString(ptPythonOptions, "uColor");
    PyLong_AsUInt32(ptPythonOptionColor, &ptOptionsOut->uColor);
    Py_DECREF(ptPythonOptionColor);
}

static inline void
pl__get_draw_line_options(PyObject* ptPythonOptions, plDrawLineOptions* ptOptionsOut)
{
    PyObject* ptPythonOptionColor = PyObject_GetAttrString(ptPythonOptions, "uColor");
    PyLong_AsUInt32(ptPythonOptionColor, &ptOptionsOut->uColor);
    Py_DECREF(ptPythonOptionColor);

    PyObject* ptPythonOptionThickness = PyObject_GetAttrString(ptPythonOptions, "fThickness");
    ptOptionsOut->fThickness = (float)PyFloat_AsDouble(ptPythonOptionThickness);
    Py_DECREF(ptPythonOptionThickness);
}

//-----------------------------------------------------------------------------
// [SECTION] implementations
//-----------------------------------------------------------------------------

PyObject*
draw_add_triangle_filled(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonP2 = NULL;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "p0",
        "p1",
        "p2",
        "options",
        NULL,
    };

	if (!pl_parse("OOOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &ptPythonP2, &ptPythonOptions))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");
    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};
    plVec2 tP1 = {0};
    plVec2 tP2 = {0};

    pl_vec2_from_py(ptPythonP0, &tP0);
    pl_vec2_from_py(ptPythonP1, &tP1);
    pl_vec2_from_py(ptPythonP2, &tP2);

    gptDraw->add_triangle_filled(ptLayer,
        tP0,
        tP1,
        tP2,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_line(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "p0",
        "p1",
        "options",
        NULL,
    };

	if (!pl_parse("OOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &ptPythonOptions))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};
    plVec2 tP1 = {0};

    pl_vec2_from_py(ptPythonP0, &tP0);
    pl_vec2_from_py(ptPythonP1, &tP1);

    gptDraw->add_line(ptLayer,
        tP0,
        tP1,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_triangle(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonP2 = NULL;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "p0",
        "p1",
        "p2",
        "options",
        NULL,
    };

	if (!pl_parse("OOOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &ptPythonP2, &ptPythonOptions))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};
    plVec2 tP1 = {0};
    plVec2 tP2 = {0};

    pl_vec2_from_py(ptPythonP0, &tP0);
    pl_vec2_from_py(ptPythonP1, &tP1);
    pl_vec2_from_py(ptPythonP2, &tP2);

    gptDraw->add_triangle(ptLayer,
        tP0,
        tP1,
        tP2,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_rect(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "pMin",
        "pMax",
        "options",
        NULL,
    };

	if (!pl_parse("OOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &ptPythonOptions))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};
    plVec2 tP1 = {0};

    pl_vec2_from_py(ptPythonP0, &tP0);
    pl_vec2_from_py(ptPythonP1, &tP1);

    gptDraw->add_rect(ptLayer,
        tP0,
        tP1,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_rect_rounded(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonOptions = NULL;

    float fRadius = 1.0f;
    uint32_t uSegments = 0;
    int iFlags = PL_DRAW_RECT_FLAG_NONE;

    static const char* apcKeywords[] = {
        "layer",
        "pMin",
        "pMax",
        "radius",
        "segments",
        "flags",
        "options",
        NULL,
    };

	if (!pl_parse("OOOfIiO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &fRadius, &uSegments, &iFlags, 
        &ptPythonOptions))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};
    plVec2 tP1 = {0};

    pl_vec2_from_py(ptPythonP0, &tP0);
    pl_vec2_from_py(ptPythonP1, &tP1);

    gptDraw->add_rect_rounded(ptLayer,
        tP0,
        tP1,
        fRadius, uSegments, iFlags, tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_quad(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonP2 = NULL;
    PyObject* ptPythonP3 = NULL;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "p0",
        "p1",
        "p2",
        "p3",
        "options",
        NULL,
    };

	if (!pl_parse("OOOOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &ptPythonP2, &ptPythonP3,
        &ptPythonOptions))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};
    plVec2 tP1 = {0};
    plVec2 tP2 = {0};
    plVec2 tP3 = {0};

    pl_vec2_from_py(ptPythonP0, &tP0);
    pl_vec2_from_py(ptPythonP1, &tP1);
    pl_vec2_from_py(ptPythonP2, &tP2);
    pl_vec2_from_py(ptPythonP3, &tP3);

    gptDraw->add_quad(ptLayer,
        tP0,
        tP1,
        tP2,
        tP3,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_circle(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonOptions = NULL;

    float fRadius = 1.0f;
    uint32_t uSegments = 0;

    static const char* apcKeywords[] = {
        "layer",
        "p",
        "radius",
        "segments",
        "options",
        NULL,
    };

	if (!pl_parse("OOfIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &fRadius, &uSegments, 
        &ptPythonOptions))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};

    pl_vec2_from_py(ptPythonP0, &tP0);

    gptDraw->add_circle(ptLayer,
        tP0,
        fRadius, uSegments, tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_bezier_quad(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonP2 = NULL;
    PyObject* ptPythonOptions = NULL;

    uint32_t uSegments = 0;

    static const char* apcKeywords[] = {
        "layer",
        "p0",
        "p1",
        "p2",
        "segments",
        "options",
        NULL,
    };

	if (!pl_parse("OOOOIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &ptPythonP2, &uSegments, 
        &ptPythonOptions))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};
    plVec2 tP1 = {0};
    plVec2 tP2 = {0};

    pl_vec2_from_py(ptPythonP0, &tP0);
    pl_vec2_from_py(ptPythonP1, &tP1);
    pl_vec2_from_py(ptPythonP2, &tP2);

    gptDraw->add_bezier_quad(ptLayer,
        tP0,
        tP1,
        tP2,
        uSegments, tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_bezier_cubic(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonP2 = NULL;
    PyObject* ptPythonP3 = NULL;
    PyObject* ptPythonOptions = NULL;

    uint32_t uSegments = 0;

    static const char* apcKeywords[] = {
        "layer",
        "p0",
        "p1",
        "p2",
        "p3",
        "segments",
        "options",
        NULL,
    };

	if (!pl_parse("OOOOOIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &ptPythonP2, &ptPythonP3,
        &uSegments, &ptPythonOptions))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};
    plVec2 tP1 = {0};
    plVec2 tP2 = {0};
    plVec2 tP3 = {0};

    pl_vec2_from_py(ptPythonP0, &tP0);
    pl_vec2_from_py(ptPythonP1, &tP1);
    pl_vec2_from_py(ptPythonP2, &tP2);
    pl_vec2_from_py(ptPythonP3, &tP3);

    gptDraw->add_bezier_cubic(ptLayer,
        tP0,
        tP1,
        tP2,
        tP3,
        uSegments, tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_polygon(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonPoints = NULL;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "points",
        "options",
        NULL,
    };

	if (!pl_parse("OOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &ptPythonPoints, &ptPythonOptions))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    if (PyTuple_Check(ptPythonPoints))
    {
        Py_ssize_t pySize = PyTuple_Size(ptPythonPoints);
        pl_sb_resize(gsbtTempVec2Vec, (uint32_t)pySize);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            PyObject* ptPythonPoint = PyTuple_GetItem(ptPythonPoints, i);
            pl_vec2_from_py(ptPythonPoint, &gsbtTempVec2Vec[i]);
        }
    }

    else if (PyList_Check(ptPythonPoints))
    {
        Py_ssize_t pySize = PyList_Size(ptPythonPoints);
        pl_sb_resize(gsbtTempVec2Vec, (uint32_t)pySize);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            PyObject* ptPythonPoint = PyList_GetItem(ptPythonPoints, i);
            pl_vec2_from_py(ptPythonPoint, &gsbtTempVec2Vec[i]);
        }
    }

    gptDraw->add_polygon(ptLayer,
        gsbtTempVec2Vec, pl_sb_size(gsbtTempVec2Vec), tOptions);

    pl_sb_reset(gsbtTempVec2Vec);

    Py_RETURN_NONE;
}