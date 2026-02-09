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
plShaderI_initialize(PyObject* self, PyObject* args)
{
    PyObject* ptFlags = PyObject_GetAttrString(args, "tFlags");
    PyObject* ptOptimizationLevel = PyObject_GetAttrString(args, "tOptimizationLevel");
    PyObject* ptCacheOutputDirectory = PyObject_GetAttrString(args, "pcCacheOutputDirectory");
    PyObject* apcIncludeDirectories = PyObject_GetAttrString(args, "apcIncludeDirectories");
    PyObject* apcDirectories = PyObject_GetAttrString(args, "apcDirectories");
    PyObject* ptMacroDefinitions = PyObject_GetAttrString(args, "ptMacroDefinitions");


    plShaderOptions tOptions = {0};

    PyLong_AsInt32(ptFlags, &tOptions.tFlags);
    PyLong_AsInt32(ptOptimizationLevel, &tOptions.tOptimizationLevel);
    tOptions.pcCacheOutputDirectory = PyUnicode_AsUTF8(ptCacheOutputDirectory);

    Py_ssize_t szIncludeCount = PyList_Size(apcIncludeDirectories);
    for (Py_ssize_t i = 0; i < szIncludeCount; ++i)
    {
        tOptions.apcIncludeDirectories[i] = PyUnicode_AsUTF8(PyList_GetItem(apcIncludeDirectories, i));
    }


    Py_ssize_t szDirCount = PyList_Size(apcDirectories);
    for (Py_ssize_t i = 0; i < szDirCount; ++i)
    {
        tOptions.apcDirectories[i] = PyUnicode_AsUTF8(PyList_GetItem(apcDirectories, i));
    }

    Py_ssize_t szMacroCount = PyList_Size(ptMacroDefinitions);
    for (Py_ssize_t i = 0; i < szMacroCount; ++i)
    {
        PyObject* ptDefinition = PyList_GetItem(ptMacroDefinitions, i);
        tOptions.atMacroDefinitions[i].pcName = PyUnicode_AsUTF8(PyObject_GetAttrString(ptDefinition, "pcName"));
        tOptions.atMacroDefinitions[i].pcValue = PyUnicode_AsUTF8(PyObject_GetAttrString(ptDefinition, "pcValue"));
    }

    return PyBool_FromLong(gptShader->initialize(&tOptions));
}

PyObject*
plShaderI_cleanup(PyObject* self)
{
    gptShader->cleanup();
    Py_RETURN_NONE;
}

