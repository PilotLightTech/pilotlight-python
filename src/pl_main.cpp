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

#define PL_EXPERIMENTAL
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
    #define GLFW_EXPOSE_NATIVE_WIN32

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
    #define GLFW_EXPOSE_NATIVE_COCOA

#else // linux

    #include <sys/sendfile.h> // sendfile
    #include <sys/stat.h> // stat, timespec
    #include <dlfcn.h> // dlopen, dlsym, dlclose
    #include <fcntl.h> // O_RDONLY, O_WRONLY ,O_CREAT
    #include <time.h> // clock_gettime, clock_getres
    #include <unistd.h> // usleep()
    #include <pthread.h>
    #include <X11/Xatom.h>
    #define GLFW_EXPOSE_NATIVE_X11

#endif

// imgui
#include "imgui.h"
#include "imgui_internal.h" // ImLerp
#include "imgui_impl_glfw.h"

// glfw
#include <GLFW/glfw3.h>
#include "GLFW/glfw3native.h"
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

// glfw
GLFWwindow* gptGlfwWindow;
GLFWwindow* ptMouseWindow;
GLFWcursor* atMouseCursors[PL_MOUSE_CURSOR_COUNT];
bool        bMouseIgnoreButtonUpWaitForFocusLoss;
bool        bMouseIgnoreButtonUp;
plVec2      tLastValidMousePos;
GLFWwindow* atKeyOwnerWindows[GLFW_KEY_LAST];

//-----------------------------------------------------------------------------
// [SECTION] internal enums
//-----------------------------------------------------------------------------

enum _plWindowInternalFlags
{
    PL_WINDOW_INTERNAL_FLAG_NONE                            = 0,
    PL_WINDOW_INTERNAL_FLAG_MAXIMIZE_REQUESTED              = 1 << 0,
    PL_WINDOW_INTERNAL_FLAG_MINIMIZE_REQUESTED              = 1 << 1,
    PL_WINDOW_INTERNAL_FLAG_RESIZE_REQUESTED                = 1 << 2,
    PL_WINDOW_INTERNAL_FLAG_FULL_SCREEN_REQUESTED           = 1 << 3,
};

//-----------------------------------------------------------------------------
// [SECTION] forward declarations
//-----------------------------------------------------------------------------

void  pl_glfw_error_callback           (int error, const char* description);
void  pl_glfw_mouse_button_callback    (GLFWwindow*, int button, int action, int mods);
void  pl_glfw_mouse_pos_callback       (GLFWwindow*, double x, double y);
void  pl_glfw_cursor_enter_callback    (GLFWwindow*, int entered);
void  pl_glfw_window_focus_callback    (GLFWwindow*, int focused);
void  pl_glfw_scroll_callback          (GLFWwindow*, double xoffset, double yoffset);
void  pl_glfw_char_callback            (GLFWwindow*, unsigned int c);
void  pl_glfw_key_callback             (GLFWwindow*, int keycode, int scancode, int action, int mods);
void  pl_glfw_size_callback            (GLFWwindow*, int width, int height);
void  pl_glfw_window_iconified_callback(GLFWwindow*, int iconified);
void  pl_glfw_window_close_callback    (GLFWwindow*);
plKey pl_glfw_key_translate            (int keycode, int scancode);

//-----------------------------------------------------------------------------
// [SECTION] window api
//-----------------------------------------------------------------------------

bool 
pl_set_window_attribute(plWindow* ptWindow, plWindowAttribute tAttribute, const plWindowAttributeValue* ptValue)
{
    GLFWwindow* ptGlfwWindow = (GLFWwindow*)ptWindow->_pBackendData2;
    switch (tAttribute)
    {
        case PL_WINDOW_ATTRIBUTE_SIZE:
        {
            ptWindow->_tInternalFlags |= PL_WINDOW_INTERNAL_FLAG_RESIZE_REQUESTED;
            // ptWindow->_tInternalFlags |= PL_WINDOW_INTERNAL_FLAG_ATTRIBUTE_CHANGE_REQUESTED; // incase full screen
            ptWindow->_uRequestedWidth = ptValue->tuVec2.x;
            ptWindow->_uRequestedHeight = ptValue->tuVec2.y;
            glfwGetWindowPos(ptGlfwWindow, &ptWindow->_iXPos, &ptWindow->_iYPos);
            break;
        }

        case PL_WINDOW_ATTRIBUTE_POSITION:
        {
            glfwSetWindowPos(ptGlfwWindow, ptValue->tiVec2.x, ptValue->tiVec2.y);
            break;
        }
        
        case PL_WINDOW_ATTRIBUTE_MAXIMIZED:
            ptWindow->_tInternalFlags |= PL_WINDOW_INTERNAL_FLAG_MAXIMIZE_REQUESTED;
            break;

        case PL_WINDOW_ATTRIBUTE_MINIMIZED:
            if(ptValue->bValue)
                ptWindow->_tInternalFlags |= PL_WINDOW_INTERNAL_FLAG_MINIMIZE_REQUESTED;
            else
            {
                ptWindow->_tInternalFlags &= ~PL_WINDOW_INTERNAL_FLAG_MINIMIZE_REQUESTED;
                glfwRestoreWindow(ptGlfwWindow);
            }
            break;

        case PL_WINDOW_ATTRIBUTE_VISIBLE:
            if(ptValue->bValue)
                glfwShowWindow(ptGlfwWindow);
            else
                glfwHideWindow(ptGlfwWindow);
            break;

        case PL_WINDOW_ATTRIBUTE_FOCUSED:
            glfwFocusWindow(ptGlfwWindow);
            break;

        case PL_WINDOW_ATTRIBUTE_SIZE_LIMITS:
            glfwSetWindowSizeLimits(ptGlfwWindow, (int)ptValue->tuVec4.x, (int)ptValue->tuVec4.y, (int)ptValue->tuVec4.z, (int)ptValue->tuVec4.w);
            break;

        default:
            break;
    }
    return true;
}

