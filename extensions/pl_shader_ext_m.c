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
shader_initialize(PyObject* self, PyObject* args)
{
    PyObject* ptFlags = PyObject_GetAttrString(args, "eFlags");
    PyObject* ptOptimizationLevel = PyObject_GetAttrString(args, "eOptimizationLevel");
    PyObject* apcIncludeDirectories = PyObject_GetAttrString(args, "apcIncludeDirectories");
    PyObject* apcDirectories = PyObject_GetAttrString(args, "apcDirectories");
    PyObject* ptMacroDefinitions = PyObject_GetAttrString(args, "ptMacroDefinitions");


    plShaderOptions tOptions = {0};

    PyLong_AsInt32(ptFlags, &tOptions.eFlags);
    PyLong_AsInt32(ptOptimizationLevel, &tOptions.eOptimizationLevel);

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
shader_cleanup(PyObject* self)
{
    gptShader->cleanup();
    Py_RETURN_NONE;
}

PyObject*
shader_load_glsl(PyObject* self, PyObject* args, PyObject* kwargs)
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
	if (!pl_parse_args("ss|sO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcShader, &pcEntryFunc, &pcFile, &ptOptions))
		return NULL;

    plShaderOptions tOptions = {0};

    if(ptOptions)
    {
        PyObject* ptFlags = PyObject_GetAttrString(ptOptions, "eFlags");
        PyObject* ptOptimizationLevel = PyObject_GetAttrString(ptOptions, "eOptimizationLevel");
        PyObject* apcIncludeDirectories = PyObject_GetAttrString(ptOptions, "apcIncludeDirectories");
        PyObject* apcDirectories = PyObject_GetAttrString(ptOptions, "apcDirectories");
        PyObject* ptMacroDefinitions = PyObject_GetAttrString(ptOptions, "ptMacroDefinitions");

        PyLong_AsInt32(ptFlags, &tOptions.eFlags);
        PyLong_AsInt32(ptOptimizationLevel, &tOptions.eOptimizationLevel);

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
shader_compile_glsl(PyObject* self, PyObject* args, PyObject* kwargs)
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
	if (!pl_parse_args("ss|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcShader, &pcEntryFunc, &ptOptions))
		return NULL;

    plShaderOptions tOptions = {0};

    if(ptOptions)
    {
        PyObject* ptFlags = PyObject_GetAttrString(ptOptions, "eFlags");
        PyObject* ptOptimizationLevel = PyObject_GetAttrString(ptOptions, "eOptimizationLevel");
        PyObject* apcIncludeDirectories = PyObject_GetAttrString(ptOptions, "apcIncludeDirectories");
        PyObject* apcDirectories = PyObject_GetAttrString(ptOptions, "apcDirectories");
        PyObject* ptMacroDefinitions = PyObject_GetAttrString(ptOptions, "ptMacroDefinitions");

        PyLong_AsInt32(ptFlags, &tOptions.eFlags);
        PyLong_AsInt32(ptOptimizationLevel, &tOptions.eOptimizationLevel);

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
shader_write_to_disk(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "shader",
        "module",
        NULL,
    };

    const char* pcShader = NULL;
    PyObject* ptModule = NULL;
	if (!pl_parse_args("sO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
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

// options: plShaderOptions (strings are copied by the shader extension)
static PyObject*
shader_set_options(PyObject* self, PyObject* ptOptions)
{
    plShaderOptions tOptions = {0};

    PyObject* ptFlags = PyObject_GetAttrString(ptOptions, "eFlags");
    PyObject* ptLevel = ptFlags ? PyObject_GetAttrString(ptOptions, "eOptimizationLevel") : NULL;
    PyObject* ptIncludes = ptLevel ? PyObject_GetAttrString(ptOptions, "apcIncludeDirectories") : NULL;
    PyObject* ptDirectories = ptIncludes ? PyObject_GetAttrString(ptOptions, "apcDirectories") : NULL;
    PyObject* ptMacros = ptDirectories ? PyObject_GetAttrString(ptOptions, "ptMacroDefinitions") : NULL;
    PyObject* aptHeld[2 * PL_MAX_SHADER_MACRO_DEFINITIONS] = {0}; // macro name/value strs
    uint32_t uHeldCount = 0;
    bool bOk = ptMacros != NULL;

    if(bOk)
    {
        tOptions.eFlags = (int)PyLong_AsLong(ptFlags);
        tOptions.eOptimizationLevel = (int)PyLong_AsLong(ptLevel);
        bOk = !PyErr_Occurred();
    }

    if(bOk)
    {
        const Py_ssize_t szCount = PySequence_Size(ptIncludes);
        if(szCount < 0 || szCount > PL_MAX_SHADER_INCLUDE_DIRECTORIES)
        {
            if(szCount >= 0)
                PyErr_Format(PyExc_ValueError, "at most %d include directories", PL_MAX_SHADER_INCLUDE_DIRECTORIES);
            bOk = false;
        }
        for(Py_ssize_t i = 0; bOk && i < szCount; i++)
        {
            PyObject* ptItem = PySequence_GetItem(ptIncludes, i);
            tOptions.apcIncludeDirectories[i] = ptItem ? PyUnicode_AsUTF8(ptItem) : NULL;
            Py_XDECREF(ptItem); // the list keeps the str alive
            bOk = tOptions.apcIncludeDirectories[i] != NULL;
        }
    }

    if(bOk)
    {
        const Py_ssize_t szCount = PySequence_Size(ptDirectories);
        if(szCount < 0 || szCount > PL_MAX_SHADER_DIRECTORIES)
        {
            if(szCount >= 0)
                PyErr_Format(PyExc_ValueError, "at most %d directories", PL_MAX_SHADER_DIRECTORIES);
            bOk = false;
        }
        for(Py_ssize_t i = 0; bOk && i < szCount; i++)
        {
            PyObject* ptItem = PySequence_GetItem(ptDirectories, i);
            tOptions.apcDirectories[i] = ptItem ? PyUnicode_AsUTF8(ptItem) : NULL;
            Py_XDECREF(ptItem);
            bOk = tOptions.apcDirectories[i] != NULL;
        }
    }

    if(bOk)
    {
        const Py_ssize_t szCount = PySequence_Size(ptMacros);
        if(szCount < 0 || szCount > PL_MAX_SHADER_MACRO_DEFINITIONS)
        {
            if(szCount >= 0)
                PyErr_Format(PyExc_ValueError, "at most %d macro definitions", PL_MAX_SHADER_MACRO_DEFINITIONS);
            bOk = false;
        }
        for(Py_ssize_t i = 0; bOk && i < szCount; i++)
        {
            PyObject* ptDefinition = PySequence_GetItem(ptMacros, i);
            PyObject* ptName = ptDefinition ? PyObject_GetAttrString(ptDefinition, "pcName") : NULL;
            PyObject* ptValue = ptName ? PyObject_GetAttrString(ptDefinition, "pcValue") : NULL;
            Py_XDECREF(ptDefinition);
            if(ptName) aptHeld[uHeldCount++] = ptName;
            if(ptValue) aptHeld[uHeldCount++] = ptValue;
            bOk = ptValue != NULL;
            if(bOk)
            {
                tOptions.atMacroDefinitions[i].pcName = PyUnicode_AsUTF8(ptName);
                tOptions.atMacroDefinitions[i].pcValue = ptValue == Py_None ? "" : PyUnicode_AsUTF8(ptValue);
                bOk = tOptions.atMacroDefinitions[i].pcName && tOptions.atMacroDefinitions[i].pcValue;
            }
        }
    }

    if(bOk)
        gptShader->set_options(&tOptions);

    for(uint32_t i = 0; i < uHeldCount; i++)
        Py_DECREF(aptHeld[i]);
    Py_XDECREF(ptFlags);
    Py_XDECREF(ptLevel);
    Py_XDECREF(ptIncludes);
    Py_XDECREF(ptDirectories);
    Py_XDECREF(ptMacros);
    if(!bOk)
        return NULL;
    Py_RETURN_NONE;
}

// returns the current default options as a plShaderOptions (None if not initialized)
static PyObject*
shader_get_options(PyObject* self, PyObject* unused)
{
    const plShaderOptions* ptOptions = gptShader->get_options();
    if(ptOptions == NULL)
        Py_RETURN_NONE;

    PyObject* ptTypes = PyImport_ImportModule("pilotlight.types");
    if(ptTypes == NULL)
        return NULL;
    PyObject* ptResult = PyObject_CallMethod(ptTypes, "plShaderOptions", NULL);
    PyObject* ptDefinitionType = PyObject_GetAttrString(ptTypes, "plShaderMacroDefinition");
    Py_DECREF(ptTypes);
    if(ptResult == NULL || ptDefinitionType == NULL)
    {
        Py_XDECREF(ptResult);
        Py_XDECREF(ptDefinitionType);
        return NULL;
    }

    PyObject* ptIncludes = PyList_New(0);
    PyObject* ptDirectories = PyList_New(0);
    PyObject* ptMacros = PyList_New(0);
    bool bOk = ptIncludes && ptDirectories && ptMacros;
    for(uint32_t i = 0; bOk && i < ptOptions->_uIncludeDirectoriesCount; i++)
    {
        PyObject* ptStr = PyUnicode_FromString(ptOptions->apcIncludeDirectories[i]);
        bOk = ptStr && PyList_Append(ptIncludes, ptStr) == 0;
        Py_XDECREF(ptStr);
    }
    for(uint32_t i = 0; bOk && i < ptOptions->_uDirectoriesCount; i++)
    {
        PyObject* ptStr = PyUnicode_FromString(ptOptions->apcDirectories[i]);
        bOk = ptStr && PyList_Append(ptDirectories, ptStr) == 0;
        Py_XDECREF(ptStr);
    }
    for(uint32_t i = 0; bOk && i < ptOptions->_uMacroDefinitionCount; i++)
    {
        PyObject* ptDefinition = PyObject_CallObject(ptDefinitionType, NULL);
        bOk = ptDefinition != NULL;
        if(bOk)
        {
            PyObject* ptName = PyUnicode_FromString(ptOptions->atMacroDefinitions[i].pcName);
            PyObject* ptValue = PyUnicode_FromString(ptOptions->atMacroDefinitions[i].pcValue ? ptOptions->atMacroDefinitions[i].pcValue : "");
            bOk = ptName && ptValue &&
                  PyObject_SetAttrString(ptDefinition, "pcName", ptName) == 0 &&
                  PyObject_SetAttrString(ptDefinition, "pcValue", ptValue) == 0 &&
                  PyList_Append(ptMacros, ptDefinition) == 0;
            Py_XDECREF(ptName);
            Py_XDECREF(ptValue);
            Py_DECREF(ptDefinition);
        }
    }
    if(bOk)
    {
        PyObject* ptFlags = PyLong_FromLong((long)ptOptions->eFlags);
        PyObject* ptLevel = PyLong_FromLong((long)ptOptions->eOptimizationLevel);
        bOk = ptFlags && ptLevel &&
              PyObject_SetAttrString(ptResult, "eFlags", ptFlags) == 0 &&
              PyObject_SetAttrString(ptResult, "eOptimizationLevel", ptLevel) == 0 &&
              PyObject_SetAttrString(ptResult, "apcIncludeDirectories", ptIncludes) == 0 &&
              PyObject_SetAttrString(ptResult, "apcDirectories", ptDirectories) == 0 &&
              PyObject_SetAttrString(ptResult, "ptMacroDefinitions", ptMacros) == 0;
        Py_XDECREF(ptFlags);
        Py_XDECREF(ptLevel);
    }
    Py_XDECREF(ptIncludes);
    Py_XDECREF(ptDirectories);
    Py_XDECREF(ptMacros);
    Py_DECREF(ptDefinitionType);
    if(!bOk)
    {
        Py_DECREF(ptResult);
        return NULL;
    }
    return ptResult;
}

static PyMethodDef gatCommandsplShaderI[] =
{
    {"set_options", (PyCFunction)shader_set_options, METH_O | METH_STATIC, NULL},
    {"get_options", (PyCFunction)shader_get_options, METH_NOARGS | METH_STATIC, NULL},
    {"initialize", (PyCFunction)shader_initialize, METH_O | METH_STATIC, NULL},
    {"cleanup", (PyCFunction)shader_cleanup, METH_NOARGS | METH_STATIC, NULL},
    {"load_glsl", (PyCFunction)shader_load_glsl, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"compile_glsl", (PyCFunction)shader_compile_glsl, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"write_to_disk", (PyCFunction)shader_write_to_disk, METH_VARARGS | METH_STATIC, NULL},

    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plShaderI)