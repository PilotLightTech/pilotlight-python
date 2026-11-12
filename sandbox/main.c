/*
   main.c
     - embedded python application for testing, debugging, &
       general development of the python bindings
     - behaves like "python <app> [args...]": __file__, sys.argv, and
       sys.path[0] (the script's directory) are all set by Py_RunMain()

     usage: pilotlight_python [app.py] [args...]   (default: ../sandbox/app.py)
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] forward declarations
// [SECTION] entry point
// [SECTION] helpers
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>

#define PL_ARRAYSIZE(X) ((int)(sizeof(X) / sizeof(*(X))))

//-----------------------------------------------------------------------------
// [SECTION] forward declarations
//-----------------------------------------------------------------------------

int pl_init_python(const char* pcAppName, int iArgc, char* apcArgv[]);

//-----------------------------------------------------------------------------
// [SECTION] entry point
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{

    const char* pcAppName = "../sandbox/app.py";

    // remaining arguments are passed to the app (sys.argv[1:])
    int iAppArgc = 0;
    char** apcAppArgv = NULL;
    if(argc > 1)
    {
        pcAppName = argv[1];
        iAppArgc = argc - 2;
        apcAppArgv = &argv[2];
    }

    FILE* ptAppFile = fopen(pcAppName, "rb");
    if(ptAppFile == NULL)
    {
        printf("Error opening app: %s\n", pcAppName);
        return -1;
    }
    fclose(ptAppFile);

    if(pl_init_python(pcAppName, iAppArgc, apcAppArgv) != 0)
    {
        printf("Error initializing Python interpreter\n");
        return -1;
    }

    // runs the app like "python app.py" (prints uncaught exceptions,
    // handles SystemExit) then finalizes the interpreter
    return Py_RunMain();
}

//-----------------------------------------------------------------------------
// [SECTION] helpers
//-----------------------------------------------------------------------------

static int
pl__python_init_error(PyInitConfig* ptConfig)
{
    const char* pcErrMsg = NULL;
    (void)PyInitConfig_GetError(ptConfig, &pcErrMsg);
    printf("PYTHON INIT ERROR: %s\n", pcErrMsg);
    PyInitConfig_Free(ptConfig);
    return -1;
}

int
pl_init_python(const char* pcAppName, int iArgc, char* apcArgv[])
{
    PyInitConfig* ptConfig = PyInitConfig_Create();
    if (ptConfig == NULL)
    {
        printf("PYTHON INIT ERROR: memory allocation failed\n");
        return -1;
    }

    // Enable the Python Development Mode
    if (PyInitConfig_SetInt(ptConfig, "dev_mode", 1) < 0)
        return pl__python_init_error(ptConfig);

    char* const acModulePaths[] = {
    #ifdef _WIN32
        "../dependencies/cpython/PCbuild/amd64",


    #elif defined(__APPLE__)
        "../dependencies/cpython/build/debug/build/lib.macosx-26.3-arm64-3.14/",
    #else // linux
        // "../dependencies/cpython/build/debug/build/lib.linux-x86_64-3.14/",
    #endif
        "../dependencies/cpython/Lib",
        "..",
        "../examples",
        "../sandbox"
    };

    if (PyInitConfig_SetStrList(ptConfig, "module_search_paths", PL_ARRAYSIZE(acModulePaths), acModulePaths) < 0)
        return pl__python_init_error(ptConfig);

    // script to run with Py_RunMain() (sets __file__ & prepends the script's
    // directory to sys.path)
    if (PyInitConfig_SetStr(ptConfig, "run_filename", pcAppName) < 0)
        return pl__python_init_error(ptConfig);

    // the init config is isolated by default, which implies safe_path (the
    // script's directory isn't prepended to sys.path). Turn isolation off but
    // keep its other effects (ignore PYTHON* env vars & user site-packages).
    const char* apcIsolationOptions[] = {"isolated", "safe_path", "use_environment", "user_site_directory"};
    for(int i = 0; i < PL_ARRAYSIZE(apcIsolationOptions); i++)
    {
        if (PyInitConfig_SetInt(ptConfig, apcIsolationOptions[i], 0) < 0)
            return pl__python_init_error(ptConfig);
    }

    // sys.argv = [app, args...] (taken as-is, not parsed as python options)
    if (PyInitConfig_SetInt(ptConfig, "parse_argv", 0) < 0)
        return pl__python_init_error(ptConfig);

    char** apcPyArgv = malloc(sizeof(char*) * (size_t)(iArgc + 1));
    apcPyArgv[0] = (char*)pcAppName;
    for(int i = 0; i < iArgc; i++)
        apcPyArgv[i + 1] = apcArgv[i];
    const int iSetArgvResult = PyInitConfig_SetStrList(ptConfig, "argv", (size_t)(iArgc + 1), apcPyArgv);
    free(apcPyArgv);
    if (iSetArgvResult < 0)
        return pl__python_init_error(ptConfig);

    // Initialize Python with the configuration
    if (Py_InitializeFromInitConfig(ptConfig) < 0)
        return pl__python_init_error(ptConfig);

    PyInitConfig_Free(ptConfig);
    return 0;
}