bool
pl_get_window_attribute(plWindow* ptWindow, plWindowAttribute tAttribute, plWindowAttributeValue* ptValue)
{
    GLFWwindow* ptGlfwWindow = (GLFWwindow*)ptWindow->_pBackendData2;
    switch (tAttribute)
    {
        case PL_WINDOW_ATTRIBUTE_SIZE:
        {
            int iWidth = 0;
            int iHeight = 0;
            glfwGetWindowSize(ptGlfwWindow, &iWidth, &iHeight);
            ptValue->tuVec2.x = (uint32_t)iWidth;
            ptValue->tuVec2.y = (uint32_t)iHeight;
            break;
        }
        
        case PL_WINDOW_ATTRIBUTE_POSITION:
        {
            glfwGetWindowPos(ptGlfwWindow, &ptValue->tiVec2.x, &ptValue->tiVec2.y);
            break;
        }

        case PL_WINDOW_ATTRIBUTE_RESIZABLE:
        {
            if (glfwGetWindowAttrib(ptGlfwWindow, GLFW_RESIZABLE))
                ptValue->bValue = true;
            else 
                ptValue->bValue = false;
            break;
        }

        case PL_WINDOW_ATTRIBUTE_FOCUSED:
        {
            if (glfwGetWindowAttrib(ptGlfwWindow, GLFW_FOCUSED))
                ptValue->bValue = true;
            else 
                ptValue->bValue = false;
            break;
        }

        case PL_WINDOW_ATTRIBUTE_DECORATED:
        {
            if (glfwGetWindowAttrib(ptGlfwWindow, GLFW_DECORATED))
                ptValue->bValue = true;
            else 
                ptValue->bValue = false;
            break;
        }

        case PL_WINDOW_ATTRIBUTE_TOP_MOST:
        {
            if (glfwGetWindowAttrib(ptGlfwWindow, GLFW_FLOATING))
                ptValue->bValue = true;
            else 
                ptValue->bValue = false;
            break;
        }

        case PL_WINDOW_ATTRIBUTE_HOVERED:
        {
            if (glfwGetWindowAttrib(ptGlfwWindow, GLFW_HOVERED))
                ptValue->bValue = true;
            else 
                ptValue->bValue = false;
            break;
        }

        case PL_WINDOW_ATTRIBUTE_MAXIMIZED:
        {
            if (glfwGetWindowAttrib(ptGlfwWindow, GLFW_MAXIMIZED))
                ptValue->bValue = true;
            else 
                ptValue->bValue = false;
            break;
        }

        case PL_WINDOW_ATTRIBUTE_MINIMIZED:
        {
            if (glfwGetWindowAttrib(ptGlfwWindow, GLFW_ICONIFIED))
                ptValue->bValue = true;
            else 
                ptValue->bValue = false;
            break;
        }

        case PL_WINDOW_ATTRIBUTE_VISIBLE:
        {
            if (glfwGetWindowAttrib(ptGlfwWindow, GLFW_VISIBLE))
                ptValue->bValue = true;
            else 
                ptValue->bValue = false;
            break;
        }

        case PL_WINDOW_ATTRIBUTE_SIZE_LIMITS:
            return false;

        default:
            break;
    }
    return true;
}

