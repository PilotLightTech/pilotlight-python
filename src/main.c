
//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>

// custom modules
#include "module_pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] forward declarations
//-----------------------------------------------------------------------------

int pl_init_python();

//-----------------------------------------------------------------------------
// [SECTION] entry point
//-----------------------------------------------------------------------------

int main()
{

	// add our custom module
	PyImport_AppendInittab("pilotlight._pilotlight", &PyInit__pilotlight);

	Py_NoSiteFlag = 1; // this must be set to 1
	Py_DontWriteBytecodeFlag = 1;

   if(pl_init_python() != 0)
        return -1;

	// initialize python
	Py_Initialize();
	if (!Py_IsInitialized())
	{
		printf("Error initializing Python interpreter\n");
		return 1;
	}

	PyObject* ptPilotLight = PyImport_ImportModule("pilotlight._pilotlight");

    const char* pcPath = "../sandbox/main.py";

    FILE* ptDataFile = fopen(pcPath, "rb");

    // obtain file size
    fseek(ptDataFile, 0, SEEK_END);
    size_t szSize = ftell(ptDataFile);
    fseek(ptDataFile, 0, SEEK_SET);

    // copy the file into the buffer:
    char* pcFileBuffer = malloc(szSize + 1);
    memset(pcFileBuffer, 0, szSize + 1);
    size_t szResult = fread(pcFileBuffer, sizeof(char), szSize, ptDataFile);
    if (szResult != szSize)
    {
        if (feof(ptDataFile))
            printf("Error reading test.bin: unexpected end of file\n");
        else if (ferror(ptDataFile)) {
            perror("Error reading test.bin");
        }
        return -1;
    }
    fclose(ptDataFile);

    int result = PyRun_SimpleString(pcFileBuffer);

	// check if error occurred
	if (!PyErr_Occurred())
		Py_XDECREF(ptPilotLight);
	else
		PyErr_Print();
}

//-----------------------------------------------------------------------------
// [SECTION] helpers
//-----------------------------------------------------------------------------

int
pl_init_python()
{
    PyInitConfig *config = PyInitConfig_Create();
    if (config == NULL) {
        printf("PYTHON INIT ERROR: memory allocation failed\n");
        return -1;
    }

    // Enable the Python Development Mode
    if (PyInitConfig_SetInt(config, "dev_mode", 1) < 0) {
        goto error;
    }

    char* const acModulePaths[] = {
        "../dependencies/cpython/PCbuild/amd64",
        "../dependencies/cpython/Lib",
        "../sandbox",
        ".."
    };

    if (PyInitConfig_SetStrList(config, "module_search_paths", 4, acModulePaths) < 0) {
        goto error;
    }

    // Initialize Python with the configuration
    if (Py_InitializeFromInitConfig(config) < 0) {
        goto error;
    }
    PyInitConfig_Free(config);
    return 0;

    error:
        {
            // Display the error message.
            //
            // This uncommon braces style is used, because you cannot make
            // goto targets point to variable declarations.
            const char *err_msg;
            (void)PyInitConfig_GetError(config, &err_msg);
            printf("PYTHON INIT ERROR: %s\n", err_msg);
            PyInitConfig_Free(config);
            return -1;
        }
}

//-----------------------------------------------------------------------------
// [SECTION] unity build
//-----------------------------------------------------------------------------

#include "module_pilotlight.c"
