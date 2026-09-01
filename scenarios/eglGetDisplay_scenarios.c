#include <EGL/egl.h>
#include <wayland-client.h>
#include <stdio.h>

/*
 * All-in-one scenario file for eglGetDisplay(display_id).
 * main and other EGL setup steps are deliberately omitted.
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
 * SCENARIO A - EGL_DEFAULT_DISPLAY.
 * Expected: a valid EGLDisplay handle other than EGL_NO_DISPLAY.
 */
void scenario_a_default_display(void)
{
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (dpy != EGL_NO_DISPLAY) {
        printf("Scenario A passed: EGLDisplay=%p\n", (void *)dpy);
    } else {
        printf("Scenario A failed: %s\n",
               egl_error_name(eglGetError()));
    }
}


/*
 * SCENARIO B - Two calls with the same EGL_DEFAULT_DISPLAY.
 * The same EGLDisplay handle is expected in our test environment.
 */
void scenario_b_repeated_default_display(void)
{
    EGLDisplay dpy1 = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLDisplay dpy2 = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (dpy1 == EGL_NO_DISPLAY || dpy2 == EGL_NO_DISPLAY) {
        printf("Scenario B failed: could not obtain a display.\n");
        return;
    }

    if (dpy1 == dpy2) {
        printf("Scenario B passed: the handles are identical (%p).\n",
               (void *)dpy1);
    } else {
        printf("Scenario B returned a different result: dpy1=%p dpy2=%p\n",
               (void *)dpy1, (void *)dpy2);
    }
}


/*
 * SCENARIO C - Explicit Wayland display.
 * Expected: a handle other than EGL_NO_DISPLAY for a valid wl_display.
 */
void scenario_c_explicit_wayland_display(void)
{
    struct wl_display *native_dpy = wl_display_connect(NULL);

    if (native_dpy == NULL) {
        printf("Scenario C setup failed: could not obtain a Wayland display.\n");
        return;
    }

    EGLDisplay dpy =
        eglGetDisplay((EGLNativeDisplayType)native_dpy);

    if (dpy != EGL_NO_DISPLAY) {
        printf("Scenario C passed: EGLDisplay=%p\n", (void *)dpy);
    } else {
        printf("Scenario C failed: %s\n",
               egl_error_name(eglGetError()));
    }

    wl_display_disconnect(native_dpy);
}