bool
pl_set_cursor_mode(plWindow* ptWindow, plCursorMode tMode)
{
    GLFWwindow* ptGlfwWindow = (GLFWwindow*)ptWindow->_pBackendData2;
    switch(tMode)
    {
        case PL_CURSOR_MODE_NORMAL:
            glfwSetInputMode(ptGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        case PL_CURSOR_MODE_HIDDEN:
            glfwSetInputMode(ptGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            break;

        case PL_CURSOR_MODE_CAPTURED:
            glfwSetInputMode(ptGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
            break;

        case PL_CURSOR_MODE_DISABLED:
            glfwSetInputMode(ptGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;

        default:
            return false;
            break;
    }
    return true;
}

plCursorMode
pl_get_cursor_mode(plWindow* ptWindow)
{
    GLFWwindow* ptGlfwWindow = (GLFWwindow*)ptWindow->_pBackendData2;
    int iMode = glfwGetInputMode(ptGlfwWindow, GLFW_CURSOR);
    switch(iMode)
    {
        case GLFW_CURSOR_NORMAL: return PL_CURSOR_MODE_NORMAL;
        case GLFW_CURSOR_HIDDEN: return PL_CURSOR_MODE_HIDDEN;
        case GLFW_CURSOR_CAPTURED: return PL_CURSOR_MODE_CAPTURED;
        case GLFW_CURSOR_DISABLED: return PL_CURSOR_MODE_DISABLED;
        default:
            return PL_CURSOR_MODE_NORMAL;
    }
}

void
pl_show_window(plWindow* ptWindow)
{
    GLFWwindow* ptGlfwWindow = (GLFWwindow*)ptWindow->_pBackendData2;
    glfwShowWindow(ptGlfwWindow);
}

void
pl_full_screen_window(plWindow* ptWindow, int iMonitor)
{
    GLFWwindow* ptGlfwWindow = (GLFWwindow*)ptWindow->_pBackendData2;
    ptWindow->_tInternalFlags |= PL_WINDOW_INTERNAL_FLAG_FULL_SCREEN_REQUESTED;
    ptWindow->_iRequestedMonitor = iMonitor;
}

bool
pl_set_raw_mouse_input(plWindow* ptWindow, bool bValue)
{
    GLFWwindow* ptGlfwWindow = (GLFWwindow*)ptWindow->_pBackendData2;
    if (glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(ptGlfwWindow, GLFW_RAW_MOUSE_MOTION, bValue ? GLFW_TRUE : GLFW_FALSE);
        return true;
    }
    return false;
}

bool
pl_set_fullscreen(plWindow* ptWindow, const plFullScreenDesc* tDesc)
{
    GLFWwindow* ptGlfwWindow = (GLFWwindow*)ptWindow->_pBackendData2;
    switch (tDesc->tMode)
    {
        case PL_FULLSCREEN_MODE_EXCLUSIVE:
            ptWindow->_tInternalFlags |= PL_WINDOW_INTERNAL_FLAG_FULL_SCREEN_REQUESTED;
            ptWindow->_iRequestedMonitor = tDesc->iMonitor;
            return true;

        case PL_FULLSCREEN_MODE_BORDERLESS:
            return false;

        case PL_FULLSCREEN_MODE_NONE:
            ptWindow->_tInternalFlags |= PL_WINDOW_INTERNAL_FLAG_RESIZE_REQUESTED;
            ptWindow->_uRequestedWidth = 500;
            ptWindow->_uRequestedHeight = 500;
            return true;
        
        default:
            return true;
    }
}

const plWindowCapabilities*
pl_get_window_capabilities(void)
{
    static plWindowCapabilities tCapabilities = {};

    tCapabilities.uCursorModeCount = (uint32_t)PL_CURSOR_MODE_COUNT;
    tCapabilities.uAttributeCount = (uint32_t)PL_WINDOW_ATTRIBUTE_COUNT;
    tCapabilities.uFullScreenModeCount = 2;

    static const plWindowAttribute atSupportedAttributes[] = {
        PL_WINDOW_ATTRIBUTE_SIZE,
        PL_WINDOW_ATTRIBUTE_POSITION,
        PL_WINDOW_ATTRIBUTE_RESIZABLE,
        PL_WINDOW_ATTRIBUTE_DECORATED,
        PL_WINDOW_ATTRIBUTE_TOP_MOST,
        PL_WINDOW_ATTRIBUTE_MINIMIZED,
        PL_WINDOW_ATTRIBUTE_MAXIMIZED,
        PL_WINDOW_ATTRIBUTE_VISIBLE,
        PL_WINDOW_ATTRIBUTE_FOCUSED,
        PL_WINDOW_ATTRIBUTE_HOVERED
    };

    static const plCursorMode atSupportedCursorModes[] = {
        PL_CURSOR_MODE_NORMAL,
        PL_CURSOR_MODE_HIDDEN,
        PL_CURSOR_MODE_CAPTURED,
        PL_CURSOR_MODE_DISABLED
    };

    static const plFullScreenMode atSupportedScreenModes[] = {
        PL_FULLSCREEN_MODE_NONE,
        PL_FULLSCREEN_MODE_EXCLUSIVE
    };

    tCapabilities.atCursorModes = atSupportedCursorModes;
    tCapabilities.atFullScreenModes = atSupportedScreenModes;
    tCapabilities.atWindowAttributes = atSupportedAttributes;
    tCapabilities.tFlags = PL_WINDOW_CAPABILITY_FLAGS_RAW_MOUSE_INPUT;

    return &tCapabilities;
}

plWindowResult
pl_create_window(plWindowDesc tDesc, plWindow** pptWindowOut)
{
    plWindow* ptWindow = (plWindow*)malloc(sizeof(plWindow));
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_FLOATING,  GLFW_FALSE);
    glfwWindowHint(GLFW_POSITION_X,  tDesc.iXPos);
    glfwWindowHint(GLFW_POSITION_Y,  tDesc.iYPos);

    gptGlfwWindow = glfwCreateWindow((int)tDesc.uWidth, (int)tDesc.uHeight, tDesc.pcTitle, NULL, NULL);

    ptWindow->_pBackendData2 = gptGlfwWindow;

    if(gptMainWindow == NULL)
        gptMainWindow = ptWindow;
    
    glfwSetMouseButtonCallback(gptGlfwWindow, pl_glfw_mouse_button_callback);
    glfwSetCursorPosCallback(gptGlfwWindow, pl_glfw_mouse_pos_callback);
    glfwSetCursorEnterCallback(gptGlfwWindow, pl_glfw_cursor_enter_callback);
    glfwSetScrollCallback(gptGlfwWindow, pl_glfw_scroll_callback);
    glfwSetCharCallback(gptGlfwWindow, pl_glfw_char_callback);
    glfwSetKeyCallback(gptGlfwWindow, pl_glfw_key_callback);
    glfwSetWindowFocusCallback(gptGlfwWindow, pl_glfw_window_focus_callback);
    glfwSetWindowSizeCallback(gptGlfwWindow, pl_glfw_size_callback);
    glfwSetWindowIconifyCallback(gptGlfwWindow, pl_glfw_window_iconified_callback);
    glfwSetWindowCloseCallback(gptGlfwWindow, pl_glfw_window_close_callback);

    #ifdef PL_METAL_BACKEND
    if(pl_sb_size(gsbtWindows) == 0)
        ImGui_ImplGlfw_InitForOther(gptGlfwWindow, true);
    #else
    if(pl_sb_size(gsbtWindows) == 0)
        ImGui_ImplGlfw_InitForVulkan(gptGlfwWindow, true);
    #endif

    #ifdef _WIN32
    HWND tHandle = glfwGetWin32Window(gptGlfwWindow);
    ptWindow->_pBackendData = tHandle;
    #elif defined(__APPLE__)
    device = MTLCreateSystemDefaultDevice();
    gptIOCtx->pBackendPlatformData = device;

    nswin = glfwGetCocoaWindow(gptGlfwWindow);
    layer = [CAMetalLayer layer];
    layer.device = device;
    layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    nswin.contentView.layer = layer;
    nswin.contentView.wantsLayer = YES;

    typedef struct _plWindowData
    {
        void*           ptWindow;
        void* ptViewController;
        CAMetalLayer* ptLayer;
    } plWindowData;

    plWindowData* ptData = (plWindowData*)malloc(sizeof(plWindowData));
    ptData->ptLayer = layer;
    ptWindow->_pBackendData = ptData;

    #else // linux
    typedef struct plPlatformData
    {
        uint32_t header;
        Display* dpy;
        Window window;
    } plPlatformData;
    static plPlatformData tPlatformData = {UINT32_MAX};
    tPlatformData.dpy = glfwGetX11Display();
    tPlatformData.window = glfwGetX11Window(gptGlfwWindow);
    ptWindow->_pBackendData = &tPlatformData;
    #endif
    *pptWindowOut = ptWindow;
    pl_sb_push(gsbtWindows, ptWindow);
    return PL_WINDOW_RESULT_SUCCESS;
}

void
pl_destroy_window(plWindow* ptWindow)
{
    free(ptWindow);
}

//-----------------------------------------------------------------------------
// [SECTION] glfw callbacks
//-----------------------------------------------------------------------------

void
pl_glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // if(ImGui::GetIO().WantCaptureMouse)
    //     return;

    // update key modifiers
    gptIOI->add_key_event(PL_KEY_MOD_CTRL,  (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS));
    gptIOI->add_key_event(PL_KEY_MOD_SHIFT, (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)   == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)   == GLFW_PRESS));
    gptIOI->add_key_event(PL_KEY_MOD_ALT,   (glfwGetKey(window, GLFW_KEY_LEFT_ALT)     == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_ALT)     == GLFW_PRESS));
    gptIOI->add_key_event(PL_KEY_MOD_SUPER, (glfwGetKey(window, GLFW_KEY_LEFT_SUPER)   == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SUPER)   == GLFW_PRESS));

    plIO* ptIO = gptIOI->get_io();
    if (button >= 0 && button < PL_MOUSE_BUTTON_COUNT)
        gptIOI->add_mouse_button_event(button, action == GLFW_PRESS);
}

