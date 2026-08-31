#include <EGL/egl.h>
#include <stddef.h>
#include <stdio.h>

static const char *egl_error_name(EGLint error)
{
    switch (error) {
    case EGL_SUCCESS:             return "EGL_SUCCESS";
    case EGL_NOT_INITIALIZED:     return "EGL_NOT_INITIALIZED";
    case EGL_BAD_ACCESS:          return "EGL_BAD_ACCESS";
    case EGL_BAD_ALLOC:           return "EGL_BAD_ALLOC";
    case EGL_BAD_ATTRIBUTE:       return "EGL_BAD_ATTRIBUTE";
    case EGL_BAD_CONTEXT:         return "EGL_BAD_CONTEXT";
    case EGL_BAD_CONFIG:          return "EGL_BAD_CONFIG";
    case EGL_BAD_CURRENT_SURFACE: return "EGL_BAD_CURRENT_SURFACE";
    case EGL_BAD_DISPLAY:         return "EGL_BAD_DISPLAY";
    case EGL_BAD_SURFACE:         return "EGL_BAD_SURFACE";
    case EGL_BAD_MATCH:           return "EGL_BAD_MATCH";
    case EGL_BAD_PARAMETER:       return "EGL_BAD_PARAMETER";
    case EGL_BAD_NATIVE_PIXMAP:   return "EGL_BAD_NATIVE_PIXMAP";
    case EGL_BAD_NATIVE_WINDOW:   return "EGL_BAD_NATIVE_WINDOW";
    default:                      return "UNKNOWN_EGL_ERROR";
    }
}

static void print_surface_result(
    const char *scenario,
    EGLSurface surface)
{
    if (surface != EGL_NO_SURFACE) {
        printf("%s PASS: eglCreateWindowSurface returned a valid surface.\n",
               scenario);
    } else {
        EGLint error = eglGetError();
        printf("%s FAIL: eglCreateWindowSurface returned EGL_NO_SURFACE, error=%s.\n",
               scenario,
               egl_error_name(error));
    }
}

/*
 * Scenario 1A - Valid EGLDisplay
 *
 * Purpose:
 * Show window surface creation with a valid initialized display.
 */
void scenario_valid_display(
    EGLDisplay dpy,
    EGLConfig config,
    EGLNativeWindowType win)
{
    (void)eglGetError();

    EGLSurface surface = eglCreateWindowSurface(
        dpy,
        config,
        win,
        NULL
    );

    print_surface_result("Scenario 1A", surface);
}

/*
 * Scenario 1B - EGL_NO_DISPLAY
 *
 * Purpose:
 * Show the EGL_BAD_DISPLAY result for an invalid display.
 */
void scenario_no_display(
    EGLConfig config,
    EGLNativeWindowType win)
{
    (void)eglGetError();

    EGLSurface surface = eglCreateWindowSurface(
        EGL_NO_DISPLAY,
        config,
        win,
        NULL
    );

    if (surface == EGL_NO_SURFACE) {
        EGLint error = eglGetError();
        printf("Scenario 1B %s: error=%s.\n",
               error == EGL_BAD_DISPLAY ? "PASS" : "FAIL",
               egl_error_name(error));
    } else {
        printf("Scenario 1B FAIL: an invalid display created a surface.\n");
    }
}

/*
 * Scenario 2A - Valid EGLConfig
 *
 * Purpose:
 * Show surface creation with a valid window-capable config.
 */
void scenario_valid_config(
    EGLDisplay dpy,
    EGLConfig valid_config,
    EGLNativeWindowType win)
{
    (void)eglGetError();

    EGLSurface surface = eglCreateWindowSurface(
        dpy,
        valid_config,
        win,
        NULL
    );

    print_surface_result("Scenario 2A", surface);
}

/*
 * Scenario 2B - Invalid EGLConfig
 *
 * Purpose:
 * Show the EGL_BAD_CONFIG result for an invalid config handle.
 */
void scenario_invalid_config(
    EGLDisplay dpy,
    EGLNativeWindowType win)
{
    (void)eglGetError();

    EGLSurface surface = eglCreateWindowSurface(
        dpy,
        (EGLConfig)0,
        win,
        NULL
    );

    if (surface == EGL_NO_SURFACE) {
        EGLint error = eglGetError();
        printf("Scenario 2B %s: error=%s.\n",
               error == EGL_BAD_CONFIG ? "PASS" : "FAIL",
               egl_error_name(error));
    } else {
        printf("Scenario 2B FAIL: an invalid config created a surface.\n");
    }
}

/*
 * Scenario 3A - Valid native window
 *
 * Purpose:
 * Show surface creation with a native window compatible with the EGL platform.
 */
void scenario_valid_native_window(
    EGLDisplay dpy,
    EGLConfig config,
    EGLNativeWindowType win)
{
    (void)eglGetError();

    EGLSurface surface = eglCreateWindowSurface(
        dpy,
        config,
        win,
        NULL
    );

    print_surface_result("Scenario 3A", surface);
}

/*
 * Scenario 3B - Invalid native window
 *
 * Purpose:
 * Observe the implementation result for an invalid native window handle.
 */
void scenario_invalid_native_window(
    EGLDisplay dpy,
    EGLConfig config)
{
    (void)eglGetError();

    EGLSurface surface = eglCreateWindowSurface(
        dpy,
        config,
        (EGLNativeWindowType)0,
        NULL
    );

    if (surface == EGL_NO_SURFACE) {
        EGLint error = eglGetError();
        printf("Scenario 3B %s: error=%s.\n",
               error == EGL_BAD_NATIVE_WINDOW ? "PASS" : "OBSERVED",
               egl_error_name(error));
    } else {
        printf("Scenario 3B OBSERVED: the implementation accepted the native handle.\n");
    }
}

/*
 * Scenario 4A - NULL attribute list
 *
 * Purpose:
 * Show EGL 1.0 window surface creation with attrib_list set to NULL.
 */
void scenario_null_attribute_list(
    EGLDisplay dpy,
    EGLConfig config,
    EGLNativeWindowType win)
{
    (void)eglGetError();

    EGLSurface surface = eglCreateWindowSurface(
        dpy,
        config,
        win,
        NULL
    );

    print_surface_result("Scenario 4A", surface);
}

/*
 * Scenario 4B - Empty EGL_NONE attribute list
 *
 * Purpose:
 * Show EGL 1.0 window surface creation with an empty attribute list.
 */
void scenario_empty_attribute_list(
    EGLDisplay dpy,
    EGLConfig config,
    EGLNativeWindowType win)
{
    const EGLint attrib_list[] = { EGL_NONE };

    (void)eglGetError();

    EGLSurface surface = eglCreateWindowSurface(
        dpy,
        config,
        win,
        attrib_list
    );

    print_surface_result("Scenario 4B", surface);
}
