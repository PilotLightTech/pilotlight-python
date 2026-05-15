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

typedef struct _pyplDrawI
{
    PyObject_HEAD
} pyplDrawI;

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
pl__get_draw_text_options(PyObject* ptPythonOptions, plDrawTextOptions* ptOptionsOut)
{
    PyObject* ptPythonOptionColor = PyObject_GetAttrString(ptPythonOptions, "uColor");
    PyLong_AsUInt32(ptPythonOptionColor, &ptOptionsOut->uColor);
    Py_DECREF(ptPythonOptionColor);

    PyObject* ptPythonOptionSize = PyObject_GetAttrString(ptPythonOptions, "fSize");
    ptOptionsOut->fSize = (float)PyFloat_AsDouble(ptPythonOptionSize);
    Py_DECREF(ptPythonOptionSize);

    PyObject* ptPythonOptionWrap = PyObject_GetAttrString(ptPythonOptions, "fWrap");
    ptOptionsOut->fWrap = (float)PyFloat_AsDouble(ptPythonOptionWrap);
    Py_DECREF(ptPythonOptionWrap);

    ptOptionsOut->ptFont = NULL;
    PyObject* ptPythonOptionFont = PyObject_GetAttrString(ptPythonOptions, "ptFont");
    ptOptionsOut->ptFont = PyCapsule_GetPointer(ptPythonOptionFont, "plFont");
    Py_DECREF(ptPythonOptionFont);
}

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
draw_new_frame(PyObject* self)
{
    gptDraw->new_frame();
    Py_RETURN_NONE;
}

PyObject*
draw_initialize(PyObject* self, PyObject* arg)
{
    PyObject* ptPythonDevice = PyObject_GetAttrString(arg, "ptDevice");
    plDevice* ptDevice = PyCapsule_GetPointer(ptPythonDevice, "plDevice");
    Py_DECREF(ptPythonDevice);

    plDrawInit tInit = {
        .ptDevice = ptDevice
    };
    gptDraw->initialize(&tInit);

    Py_RETURN_NONE;
}

PyObject*
draw_cleanup(PyObject* self)
{
    gptDraw->cleanup();
    Py_RETURN_NONE;
}

PyObject*
draw_submit_2d_drawlist(PyObject* self, PyObject* args)
{
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonEncoder = NULL;
    float fWidth = 0.0f;
    float fHeight = 0.0f;
    int iSampleCount = 1;
    PyObject* ptPythonFontAtlas = NULL;

    static const char* apcKeywords[] = {
        "drawlist",
        "encoder",
        "width",
        "height",
        "sampleCount",
        NULL,
    };

	if (!pl_parse("OOffi", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonEncoder, &fWidth, &fHeight, &iSampleCount))
		return NULL;

    plDrawList2D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList2D");
    plRenderEncoder* ptEncoder = PyCapsule_GetPointer(ptPythonEncoder, "plRenderEncoder");
    gptDraw->submit_2d_drawlist(ptDrawlist, ptEncoder, fWidth, fHeight, (uint32_t)iSampleCount);
    Py_RETURN_NONE;
}

PyObject*
draw_create_font_atlas(PyObject* self)
{
    return PyCapsule_New(gptDraw->create_font_atlas(), "plFontAtlas", NULL);
}