void
pl_glfw_mouse_pos_callback(GLFWwindow* window, double x, double y)
{
    gptIOI->add_mouse_pos_event((float)x, (float)y);
    tLastValidMousePos.x = (float)x;
    tLastValidMousePos.y = (float)y;
}

void
pl_glfw_cursor_enter_callback(GLFWwindow* window, int entered)
{
    // if(ImGui::GetIO().WantCaptureMouse)
    //     return;

    plIO* ptIO = gptIOI->get_io();
    if (entered)
    {
        ptMouseWindow = window;
        gptIOI->add_mouse_pos_event(tLastValidMousePos.x, tLastValidMousePos.y);
    }
    else if (ptMouseWindow == window)
    {
        tLastValidMousePos = ptIO->_tMousePos;
        ptMouseWindow = NULL;
        gptIOI->add_mouse_pos_event(-FLT_MAX, -FLT_MAX);
    }
}

void
pl_glfw_window_focus_callback(GLFWwindow* window, int focused)
{
    // Workaround for Linux: when losing focus with bMouseIgnoreButtonUpWaitForFocusLoss set, we will temporarily ignore subsequent Mouse Up events
    bMouseIgnoreButtonUp = (bMouseIgnoreButtonUpWaitForFocusLoss && focused == 0);
    bMouseIgnoreButtonUpWaitForFocusLoss = false;

    // gptIOI->add_focus_event(focused != 0);
}

void
pl_glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // if(ImGui::GetIO().WantCaptureMouse)
    //     return;
    gptIOI->add_mouse_wheel_event((float)xoffset, (float)yoffset);
}

