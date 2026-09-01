#include <EGL/egl.h>
#include <stdio.h>

/*
 * All-in-one scenario file for eglGetCurrentDisplay().
 * Because the function takes no parameters, the current EGL state is changed.
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
 * SCENARIO A - No current context.
 * Expected: EGL_NO_DISPLAY.
 */
void scenario_a_no_current_context(void)
{
    EGLDisplay current = eglGetCurrentDisplay();

    if (current == EGL_NO_DISPLAY) {
        printf("Scenario A passed: returned EGL_NO_DISPLAY.\n");
    } else {
        printf("Scenario A returned a different result: current=%p\n",
               (void *)current);
    }
}


/*
 * SCENARIO B - The display is initialized, but there is no current context.
 * Expected: EGL_NO_DISPLAY even though eglInitialize succeeds.
 */
void scenario_b_initialized_only(EGLDisplay dpy)
{
    EGLint major = -1;
    EGLint minor = -1;

    if (eglInitialize(dpy, &major, &minor) == EGL_FALSE) {
        printf("Scenario B setup failed: %s\n",
               egl_error_name(eglGetError()));
        return;
    }

    EGLDisplay current = eglGetCurrentDisplay();

    if (current == EGL_NO_DISPLAY) {
        printf("Scenario B passed: returned EGL_NO_DISPLAY.\n");
    } else {
        printf("Scenario B returned a different result: current=%p\n",
               (void *)current);
    }
}


/*
 * SCENARIO C - After eglMakeCurrent.
 * Expected: eglGetCurrentDisplay() == dpy.
 */
void scenario_c_after_make_current(
    EGLDisplay dpy,
    EGLSurface surface,
    EGLContext context)
{
    if (eglMakeCurrent(dpy, surface, surface, context) == EGL_FALSE) {
        printf("Scenario C setup failed: %s\n",
               egl_error_name(eglGetError()));
        return;
    }

    EGLDisplay current = eglGetCurrentDisplay();

    if (current == dpy) {
        printf("Scenario C passed: the current display matches dpy.\n");
    } else {
        printf("Scenario C failed: current=%p dpy=%p\n",
               (void *)current, (void *)dpy);
    }
}


/*
 * SCENARIO D - Release the current context.
 * Expected: EGL_NO_DISPLAY after release.
 */
void scenario_d_after_release(EGLDisplay dpy)
{
    if (eglMakeCurrent(
            dpy,
            EGL_NO_SURFACE,
            EGL_NO_SURFACE,
            EGL_NO_CONTEXT) == EGL_FALSE) {

        printf("Scenario D setup failed: %s\n",
               egl_error_name(eglGetError()));
        return;
    }

    EGLDisplay current = eglGetCurrentDisplay();

    if (current == EGL_NO_DISPLAY) {
        printf("Scenario D passed: returned EGL_NO_DISPLAY.\n");
    } else {
        printf("Scenario D failed: current=%p\n",
               (void *)current);
    }
}
