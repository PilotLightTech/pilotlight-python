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
#include "pl_py_types.h"

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

static inline void
pl__get_sphere_options(PyObject* ptPythonOptions, plSphere* ptOut)
{
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonOptions, "fRadius");
        ptOut->fRadius = (float)PyFloat_AsDouble(ptPythonObject);
        Py_DECREF(ptPythonObject);
    }
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonOptions, "tCenter");
        pl_vec3_from_py(ptPythonObject, &ptOut->tCenter);
        Py_DECREF(ptPythonObject);
    }
}

static inline void
pl__get_capsule_options(PyObject* ptPythonOptions, plCapsule* ptOut)
{
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonOptions, "fRadius");
        ptOut->fRadius = (float)PyFloat_AsDouble(ptPythonObject);
        Py_DECREF(ptPythonObject);
    }
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonOptions, "tBasePos");
        pl_vec3_from_py(ptPythonObject, &ptOut->tBasePos);
        Py_DECREF(ptPythonObject);
    }
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonOptions, "tTipPos");
        pl_vec3_from_py(ptPythonObject, &ptOut->tTipPos);
        Py_DECREF(ptPythonObject);
    }
}

static inline void
pl__get_cone_options(PyObject* ptPythonOptions, plCone* ptOut)
{
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonOptions, "fRadius");
        ptOut->fRadius = (float)PyFloat_AsDouble(ptPythonObject);
        Py_DECREF(ptPythonObject);
    }
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonOptions, "tBasePos");
        pl_vec3_from_py(ptPythonObject, &ptOut->tBasePos);
        Py_DECREF(ptPythonObject);
    }
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonOptions, "tTipPos");
        pl_vec3_from_py(ptPythonObject, &ptOut->tTipPos);
        Py_DECREF(ptPythonObject);
    }
}

static inline void
pl__get_cylinder_options(PyObject* ptPythonOptions, plCylinder* ptOut)
{
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonOptions, "fRadius");
        ptOut->fRadius = (float)PyFloat_AsDouble(ptPythonObject);
        Py_DECREF(ptPythonObject);
    }
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonOptions, "tBasePos");
        pl_vec3_from_py(ptPythonObject, &ptOut->tBasePos);
        Py_DECREF(ptPythonObject);
    }
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonOptions, "tTipPos");
        pl_vec3_from_py(ptPythonObject, &ptOut->tTipPos);
        Py_DECREF(ptPythonObject);
    }
}

static inline void
pl__get_frustum_options(PyObject* ptPythonOptions, plDrawFrustumDesc* ptOut)
{
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonOptions, "fYFov");
        ptOut->fYFov = (float)PyFloat_AsDouble(ptPythonObject);
        Py_DECREF(ptPythonObject);
    }
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonOptions, "fAspectRatio");
        ptOut->fAspectRatio = (float)PyFloat_AsDouble(ptPythonObject);
        Py_DECREF(ptPythonObject);
    }
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonOptions, "fNearZ");
        ptOut->fNearZ = (float)PyFloat_AsDouble(ptPythonObject);
        Py_DECREF(ptPythonObject);
    }
    {
        PyObject* ptPythonObject = PyObject_GetAttrString(ptPythonOptions, "fFarZ");
        ptOut->fFarZ = (float)PyFloat_AsDouble(ptPythonObject);
        Py_DECREF(ptPythonObject);
    }
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

    static const char* apcKeywords[] = {
        "drawlist",
        "encoder",
        "width",
        "height",
        "sampleCount",
        NULL,
    };

	if (!pl_parse_args("OOffi", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonEncoder, &fWidth, &fHeight, &iSampleCount))
		return NULL;

    plDrawList2D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList2D");
    plRenderEncoder* ptEncoder = PyCapsule_GetPointer(ptPythonEncoder, "plRenderEncoder");
    gptDraw->submit_2d_drawlist(ptDrawlist, ptEncoder, fWidth, fHeight, (uint32_t)iSampleCount);
    Py_RETURN_NONE;
}