void
pl_glfw_char_callback(GLFWwindow* window, unsigned int c)
{
    // if(ImGui::GetIO().WantTextInput)
    //     return;
    gptIOI->add_text_event(c);
}

int
pl_translate_untranslated_key(int key, int scancode)
{
    if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_EQUAL)
        return key;

        GLFWerrorfun prev_error_callback = glfwSetErrorCallback(NULL);
        const char* key_name = glfwGetKeyName(key, scancode);
        glfwSetErrorCallback(prev_error_callback);
        (void)glfwGetError(NULL);

        if (key_name && key_name[0] != 0 && key_name[1] == 0)
        {
            const char char_names[] = "`-=[]\\,;\'./";
            const int char_keys[] = { GLFW_KEY_GRAVE_ACCENT, GLFW_KEY_MINUS, GLFW_KEY_EQUAL, GLFW_KEY_LEFT_BRACKET, GLFW_KEY_RIGHT_BRACKET, GLFW_KEY_BACKSLASH, GLFW_KEY_COMMA, GLFW_KEY_SEMICOLON, GLFW_KEY_APOSTROPHE, GLFW_KEY_PERIOD, GLFW_KEY_SLASH, 0 };
            PL_ASSERT(PL_ARRAYSIZE(char_names) == PL_ARRAYSIZE(char_keys));
            if (key_name[0] >= '0' && key_name[0] <= '9')               { key = GLFW_KEY_0 + (key_name[0] - '0'); }
            else if (key_name[0] >= 'A' && key_name[0] <= 'Z')          { key = GLFW_KEY_A + (key_name[0] - 'A'); }
            else if (key_name[0] >= 'a' && key_name[0] <= 'z')          { key = GLFW_KEY_A + (key_name[0] - 'a'); }
            else
            {
                const char* p = strchr(char_names, key_name[0]);
                if(p)
                {
                    key = char_keys[p - char_names];
                }
            }

        }
        // if (action == GLFW_PRESS) printf("key %d scancode %d name '%s'\n", key, scancode, key_name);
    return key;
}

void
pl_glfw_size_callback(GLFWwindow* window, int width, int height)
{
    gptIOCtx->bViewportSizeChanged = true;
    if(width == 0 || height == 0)
    {
        gptIOCtx->bViewportMinimized = true;
    }
    else
    {
        int fwidth, fheight;
        glfwGetFramebufferSize(window, &fwidth, &fheight);
        gptIOCtx->tMainFramebufferScale.x = (float)fwidth / (float)width;
        gptIOCtx->tMainFramebufferScale.y = (float)fheight / (float)height;

        gptIOCtx->bViewportMinimized = false;
        gptIOCtx->tMainViewportSize.x = (float)width;
        gptIOCtx->tMainViewportSize.y = (float)height;
        // gsbtWindows[0]->tDesc.uWidth = (uint32_t)width;
        // gsbtWindows[0]->tDesc.uHeight = (uint32_t)height;

        // pl__app_resize(gptMainWindow, gpUserData);
    }
}

void
pl_glfw_window_iconified_callback(GLFWwindow* window, int iconified)
{
    gptIOCtx->bViewportMinimized = iconified;
}

void
pl_glfw_window_close_callback(GLFWwindow* window)
{
    gptIOCtx->bRunning = false;
}

void
pl_glfw_key_callback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_RELEASE)
        return;

    // if(ImGui::GetIO().WantCaptureKeyboard)
    //     return;

    // update key modifiers
    gptIOI->add_key_event(PL_KEY_MOD_CTRL,  (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS));
    gptIOI->add_key_event(PL_KEY_MOD_SHIFT, (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)   == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)   == GLFW_PRESS));
    gptIOI->add_key_event(PL_KEY_MOD_ALT,   (glfwGetKey(window, GLFW_KEY_LEFT_ALT)     == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_ALT)     == GLFW_PRESS));
    gptIOI->add_key_event(PL_KEY_MOD_SUPER, (glfwGetKey(window, GLFW_KEY_LEFT_SUPER)   == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SUPER)   == GLFW_PRESS));

    if (keycode >= 0 && keycode < PL_ARRAYSIZE(atKeyOwnerWindows))
        atKeyOwnerWindows[keycode] = (action == GLFW_PRESS) ? window : NULL;

    keycode = pl_translate_untranslated_key(keycode, scancode);

    plKey imgui_key = pl_glfw_key_translate(keycode, scancode);
    gptIOI->add_key_event(imgui_key, (action == GLFW_PRESS));
}