PyObject*
plShaderI_load_glsl(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "shader",
        "entryFunc",
        "file",
        "options",
        NULL,
    };

    const char* pcShader = NULL;
    const char* pcEntryFunc = NULL;
    const char* pcFile = NULL;
    PyObject* ptOptions = NULL;
	if (!pl_parse("ss|sO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcShader, &pcEntryFunc, &pcFile, &ptOptions))
		return NULL;

    plShaderOptions tOptions = {0};

    if(ptOptions)
    {
        PyObject* ptFlags = PyObject_GetAttrString(ptOptions, "tFlags");
        PyObject* ptOptimizationLevel = PyObject_GetAttrString(ptOptions, "tOptimizationLevel");
        PyObject* ptCacheOutputDirectory = PyObject_GetAttrString(ptOptions, "pcCacheOutputDirectory");
        PyObject* apcIncludeDirectories = PyObject_GetAttrString(ptOptions, "apcIncludeDirectories");
        PyObject* apcDirectories = PyObject_GetAttrString(ptOptions, "apcDirectories");
        PyObject* ptMacroDefinitions = PyObject_GetAttrString(ptOptions, "ptMacroDefinitions");

        PyLong_AsInt32(ptFlags, &tOptions.tFlags);
        PyLong_AsInt32(ptOptimizationLevel, &tOptions.tOptimizationLevel);
        tOptions.pcCacheOutputDirectory = PyUnicode_AsUTF8(ptCacheOutputDirectory);

        Py_ssize_t szIncludeCount = PyList_Size(apcIncludeDirectories);
        for (Py_ssize_t i = 0; i < szIncludeCount; ++i)
        {
            tOptions.apcIncludeDirectories[i] = PyUnicode_AsUTF8(PyList_GetItem(apcIncludeDirectories, i));
        }


        Py_ssize_t szDirCount = PyList_Size(apcDirectories);
        for (Py_ssize_t i = 0; i < szDirCount; ++i)
        {
            tOptions.apcDirectories[i] = PyUnicode_AsUTF8(PyList_GetItem(apcDirectories, i));
        }

        Py_ssize_t szMacroCount = PyList_Size(ptMacroDefinitions);
        for (Py_ssize_t i = 0; i < szMacroCount; ++i)
        {
            PyObject* ptDefinition = PyList_GetItem(ptMacroDefinitions, i);
            tOptions.atMacroDefinitions[i].pcName = PyUnicode_AsUTF8(PyObject_GetAttrString(ptDefinition, "pcName"));
            tOptions.atMacroDefinitions[i].pcValue = PyUnicode_AsUTF8(PyObject_GetAttrString(ptDefinition, "pcValue"));
        }
    }

    plShaderModule tModule = gptShader->load_glsl(pcShader, pcEntryFunc, pcFile, ptOptions ? &tOptions : NULL);
    return Py_BuildValue("(nOs)", tModule.szCodeSize, PyCapsule_New(tModule.puCode, "plShaderModule.puCode", NULL), tModule.pcEntryFunc);
}

PyObject*
plShaderI_compile_glsl(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "shader",
        "entryFunc",
        "options",
        NULL,
    };

    const char* pcShader = NULL;
    const char* pcEntryFunc = NULL;
    PyObject* ptOptions = NULL;
	if (!pl_parse("ss|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcShader, &pcEntryFunc, &ptOptions))
		return NULL;

    plShaderOptions tOptions = {0};

    if(ptOptions)
    {
        PyObject* ptFlags = PyObject_GetAttrString(ptOptions, "tFlags");
        PyObject* ptOptimizationLevel = PyObject_GetAttrString(ptOptions, "tOptimizationLevel");
        PyObject* ptCacheOutputDirectory = PyObject_GetAttrString(ptOptions, "pcCacheOutputDirectory");
        PyObject* apcIncludeDirectories = PyObject_GetAttrString(ptOptions, "apcIncludeDirectories");
        PyObject* apcDirectories = PyObject_GetAttrString(ptOptions, "apcDirectories");
        PyObject* ptMacroDefinitions = PyObject_GetAttrString(ptOptions, "ptMacroDefinitions");

        PyLong_AsInt32(ptFlags, &tOptions.tFlags);
        PyLong_AsInt32(ptOptimizationLevel, &tOptions.tOptimizationLevel);
        tOptions.pcCacheOutputDirectory = PyUnicode_AsUTF8(ptCacheOutputDirectory);

        Py_ssize_t szIncludeCount = PyList_Size(apcIncludeDirectories);
        for (Py_ssize_t i = 0; i < szIncludeCount; ++i)
        {
            tOptions.apcIncludeDirectories[i] = PyUnicode_AsUTF8(PyList_GetItem(apcIncludeDirectories, i));
        }


        Py_ssize_t szDirCount = PyList_Size(apcDirectories);
        for (Py_ssize_t i = 0; i < szDirCount; ++i)
        {
            tOptions.apcDirectories[i] = PyUnicode_AsUTF8(PyList_GetItem(apcDirectories, i));
        }

        Py_ssize_t szMacroCount = PyList_Size(ptMacroDefinitions);
        for (Py_ssize_t i = 0; i < szMacroCount; ++i)
        {
            PyObject* ptDefinition = PyList_GetItem(ptMacroDefinitions, i);
            tOptions.atMacroDefinitions[i].pcName = PyUnicode_AsUTF8(PyObject_GetAttrString(ptDefinition, "pcName"));
            tOptions.atMacroDefinitions[i].pcValue = PyUnicode_AsUTF8(PyObject_GetAttrString(ptDefinition, "pcValue"));
        }
    }

    plShaderModule tModule = gptShader->compile_glsl(pcShader, pcEntryFunc, ptOptions ? &tOptions : NULL);
    return Py_BuildValue("(nOs)", tModule.szCodeSize, PyCapsule_New(tModule.puCode, "plShaderModule.puCode", NULL), tModule.szCodeSize);
}

PyObject*
plShaderI_write_to_disk(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "shader",
        "module",
        NULL,
    };

    const char* pcShader = NULL;
    PyObject* ptModule = NULL;
	if (!pl_parse("sO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &pcShader, &ptModule))
		return NULL;

    plShaderModule tModule = {
        .pcEntryFunc = PyUnicode_AsUTF8(PyTuple_GetItem(ptModule, 2)),
        .szCodeSize = PyLong_AsSize_t(PyTuple_GetItem(ptModule, 0)),
        .puCode = PyCapsule_GetPointer(PyTuple_GetItem(ptModule, 1), "plShaderModule.puCode")
    };
    gptShader->write_to_disk(pcShader, &tModule);
    Py_RETURN_NONE;
}

plPythonIntConstantPair gatShaderIntPairs[] = {
    PL_ADD_INT_CONSTANT(PL_SHADER_FLAGS_NONE),
    PL_ADD_INT_CONSTANT(PL_SHADER_FLAGS_INCLUDE_DEBUG),
    PL_ADD_INT_CONSTANT(PL_SHADER_FLAGS_ALWAYS_COMPILE),
    PL_ADD_INT_CONSTANT(PL_SHADER_FLAGS_NEVER_CACHE),
    PL_ADD_INT_CONSTANT(PL_SHADER_FLAGS_METAL_OUTPUT),
    PL_ADD_INT_CONSTANT(PL_SHADER_FLAGS_SPIRV_OUTPUT),
    PL_ADD_INT_CONSTANT(PL_SHADER_FLAGS_AUTO_OUTPUT),

    PL_ADD_INT_CONSTANT(PL_SHADER_OPTIMIZATION_NONE),
    PL_ADD_INT_CONSTANT(PL_SHADER_OPTIMIZATION_SIZE),
    PL_ADD_INT_CONSTANT(PL_SHADER_OPTIMIZATION_PERFORMANCE),
};