PyObject*
draw_build_font_atlas(PyObject* self, PyObject* args)
{
    PyObject* ptPythonCommandBuffer = NULL;
    PyObject* ptPythonFontAtlas = NULL;

    static const char* apcKeywords[] = {
        "commandBuffer",
        "atlas",
        NULL,
    };

	if (!pl_parse("OO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonCommandBuffer, &ptPythonFontAtlas))
		return NULL;

    plCommandBuffer* ptCommandBuffer = PyCapsule_GetPointer(ptPythonCommandBuffer, "plCommandBuffer");
    plFontAtlas* ptAtlas = PyCapsule_GetPointer(ptPythonFontAtlas, "plFontAtlas");
    bool bResult = gptDraw->build_font_atlas(ptCommandBuffer, ptAtlas);
    return PyBool_FromLong(bResult);
}

PyObject*
draw_cleanup_font_atlas(PyObject* self, PyObject* arg)
{
    plFontAtlas* ptAtlas = PyCapsule_GetPointer(arg, "plFontAtlas");
    gptDraw->cleanup_font_atlas(ptAtlas);
    Py_RETURN_NONE;
}

PyObject*
draw_set_font_atlas(PyObject* self, PyObject* arg)
{
    plFontAtlas* ptAtlas = PyCapsule_GetPointer(arg, "plFontAtlas");
    gptDraw->set_font_atlas(ptAtlas);
    Py_RETURN_NONE;
}

PyObject*
draw_get_current_font_atlas(PyObject* self)
{
    return PyCapsule_New(gptDraw->get_current_font_atlas(), "plFontAtlas", NULL);
}

PyObject*
draw_get_first_font(PyObject* self, PyObject* arg)
{
    plFontAtlas* ptAtlas = PyCapsule_GetPointer(arg, "plFontAtlas");
    return PyCapsule_New(gptDraw->get_first_font(ptAtlas), "plFont", NULL);
}

PyObject*
draw_add_default_font(PyObject* self, PyObject* arg)
{
    plFontAtlas* ptAtlas = PyCapsule_GetPointer(arg, "plFontAtlas");
    return PyCapsule_New(gptDraw->add_default_font(ptAtlas), "plFont", NULL);
}

PyObject*
draw_add_font_from_file_ttf(PyObject* self, PyObject* args)
{
    PyObject* ptPythonConfig = NULL;
    PyObject* ptPythonFontAtlas = NULL;
    const char* pcFile = NULL;

    static const char* apcKeywords[] = {
        "atlas",
        "config",
        "file",
        NULL,
    };

	if (!pl_parse("OOs", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonFontAtlas, &ptPythonConfig, &pcFile))
		return NULL;

    plFontAtlas* ptAtlas = PyCapsule_GetPointer(ptPythonFontAtlas, "plFontAtlas");

    plFontConfig tConfig = {0};

    PyObject* ptPythonConfigSize = PyObject_GetAttrString(ptPythonConfig, "fSize");
    tConfig.fSize = (float)PyFloat_AsDouble(ptPythonConfigSize);
    Py_DECREF(ptPythonConfigSize);

    PyObject* ptPythonOptionV = PyObject_GetAttrString(ptPythonConfig, "uVOverSampling");
    PyLong_AsUInt32(ptPythonOptionV, &tConfig.uVOverSampling);
    Py_DECREF(ptPythonOptionV);

    PyObject* ptPythonOptionH = PyObject_GetAttrString(ptPythonConfig, "uHOverSampling");
    PyLong_AsUInt32(ptPythonOptionH, &tConfig.uHOverSampling);
    Py_DECREF(ptPythonOptionH);

    PyObject* ptPythonOptionMergeFont = PyObject_GetAttrString(ptPythonConfig, "ptMergeFont");
    if(!Py_IsNone(ptPythonOptionMergeFont))
    {
        tConfig.ptMergeFont = PyCapsule_GetPointer(ptPythonConfig, "plFont");
        Py_DECREF(ptPythonOptionMergeFont);
    }
    
    PyObject* ptPythonRanges = PyObject_GetAttrString(ptPythonConfig, "ptRanges");
    plFontRange* sbtRanges = NULL;
    if (PyList_Check(ptPythonRanges))
    {
        Py_ssize_t pySize = PyList_Size(ptPythonRanges);
        pl_sb_resize(sbtRanges, (uint32_t)pySize);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            PyObject* ptFontRange = PyList_GetItem(ptPythonRanges, i);

            plFontRange tRange = {0};

            PyObject* ptPythonIFirstCodePoint = PyObject_GetAttrString(ptFontRange, "iFirstCodePoint");
            PyLong_AsInt32(ptPythonIFirstCodePoint, &tRange.iFirstCodePoint);
            Py_DECREF(ptPythonIFirstCodePoint);

            PyObject* ptPythonCharCount = PyObject_GetAttrString(ptFontRange, "uCharCount");
            PyLong_AsUInt32(ptPythonCharCount, &tRange.uCharCount);
            Py_DECREF(ptPythonCharCount);

            sbtRanges[i] = tRange;

        }
    }
    Py_DECREF(ptPythonRanges);

    PyObject* ptPythonIndividualChars = PyObject_GetAttrString(ptPythonConfig, "piIndividualChars");
    int* sbiChars = NULL;
    if (PyList_Check(ptPythonIndividualChars))
    {
        Py_ssize_t pySize = PyList_Size(ptPythonIndividualChars);
        pl_sb_resize(sbiChars, (uint32_t)pySize);
        for (Py_ssize_t i = 0; i < pySize; ++i)
        {
            PyObject* ptFontRange = PyList_GetItem(ptPythonIndividualChars, i);
            PyLong_AsInt32(ptFontRange, &sbiChars[i]);
        }
    }
    Py_DECREF(ptPythonIndividualChars);

    tConfig.uRangeCount = pl_sb_size(sbtRanges);
    tConfig.uIndividualCharCount = pl_sb_size(sbiChars);
    tConfig.ptRanges = sbtRanges;
    tConfig.piIndividualChars = sbiChars;
    plFont* ptFont = gptDraw->add_font_from_file_ttf(ptAtlas, tConfig, pcFile);
    pl_sb_free(sbtRanges);
    pl_sb_free(sbiChars);
    return PyCapsule_New(ptFont, "plFont", NULL);
}

PyObject*
draw_calculate_text_size(PyObject* self, PyObject* args)
{
 
    const char* pcText = NULL;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "text",
        "options",
        NULL,
    };

	if (!pl_parse("OOsO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &pcText, &ptPythonOptions))
		return NULL;

    plDrawTextOptions tOptions = {0};
    pl__get_draw_text_options(ptPythonOptions, &tOptions);

    plVec2 tSize = gptDraw->calculate_text_size(pcText, tOptions);
    return pl_vec2_to_py(tSize);
}

PyObject*
draw_request_2d_drawlist(PyObject* self)
{
    return PyCapsule_New(gptDraw->request_2d_drawlist(), "plDrawList2D", NULL);
}

PyObject*
draw_return_2d_drawlist(PyObject* self, PyObject* arg)
{
    plDrawList2D* ptDrawList = PyCapsule_GetPointer(arg, "plDrawList2D");
    gptDraw->return_2d_drawlist(ptDrawList);
    Py_RETURN_NONE;
}

PyObject*
draw_request_2d_layer(PyObject* self, PyObject* arg)
{
    plDrawList2D* ptDrawList = PyCapsule_GetPointer(arg, "plDrawList2D");
    return PyCapsule_New(gptDraw->request_2d_layer(ptDrawList), "plDrawLayer2D", NULL);
}

PyObject*
draw_return_2d_layer(PyObject* self, PyObject* arg)
{
    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(arg, "plDrawLayer2D");
    gptDraw->return_2d_layer(ptLayer);
    Py_RETURN_NONE;
}

PyObject*
draw_submit_2d_layer(PyObject* self, PyObject* arg)
{
    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(arg, "plDrawLayer2D");
    gptDraw->submit_2d_layer(ptLayer);
    Py_RETURN_NONE;
}

PyObject*
draw_add_text(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    const char* pcText = NULL;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "p",
        "text",
        "options",
        NULL,
    };

	if (!pl_parse("OOsO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &pcText, &ptPythonOptions))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    plDrawTextOptions tOptions = {0};
    pl__get_draw_text_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};
    plVec2 tP1 = {0};
    plVec2 tP2 = {0};

    pl_vec2_from_py(ptPythonP0, &tP0);

    gptDraw->add_text(ptLayer, tP0, pcText, tOptions);
    Py_RETURN_NONE;
}

PyObject*
draw_add_text_clipped(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    const char* pcText = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonP2 = NULL;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "p",
        "text",
        "clipMin",
        "clipMax",
        "options",
        NULL,
    };

	if (!pl_parse("OOsO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &pcText, &ptPythonP1, &ptPythonP2, &ptPythonOptions))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    plDrawTextOptions tOptions = {0};
    pl__get_draw_text_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};
    plVec2 tP1 = {0};
    plVec2 tP2 = {0};

    pl_vec2_from_py(ptPythonP0, &tP0);
    pl_vec2_from_py(ptPythonP0, &tP1);
    pl_vec2_from_py(ptPythonP0, &tP2);

    gptDraw->add_text_clipped(ptLayer, tP0, pcText, tP1, tP2, tOptions);
    Py_RETURN_NONE;
}

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
draw_add_triangles_filled(PyObject* self, PyObject* args)
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
    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    uint32_t uPointCount = 0;
    plVec2* atList = pl_get_vec2_list_from_python(ptPythonPoints, &uPointCount);

    gptDraw->add_triangles_filled(ptLayer,
        atList,
        uPointCount,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_rect_filled(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "minP",
        "maxP",
        "options",
        NULL,
    };

	if (!pl_parse("OOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &ptPythonP1, &ptPythonOptions))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");
    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};
    plVec2 tP1 = {0};

    pl_vec2_from_py(ptPythonP0, &tP0);
    pl_vec2_from_py(ptPythonP1, &tP1);

    gptDraw->add_rect_filled(ptLayer,
        tP0,
        tP1,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_rect_rounded_filled(PyObject* self, PyObject* args)
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
    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};
    plVec2 tP1 = {0};

    pl_vec2_from_py(ptPythonP0, &tP0);
    pl_vec2_from_py(ptPythonP1, &tP1);

    gptDraw->add_rect_rounded_filled(ptLayer, tP0,tP1, fRadius, uSegments, iFlags, tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_quad_filled(PyObject* self, PyObject* args)
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

    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};
    plVec2 tP1 = {0};
    plVec2 tP2 = {0};
    plVec2 tP3 = {0};

    pl_vec2_from_py(ptPythonP0, &tP0);
    pl_vec2_from_py(ptPythonP1, &tP1);
    pl_vec2_from_py(ptPythonP2, &tP2);
    pl_vec2_from_py(ptPythonP3, &tP3);

    gptDraw->add_quad_filled(ptLayer,
        tP0,
        tP1,
        tP2,
        tP3,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_circle_filled(PyObject* self, PyObject* args)
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

    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};

    pl_vec2_from_py(ptPythonP0, &tP0);

    gptDraw->add_circle_filled(ptLayer,
        tP0,
        fRadius, uSegments, tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_convex_polygon_filled(PyObject* self, PyObject* args)
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
    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    uint32_t uPointCount = 0;
    plVec2* atList = pl_get_vec2_list_from_python(ptPythonPoints, &uPointCount);

    gptDraw->add_convex_polygon_filled(ptLayer,
        atList,
        uPointCount,
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
draw_add_lines(PyObject* self, PyObject* args)
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

    uint32_t uPointCount = 0;
    plVec2* atList = pl_get_vec2_list_from_python(ptPythonPoints, &uPointCount);

    gptDraw->add_lines(ptLayer,
        atList,
        uPointCount,
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

static PyMethodDef gatplDrawICommands[] =
{
    {"new_frame", (PyCFunction)draw_new_frame, METH_NOARGS | METH_STATIC, NULL},
    {"cleanup", (PyCFunction)draw_cleanup, METH_NOARGS | METH_STATIC, NULL},
    {"initialize", (PyCFunction)draw_initialize, METH_O | METH_STATIC, NULL},
    {"create_font_atlas", (PyCFunction)draw_create_font_atlas, METH_NOARGS | METH_STATIC, NULL},
    {"build_font_atlas", (PyCFunction)draw_build_font_atlas, METH_VARARGS | METH_STATIC, NULL},
    {"get_current_font_atlas", (PyCFunction)draw_get_current_font_atlas, METH_NOARGS | METH_STATIC, NULL},
    {"get_first_font", (PyCFunction)draw_get_first_font, METH_O | METH_STATIC, NULL},
    {"add_default_font", (PyCFunction)draw_add_default_font, METH_O | METH_STATIC, NULL},
    {"add_font_from_file_ttf", (PyCFunction)draw_add_font_from_file_ttf, METH_VARARGS | METH_STATIC, NULL},
    {"cleanup_font_atlas", (PyCFunction)draw_cleanup_font_atlas, METH_O | METH_STATIC, NULL},
    {"set_font_atlas", (PyCFunction)draw_set_font_atlas, METH_O | METH_STATIC, NULL},
    {"calculate_text_size", (PyCFunction)draw_calculate_text_size, METH_VARARGS | METH_STATIC, NULL},
    {"request_2d_drawlist", (PyCFunction)draw_request_2d_drawlist, METH_NOARGS | METH_STATIC, NULL},
    {"return_2d_drawlist", (PyCFunction)draw_return_2d_drawlist, METH_O | METH_STATIC, NULL},
    {"submit_2d_drawlist", (PyCFunction)draw_submit_2d_drawlist, METH_VARARGS | METH_STATIC, NULL},
    {"request_2d_layer", (PyCFunction)draw_request_2d_layer, METH_O | METH_STATIC, NULL},
    {"return_2d_layer", (PyCFunction)draw_return_2d_layer, METH_O | METH_STATIC, NULL},
    {"submit_2d_layer", (PyCFunction)draw_submit_2d_layer, METH_O | METH_STATIC, NULL},
    {"add_text", (PyCFunction)draw_add_text, METH_VARARGS | METH_STATIC, NULL},
    {"add_text_clipped", (PyCFunction)draw_add_text_clipped, METH_VARARGS | METH_STATIC, NULL},
    {"add_triangle", (PyCFunction)draw_add_triangle, METH_VARARGS | METH_STATIC, NULL},
    {"add_line", (PyCFunction)draw_add_line, METH_VARARGS | METH_STATIC, NULL},
    {"add_lines", (PyCFunction)draw_add_lines, METH_VARARGS | METH_STATIC, NULL},
    {"add_rect", (PyCFunction)draw_add_rect, METH_VARARGS | METH_STATIC, NULL},
    {"add_rect_rounded", (PyCFunction)draw_add_rect_rounded, METH_VARARGS | METH_STATIC, NULL},
    {"add_quad", (PyCFunction)draw_add_quad, METH_VARARGS | METH_STATIC, NULL},
    {"add_circle", (PyCFunction)draw_add_circle, METH_VARARGS | METH_STATIC, NULL},
    {"add_polygon", (PyCFunction)draw_add_polygon, METH_VARARGS | METH_STATIC, NULL},
    {"add_bezier_quad", (PyCFunction)draw_add_bezier_quad, METH_VARARGS | METH_STATIC, NULL},
    {"add_bezier_cubic", (PyCFunction)draw_add_bezier_cubic, METH_VARARGS | METH_STATIC, NULL},
    {"add_triangle_filled", (PyCFunction)draw_add_triangle_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_triangles_filled", (PyCFunction)draw_add_triangles_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_rect_filled", (PyCFunction)draw_add_rect_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_rect_rounded_filled", (PyCFunction)draw_add_rect_rounded_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_quad_filled", (PyCFunction)draw_add_quad_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_circle_filled", (PyCFunction)draw_add_circle_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_convex_polygon_filled", (PyCFunction)draw_add_convex_polygon_filled, METH_VARARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot gatplDrawISlots[] = {
    {Py_tp_methods, (void*)gatplDrawICommands},
    {0, 0}
};

static PyType_Spec plDrawISpec = {
    "pilotlight.plDrawI",
    sizeof(pyplDrawI),
    0,
    Py_TPFLAGS_DEFAULT,
    gatplDrawISlots
};