plKey
pl_glfw_key_translate(int keycode, int scancode)
{
    switch (keycode)
    {
        case GLFW_KEY_TAB: return PL_KEY_TAB;
        case GLFW_KEY_LEFT: return PL_KEY_LEFT_ARROW;
        case GLFW_KEY_RIGHT: return PL_KEY_RIGHT_ARROW;
        case GLFW_KEY_UP: return PL_KEY_UP_ARROW;
        case GLFW_KEY_DOWN: return PL_KEY_DOWN_ARROW;
        case GLFW_KEY_PAGE_UP: return PL_KEY_PAGE_UP;
        case GLFW_KEY_PAGE_DOWN: return PL_KEY_PAGE_DOWN;
        case GLFW_KEY_HOME: return PL_KEY_HOME;
        case GLFW_KEY_END: return PL_KEY_END;
        case GLFW_KEY_INSERT: return PL_KEY_INSERT;
        case GLFW_KEY_DELETE: return PL_KEY_DELETE;
        case GLFW_KEY_BACKSPACE: return PL_KEY_BACKSPACE;
        case GLFW_KEY_SPACE: return PL_KEY_SPACE;
        case GLFW_KEY_ENTER: return PL_KEY_ENTER;
        case GLFW_KEY_ESCAPE: return PL_KEY_ESCAPE;
        case GLFW_KEY_APOSTROPHE: return PL_KEY_APOSTROPHE;
        case GLFW_KEY_COMMA: return PL_KEY_COMMA;
        case GLFW_KEY_MINUS: return PL_KEY_MINUS;
        case GLFW_KEY_PERIOD: return PL_KEY_PERIOD;
        case GLFW_KEY_SLASH: return PL_KEY_SLASH;
        case GLFW_KEY_SEMICOLON: return PL_KEY_SEMICOLON;
        case GLFW_KEY_EQUAL: return PL_KEY_EQUAL;
        case GLFW_KEY_LEFT_BRACKET: return PL_KEY_LEFT_BRACKET;
        case GLFW_KEY_BACKSLASH: return PL_KEY_BACKSLASH;
        // case GLFW_KEY_WORLD_1: return PL_KEY_Oem102;
        // case GLFW_KEY_WORLD_2: return PL_KEY_Oem102;
        case GLFW_KEY_RIGHT_BRACKET: return PL_KEY_RIGHT_BRACKET;
        case GLFW_KEY_GRAVE_ACCENT: return PL_KEY_GRAVE_ACCENT;
        case GLFW_KEY_CAPS_LOCK: return PL_KEY_CAPS_LOCK;
        case GLFW_KEY_SCROLL_LOCK: return PL_KEY_SCROLL_LOCK;
        case GLFW_KEY_NUM_LOCK: return PL_KEY_NUM_LOCK;
        case GLFW_KEY_PRINT_SCREEN: return PL_KEY_PRINT_SCREEN;
        case GLFW_KEY_PAUSE: return PL_KEY_PAUSE;
        case GLFW_KEY_KP_0: return PL_KEY_KEYPAD_0;
        case GLFW_KEY_KP_1: return PL_KEY_KEYPAD_1;
        case GLFW_KEY_KP_2: return PL_KEY_KEYPAD_2;
        case GLFW_KEY_KP_3: return PL_KEY_KEYPAD_3;
        case GLFW_KEY_KP_4: return PL_KEY_KEYPAD_4;
        case GLFW_KEY_KP_5: return PL_KEY_KEYPAD_5;
        case GLFW_KEY_KP_6: return PL_KEY_KEYPAD_6;
        case GLFW_KEY_KP_7: return PL_KEY_KEYPAD_7;
        case GLFW_KEY_KP_8: return PL_KEY_KEYPAD_8;
        case GLFW_KEY_KP_9: return PL_KEY_KEYPAD_9;
        case GLFW_KEY_KP_DECIMAL: return PL_KEY_KEYPAD_DECIMAL;
        case GLFW_KEY_KP_DIVIDE: return PL_KEY_KEYPAD_DIVIDE;
        case GLFW_KEY_KP_MULTIPLY: return PL_KEY_KEYPAD_MULTIPLY;
        case GLFW_KEY_KP_SUBTRACT: return PL_KEY_KEYPAD_SUBTRACT;
        case GLFW_KEY_KP_ADD: return PL_KEY_KEYPAD_ADD;
        case GLFW_KEY_KP_ENTER: return PL_KEY_KEYPAD_ENTER;
        case GLFW_KEY_KP_EQUAL: return PL_KEY_KEYPAD_EQUAL;
        case GLFW_KEY_LEFT_SHIFT: return PL_KEY_LEFT_SHIFT;
        case GLFW_KEY_LEFT_CONTROL: return PL_KEY_LEFT_CTRL;
        case GLFW_KEY_LEFT_ALT: return PL_KEY_LEFT_ALT;
        case GLFW_KEY_LEFT_SUPER: return PL_KEY_LEFT_SUPER;
        case GLFW_KEY_RIGHT_SHIFT: return PL_KEY_RIGHT_SHIFT;
        case GLFW_KEY_RIGHT_CONTROL: return PL_KEY_RIGHT_CTRL;
        case GLFW_KEY_RIGHT_ALT: return PL_KEY_RIGHT_ALT;
        case GLFW_KEY_RIGHT_SUPER: return PL_KEY_RIGHT_SUPER;
        case GLFW_KEY_MENU: return PL_KEY_MENU;
        case GLFW_KEY_0: return PL_KEY_0;
        case GLFW_KEY_1: return PL_KEY_1;
        case GLFW_KEY_2: return PL_KEY_2;
        case GLFW_KEY_3: return PL_KEY_3;
        case GLFW_KEY_4: return PL_KEY_4;
        case GLFW_KEY_5: return PL_KEY_5;
        case GLFW_KEY_6: return PL_KEY_6;
        case GLFW_KEY_7: return PL_KEY_7;
        case GLFW_KEY_8: return PL_KEY_8;
        case GLFW_KEY_9: return PL_KEY_9;
        case GLFW_KEY_A: return PL_KEY_A;
        case GLFW_KEY_B: return PL_KEY_B;
        case GLFW_KEY_C: return PL_KEY_C;
        case GLFW_KEY_D: return PL_KEY_D;
        case GLFW_KEY_E: return PL_KEY_E;
        case GLFW_KEY_F: return PL_KEY_F;
        case GLFW_KEY_G: return PL_KEY_G;
        case GLFW_KEY_H: return PL_KEY_H;
        case GLFW_KEY_I: return PL_KEY_I;
        case GLFW_KEY_J: return PL_KEY_J;
        case GLFW_KEY_K: return PL_KEY_K;
        case GLFW_KEY_L: return PL_KEY_L;
        case GLFW_KEY_M: return PL_KEY_M;
        case GLFW_KEY_N: return PL_KEY_N;
        case GLFW_KEY_O: return PL_KEY_O;
        case GLFW_KEY_P: return PL_KEY_P;
        case GLFW_KEY_Q: return PL_KEY_Q;
        case GLFW_KEY_R: return PL_KEY_R;
        case GLFW_KEY_S: return PL_KEY_S;
        case GLFW_KEY_T: return PL_KEY_T;
        case GLFW_KEY_U: return PL_KEY_U;
        case GLFW_KEY_V: return PL_KEY_V;
        case GLFW_KEY_W: return PL_KEY_W;
        case GLFW_KEY_X: return PL_KEY_X;
        case GLFW_KEY_Y: return PL_KEY_Y;
        case GLFW_KEY_Z: return PL_KEY_Z;
        case GLFW_KEY_F1: return PL_KEY_F1;
        case GLFW_KEY_F2: return PL_KEY_F2;
        case GLFW_KEY_F3: return PL_KEY_F3;
        case GLFW_KEY_F4: return PL_KEY_F4;
        case GLFW_KEY_F5: return PL_KEY_F5;
        case GLFW_KEY_F6: return PL_KEY_F6;
        case GLFW_KEY_F7: return PL_KEY_F7;
        case GLFW_KEY_F8: return PL_KEY_F8;
        case GLFW_KEY_F9: return PL_KEY_F9;
        case GLFW_KEY_F10: return PL_KEY_F10;
        case GLFW_KEY_F11: return PL_KEY_F11;
        case GLFW_KEY_F12: return PL_KEY_F12;
        case GLFW_KEY_F13: return PL_KEY_F13;
        case GLFW_KEY_F14: return PL_KEY_F14;
        case GLFW_KEY_F15: return PL_KEY_F15;
        case GLFW_KEY_F16: return PL_KEY_F16;
        case GLFW_KEY_F17: return PL_KEY_F17;
        case GLFW_KEY_F18: return PL_KEY_F18;
        case GLFW_KEY_F19: return PL_KEY_F19;
        case GLFW_KEY_F20: return PL_KEY_F20;
        case GLFW_KEY_F21: return PL_KEY_F21;
        case GLFW_KEY_F22: return PL_KEY_F22;
        case GLFW_KEY_F23: return PL_KEY_F23;
        case GLFW_KEY_F24: return PL_KEY_F24;
        default: return PL_KEY_NONE;
    }
}

