/*
   main.c
     - embedded python application for testing, debugging, &
       general development of the python bindings
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

int pl_init_python();

//-----------------------------------------------------------------------------
// [SECTION] entry point
//-----------------------------------------------------------------------------

int main()
{

   if(pl_init_python() != 0)
   {
        printf("Error initializing Python interpreter\n");
        return -1;
   }

	// initialize python
	Py_Initialize();
	if (!Py_IsInitialized())
	{
		printf("Error initializing Python interpreter\n");
		return 1;
	}

    FILE* ptDataFile = fopen("../sandbox/app.py", "rb");

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
            printf("Error reading app.py: unexpected end of file\n");
        else if (ferror(ptDataFile))
            perror("Error reading app.py");
        return -1;
    }
    fclose(ptDataFile);

    int iResult = PyRun_SimpleString(pcFileBuffer);

	// check if error occurred
	if (PyErr_Occurred())
		PyErr_Print();
}

//-----------------------------------------------------------------------------
// [SECTION] helpers
//-----------------------------------------------------------------------------

int
pl_init_python()
{
    PyInitConfig* ptConfig = PyInitConfig_Create();
    if (ptConfig == NULL)
    {
        printf("PYTHON INIT ERROR: memory allocation failed\n");
        return -1;
    }

    // Enable the Python Development Mode
    if (PyInitConfig_SetInt(ptConfig, "dev_mode", 1) < 0)
    {
        // Display the error message.
        //
        // This uncommon braces style is used, because you cannot make
        // goto targets point to variable declarations.
        const char* pcErrMsg = NULL;
        (void)PyInitConfig_GetError(ptConfig, &pcErrMsg);
        printf("PYTHON INIT ERROR: %s\n", pcErrMsg);
        PyInitConfig_Free(ptConfig);
        return -1;
    }

    char* const acModulePaths[] = { 
    #ifdef _WIN32
        "../dependencies/cpython/PCbuild/amd64",


    #elif defined(__APPLE__)
    #else // linux
        // "../dependencies/cpython/build/debug/build/lib.linux-x86_64-3.14/",
    #endif
        "../dependencies/cpython/Lib",
        "..",
        "../sandbox"
    };

    if (PyInitConfig_SetStrList(ptConfig, "module_search_paths", PL_ARRAYSIZE(acModulePaths), acModulePaths) < 0)
    {
        // Display the error message.
        //
        // This uncommon braces style is used, because you cannot make
        // goto targets point to variable declarations.
        const char* pcErrMsg = NULL;
        (void)PyInitConfig_GetError(ptConfig, &pcErrMsg);
        printf("PYTHON INIT ERROR: %s\n", pcErrMsg);
        PyInitConfig_Free(ptConfig);
        return -1;
    }

    // Initialize Python with the configuration
    if (Py_InitializeFromInitConfig(ptConfig) < 0)
    {
        // Display the error message.
        //
        // This uncommon braces style is used, because you cannot make
        // goto targets point to variable declarations.
        const char* pcErrMsg = NULL;
        (void)PyInitConfig_GetError(ptConfig, &pcErrMsg);
        printf("PYTHON INIT ERROR: %s\n", pcErrMsg);
        PyInitConfig_Free(ptConfig);
        return -1;
    }
    PyInitConfig_Free(ptConfig);
    return 0;
}
