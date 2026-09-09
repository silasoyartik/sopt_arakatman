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

static void print_error_result(
    const char *scenario,
    EGLint expected,
    EGLint actual)
{
    printf("%s %s: expected=%s, actual=%s.\n",
           scenario,
           actual == expected ? "PASS" : "OBSERVED",
           egl_error_name(expected),
           egl_error_name(actual));
}

/*
 * Scenario 1 - EGL_SUCCESS
 *
 * Purpose:
 * Show the result when no EGL error is recorded.
 */
void scenario_error_success(void)
{
    (void)eglGetError();

    print_error_result("Scenario 1", EGL_SUCCESS, eglGetError());
}

/*
 * Scenario 2 - EGL_NOT_INITIALIZED
 *
 * Purpose:
 * Show the error from an operation on an uninitialized display.
 */
void scenario_error_not_initialized(EGLDisplay dpy)
{
    EGLint num_config = 0;

    (void)eglGetError();
    (void)eglChooseConfig(dpy, NULL, NULL, 0, &num_config);

    print_error_result("Scenario 2", EGL_NOT_INITIALIZED, eglGetError());
}

/*
 * Scenario 3 - EGL_BAD_ACCESS
 *
 * Purpose:
 * Show the error when another thread owns the supplied current context.
 */
void scenario_error_bad_access(
    EGLDisplay dpy,
    EGLSurface surface,
    EGLContext context)
{
    (void)eglGetError();
    (void)eglMakeCurrent(dpy, surface, surface, context);

    print_error_result("Scenario 3", EGL_BAD_ACCESS, eglGetError());
}

/*
 * Scenario 4 - EGL_BAD_ALLOC
 *
 * Purpose:
 * Print the error recorded after a failed surface allocation.
 */
void scenario_observe_error_after_failed_surface_creation(
    EGLSurface creation_result)
{
    if (creation_result != EGL_NO_SURFACE) {
        printf("Scenario 4 OBSERVED: surface creation succeeded.\n");
        return;
    }

    print_error_result("Scenario 4", EGL_BAD_ALLOC, eglGetError());
}

/*
 * Scenario 5 - EGL_BAD_ATTRIBUTE
 *
 * Purpose:
 * Show the error produced by an unknown config attribute.
 */
void scenario_error_bad_attribute(EGLDisplay dpy)
{
    const EGLint attributes[] = { 0x7FFFFFFF, 1, EGL_NONE };
    EGLint num_config = 0;

    (void)eglGetError();
    (void)eglChooseConfig(dpy, attributes, NULL, 0, &num_config);

    print_error_result("Scenario 5", EGL_BAD_ATTRIBUTE, eglGetError());
}

/*
 * Scenario 6 - EGL_BAD_CONTEXT
 *
 * Purpose:
 * Show the error produced by an invalid context handle.
 */
void scenario_error_bad_context(EGLDisplay dpy)
{
    (void)eglGetError();
    (void)eglDestroyContext(dpy, (EGLContext)0);

    print_error_result("Scenario 6", EGL_BAD_CONTEXT, eglGetError());
}

/*
 * Scenario 7 - EGL_BAD_CONFIG
 *
 * Purpose:
 * Show the error produced by an invalid config handle.
 */
void scenario_error_bad_config(EGLDisplay dpy)
{
    (void)eglGetError();
    (void)eglCreateContext(dpy, (EGLConfig)0, EGL_NO_CONTEXT, NULL);

    print_error_result("Scenario 7", EGL_BAD_CONFIG, eglGetError());
}

/*
 * Scenario 8 - EGL_BAD_CURRENT_SURFACE
 *
 * Purpose:
 * Observe the error after the current native surface becomes invalid.
 */
void scenario_error_bad_current_surface(
    EGLDisplay dpy,
    EGLSurface current_surface)
{
    (void)eglGetError();
    (void)eglSwapBuffers(dpy, current_surface);

    print_error_result("Scenario 8", EGL_BAD_CURRENT_SURFACE, eglGetError());
}

/*
 * Scenario 9 - EGL_BAD_DISPLAY
 *
 * Purpose:
 * Show the error produced by EGL_NO_DISPLAY.
 */
void scenario_error_bad_display(void)
{
    (void)eglGetError();
    (void)eglInitialize(EGL_NO_DISPLAY, NULL, NULL);

    print_error_result("Scenario 9", EGL_BAD_DISPLAY, eglGetError());
}

/*
 * Scenario 10 - EGL_BAD_SURFACE
 *
 * Purpose:
 * Show the error produced by an invalid surface handle.
 */
void scenario_error_bad_surface(EGLDisplay dpy)
{
    (void)eglGetError();
    (void)eglDestroySurface(dpy, (EGLSurface)0);

    print_error_result("Scenario 10", EGL_BAD_SURFACE, eglGetError());
}

/*
 * Scenario 11 - EGL_BAD_MATCH
 *
 * Purpose:
 * Show the error produced by incompatible context and surface objects.
 */
void scenario_error_bad_match(
    EGLDisplay dpy,
    EGLSurface incompatible_surface,
    EGLContext context)
{
    (void)eglGetError();
    (void)eglMakeCurrent(
        dpy,
        incompatible_surface,
        incompatible_surface,
        context
    );

    print_error_result("Scenario 11", EGL_BAD_MATCH, eglGetError());
}

/*
 * Scenario 12 - EGL_BAD_PARAMETER
 *
 * Purpose:
 * Show the error produced by a missing required output pointer.
 */
void scenario_error_bad_parameter(EGLDisplay dpy)
{
    (void)eglGetError();
    (void)eglChooseConfig(dpy, NULL, NULL, 0, NULL);

    print_error_result("Scenario 12", EGL_BAD_PARAMETER, eglGetError());
}

/*
 * Scenario 13 - EGL_BAD_NATIVE_PIXMAP
 *
 * Purpose:
 * Observe the implementation result for an invalid native pixmap.
 */
void scenario_error_bad_native_pixmap(
    EGLDisplay dpy,
    EGLConfig config)
{
    (void)eglGetError();
    (void)eglCreatePixmapSurface(
        dpy,
        config,
        (EGLNativePixmapType)0,
        NULL
    );

    print_error_result("Scenario 13", EGL_BAD_NATIVE_PIXMAP, eglGetError());
}

/*
 * Scenario 14 - EGL_BAD_NATIVE_WINDOW
 *
 * Purpose:
 * Observe the implementation result for an invalid native window.
 */
void scenario_error_bad_native_window(
    EGLDisplay dpy,
    EGLConfig config)
{
    (void)eglGetError();
    (void)eglCreateWindowSurface(
        dpy,
        config,
        (EGLNativeWindowType)0,
        NULL
    );

    print_error_result("Scenario 14", EGL_BAD_NATIVE_WINDOW, eglGetError());
}

/*
 * Scenario 15 - Error state reset
 *
 * Purpose:
 * Show that reading an EGL error resets the thread error state.
 */
void scenario_error_is_cleared_after_read(EGLDisplay dpy)
{
    (void)eglGetError();
    (void)eglDestroySurface(dpy, (EGLSurface)0);

    EGLint first_error = eglGetError();
    EGLint second_error = eglGetError();

    printf("Scenario 15 %s: first=%s, second=%s.\n",
           first_error == EGL_BAD_SURFACE && second_error == EGL_SUCCESS
               ? "PASS"
               : "OBSERVED",
           egl_error_name(first_error),
           egl_error_name(second_error));
}
