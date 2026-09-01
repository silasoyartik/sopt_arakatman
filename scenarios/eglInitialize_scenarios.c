#include <EGL/egl.h>
#include <stddef.h>
#include <stdio.h>

/*
 * All-in-one scenario file for eglInitialize(dpy, major, minor).
 * A fresh EGLDisplay should preferably be used for scenarios A-D.
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
 * SENARYO A - Display initialization başarılı olur, major ve minor birlikte verilir.
 * Beklenen: EGL_TRUE ve test ortamimizda version=1.5.
 */
void scenario_a_major_and_minor(EGLDisplay dpy)
{
    EGLint major = -1;
    EGLint minor = -1;

    if (eglInitialize(dpy, &major, &minor) == EGL_TRUE) {
        printf("Scenario A passed: version=%d.%d\n",
               major, minor);
    } else {
        printf("Scenario A failed: %s\n",
               egl_error_name(eglGetError()));
    }
}


/*
 * SCENARIO B - major=NULL, minor pointer.
 * EGL_TRUE with minor remaining -1 is expected in the tested Mesa environment.
 */
void scenario_b_major_null(EGLDisplay dpy)
{
    EGLint minor = -1;

    if (eglInitialize(dpy, NULL, &minor) == EGL_FALSE) {
        printf("Scenario B failed: %s\n",
               egl_error_name(eglGetError()));
        return;
    }

    if (minor == -1) {
        printf("Scenario B passed: EGL_TRUE, minor remained -1.\n");
    } else {
        printf("Scenario B observed different implementation behavior: minor=%d\n",
               minor);
    }
}


/*
 * SCENARIO C - major pointer, minor=NULL.
 * EGL_TRUE with major remaining -1 is expected in the tested Mesa environment.
 */
void scenario_c_minor_null(EGLDisplay dpy)
{
    EGLint major = -1;

    if (eglInitialize(dpy, &major, NULL) == EGL_FALSE) {
        printf("Scenario C failed: %s\n",
               egl_error_name(eglGetError()));
        return;
    }

    if (major == -1) {
        printf("Scenario C passed: EGL_TRUE, major remained -1.\n");
    } else {
        printf("Scenario C observed different implementation behavior: major=%d\n",
               major);
    }
}


/*
 * SCENARIO D - major=NULL and minor=NULL.
 * Expected: EGL_TRUE without retrieving version output.
 */
void scenario_d_both_null(EGLDisplay dpy)
{
    if (eglInitialize(dpy, NULL, NULL) == EGL_TRUE) {
        printf("Scenario D passed: returned EGL_TRUE.\n");
    } else {
        printf("Scenario D failed: %s\n",
               egl_error_name(eglGetError()));
    }
}


/*
 * SCENARIO E - dpy=EGL_NO_DISPLAY.
 * Expected: EGL_FALSE and EGL_BAD_DISPLAY.
 */
void scenario_e_invalid_display(void)
{
    EGLint major = -1;
    EGLint minor = -1;

    if (eglInitialize(EGL_NO_DISPLAY, &major, &minor) == EGL_FALSE) {
        EGLint error = eglGetError();

        if (error == EGL_BAD_DISPLAY) {
            printf("Scenario E passed: received EGL_BAD_DISPLAY.\n");
        } else {
            printf("Scenario E returned a different error: %s\n",
                   egl_error_name(error));
        }
    } else {
        printf("Scenario E returned an unexpected result: EGL_TRUE.\n");
    }
}
