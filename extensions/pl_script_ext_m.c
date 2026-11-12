/*
   pl_script_ext_m.c
     * plScriptI
     * python scripts:
         In C, a script component names a shared library (i.e. "pl_script_camera")
         which the script extension loads through the extension registry; the
         library's pl_load_script() registers a plScriptInterface under that name.

         Here, the same name is resolved to a python module instead. The module
         provides:
             run(library, entity)    # required, called every update while playing
             setup(library, entity)  # optional, called once when the script loads

         Module lookup (first match wins):
             1. scripts registered explicitly with plScriptI.register(...)
             2. normal python import of <name> (sys.path, i.e. next to the app)
             3. pilotlight.<name> (scripts shipped with the package)

         Every python script shares one pair of C trampolines that forward to
         the module based on the component's script name. If a script raises,
         the traceback is printed and that script is disabled.
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] python script registry
// [SECTION] trampolines
// [SECTION] script loading (called by pl_main.c)
// [SECTION] implementations
// [SECTION] method tables
// [SECTION] constants
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] python script registry
//-----------------------------------------------------------------------------

// name -> (run, setup) tuple; setup may be None
static PyObject* gptPyScripts = NULL;

// names of scripts that failed (no module, missing run, or raised); skipped until re-registered
static PyObject* gptPyDisabledScripts = NULL;

// names already registered with the api registry (interned strings)
static const char** gsbpcPyScriptApis = NULL;

static bool
pl__py_script_registry_init(void)
{
    if(gptPyScripts == NULL)
        gptPyScripts = PyDict_New();
    if(gptPyDisabledScripts == NULL)
        gptPyDisabledScripts = PySet_New(NULL);
    return gptPyScripts && gptPyDisabledScripts;
}

static void
pl__py_script_disable(const char* pcName)
{
    PyObject* ptName = PyUnicode_FromString(pcName);
    if(ptName)
    {
        PySet_Add(gptPyDisabledScripts, ptName);
        Py_DECREF(ptName);
    }
}

static void
pl__py_script_enable(const char* pcName)
{
    PyObject* ptName = PyUnicode_FromString(pcName);
    if(ptName)
    {
        PySet_Discard(gptPyDisabledScripts, ptName);
        Py_DECREF(ptName);
    }
}

static bool
pl__py_script_is_disabled(const char* pcName)
{
    PyObject* ptName = PyUnicode_FromString(pcName);
    if(ptName == NULL)
        return true;
    int iResult = PySet_Contains(gptPyDisabledScripts, ptName);
    Py_DECREF(ptName);
    return iResult == 1;
}

// stores run/setup for a name (setup may be NULL/None); returns false on bad input
static bool
pl__py_script_store(const char* pcName, PyObject* ptRun, PyObject* ptSetup)
{
    if(!PyCallable_Check(ptRun))
    {
        PyErr_Format(PyExc_TypeError, "script '%s': run must be callable", pcName);
        return false;
    }
    if(ptSetup == NULL)
        ptSetup = Py_None;
    if(ptSetup != Py_None && !PyCallable_Check(ptSetup))
    {
        PyErr_Format(PyExc_TypeError, "script '%s': setup must be callable or None", pcName);
        return false;
    }

    PyObject* ptEntry = PyTuple_Pack(2, ptRun, ptSetup);
    if(ptEntry == NULL)
        return false;
    int iResult = PyDict_SetItemString(gptPyScripts, pcName, ptEntry);
    Py_DECREF(ptEntry);
    if(iResult != 0)
        return false;
    pl__py_script_enable(pcName);
    return true;
}

// resolves a script name to a python module & stores its run/setup.
// returns false (with a printed message) if it can't be resolved.
static bool
pl__py_script_import(const char* pcName)
{
    PyObject* ptModule = PyImport_ImportModule(pcName);
    if(ptModule == NULL && PyErr_ExceptionMatches(PyExc_ModuleNotFoundError))
    {
        PyErr_Clear();
        PyObject* ptPackageName = PyUnicode_FromFormat("pilotlight.%s", pcName);
        if(ptPackageName)
        {
            ptModule = PyImport_Import(ptPackageName);
            Py_DECREF(ptPackageName);
        }
    }

    if(ptModule == NULL)
    {
        if(PyErr_ExceptionMatches(PyExc_ModuleNotFoundError))
        {
            PyErr_Clear();
            printf("[ERROR] (Python Script) no python module found for script '%s'\n", pcName);
        }
        else
        {
            printf("[ERROR] (Python Script) failed to import script '%s'\n", pcName);
            PyErr_Print();
        }
        return false;
    }

    PyObject* ptRun = PyObject_GetAttrString(ptModule, "run");
    if(ptRun == NULL)
    {
        PyErr_Clear();
        printf("[ERROR] (Python Script) script module '%s' has no run(library, entity) function\n", pcName);
        Py_DECREF(ptModule);
        return false;
    }

    PyObject* ptSetup = NULL;
    if(PyObject_HasAttrString(ptModule, "setup"))
        ptSetup = PyObject_GetAttrString(ptModule, "setup");

    bool bResult = pl__py_script_store(pcName, ptRun, ptSetup);
    if(!bResult)
        PyErr_Print();

    Py_XDECREF(ptSetup);
    Py_DECREF(ptRun);
    Py_DECREF(ptModule);
    return bResult;
}

//-----------------------------------------------------------------------------
// [SECTION] trampolines
//-----------------------------------------------------------------------------

// calls run (index 0) or setup (index 1) for the entity's script
static void
pl__py_script_call(plComponentLibrary* ptLibrary, plEntity tEntity, Py_ssize_t szIndex)
{
    const plScriptComponent* ptScript = gptEcs->get_component(ptLibrary, gptScript->get_ecs_type_key(), tEntity);
    if(ptScript == NULL || ptScript->pcPath == NULL)
        return;

    const char* pcName = ptScript->pcPath;
    if(pl__py_script_is_disabled(pcName))
        return;

    PyObject* ptEntry = PyDict_GetItemString(gptPyScripts, pcName); // borrowed
    if(ptEntry == NULL)
    {
        printf("[WARN] (Python Script) no python script registered for '%s'\n", pcName);
        pl__py_script_disable(pcName);
        return;
    }

    PyObject* ptFunc = PyTuple_GetItem(ptEntry, szIndex); // borrowed
    if(ptFunc == Py_None)
        return;

    PyObject* ptPyLibrary = pl_library_to_py(ptLibrary);
    PyObject* ptPyEntity = pl_entity_to_py(gptScript->get_ecs_type_key(), tEntity);
    PyObject* ptResult = NULL;
    if(ptPyLibrary && ptPyEntity)
        ptResult = PyObject_CallFunctionObjArgs(ptFunc, ptPyLibrary, ptPyEntity, NULL);
    Py_XDECREF(ptPyLibrary);
    Py_XDECREF(ptPyEntity);

    if(ptResult == NULL)
    {
        printf("[ERROR] (Python Script) script '%s' raised an exception and has been disabled\n", pcName);
        PyErr_Print();
        pl__py_script_disable(pcName);
        return;
    }
    Py_DECREF(ptResult);
}

static void
pl__py_script_run(plComponentLibrary* ptLibrary, plEntity tEntity)
{
    pl__py_script_call(ptLibrary, tEntity, 0);
}

static void
pl__py_script_setup(plComponentLibrary* ptLibrary, plEntity tEntity)
{
    pl__py_script_call(ptLibrary, tEntity, 1);
}

// registers the trampolines with the api registry under the script name
// (this is what a C script's pl_load_script() does)
static void
pl__py_script_register_api(const char* pcName)
{
    for(uint32_t i = 0; i < pl_sb_size(gsbpcPyScriptApis); i++)
    {
        if(strcmp(gsbpcPyScriptApis[i], pcName) == 0)
            return;
    }

    const char* pcInternedName = gptString->intern(pcName); // api registry keeps the pointer
    const plScriptInterface tApi = {
        .setup = pl__py_script_setup,
        .run   = pl__py_script_run
    };
    gptApiRegistry->set_api(pcInternedName, (plVersion)plScriptInterface_version, &tApi, sizeof(plScriptInterface));
    pl_sb_push(gsbpcPyScriptApis, pcInternedName);
}

//-----------------------------------------------------------------------------
// [SECTION] script loading (called by pl_main.c)
//-----------------------------------------------------------------------------

// replaces the extension registry load for scripts; always succeeds so the
// script extension never sees a missing interface
bool
pl_python_script_load(const char* pcName)
{
    if(!pl__py_script_registry_init())
    {
        PyErr_Print();
        return false;
    }

    if(PyDict_GetItemString(gptPyScripts, pcName) == NULL && !pl__py_script_is_disabled(pcName))
    {
        if(!pl__py_script_import(pcName))
            pl__py_script_disable(pcName);
    }

    pl__py_script_register_api(pcName);
    return true;
}

// returns false if pcName isn't a python script (so the caller can fall back)
bool
pl_python_script_unload(const char* pcName)
{
    bool bPythonScript = false;
    for(uint32_t i = 0; i < pl_sb_size(gsbpcPyScriptApis); i++)
    {
        if(strcmp(gsbpcPyScriptApis[i], pcName) == 0)
        {
            bPythonScript = true;
            break;
        }
    }

    if(gptPyScripts && PyDict_GetItemString(gptPyScripts, pcName))
    {
        PyDict_DelItemString(gptPyScripts, pcName);
        bPythonScript = true;
    }
    return bPythonScript;
}

void
pl_python_script_cleanup(void)
{
    // api entries are removed by the registry itself on shutdown
    Py_CLEAR(gptPyScripts);
    Py_CLEAR(gptPyDisabledScripts);
    pl_sb_free(gsbpcPyScriptApis);
}

//-----------------------------------------------------------------------------
// [SECTION] implementations
//-----------------------------------------------------------------------------

PL_PY_BIND_VOID(script_register_ecs_components, gptScript->register_ecs_components())
PL_PY_BIND_KEY(script_get_ecs_type_key,         gptScript->get_ecs_type_key())
PL_PY_BIND_LIBRARY(script_run_update_system,    gptScript->run_update_system(ptLibrary))

// register a python script explicitly (instead of resolving it from a module)
static PyObject*
script_register(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "name",
        "run",
        "setup",
        NULL,
    };

    const char* pcName = NULL;
    PyObject* ptRun = NULL;
    PyObject* ptSetup = Py_None;
	if (!pl_parse_args("sO|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcName, &ptRun, &ptSetup))
		return NULL;

    if(!pl__py_script_registry_init())
        return NULL;
    if(!pl__py_script_store(pcName, ptRun, ptSetup))
        return NULL;
    pl__py_script_register_api(pcName);
    Py_RETURN_NONE;
}

static PyObject*
script_unregister(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "name",
        NULL,
    };

    const char* pcName = NULL;
	if (!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &pcName))
		return NULL;

    pl_python_script_unload(pcName);
    if(PyErr_Occurred())
        return NULL;
    Py_RETURN_NONE;
}

// creates a new entity with a script component
static PyObject*
script_create(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "library",
        "script",
        "flags",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
    const char* pcScript = NULL;
    int iFlags = PL_SCRIPT_FLAG_PLAYING;
	if (!pl_parse_args("Os|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyLibrary, &pcScript, &iFlags))
		return NULL;

    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;

    plEntity tEntity = gptScript->create(ptLibrary, gptString->intern(pcScript), iFlags, NULL);
    if(PyErr_Occurred()) // a script's setup may have failed (already reported)
        PyErr_Clear();
    return pl_entity_to_py(gptScript->get_ecs_type_key(), tEntity);
}

// adds a script component to an existing entity
static PyObject*
script_attach(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "library",
        "script",
        "entity",
        "flags",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
    const char* pcScript = NULL;
    PyObject* ptPyEntity = NULL;
    int iFlags = PL_SCRIPT_FLAG_PLAYING;
	if (!pl_parse_args("OsO|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyLibrary, &pcScript, &ptPyEntity, &iFlags))
		return NULL;

    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;

    gptScript->attach(ptLibrary, gptString->intern(pcScript), iFlags, pl_get_entity_from_python(ptPyEntity).tEntity, NULL);
    if(PyErr_Occurred())
        PyErr_Clear();
    Py_RETURN_NONE;
}

// (re)loads the script of an entity's script component
static PyObject*
script_load(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "library",
        "entity",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
    PyObject* ptPyEntity = NULL;
	if (!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyLibrary, &ptPyEntity))
		return NULL;

    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;

    gptScript->load(ptLibrary, pl_get_entity_from_python(ptPyEntity).tEntity);
    if(PyErr_Occurred())
        PyErr_Clear();
    Py_RETURN_NONE;
}

//-----------------------------------------------------------------------------
// [SECTION] method tables
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplScriptI[] =
{
    {"register_ecs_components", (PyCFunction)script_register_ecs_components, METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key",        (PyCFunction)script_get_ecs_type_key,        METH_NOARGS | METH_STATIC, NULL},
    {"run_update_system",       (PyCFunction)script_run_update_system,       METH_O | METH_STATIC, NULL},
    {"register",                (PyCFunction)script_register,                METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"unregister",              (PyCFunction)script_unregister,              METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"create",                  (PyCFunction)script_create,                  METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"attach",                  (PyCFunction)script_attach,                  METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"load",                    (PyCFunction)script_load,                    METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plScriptI)

//-----------------------------------------------------------------------------
// [SECTION] constants
//-----------------------------------------------------------------------------

plPythonIntConstantPair gatScriptIntPairs[] = {
    PL_ADD_INT_CONSTANT(PL_SCRIPT_FLAG_NONE),
    PL_ADD_INT_CONSTANT(PL_SCRIPT_FLAG_PLAYING),
    PL_ADD_INT_CONSTANT(PL_SCRIPT_FLAG_PLAY_ONCE),
    PL_ADD_INT_CONSTANT(PL_SCRIPT_FLAG_RELOADABLE),
};
