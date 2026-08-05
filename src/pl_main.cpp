/*
   pl_main.c
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] global data
// [SECTION] internal enums
// [SECTION] forward declarations
// [SECTION] window api
// [SECTION] glfw callbacks
// [SECTION] thread api
// [SECTION] library api
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#ifdef _WIN32
    const char* gpcLibraryExtension = "dll";
    const char* gpcLibraryPrefix    = "";
#elif defined(__APPLE__)
    const char* gpcLibraryExtension = "dylib";
    const char* gpcLibraryPrefix    = "lib";
#else // linux
    const char* gpcLibraryExtension = "so";
    const char* gpcLibraryPrefix    = "lib";
#endif

// platform specifics
#if defined(_WIN32)

    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

#elif defined(__APPLE__)

    #import <Cocoa/Cocoa.h>
    #import <Metal/Metal.h>
    #import <QuartzCore/CAMetalLayer.h>
    #include <time.h> // clock_gettime_nsec_np
    #include <sys/stat.h> // timespec
    #include <copyfile.h> // copyfile
    #include <dlfcn.h>    // dlopen, dlsym, dlclose
    #include <unistd.h> // close
    #include <fcntl.h> // O_RDONLY, O_WRONLY ,O_CREAT
    #include <pthread.h>

#else // linux

    #include <sys/sendfile.h> // sendfile
    #include <sys/stat.h> // stat, timespec
    #include <dlfcn.h> // dlopen, dlsym, dlclose
    #include <fcntl.h> // O_RDONLY, O_WRONLY ,O_CREAT
    #include <time.h> // clock_gettime, clock_getres
    #include <unistd.h> // usleep()
    #include <pthread.h>
    #include <X11/Xatom.h>

#endif

#include "pl_internal.h"

#define PL_SINGLE_UNIT_BUILD
#include "pl.c"

// extern plIO* gptIOCtx;

//-----------------------------------------------------------------------------
// [SECTION] global data
//-----------------------------------------------------------------------------

#ifdef _WIN32

    INT64 ilTime = 0;
    INT64 ilTicksPerSecond = 0;

#elif defined(__APPLE__)

    id<MTLDevice>  device;
    NSWindow*      nswin;
    CFTimeInterval gtTime;
    CAMetalLayer*  layer;
    static inline CFTimeInterval pl__get_absolute_time(void) { return (CFTimeInterval)((double)(clock_gettime_nsec_np(CLOCK_UPTIME_RAW)) / 1e9); }

#else // linux

    double gdTime      = 0.0;
    double gdFrequency = 0.0;

    static inline double
    pl__get_linux_absolute_time(void)
    {
        struct timespec ts;
        if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) 
        {
            assert(false && "clock_gettime() failed");
        }
        uint64_t nsec_count = ts.tv_nsec + ts.tv_sec * 1e9;
        return (double)nsec_count / gdFrequency;    
    }
#endif


//-----------------------------------------------------------------------------
// [SECTION] thread api
//-----------------------------------------------------------------------------

#ifdef _WIN32
typedef struct _plMutex
{
    HANDLE tHandle;
} plMutex;

void
pl_create_mutex(plMutex** ppMutexOut)
{
    HANDLE tHandle = CreateMutex(NULL, FALSE, NULL);
    if(tHandle)
    {
        (*ppMutexOut) = (plMutex*)malloc(sizeof(plMutex));
        (*ppMutexOut)->tHandle = tHandle;
    }
}

void
pl_destroy_mutex(plMutex** ptMutex)
{
    CloseHandle((*ptMutex)->tHandle);
    free((*ptMutex));
    (*ptMutex) = NULL;
}

void
pl_lock_mutex(plMutex* ptMutex)
{
    DWORD dwWaitResult = WaitForSingleObject(ptMutex->tHandle, INFINITE);
    PL_ASSERT(dwWaitResult == WAIT_OBJECT_0);
}

void
pl_unlock_mutex(plMutex* ptMutex)
{
    if(!ReleaseMutex(ptMutex->tHandle))
    {
        printf("ReleaseMutex error: %d\n", GetLastError());
        PL_ASSERT(false);
    }
}
#else // linux

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
// [SECTION] library api
//-----------------------------------------------------------------------------

plLibraryResult
pl_load_library(plLibraryDesc tDesc, plSharedLibrary** pptLibraryOut)
{
    return 0;
}

void
pl_reload_library(plSharedLibrary* ptLibrary)
{
}

void*
pl_load_library_function(plSharedLibrary* ptLibrary, const char* name)
{
    return NULL;
}

bool
pl_has_library_changed(plSharedLibrary* ptLibrary)
{
    return false;
}


extern "C" const plApiRegistryI*
pl__python_load(void)
{
    return pl__load_api_registry();
}

extern "C" void
pl__python_setup(void)
{

    const plApiRegistryI* ptApiRegistry = pl__load_api_registry();
    pl__load_core_apis();
    // pl_load_ext((plApiRegistryI*)ptApiRegistry, false);
    // pl_load_platform_ext((plApiRegistryI*)ptApiRegistry, false);

    gptDataRegistry = pl_get_api_latest(ptApiRegistry, plDataRegistryI);


    // setup timers
    #ifdef _WIN32
        QueryPerformanceFrequency((LARGE_INTEGER*)&ilTicksPerSecond);
        QueryPerformanceCounter((LARGE_INTEGER*)&ilTime);
    #elif defined(__APPLE__)
    #else // linux
    #endif

    gptIOCtx = gptIOI->get_io();
    gptIOCtx->bHotReloadActive = false;

    // command line args
    gptIOCtx->iArgc = 0;
    gptIOCtx->apArgv = NULL;
}

extern "C" void
pl__python_shutdown(void)
{
}

extern "C" int
pl__python_pre_update(void)
{
    #if defined(__APPLE__)
        @autoreleasepool
    #endif
    {
        // while queue has messages, remove and dispatch them (but do not block on empty queue)
        MSG tMsg = {0};
        while (PeekMessage(&tMsg, NULL, 0, 0, PM_REMOVE))
        {
            // check for quit because peekmessage does not signal this via return val
            if (tMsg.message == WM_QUIT)
            {
                gptIOCtx->bRunning = false;
                break;
            }
            // TranslateMessage will post auxilliary WM_CHAR messages from key msgs
            TranslateMessage(&tMsg);
            DispatchMessage(&tMsg);
        }

        if(gptIOCtx->platform_new_frame)
            gptIOCtx->platform_new_frame(gptIOCtx->pBackendPlatformData);

        pl__garbage_collect_data_reg();

        // update time step
        #ifdef _WIN32
            INT64 ilCurrentTime = 0;
            QueryPerformanceCounter((LARGE_INTEGER*)&ilCurrentTime);
            gptIOCtx->fDeltaTime = (float)(ilCurrentTime - ilTime) / ilTicksPerSecond;
            ilTime = ilCurrentTime;
        #elif defined(__APPLE__)
            if(gtTime == 0.0)
                gtTime = pl__get_absolute_time();
            double dCurrentTime = pl__get_absolute_time();
            gptIOCtx->fDeltaTime = (float)(dCurrentTime - gtTime);
            gtTime = dCurrentTime;
        #else // linux
            const double dCurrentTime = pl__get_linux_absolute_time();
            gptIOCtx->fDeltaTime = (float)(dCurrentTime - gdTime);
            gdTime = dCurrentTime;
        #endif
    }
    return 0;
}

#define PL_STRING_IMPLEMENTATION
#include "pl_string.h"
#undef PL_STRING_IMPLEMENTATION