void
pl_glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
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


extern "C" const plWindowI* ptWindows2;
// const plDataRegistryI* gptDataRegistry = nullptr;

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

    ptWindows2 = pl_get_api_latest(ptApiRegistry, plWindowI);
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

    glfwSetErrorCallback(pl_glfw_error_callback);

    // setup glfw
    if (!glfwInit())
        return;

    atMouseCursors[PL_MOUSE_CURSOR_ARROW]       = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    atMouseCursors[PL_MOUSE_CURSOR_TEXT_INPUT]  = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    atMouseCursors[PL_MOUSE_CURSOR_RESIZE_NS]   = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    atMouseCursors[PL_MOUSE_CURSOR_RESIZE_EW]   = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    atMouseCursors[PL_MOUSE_CURSOR_HAND]        = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    atMouseCursors[PL_MOUSE_CURSOR_RESIZE_ALL]  = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
    atMouseCursors[PL_MOUSE_CURSOR_RESIZE_NESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
    atMouseCursors[PL_MOUSE_CURSOR_RESIZE_NWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    atMouseCursors[PL_MOUSE_CURSOR_NOT_ALLOWED] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);

    // set clipboard functions (may need to move this to OS api)
    gptIOCtx->set_clipboard_text_fn = [](void* pUnused, const char* text) { glfwSetClipboardString(nullptr, text); };
    gptIOCtx->get_clipboard_text_fn = [](void* pUnused) { return glfwGetClipboardString(nullptr); };

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGuiContext* ptImGuiCtx = ImGui::CreateContext();

    ImGuiMemAllocFunc p_alloc_func = nullptr;
    ImGuiMemFreeFunc p_free_func = nullptr;
    void* p_user_data = nullptr;
    ImGui::GetAllocatorFunctions(&p_alloc_func, &p_free_func, &p_user_data);

    gptDataRegistry->set_data("imgui", ptImGuiCtx);
    gptDataRegistry->set_data("imgui allocate", (void*)p_alloc_func);
    gptDataRegistry->set_data("imgui free", (void*)p_free_func);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // setup pilot light style
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Button]                 = ImVec4(0.51f, 0.02f, 0.10f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.61f, 0.02f, 0.10f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.87f, 0.02f, 0.10f, 1.00f);
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.25f, 0.10f, 0.10f, 0.78f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.15f, 0.10f, 0.10f, 0.78f);
    colors[ImGuiCol_Border]                 = ImVec4(0.33f, 0.02f, 0.10f, 1.00f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.23f, 0.02f, 0.10f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.33f, 0.02f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.85f);
    colors[ImGuiCol_ScrollbarGrab]          = colors[ImGuiCol_Button];
    colors[ImGuiCol_ScrollbarGrabHovered]   = colors[ImGuiCol_ButtonHovered];
    colors[ImGuiCol_ScrollbarGrabActive]    = colors[ImGuiCol_ButtonActive];
    colors[ImGuiCol_CheckMark]              = colors[ImGuiCol_ButtonActive];
    colors[ImGuiCol_SliderGrab]             = colors[ImGuiCol_Button];
    colors[ImGuiCol_SliderGrabActive]       = colors[ImGuiCol_ButtonActive];
    colors[ImGuiCol_Header]                 = colors[ImGuiCol_Button];
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.98f, 0.59f, 0.26f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.98f, 0.59f, 0.26f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.98f, 0.59f, 0.26f, 0.95f);
    colors[ImGuiCol_InputTextCursor]        = colors[ImGuiCol_Text];
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabSelected]            = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabSelectedOverline]    = colors[ImGuiCol_HeaderActive];
    colors[ImGuiCol_TabDimmed]              = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabDimmedSelected]      = ImLerp(colors[ImGuiCol_TabSelected],  colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 0.00f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.51f, 0.02f, 0.10f, 0.7f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextLink]               = colors[ImGuiCol_HeaderActive];
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavCursor]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    // when viewports are enabled we tweak WindowRounding/WindowBg so
    // platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
}

