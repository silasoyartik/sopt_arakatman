#include <EGL/egl.h>
#include <stdio.h>

static const char *egl_error_name(EGLint error)
{
    switch (error) {
    case EGL_SUCCESS:         return "EGL_SUCCESS";
    case EGL_NOT_INITIALIZED: return "EGL_NOT_INITIALIZED";
    case EGL_BAD_DISPLAY:     return "EGL_BAD_DISPLAY";
    case EGL_BAD_SURFACE:     return "EGL_BAD_SURFACE";
    default:                  return "OTHER_EGL_ERROR";
    }
}

/*
 * Scenario 1 - Valid non-current surface
 *
 * Purpose:
 * Show successful destruction of a surface that is not current.
 */
void scenario_destroy_non_current_surface(
    EGLDisplay dpy,
    EGLSurface surface)
{
    (void)eglGetError();

    EGLBoolean result = eglDestroySurface(dpy, surface);

    if (result == EGL_TRUE) {
        printf("Scenario 1 PASS: eglDestroySurface returned EGL_TRUE.\n");
    } else {
        printf("Scenario 1 FAIL: error=%s.\n",
               egl_error_name(eglGetError()));
    }
}

/*
 * Scenario 2 - Valid current surface
 *
 * Purpose:
 * Show deferred destruction of a surface that is current.
 */
void scenario_destroy_current_surface(
    EGLDisplay dpy,
    EGLSurface surface,
    EGLContext context)
{
    (void)eglGetError();

    if (eglMakeCurrent(dpy, surface, surface, context) == EGL_FALSE) {
        printf("Scenario 2 FAIL: eglMakeCurrent error=%s.\n",
               egl_error_name(eglGetError()));
        return;
    }

    if (eglDestroySurface(dpy, surface) == EGL_FALSE) {
        printf("Scenario 2 FAIL: eglDestroySurface error=%s.\n",
               egl_error_name(eglGetError()));
        return;
    }

    if (eglMakeCurrent(
            dpy,
            EGL_NO_SURFACE,
            EGL_NO_SURFACE,
            EGL_NO_CONTEXT) == EGL_TRUE) {
        printf("Scenario 2 PASS: destruction was accepted and the surface was released.\n");
    } else {
        printf("Scenario 2 FAIL: surface release error=%s.\n",
               egl_error_name(eglGetError()));
    }
}

/*
 * Scenario 3 - Invalid EGLSurface
 *
 * Purpose:
 * Show the EGL_BAD_SURFACE result for an invalid surface handle.
 */
void scenario_destroy_invalid_surface(EGLDisplay dpy)
{
    (void)eglGetError();

    EGLBoolean result = eglDestroySurface(dpy, (EGLSurface)0);
    EGLint error = eglGetError();

    printf("Scenario 3 %s: result=%s, error=%s.\n",
           result == EGL_FALSE && error == EGL_BAD_SURFACE ? "PASS" : "FAIL",
           result == EGL_TRUE ? "EGL_TRUE" : "EGL_FALSE",
           egl_error_name(error));
}

/*
 * Scenario 4 - EGL_NO_DISPLAY
 *
 * Purpose:
 * Show the EGL_BAD_DISPLAY result for an invalid display.
 */
void scenario_destroy_surface_no_display(EGLSurface surface)
{
    (void)eglGetError();

    EGLBoolean result = eglDestroySurface(EGL_NO_DISPLAY, surface);
    EGLint error = eglGetError();

    printf("Scenario 4 %s: result=%s, error=%s.\n",
           result == EGL_FALSE && error == EGL_BAD_DISPLAY ? "PASS" : "FAIL",
           result == EGL_TRUE ? "EGL_TRUE" : "EGL_FALSE",
           egl_error_name(error));
}

/*
 * Scenario 5 - Uninitialized EGLDisplay
 *
 * Purpose:
 * Show the error returned for a valid but uninitialized display.
 */
void scenario_destroy_surface_uninitialized_display(
    EGLDisplay uninitialized_dpy,
    EGLSurface surface)
{
    (void)eglGetError();

    EGLBoolean result = eglDestroySurface(uninitialized_dpy, surface);
    EGLint error = eglGetError();

    printf("Scenario 5 %s: result=%s, error=%s.\n",
           result == EGL_FALSE && error == EGL_NOT_INITIALIZED ? "PASS" : "OBSERVED",
           result == EGL_TRUE ? "EGL_TRUE" : "EGL_FALSE",
           egl_error_name(error));
}