PyObject*
draw_submit_3d_drawlist(PyObject* self, PyObject* args)
{
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonEncoder = NULL;
    float fWidth = 0.0f;
    float fHeight = 0.0f;
    PyObject* ptPythonMVP = NULL;
    int iFlags = 0;
    int iSampleCount = 1;

    static const char* apcKeywords[] = {
        "drawlist",
        "encoder",
        "width",
        "height",
        "mvp",
        "flags",
        "sampleCount",
        NULL,
    };

	if (!pl_parse_args("OOffOii", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonEncoder, &fWidth, &fHeight, &ptPythonMVP, &iFlags, &iSampleCount))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");
    plRenderEncoder* ptEncoder = PyCapsule_GetPointer(ptPythonEncoder, "plRenderEncoder");
    pyplMat4* m = (pyplMat4*)ptPythonMVP;
    gptDraw->submit_3d_drawlist(ptDrawlist, ptEncoder, fWidth, fHeight, &m->m, iFlags, (uint32_t)iSampleCount);
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

	if (!pl_parse_args("OO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOs", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOsO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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
draw_request_3d_drawlist(PyObject* self)
{
    return PyCapsule_New(gptDraw->request_3d_drawlist(), "plDrawList3D", NULL);
}

PyObject*
draw_return_3d_drawlist(PyObject* self, PyObject* arg)
{
    plDrawList3D* ptDrawList = PyCapsule_GetPointer(arg, "plDrawList3D");
    gptDraw->return_3d_drawlist(ptDrawList);
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

	if (!pl_parse_args("OOsO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &ptPythonP0, &pcText, &ptPythonOptions))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");

    plDrawTextOptions tOptions = {0};
    pl__get_draw_text_options(ptPythonOptions, &tOptions);

    plVec2 tP0 = {0};

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

	if (!pl_parse_args("OOsO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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
draw_use_linear_sampler(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;

    static const char* apcKeywords[] = {
        "layer",
        NULL,
    };

	if (!pl_parse_args("O", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");
    gptDraw->use_linear_sampler(ptLayer);
    Py_RETURN_NONE;
}

PyObject*
draw_use_nearest_sampler(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;

    static const char* apcKeywords[] = {
        "layer",
        NULL,
    };

	if (!pl_parse_args("O", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");
    gptDraw->use_nearest_sampler(ptLayer);
    Py_RETURN_NONE;
}

PyObject*
draw_add_image(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;
    uint32_t uTextureId = 0;
    PyObject* ptPythonminP = NULL;
    PyObject* ptPythonmaxP = NULL;
    PyObject* ptPythonminUV = NULL;
    PyObject* ptPythonmaxUV = NULL;
    uint32_t uColor = PL_COLOR_32_WHITE;

    static const char* apcKeywords[] = {
        "layer",
        "textureID",
        "minP",
        "maxP",
        "minUV",
        "maxUV",
        "color",
        NULL,
    };

	if (!pl_parse_args("OIOO|OOI", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &uTextureId, &ptPythonminP, &ptPythonmaxP, &ptPythonminUV, &ptPythonmaxUV, &uColor))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");
    plVec2 tMinP = {0};
    plVec2 tMaxP = {0};
    plVec2 tMinUV = {0};
    plVec2 tMaxUV = {1.0f, 1.0f};

    pl_vec2_from_py(ptPythonminP, &tMinP);
    pl_vec2_from_py(ptPythonmaxP, &tMaxP);
    if(ptPythonminUV)
        pl_vec2_from_py(ptPythonminUV, &tMinUV);
    if(ptPythonmaxUV)
        pl_vec2_from_py(ptPythonmaxUV, &tMaxUV);

    gptDraw->add_image_ex(ptLayer,
        uTextureId,
        tMinP,
        tMaxP,
        tMinUV,
        tMaxUV,
        uColor);

    Py_RETURN_NONE;
}

PyObject*
draw_add_image_quad(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonLayer = NULL;
    uint32_t uTextureId = 0;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonP2 = NULL;
    PyObject* ptPythonP3 = NULL;
    PyObject* ptPythonP0UV = NULL;
    PyObject* ptPythonP1UV = NULL;
    PyObject* ptPythonP2UV = NULL;
    PyObject* ptPythonP3UV = NULL;
    uint32_t uColor = PL_COLOR_32_WHITE;

    static const char* apcKeywords[] = {
        "layer",
        "textureID",
        "p0",
        "p1",
        "p2",
        "p3",
        "p0UV",
        "p1UV",
        "p2UV",
        "p3UV",
        "color",
        NULL,
    };

	if (!pl_parse_args("OIOOOO|OOOOI", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonLayer, &uTextureId, &ptPythonP0, &ptPythonP1, &ptPythonP2, &ptPythonP3,
        &ptPythonP0UV, &ptPythonP1UV, &ptPythonP2UV, &ptPythonP3UV,
        &uColor))
		return NULL;

    plDrawLayer2D* ptLayer = PyCapsule_GetPointer(ptPythonLayer, "plDrawLayer2D");
    plVec2 tP0 = {0};
    plVec2 tP1 = {0};
    plVec2 tP2 = {0};
    plVec2 tP3 = {0};
    plVec2 tP0UV = {0};
    plVec2 tP1UV = {0};
    plVec2 tP2UV = {0};
    plVec2 tP3UV = {0};
    
    pl_vec2_from_py(ptPythonP0, &tP0);
    pl_vec2_from_py(ptPythonP1, &tP1);
    pl_vec2_from_py(ptPythonP2, &tP2);
    pl_vec2_from_py(ptPythonP3, &tP3);
    if(ptPythonP0UV) pl_vec2_from_py(ptPythonP0UV, &tP0UV);
    if(ptPythonP1UV) pl_vec2_from_py(ptPythonP1UV, &tP1UV);
    if(ptPythonP2UV) pl_vec2_from_py(ptPythonP2UV, &tP2UV);
    if(ptPythonP3UV) pl_vec2_from_py(ptPythonP3UV, &tP3UV);

    gptDraw->add_image_quad_ex(ptLayer,
        uTextureId,
        tP0,
        tP1,
        tP2,
        tP3,
        tP0UV,
        tP1UV,
        tP2UV,
        tP3UV,
        uColor);

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

	if (!pl_parse_args("OOOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOOfIiO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOOOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOfIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOOfIiO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOOOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOfIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOOOIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOOOOIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

	if (!pl_parse_args("OOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

PyObject*
draw_add_3d_transform(PyObject* self, PyObject* args)
{
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonMVP = NULL;
    float fLength = 0.0f;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "drawlist",
        "transform",
        "length",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOfO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonMVP, &fLength, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");
    pyplMat4* m = (pyplMat4*)ptPythonMVP;

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);


    gptDraw->add_3d_transform(ptDrawlist, &m->m, fLength, tOptions);
    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_line(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
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

	if (!pl_parse_args("OOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonP0, &ptPythonP1, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plVec3 tP0 = {0};
    plVec3 tP1 = {0};

    pl_vec3_from_py(ptPythonP0, &tP0);
    pl_vec3_from_py(ptPythonP1, &tP1);

    gptDraw->add_3d_line(
        ptDrawlist,
        tP0,
        tP1,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_aabb(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
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

	if (!pl_parse_args("OOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonP0, &ptPythonP1, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plVec3 tP0 = {0};
    plVec3 tP1 = {0};

    pl_vec3_from_py(ptPythonP0, &tP0);
    pl_vec3_from_py(ptPythonP1, &tP1);

    gptDraw->add_3d_aabb(
        ptDrawlist,
        tP0,
        tP1,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_bezier_quad(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonP2 = NULL;
    PyObject* ptPythonOptions = NULL;
    uint32_t uSegments = 12;

    static const char* apcKeywords[] = {
        "layer",
        "p0",
        "p1",
        "p2",
        "segments",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOOOIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonP0, &ptPythonP1, &ptPythonP2, &uSegments, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plVec3 tP0 = {0};
    plVec3 tP1 = {0};
    plVec3 tP2 = {0};

    pl_vec3_from_py(ptPythonP0, &tP0);
    pl_vec3_from_py(ptPythonP1, &tP1);
    pl_vec3_from_py(ptPythonP2, &tP2);

    gptDraw->add_3d_bezier_quad(
        ptDrawlist,
        tP0,
        tP1,
        tP2,
        uSegments,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_bezier_cubic(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonP0 = NULL;
    PyObject* ptPythonP1 = NULL;
    PyObject* ptPythonP2 = NULL;
    PyObject* ptPythonP3 = NULL;
    PyObject* ptPythonOptions = NULL;
    uint32_t uSegments = 12;

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

	if (!pl_parse_args("OOOOOIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonP0, &ptPythonP1, &ptPythonP2, &ptPythonP3, &uSegments, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plVec3 tP0 = {0};
    plVec3 tP1 = {0};
    plVec3 tP2 = {0};
    plVec3 tP3 = {0};

    pl_vec3_from_py(ptPythonP0, &tP0);
    pl_vec3_from_py(ptPythonP1, &tP1);
    pl_vec3_from_py(ptPythonP2, &tP2);
    pl_vec3_from_py(ptPythonP3, &tP3);

    gptDraw->add_3d_bezier_cubic(
        ptDrawlist,
        tP0,
        tP1,
        tP2,
        tP3,
        uSegments,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_cross(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonP0 = NULL;
    float fLength = 1.0f;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "p0",
        "length",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOfO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonP0, &fLength, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plVec3 tP0 = {0};

    pl_vec3_from_py(ptPythonP0, &tP0);

    gptDraw->add_3d_cross(
        ptDrawlist,
        tP0,
        fLength,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_frustum(PyObject* self, PyObject* args)
{
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonMVP = NULL;
    PyObject* ptPythonDesc = NULL;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "drawlist",
        "mvp",
        "frustum",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonMVP, &ptPythonDesc, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");
    pyplMat4* m = (pyplMat4*)ptPythonMVP;

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plDrawFrustumDesc tDrawFrustumDesc = {0};
    pl__get_frustum_options(ptPythonDesc, &tDrawFrustumDesc);

    gptDraw->add_3d_frustum(ptDrawlist, &m->m, tDrawFrustumDesc, tOptions);
    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_sphere(PyObject* self, PyObject* args)
{
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonDesc = NULL;
    uint32_t uLatBands = 16;
    uint32_t uLongBands = 16;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "drawlist",
        "sphere",
        "latBands",
        "longBands",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOIIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonDesc, &uLatBands, &uLongBands, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plSphere tDesc = {0};
    pl__get_sphere_options(ptPythonDesc, &tDesc);

    gptDraw->add_3d_sphere(ptDrawlist, tDesc, uLatBands, uLongBands, tOptions);
    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_capsule(PyObject* self, PyObject* args)
{
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonDesc = NULL;
    uint32_t uLatBands = 16;
    uint32_t uLongBands = 16;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "drawlist",
        "capsule",
        "latBands",
        "longBands",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOIIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonDesc, &uLatBands, &uLongBands, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plCapsule tDesc = {0};
    pl__get_capsule_options(ptPythonDesc, &tDesc);

    gptDraw->add_3d_capsule(ptDrawlist, tDesc, uLatBands, uLongBands, tOptions);
    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_cylinder(PyObject* self, PyObject* args)
{
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonDesc = NULL;
    uint32_t uSegments = 16;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "drawlist",
        "capsule",
        "segments",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonDesc, &uSegments, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plCylinder tDesc = {0};
    pl__get_cylinder_options(ptPythonDesc, &tDesc);

    gptDraw->add_3d_cylinder(ptDrawlist, tDesc, uSegments, tOptions);
    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_cone(PyObject* self, PyObject* args)
{
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonDesc = NULL;
    uint32_t uSegments = 16;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "drawlist",
        "cone",
        "segments",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonDesc, &uSegments, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plCone tDesc = {0};
    pl__get_cone_options(ptPythonDesc, &tDesc);

    gptDraw->add_3d_cone(ptDrawlist, tDesc, uSegments, tOptions);
    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_centered_box(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonCenter = NULL;
    float fWidth = 0.0f;
    float fHeight = 0.0f;
    float fDepth = 0.0f;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "center",
        "width",
        "height",
        "depth",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOfffO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonCenter, &fWidth, &fHeight, &fDepth, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plVec3 tCenter = {0};
    pl_vec3_from_py(ptPythonCenter, &tCenter);

    gptDraw->add_3d_centered_box(
        ptDrawlist,
        tCenter,
        fWidth,
        fHeight,
        fDepth,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_circle_xz(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonCenter = NULL;
    float fRadius = 0.0f;
    uint32_t uSegments = 16;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "center",
        "radius",
        "segments",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOfIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonCenter, &fRadius, &uSegments, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawLineOptions tOptions = {0};
    pl__get_draw_line_options(ptPythonOptions, &tOptions);

    plVec3 tCenter = {0};
    pl_vec3_from_py(ptPythonCenter, &tCenter);

    gptDraw->add_3d_circle_xz(
        ptDrawlist,
        tCenter,
        fRadius,
        uSegments,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_circle_xz_filled(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonCenter = NULL;
    float fRadius = 0.0f;
    uint32_t uSegments = 16;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "center",
        "radius",
        "segments",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOfIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonCenter, &fRadius, &uSegments, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plVec3 tCenter = {0};
    pl_vec3_from_py(ptPythonCenter, &tCenter);

    gptDraw->add_3d_circle_xz_filled(
        ptDrawlist,
        tCenter,
        fRadius,
        uSegments,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_cylinder_filled(PyObject* self, PyObject* args)
{
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonDesc = NULL;
    uint32_t uSegments = 16;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "drawlist",
        "capsule",
        "segments",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonDesc, &uSegments, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plCylinder tDesc = {0};
    pl__get_cylinder_options(ptPythonDesc, &tDesc);

    gptDraw->add_3d_cylinder_filled(ptDrawlist, tDesc, uSegments, tOptions);
    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_cone_filled(PyObject* self, PyObject* args)
{
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonDesc = NULL;
    uint32_t uSegments = 16;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "drawlist",
        "cone",
        "segments",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonDesc, &uSegments, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plCone tDesc = {0};
    pl__get_cone_options(ptPythonDesc, &tDesc);

    gptDraw->add_3d_cone_filled(ptDrawlist, tDesc, uSegments, tOptions);
    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_centered_box_filled(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonCenter = NULL;
    float fWidth = 0.0f;
    float fHeight = 0.0f;
    float fDepth = 0.0f;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "center",
        "width",
        "height",
        "depth",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOfffO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonCenter, &fWidth, &fHeight, &fDepth, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plVec3 tCenter = {0};
    pl_vec3_from_py(ptPythonCenter, &tCenter);

    gptDraw->add_3d_centered_box_filled(
        ptDrawlist,
        tCenter,
        fWidth,
        fHeight,
        fDepth,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_sphere_filled(PyObject* self, PyObject* args)
{
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonDesc = NULL;
    uint32_t uLatBands = 16;
    uint32_t uLongBands = 16;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "drawlist",
        "sphere",
        "latBands",
        "longBands",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOIIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonDesc, &uLatBands, &uLongBands, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plSphere tDesc = {0};
    pl__get_sphere_options(ptPythonDesc, &tDesc);

    gptDraw->add_3d_sphere_filled(ptDrawlist, tDesc, uLatBands, uLongBands, tOptions);
    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_triangle_filled(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
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

	if (!pl_parse_args("OOOOO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonP0, &ptPythonP1, &ptPythonP2, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plVec3 tP0 = {0};
    plVec3 tP1 = {0};
    plVec3 tP2 = {0};

    pl_vec3_from_py(ptPythonP0, &tP0);
    pl_vec3_from_py(ptPythonP1, &tP1);
    pl_vec3_from_py(ptPythonP2, &tP2);

    gptDraw->add_3d_triangle_filled(
        ptDrawlist,
        tP0,
        tP1,
        tP2,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_band_xz_filled(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonCenter = NULL;
    float fInnerRadius = 0.0f;
    float fOuterRadius = 0.0f;
    uint32_t uSegments = 16;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "center",
        "innerRadius",
        "outerRadius",
        "uSegments",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOffIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonCenter, &fInnerRadius, &fOuterRadius, &uSegments, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plVec3 tCenter = {0};
    pl_vec3_from_py(ptPythonCenter, &tCenter);

    gptDraw->add_3d_band_xz_filled(
        ptDrawlist,
        tCenter,
        fInnerRadius,
        fOuterRadius,
        uSegments,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_band_xy_filled(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonCenter = NULL;
    float fInnerRadius = 0.0f;
    float fOuterRadius = 0.0f;
    uint32_t uSegments = 16;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "center",
        "innerRadius",
        "outerRadius",
        "uSegments",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOffIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonCenter, &fInnerRadius, &fOuterRadius, &uSegments, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plVec3 tCenter = {0};
    pl_vec3_from_py(ptPythonCenter, &tCenter);

    gptDraw->add_3d_band_xy_filled(
        ptDrawlist,
        tCenter,
        fInnerRadius,
        fOuterRadius,
        uSegments,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_band_yz_filled(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonCenter = NULL;
    float fInnerRadius = 0.0f;
    float fOuterRadius = 0.0f;
    uint32_t uSegments = 16;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "center",
        "innerRadius",
        "outerRadius",
        "uSegments",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOffIO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonCenter, &fInnerRadius, &fOuterRadius, &uSegments, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plVec3 tCenter = {0};
    pl_vec3_from_py(ptPythonCenter, &tCenter);

    gptDraw->add_3d_band_yz_filled(
        ptDrawlist,
        tCenter,
        fInnerRadius,
        fOuterRadius,
        uSegments,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_plane_xz_filled(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonCenter = NULL;
    float fWidth = 0.0f;
    float fHeight = 0.0f;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "center",
        "width",
        "height",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOffO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonCenter, &fWidth, &fHeight, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plVec3 tCenter = {0};
    pl_vec3_from_py(ptPythonCenter, &tCenter);

    gptDraw->add_3d_plane_xz_filled(
        ptDrawlist,
        tCenter,
        fWidth,
        fHeight,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_plane_xy_filled(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonCenter = NULL;
    float fWidth = 0.0f;
    float fHeight = 0.0f;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "center",
        "width",
        "height",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOffO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonCenter, &fWidth, &fHeight, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plVec3 tCenter = {0};
    pl_vec3_from_py(ptPythonCenter, &tCenter);

    gptDraw->add_3d_plane_xy_filled(
        ptDrawlist,
        tCenter,
        fWidth,
        fHeight,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_plane_yz_filled(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonCenter = NULL;
    float fWidth = 0.0f;
    float fHeight = 0.0f;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "layer",
        "center",
        "width",
        "height",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOffO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonCenter, &fWidth, &fHeight, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawlist = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawSolidOptions tOptions = {0};
    pl__get_draw_solid_options(ptPythonOptions, &tOptions);

    plVec3 tCenter = {0};
    pl_vec3_from_py(ptPythonCenter, &tCenter);

    gptDraw->add_3d_plane_yz_filled(
        ptDrawlist,
        tCenter,
        fWidth,
        fHeight,
        tOptions);

    Py_RETURN_NONE;
}

PyObject*
draw_add_3d_text(PyObject* self, PyObject* args)
{
 
    PyObject* ptPythonDrawlist = NULL;
    PyObject* ptPythonP0 = NULL;
    const char* pcText = NULL;
    PyObject* ptPythonOptions = NULL;

    static const char* apcKeywords[] = {
        "drawlist",
        "p",
        "text",
        "options",
        NULL,
    };

	if (!pl_parse_args("OOsO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPythonDrawlist, &ptPythonP0, &pcText, &ptPythonOptions))
		return NULL;

    plDrawList3D* ptDrawList = PyCapsule_GetPointer(ptPythonDrawlist, "plDrawList3D");

    plDrawTextOptions tOptions = {0};
    pl__get_draw_text_options(ptPythonOptions, &tOptions);

    plVec3 tP0 = {0};

    pl_vec3_from_py(ptPythonP0, &tP0);

    gptDraw->add_3d_text(ptDrawList, tP0, pcText, tOptions);
    Py_RETURN_NONE;
}

PyObject*
draw_create_bind_group_for_texture(PyObject* self, PyObject* args)
{
 
    plTextureHandle tHandle = {0};

    static const char* apcKeywords[] = {
        "handle",
        NULL,
    };

	if (!pl_parse_args("I", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &tHandle.uData))
		return NULL;
    plBindGroupHandle tBGHandle = gptDraw->create_bind_group_for_texture(tHandle);
    return PyLong_FromUInt32(tBGHandle.uData);
}

static PyMethodDef gatCommandsplDrawI[] =
{
    {"new_frame", (PyCFunction)draw_new_frame, METH_NOARGS | METH_STATIC, NULL},
    {"cleanup", (PyCFunction)draw_cleanup, METH_NOARGS | METH_STATIC, NULL},
    {"initialize", (PyCFunction)draw_initialize, METH_O | METH_STATIC, NULL},
    {"create_font_atlas", (PyCFunction)draw_create_font_atlas, METH_NOARGS | METH_STATIC, NULL},
    {"build_font_atlas", (PyCFunction)draw_build_font_atlas, METH_VARARGS | METH_STATIC, NULL},
    {"create_bind_group_for_texture", (PyCFunction)draw_create_bind_group_for_texture, METH_VARARGS | METH_STATIC, NULL},
    {"get_current_font_atlas", (PyCFunction)draw_get_current_font_atlas, METH_NOARGS | METH_STATIC, NULL},
    {"get_first_font", (PyCFunction)draw_get_first_font, METH_O | METH_STATIC, NULL},
    {"add_default_font", (PyCFunction)draw_add_default_font, METH_O | METH_STATIC, NULL},
    {"add_font_from_file_ttf", (PyCFunction)draw_add_font_from_file_ttf, METH_VARARGS | METH_STATIC, NULL},
    {"cleanup_font_atlas", (PyCFunction)draw_cleanup_font_atlas, METH_O | METH_STATIC, NULL},
    {"set_font_atlas", (PyCFunction)draw_set_font_atlas, METH_O | METH_STATIC, NULL},
    {"calculate_text_size", (PyCFunction)draw_calculate_text_size, METH_VARARGS | METH_STATIC, NULL},
    {"request_2d_drawlist", (PyCFunction)draw_request_2d_drawlist, METH_NOARGS | METH_STATIC, NULL},
    {"return_2d_drawlist", (PyCFunction)draw_return_2d_drawlist, METH_O | METH_STATIC, NULL},
    {"request_3d_drawlist", (PyCFunction)draw_request_3d_drawlist, METH_NOARGS | METH_STATIC, NULL},
    {"return_3d_drawlist", (PyCFunction)draw_return_3d_drawlist, METH_O | METH_STATIC, NULL},
    {"submit_2d_drawlist", (PyCFunction)draw_submit_2d_drawlist, METH_VARARGS | METH_STATIC, NULL},
    {"submit_3d_drawlist", (PyCFunction)draw_submit_3d_drawlist, METH_VARARGS | METH_STATIC, NULL},
    {"request_2d_layer", (PyCFunction)draw_request_2d_layer, METH_O | METH_STATIC, NULL},
    {"return_2d_layer", (PyCFunction)draw_return_2d_layer, METH_O | METH_STATIC, NULL},
    {"submit_2d_layer", (PyCFunction)draw_submit_2d_layer, METH_O | METH_STATIC, NULL},
    {"add_text", (PyCFunction)draw_add_text, METH_VARARGS | METH_STATIC, NULL},
    {"add_text_clipped", (PyCFunction)draw_add_text_clipped, METH_VARARGS | METH_STATIC, NULL},
    {"add_triangle", (PyCFunction)draw_add_triangle, METH_VARARGS | METH_STATIC, NULL},
    {"add_line", (PyCFunction)draw_add_line, METH_VARARGS | METH_STATIC, NULL},
    {"add_image", (PyCFunction)draw_add_image, METH_VARARGS | METH_STATIC, NULL},
    {"add_image_quad", (PyCFunction)draw_add_image_quad, METH_VARARGS | METH_STATIC, NULL},
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
    {"add_3d_transform", (PyCFunction)draw_add_3d_transform, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_line", (PyCFunction)draw_add_3d_line, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_aabb", (PyCFunction)draw_add_3d_aabb, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_bezier_quad", (PyCFunction)draw_add_3d_bezier_quad, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_bezier_cubic", (PyCFunction)draw_add_3d_bezier_cubic, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_cross", (PyCFunction)draw_add_3d_cross, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_frustum", (PyCFunction)draw_add_3d_frustum, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_sphere", (PyCFunction)draw_add_3d_sphere, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_capsule", (PyCFunction)draw_add_3d_capsule, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_cylinder", (PyCFunction)draw_add_3d_cylinder, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_cone", (PyCFunction)draw_add_3d_cone, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_centered_box", (PyCFunction)draw_add_3d_centered_box, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_circle_xz", (PyCFunction)draw_add_3d_circle_xz, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_circle_xz_filled", (PyCFunction)draw_add_3d_circle_xz_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_cylinder_filled", (PyCFunction)draw_add_3d_cylinder_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_cone_filled", (PyCFunction)draw_add_3d_cone_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_centered_box_filled", (PyCFunction)draw_add_3d_centered_box_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_sphere_filled", (PyCFunction)draw_add_3d_sphere_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_triangle_filled", (PyCFunction)draw_add_3d_triangle_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_band_xz_filled", (PyCFunction)draw_add_3d_band_xz_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_band_xy_filled", (PyCFunction)draw_add_3d_band_xy_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_band_yz_filled", (PyCFunction)draw_add_3d_band_yz_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_plane_xz_filled", (PyCFunction)draw_add_3d_plane_xz_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_plane_xy_filled", (PyCFunction)draw_add_3d_plane_xy_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_plane_yz_filled", (PyCFunction)draw_add_3d_plane_yz_filled, METH_VARARGS | METH_STATIC, NULL},
    {"add_3d_text", (PyCFunction)draw_add_3d_text, METH_VARARGS | METH_STATIC, NULL},
    {"use_linear_sampler", (PyCFunction)draw_use_linear_sampler, METH_VARARGS | METH_STATIC, NULL},
    {"use_nearest_sampler", (PyCFunction)draw_use_nearest_sampler, METH_VARARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plDrawI)