extern "C" void
pl__python_shutdown(void)
{
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

extern "C" int
pl__python_pre_update(void)
{
    glfwPollEvents();
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

    // start imgui glfw frame
    ImGui_ImplGlfw_NewFrame();

    // update mouse cursor
    plMouseCursor tCursor0 = gptIOCtx->tNextCursor;
    if(tCursor0 != PL_MOUSE_CURSOR_ARROW)
    {
        glfwSetCursor(gptGlfwWindow, atMouseCursors[tCursor0] ? atMouseCursors[tCursor0] : atMouseCursors[PL_MOUSE_CURSOR_ARROW]);
        glfwSetInputMode(gptGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    gptIOCtx->tNextCursor = PL_MOUSE_CURSOR_ARROW;
    gptIOCtx->bCursorChanged = false;

    // handle retina displays
    #ifdef __APPLE__

        float fCurrentScale = nswin.screen.backingScaleFactor;
        layer.contentsScale = fCurrentScale;

        int width, height;
        glfwGetFramebufferSize(gptGlfwWindow, &width, &height);
        layer.drawableSize = CGSizeMake(width, height);
        gptIOCtx->pBackendPlatformData = layer;

        // Setup display size (every frame to accommodate for window resizing)
        int w, h;
        glfwGetWindowSize(gptGlfwWindow, &w, &h);

        if (w > 0 && h > 0)
        {
            bool bResize = false;

            if(w != gptIOCtx->tMainViewportSize.x || h != gptIOCtx->tMainViewportSize.y)
                bResize = true;
            else if(fCurrentScale != gptIOCtx->tMainFramebufferScale.x || fCurrentScale != gptIOCtx->tMainFramebufferScale.y )
                bResize = true;

            if(bResize)
            {
                gptIOCtx->tMainViewportSize.x = w;
                gptIOCtx->tMainViewportSize.y = h;
                gptIOCtx->tMainFramebufferScale.x = fCurrentScale;
                gptIOCtx->tMainFramebufferScale.y = fCurrentScale;
                return 1;
            }
        }
    #else

        int width, height;
        glfwGetFramebufferSize(gptGlfwWindow, &width, &height);

        // Setup display size (every frame to accommodate for window resizing)
        int w, h;
        glfwGetWindowSize(gptGlfwWindow, &w, &h);

        if (w > 0 && h > 0)
        {
            bool bResize = false;

            if((float)w != gptIOCtx->tMainViewportSize.x || (float)h != gptIOCtx->tMainViewportSize.y)
                bResize = true;

            if(bResize)
            {
                gptIOCtx->tMainViewportSize.x = (float)w;
                gptIOCtx->tMainViewportSize.y = (float)h;
                gptIOCtx->tMainFramebufferScale.x = 1.0f;
                gptIOCtx->tMainFramebufferScale.y = 1.0f;
                return 1;

            }
        }
    #endif

    if (glfwGetWindowAttrib(gptGlfwWindow, GLFW_ICONIFIED) != 0)
    {
        #ifdef _WIN32
            Sleep(10);
        #else
            usleep(10 * 1000);
        #endif
    }

    return 0;
}

#define PL_STRING_IMPLEMENTATION
#include "pl_string.h"
#undef PL_STRING_IMPLEMENTATION

#include "imgui_impl_glfw.cpp"
