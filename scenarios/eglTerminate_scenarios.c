#include <EGL/egl.h>
#include <stdio.h>

/*
 * All-in-one scenario file for eglTerminate(pDpyID).
 * main, window setup, rendering, and lengthy cleanup steps are deliberately
 * omitted; the focus is solely on pDpyID behavior.
 */

static const char *egl_error_name(EGLint error)
{
    switch (error) {
    case EGL_SUCCESS:         return "EGL_SUCCESS";
    case EGL_NOT_INITIALIZED: return "EGL_NOT_INITIALIZED";
    case EGL_BAD_ACCESS:      return "EGL_BAD_ACCESS";
    case EGL_BAD_ALLOC:       return "EGL_BAD_ALLOC";
    case EGL_BAD_ATTRIBUTE:   return "EGL_BAD_ATTRIBUTE";
    case EGL_BAD_CONTEXT:     return "EGL_BAD_CONTEXT";
    case EGL_BAD_CONFIG:      return "EGL_BAD_CONFIG";
    case EGL_BAD_SURFACE:     return "EGL_BAD_SURFACE";
    case EGL_BAD_DISPLAY:     return "EGL_BAD_DISPLAY";
    case EGL_BAD_MATCH:       return "EGL_BAD_MATCH";
    case EGL_BAD_PARAMETER:   return "EGL_BAD_PARAMETER";
    default:                  return "UNKNOWN_EGL_ERROR";
    }
}

/*
 * SCENARIO A - Valid, initialized display.
 * In normal use, the display is initialized and the required EGL resources are
 * assumed to have been used; the termination result is checked with eglTerminate(dpy).
 */
void scenario_a_valid_initialized_display(EGLDisplay dpy)
{
    EGLint major = 0;
    EGLint minor = 0;

    if (dpy == EGL_NO_DISPLAY) {
        printf("Scenario A invalid: pDpyID cannot be EGL_NO_DISPLAY.\n");
        return;
    }

    if (eglInitialize(dpy, &major, &minor) == EGL_FALSE) {
        printf("Scenario A setup failed: eglInitialize error=%s\n",
               egl_error_name(eglGetError()));
        return;
    }

    if (eglTerminate(dpy) == EGL_TRUE) {
        printf("Scenario A passed: eglTerminate returned EGL_TRUE.\n");
    } else {
        printf("Scenario A failed: eglTerminate error=%s\n",
               egl_error_name(eglGetError()));
    }
}

/*
 * SCENARIO B - Invalid display / EGL_NO_DISPLAY.
 * eglTerminate is called without obtaining a valid display; EGL_FALSE and an
 * EGL_BAD_DISPLAY error are expected in this negative case.
 */
void scenario_b_invalid_display(void)
{
    EGLDisplay dpy = EGL_NO_DISPLAY;

    if (eglTerminate(dpy) == EGL_FALSE) {
        EGLint error = eglGetError();

        if (error == EGL_BAD_DISPLAY) {
            printf("Scenario B passed: the invalid display produced EGL_BAD_DISPLAY.\n");
        } else {
            printf("Scenario B returned a different error: %s\n", egl_error_name(error));
        }
    } else {
        printf("Scenario B returned an unexpected result: eglTerminate returned EGL_TRUE.\n");
    }
}

/*
 * SCENARIO C - Valid but uninitialized display.
 * The display handle is valid, but eglInitialize is not called; termination
 * behavior in this state is checked directly with eglTerminate(dpy).
 */
void scenario_c_valid_but_uninitialized_display(EGLDisplay dpy)
{
    if (dpy == EGL_NO_DISPLAY) {
        printf("Scenario C invalid: a valid EGLDisplay must be obtained first.\n");
        return;
    }

    if (eglTerminate(dpy) == EGL_TRUE) {
        printf("Scenario C passed: returned EGL_TRUE for an uninitialized display.\n");
    } else {
        printf("Scenario C failed: eglTerminate error=%s\n",
               egl_error_name(eglGetError()));
    }
}
