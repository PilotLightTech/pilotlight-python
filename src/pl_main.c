/*
   pl_main.c
     * python host "runtime" for Pilot Light
     * replaces the standard pilot_light executable (pl_main_xxx.c). Python owns the main loop, so this file only
       provides:
        - core api setup (pl.c)
        - os specifics required by pl.c (library stubs, mutexes)
        - a frame loop that drives the user's python app object
     * windowing/input is handled entirely by the platform extension
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] global data
// [SECTION] helpers
// [SECTION] python scripts
// [SECTION] library api (stubs, all extensions are statically linked)
// [SECTION] thread api
// [SECTION] python host api
// [SECTION] unity build
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pl_py_shared_rename.h" // avoid duplicate symbols with other units

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#elif defined(__APPLE__)
    #import <Cocoa/Cocoa.h>
    #include <time.h> // clock_gettime_nsec_np
    #include <pthread.h>
#else // linux
    #include <time.h> // clock_gettime, clock_getres
    #include <pthread.h>
#endif

#include "pl_internal.h"
#include "pl_platform_ext.h"

//-----------------------------------------------------------------------------
// [SECTION] global data
//-----------------------------------------------------------------------------

#ifdef _WIN32
    const char* gpcLibraryExtension = "dll";
    const char* gpcLibraryPrefix    = "";
    static INT64 gilTime            = 0;
    static INT64 gilTicksPerSecond  = 0;
#elif defined(__APPLE__)
    const char* gpcLibraryExtension = "dylib";
    const char* gpcLibraryPrefix    = "lib";
    static double gdTime            = 0.0;
#else // linux
    const char* gpcLibraryExtension = "so";
    const char* gpcLibraryPrefix    = "lib";
    static double gdTime            = 0.0;
    static double gdFrequency       = 0.0;
#endif

// python app object currently being run (borrowed while pl_run is active)
static PyObject* gptPyApp     = NULL;
static bool      gbPyAppError = false;
static bool      gbCoreLoaded = false;
static bool      gbExtsLoaded = false;

// defined in pl.c (included at bottom of file)
const plApiRegistryI* pl__load_api_registry(void);

// extension entry points (defined in other translation units)
void pl_load_ext            (plApiRegistryI*, bool);
void pl_unload_ext          (plApiRegistryI*, bool);
void pl_load_graphics_ext   (plApiRegistryI*, bool);
void pl_unload_graphics_ext (plApiRegistryI*, bool);
void pl_load_shader_ext     (plApiRegistryI*, bool);
void pl_unload_shader_ext   (plApiRegistryI*, bool);

// python scripts (defined in pl_script_ext_m.c, part of the pilotlight.c unit)
bool pl_python_script_load   (const char* name);
bool pl_python_script_unload (const char* name);
void pl_python_script_cleanup(void);

// original extension registry functions (from pl.c)
static bool (*gpfOriginalExtensionLoad)  (const char*, const char*, const char*, bool) = NULL;
static bool (*gpfOriginalExtensionUnload)(const char*) = NULL;

//-----------------------------------------------------------------------------
// [SECTION] helpers
//-----------------------------------------------------------------------------

static double
pl__py_get_time(void)
{
    #ifdef _WIN32
        INT64 ilCurrentTime = 0;
        QueryPerformanceCounter((LARGE_INTEGER*)&ilCurrentTime);
        return (double)ilCurrentTime / (double)gilTicksPerSecond;
    #elif defined(__APPLE__)
        return (double)clock_gettime_nsec_np(CLOCK_UPTIME_RAW) / 1e9;
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        uint64_t nsec_count = ts.tv_nsec + ts.tv_sec * 1e9;
        return (double)nsec_count / gdFrequency;
    #endif
}

// calls a method on the python app object, returns false on python error
static bool
pl__py_call_app(const char* pcMethod)
{
    if(gptPyApp == NULL || gbPyAppError)
        return false;

    PyObject* ptResult = PyObject_CallMethod(gptPyApp, pcMethod, NULL);
    if(ptResult == NULL)
    {
        PyErr_Print();
        gbPyAppError = true;
        gptIOCtx->bRunning = false;
        return false;
    }
    Py_DECREF(ptResult);
    return true;
}

// trampolines used by platform backends (i.e. during live resizing on win32)
static void
pl__py_app_update(void* pUnused)
{
    (void)pUnused;
    pl__py_call_app("pl_app_update");
}

static void
pl__py_app_resize(void* ptWindow, void* pUnused)
{
    (void)ptWindow;
    (void)pUnused;
    pl__py_call_app("pl_app_resize");
}

static void
pl__py_pump_os_events(void)
{
    #ifdef _WIN32
        MSG tMsg = {0};
        while (PeekMessage(&tMsg, NULL, 0, 0, PM_REMOVE))
        {
            if (tMsg.message == WM_QUIT)
            {
                gptIOCtx->bRunning = false;
                break;
            }
            TranslateMessage(&tMsg);
            DispatchMessage(&tMsg);
        }
    #elif defined(__APPLE__)
        @autoreleasepool
        {
            NSEvent* ptEvent = nil;
            while((ptEvent = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                untilDate:[NSDate distantPast]
                                                   inMode:NSDefaultRunLoopMode
                                                  dequeue:YES]))
            {
                [NSApp sendEvent:ptEvent];
            }
        }
    #else
        // x11 events are polled by the platform extension's new frame
    #endif
}

//-----------------------------------------------------------------------------
// [SECTION] python scripts
//-----------------------------------------------------------------------------

// The script extension loads a script component's code by asking the
// extension registry for a library named after the script (load function
// "pl_load_script"). Those requests are resolved to python modules instead.
// Everything else goes to the original extension registry.

static bool
pl__py_load_extension(const char* pcName, const char* pcLoadFunc, const char* pcUnloadFunc, bool bReloadable)
{
    if(pcLoadFunc && strcmp(pcLoadFunc, "pl_load_script") == 0)
        return pl_python_script_load(pcName);
    return gpfOriginalExtensionLoad(pcName, pcLoadFunc, pcUnloadFunc, bReloadable);
}

static bool
pl__py_unload_extension(const char* pcName)
{
    if(pl_python_script_unload(pcName))
        return true;
    return gpfOriginalExtensionUnload(pcName);
}

//-----------------------------------------------------------------------------
// [SECTION] library api (stubs, all extensions are statically linked)
//-----------------------------------------------------------------------------

plLibraryResult
pl_load_library(plLibraryDesc tDesc, plSharedLibrary** pptLibraryOut)
{
    (void)tDesc;
    (void)pptLibraryOut;
    return PL_LIBRARY_RESULT_FAIL;
}

void
pl_reload_library(plSharedLibrary* ptLibrary)
{
    (void)ptLibrary;
}

void*
pl_load_library_function(plSharedLibrary* ptLibrary, const char* pcName)
{
    (void)ptLibrary;
    (void)pcName;
    return NULL;
}

bool
pl_has_library_changed(plSharedLibrary* ptLibrary)
{
    (void)ptLibrary;
    return false;
}

//-----------------------------------------------------------------------------
// [SECTION] thread api
//-----------------------------------------------------------------------------

#ifdef _WIN32

typedef struct _plMutex
{
    HANDLE tHandle;
} plMutex;

void
pl_create_mutex(plMutex** pptMutexOut)
{
    HANDLE tHandle = CreateMutex(NULL, FALSE, NULL);
    if(tHandle)
    {
        (*pptMutexOut) = (plMutex*)malloc(sizeof(plMutex));
        (*pptMutexOut)->tHandle = tHandle;
    }
}

void
pl_destroy_mutex(plMutex** pptMutex)
{
    CloseHandle((*pptMutex)->tHandle);
    free((*pptMutex));
    (*pptMutex) = NULL;
}

void
pl_lock_mutex(plMutex* ptMutex)
{
    DWORD dwWaitResult = WaitForSingleObject(ptMutex->tHandle, INFINITE);
    PL_ASSERT(dwWaitResult == WAIT_OBJECT_0);
    (void)dwWaitResult;
}

void
pl_unlock_mutex(plMutex* ptMutex)
{
    if(!ReleaseMutex(ptMutex->tHandle))
    {
        printf("ReleaseMutex error: %lu\n", GetLastError());
        PL_ASSERT(false);
    }
}

#else // linux & macos

typedef struct _plMutex
{
    pthread_mutex_t tHandle;
} plMutex;

void
pl_create_mutex(plMutex** pptMutexOut)
{
    *pptMutexOut = (plMutex*)malloc(sizeof(plMutex));
    if(pthread_mutex_init(&(*pptMutexOut)->tHandle, NULL)) //-V522
    {
        PL_ASSERT(false);
    }
}

void
pl_lock_mutex(plMutex* ptMutex)
{
    pthread_mutex_lock(&ptMutex->tHandle);
}

void
pl_unlock_mutex(plMutex* ptMutex)
{
    pthread_mutex_unlock(&ptMutex->tHandle);
}

void
pl_destroy_mutex(plMutex** pptMutex)
{
    pthread_mutex_destroy(&(*pptMutex)->tHandle);
    free((*pptMutex));
    *pptMutex = NULL;
}

#endif

//-----------------------------------------------------------------------------
// [SECTION] python host api
//-----------------------------------------------------------------------------

const plApiRegistryI*
pl__python_load(void)
{
    return pl__load_api_registry();
}

// loads core apis & all statically linked extensions (safe to call multiple times)
void
pl__python_setup(void)
{
    plApiRegistryI* ptApiRegistry = (plApiRegistryI*)pl__load_api_registry();

    if(!gbCoreLoaded)
    {
        pl__load_core_apis();
        gbCoreLoaded = true;

        // route script loads to python modules (see [SECTION] python scripts)
        // (the registry hands out a pointer to its stored copy of the API,
        //  so patching it here is seen by every extension)
        plExtensionRegistryI* ptExtensionRegistry = (plExtensionRegistryI*)pl_get_api_latest(ptApiRegistry, plExtensionRegistryI);
        gpfOriginalExtensionLoad   = ptExtensionRegistry->load;
        gpfOriginalExtensionUnload = ptExtensionRegistry->unload;
        ptExtensionRegistry->load   = pl__py_load_extension;
        ptExtensionRegistry->unload = pl__py_unload_extension;

        // setup timers
        #ifdef _WIN32
            QueryPerformanceFrequency((LARGE_INTEGER*)&gilTicksPerSecond);
        #elif defined(__APPLE__)
        #else // linux
            static struct timespec ts;
            if (clock_getres(CLOCK_MONOTONIC, &ts) != 0)
            {
                PL_ASSERT(false && "clock_getres() failed");
            }
            gdFrequency = 1e9/((double)ts.tv_nsec + (double)ts.tv_sec * (double)1e9);
        #endif
    }

    gptIOCtx = gptIOI->get_io();
    gptIOCtx->bHotReloadActive = false;
    gptIOCtx->iArgc = 0;
    gptIOCtx->apArgv = NULL;

    if(!gbExtsLoaded)
    {
        #ifdef __APPLE__
            // windows created by the platform extension require NSApp
            [NSApplication sharedApplication];
            [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        #endif

        pl_load_platform_ext(ptApiRegistry, false);
        pl_load_shader_ext(ptApiRegistry, false);
        pl_load_graphics_ext(ptApiRegistry, false);
        pl_load_ext(ptApiRegistry, false);
        gbExtsLoaded = true;
    }
}

// runs the python application (blocks until app exits)
//   returns 0 on success, non-zero if the app raised an exception
int
pl__python_run(PyObject* ptApp)
{
    pl__python_setup();

    gptPyApp = ptApp;
    gbPyAppError = false;
    gptIOCtx->bRunning = true;
    gptIOCtx->_bFirstLoadComplete = false;
    gptIOCtx->pl_app_update = pl__py_app_update;
    gptIOCtx->pl_app_resize = pl__py_app_resize;
    gptIOCtx->pAppUserData  = NULL;

    if(!pl__py_call_app("pl_app_load"))
    {
        gptPyApp = NULL;
        return 1;
    }

    if(gptIOCtx->platform_setup)
        gptIOCtx->pBackendPlatformData = gptIOCtx->platform_setup();
    gptIOCtx->_bFirstLoadComplete = true;

    if(gbApisDirty)
        pl__check_apis();

    #ifdef __APPLE__
        [NSApp finishLaunching];
        [NSApp activateIgnoringOtherApps:YES];
    #endif

    #ifdef _WIN32
        QueryPerformanceCounter((LARGE_INTEGER*)&gilTime);
    #else
        gdTime = pl__py_get_time();
    #endif

    // main loop
    while(gptIOCtx->bRunning)
    {
        pl__py_pump_os_events();

        if(!gptIOCtx->bRunning)
            break;

        if(gptIOCtx->platform_new_frame)
            gptIOCtx->platform_new_frame(gptIOCtx->pBackendPlatformData);

        if(gbPyAppError)
            break;

        // update time step
        #ifdef _WIN32
            INT64 ilCurrentTime = 0;
            QueryPerformanceCounter((LARGE_INTEGER*)&ilCurrentTime);
            gptIOCtx->fDeltaTime = (float)(ilCurrentTime - gilTime) / gilTicksPerSecond;
            gilTime = ilCurrentTime;
        #else
            const double dCurrentTime = pl__py_get_time();
            gptIOCtx->fDeltaTime = (float)(dCurrentTime - gdTime);
            gdTime = dCurrentTime;
        #endif

        pl__garbage_collect_data_reg();

        if(gptIOCtx->bViewportMinimized)
        {
            #ifdef _WIN32
                Sleep(10);
            #else
                struct timespec tSleep = {0, 10 * 1000000};
                nanosleep(&tSleep, NULL);
            #endif
        }
        else
            pl__py_call_app("pl_app_update");

        if(gbApisDirty)
            pl__check_apis();
    }

    // app cleanup (always attempt, even after an error)
    bool bHadError = gbPyAppError;
    gbPyAppError = false;
    pl__py_call_app("pl_app_shutdown");
    bHadError = bHadError || gbPyAppError;

    gptIOCtx->pl_app_update = NULL;
    gptIOCtx->pl_app_resize = NULL;
    gptPyApp = NULL;
    return bHadError ? 1 : 0;
}

void
pl__python_shutdown(void)
{
    if(!gbExtsLoaded)
        return;

    plApiRegistryI* ptApiRegistry = (plApiRegistryI*)pl__load_api_registry();

    if(gptIOCtx->platform_cleanup)
        gptIOCtx->platform_cleanup(gptIOCtx->pBackendPlatformData);
    gptIOCtx->platform_setup     = NULL;
    gptIOCtx->platform_new_frame = NULL;
    gptIOCtx->platform_cleanup   = NULL;

    pl_python_script_cleanup();
    pl_unload_ext(ptApiRegistry, false);
    pl_unload_graphics_ext(ptApiRegistry, false);
    pl_unload_shader_ext(ptApiRegistry, false);
    pl_unload_platform_ext(ptApiRegistry, false);
    gbExtsLoaded = false;
}

//-----------------------------------------------------------------------------
// [SECTION] unity build
//-----------------------------------------------------------------------------

#include "pl.c"
