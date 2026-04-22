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

//-----------------------------------------------------------------------------
// [SECTION] implementations
//-----------------------------------------------------------------------------

PyObject*
draw_add_triangle_filled(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonP2 = NULL;
    plDrawSolidOptions tOptions = {
        .uColor = PL_COLOR_32_WHITE
    };

    static const char* apcKeywords[] = {
        "layer",
        "p0",
        "p1",
        "p2",
        "color",
        NULL,
    };

	if (!pl_parse("OOOO|$I", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &ptPythonP2, &tOptions.uColor))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    gptDraw->add_triangle_filled(ptLayer,
        pl_get_vec2_from_python(ptPythonP0),
        pl_get_vec2_from_python(ptPythonP1),
        pl_get_vec2_from_python(ptPythonP2),
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_line(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;

    plDrawLineOptions tOptions = {
        .uColor = PL_COLOR_32_WHITE,
        .fThickness = 1.0f
    };

    static const char* apcKeywords[] = {
        "layer",
        "p0",
        "p1",
        "color",
        "thickness",
        NULL,
    };

	if (!pl_parse("OOO|$If", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &tOptions.uColor, &tOptions.fThickness))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    gptDraw->add_line(ptLayer,
        pl_get_vec2_from_python(ptPythonP0),
        pl_get_vec2_from_python(ptPythonP1),
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_triangle(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonP2 = NULL;

    plDrawLineOptions tOptions = {
        .uColor = PL_COLOR_32_WHITE,
        .fThickness = 1.0f
    };

    static const char* apcKeywords[] = {
        "layer",
        "p0",
        "p1",
        "p2",
        "color",
        "thickness",
        NULL,
    };

	if (!pl_parse("OOOO|$If", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &ptPythonP2, &tOptions.uColor, &tOptions.fThickness))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    gptDraw->add_triangle(ptLayer,
        pl_get_vec2_from_python(ptPythonP0),
        pl_get_vec2_from_python(ptPythonP1),
        pl_get_vec2_from_python(ptPythonP2),
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_rect(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;

    plDrawLineOptions tOptions = {
        .uColor = PL_COLOR_32_WHITE,
        .fThickness = 1.0f
    };

    static const char* apcKeywords[] = {
        "layer",
        "pMin",
        "pMax",
        "color",
        "thickness",
        NULL,
    };

	if (!pl_parse("OOO|$If", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &tOptions.uColor, &tOptions.fThickness))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    gptDraw->add_rect(ptLayer,
        pl_get_vec2_from_python(ptPythonP0),
        pl_get_vec2_from_python(ptPythonP1),
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_rect_rounded(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;

    plDrawLineOptions tOptions = {
        .uColor = PL_COLOR_32_WHITE,
        .fThickness = 1.0f
    };

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
        "color",
        "thickness",
        NULL,
    };

	if (!pl_parse("OOOfIi|$If", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &fRadius, &uSegments, &iFlags, 
        &tOptions.uColor, &tOptions.fThickness))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    gptDraw->add_rect_rounded(ptLayer,
        pl_get_vec2_from_python(ptPythonP0),
        pl_get_vec2_from_python(ptPythonP1),
        fRadius, uSegments, iFlags, tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_quad(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonP2 = NULL;
    PyObject* ptPythonP3 = NULL;

    plDrawLineOptions tOptions = {
        .uColor = PL_COLOR_32_WHITE,
        .fThickness = 1.0f
    };

    static const char* apcKeywords[] = {
        "layer",
        "p0",
        "p1",
        "p2",
        "p3",
        "color",
        "thickness",
        NULL,
    };

	if (!pl_parse("OOOOO|$If", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &ptPythonP2, &ptPythonP3,
        &tOptions.uColor, &tOptions.fThickness))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    gptDraw->add_quad(ptLayer,
        pl_get_vec2_from_python(ptPythonP0),
        pl_get_vec2_from_python(ptPythonP1),
        pl_get_vec2_from_python(ptPythonP2),
        pl_get_vec2_from_python(ptPythonP3),
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_circle(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;

    plDrawLineOptions tOptions = {
        .uColor = PL_COLOR_32_WHITE,
        .fThickness = 1.0f
    };

    float fRadius = 1.0f;
    uint32_t uSegments = 0;

    static const char* apcKeywords[] = {
        "layer",
        "p",
        "radius",
        "segments",
        "color",
        "thickness",
        NULL,
    };

	if (!pl_parse("OOfI|$If", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &fRadius, &uSegments, 
        &tOptions.uColor, &tOptions.fThickness))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    gptDraw->add_circle(ptLayer,
        pl_get_vec2_from_python(ptPythonP0),
        fRadius, uSegments, tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_bezier_quad(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonP2 = NULL;

    plDrawLineOptions tOptions = {
        .uColor = PL_COLOR_32_WHITE,
        .fThickness = 1.0f
    };

    uint32_t uSegments = 0;

    static const char* apcKeywords[] = {
        "layer",
        "p0",
        "p1",
        "p2",
        "segments",
        "color",
        "thickness",
        NULL,
    };

	if (!pl_parse("OOOOI|$If", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &ptPythonP2, &uSegments, 
        &tOptions.uColor, &tOptions.fThickness))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    gptDraw->add_bezier_quad(ptLayer,
        pl_get_vec2_from_python(ptPythonP0),
        pl_get_vec2_from_python(ptPythonP1),
        pl_get_vec2_from_python(ptPythonP2),
        uSegments, tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_bezier_cubic(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonP2 = NULL;
    PyObject* ptPythonP3 = NULL;

    plDrawLineOptions tOptions = {
        .uColor = PL_COLOR_32_WHITE,
        .fThickness = 1.0f
    };

    uint32_t uSegments = 0;

    static const char* apcKeywords[] = {
        "layer",
        "p0",
        "p1",
        "p2",
        "p3",
        "segments",
        "color",
        "thickness",
        NULL,
    };

	if (!pl_parse("OOOOOI|$If", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &ptPythonP2, &ptPythonP3,
        &uSegments, &tOptions.uColor, &tOptions.fThickness))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    gptDraw->add_bezier_cubic(ptLayer,
        pl_get_vec2_from_python(ptPythonP0),
        pl_get_vec2_from_python(ptPythonP1),
        pl_get_vec2_from_python(ptPythonP2),
        pl_get_vec2_from_python(ptPythonP3),
        uSegments, tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_polygon(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonPoints = NULL;

    plDrawLineOptions tOptions = {
        .uColor = PL_COLOR_32_WHITE,
        .fThickness = 1.0f
    };

    static const char* apcKeywords[] = {
        "layer",
        "points",
        "color",
        "thickness",
        NULL,
    };

	if (!pl_parse("OO|$If", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPythonLayer, &ptPythonPoints, &tOptions.uColor, &tOptions.fThickness))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    if (PyTuple_Check(ptPythonPoints))
    {
        Py_ssize_t pySize = PyTuple_Size(ptPythonPoints);
        pl_sb_resize(gsbtTempVec2Vec, (uint32_t)pySize);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            PyObject* ptPythonPoint = PyTuple_GetItem(ptPythonPoints, i);
            gsbtTempVec2Vec[i] = pl_get_vec2_from_python(ptPythonPoint);
        }
    }

    else if (PyList_Check(ptPythonPoints))
    {
        Py_ssize_t pySize = PyList_Size(ptPythonPoints);
        pl_sb_resize(gsbtTempVec2Vec, (uint32_t)pySize);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            PyObject* ptPythonPoint = PyList_GetItem(ptPythonPoints, i);
            gsbtTempVec2Vec[i] = pl_get_vec2_from_python(ptPythonPoint);
        }
    }

    gptDraw->add_polygon(ptLayer,
        gsbtTempVec2Vec, pl_sb_size(gsbtTempVec2Vec), tOptions);

    pl_sb_reset(gsbtTempVec2Vec);

    Py_RETURN_NONE;
}