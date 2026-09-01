#include <EGL/egl.h>
#include <stdio.h>

static const char *egl_error_name(EGLint error)
{
    switch (error) {
    case EGL_SUCCESS:         return "EGL_SUCCESS";
    case EGL_NOT_INITIALIZED: return "EGL_NOT_INITIALIZED";
    case EGL_BAD_CONTEXT:     return "EGL_BAD_CONTEXT";
    case EGL_BAD_DISPLAY:     return "EGL_BAD_DISPLAY";
    default:                  return "OTHER_EGL_ERROR";
    }
}

/*
 * Scenario 1 - Valid non-current context
 *
 * Purpose:
 * Show successful destruction of a context that is not current.
 */
void scenario_destroy_non_current_context(
    EGLDisplay dpy,
    EGLContext context)
{
    (void)eglGetError();

    EGLBoolean result = eglDestroyContext(dpy, context);

    if (result == EGL_TRUE) {
        printf("Scenario 1 PASS: eglDestroyContext returned EGL_TRUE.\n");
    } else {
        printf("Scenario 1 FAIL: error=%s.\n",
               egl_error_name(eglGetError()));
    }
}

/*
 * Scenario 2 - Valid current context
 *
 * Purpose:
 * Show deferred destruction of a context that is current.
 */
void scenario_destroy_current_context(
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

    if (eglDestroyContext(dpy, context) == EGL_FALSE) {
        printf("Scenario 2 FAIL: eglDestroyContext error=%s.\n",
               egl_error_name(eglGetError()));
        return;
    }

    if (eglMakeCurrent(
            dpy,
            EGL_NO_SURFACE,
            EGL_NO_SURFACE,
            EGL_NO_CONTEXT) == EGL_TRUE) {
        printf("Scenario 2 PASS: destruction was accepted and the context was released.\n");
    } else {
        printf("Scenario 2 FAIL: context release error=%s.\n",
               egl_error_name(eglGetError()));
    }
}

/*
 * Scenario 3 - Invalid EGLContext
 *
 * Purpose:
 * Show the EGL_BAD_CONTEXT result for an invalid context handle.
 */
void scenario_destroy_invalid_context(EGLDisplay dpy)
{
    (void)eglGetError();

    EGLBoolean result = eglDestroyContext(dpy, (EGLContext)0);
    EGLint error = eglGetError();

    printf("Scenario 3 %s: result=%s, error=%s.\n",
           result == EGL_FALSE && error == EGL_BAD_CONTEXT ? "PASS" : "FAIL",
           result == EGL_TRUE ? "EGL_TRUE" : "EGL_FALSE",
           egl_error_name(error));
}

/*
 * Scenario 4 - EGL_NO_DISPLAY
 *
 * Purpose:
 * Show the EGL_BAD_DISPLAY result for an invalid display.
 */
void scenario_destroy_context_no_display(EGLContext context)
{
    (void)eglGetError();

    EGLBoolean result = eglDestroyContext(EGL_NO_DISPLAY, context);
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
void scenario_destroy_context_uninitialized_display(
    EGLDisplay uninitialized_dpy,
    EGLContext context)
{
    (void)eglGetError();

    EGLBoolean result = eglDestroyContext(uninitialized_dpy, context);
    EGLint error = eglGetError();

    printf("Scenario 5 %s: result=%s, error=%s.\n",
           result == EGL_FALSE && error == EGL_NOT_INITIALIZED ? "PASS" : "OBSERVED",
           result == EGL_TRUE ? "EGL_TRUE" : "EGL_FALSE",
           egl_error_name(error));
}
