#include <stdio.h>
#include <EGL/egl.h>
#include "macros.h"

/* OPTIONAL PLATFORM ADAPTATION - WAYLAND EXAMPLE
 *
 * Define GS_EGL_USE_WAYLAND only if this test is executed
 * on a Wayland-based platform.
 *
 * If the target system does not use Wayland, this section is
 * not compiled. The target platform shall provide its own
 * implementation of:
 *
 *     GS_EGL10_get_valid_native_display()
 *     GS_EGL10_release_valid_native_display()
 * 
 */

#ifdef GS_EGL_USE_WAYLAND

#include <wayland-client.h>

static struct wl_display *wayland_display = NULL;

static EGLNativeDisplayType GS_EGL10_get_valid_native_display(void)
{
    wayland_display = wl_display_connect(NULL);

    return (EGLNativeDisplayType)wayland_display;
}

static void GS_EGL10_release_valid_native_display(
    EGLNativeDisplayType native_display)
{
    (void)native_display;

    if (wayland_display != NULL)
    {
        wl_display_disconnect(wayland_display);
        wayland_display = NULL;
    }
}

#else

/* PLATFORM-SPECIFIC IMPLEMENTATION REQUIRED
 *
 * The target platform shall provide a valid native display
 * identifier that can be passed to eglGetDisplay().
 *
 * Example implementations may use the native display mechanism
 * provided by the target operating system / window system.
 */

extern EGLNativeDisplayType
GS_EGL10_get_valid_native_display(void);

extern void
GS_EGL10_release_valid_native_display(
    EGLNativeDisplayType native_display);

#endif


/*
EGL10 - Initialization - GetDisplay

The eglGetDisplay function shall return an EGLDisplay associated
with the native display specified by display_id when a matching
display is available.

Covered requirements:
    - GS-EGL10-IN-GD-002
*/

static const char* test_case =
    "GS_EGL10_IN_GD_TC_002";

static const char* test_procedure =
    "GS_EGL10_IN_GD_TP_002";

static EGLBoolean test_success = EGL_TRUE;

static EGLNativeDisplayType native_display;
static EGLDisplay egl_display = EGL_NO_DISPLAY;


/* Initialization */
void GS_EGL10_IN_GD_TP_002_init(void)
{
    /* Obtain a valid native display from the target platform. */
    native_display =
        GS_EGL10_get_valid_native_display();


    // Test Case 002
    
    /* Pass the explicit native display identifier to eglGetDisplay(). */
    egl_display =
        eglGetDisplay(native_display);


    /* A matching native display shall result in a valid EGLDisplay handle. */
    if (egl_display == EGL_NO_DISPLAY)
    {
        EGLint error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "eglGetDisplay(native_display) returned "
            "EGL_NO_DISPLAY. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }


    if (test_success)
    {
        TEST_LOG_INFO(
            "EGLDisplay obtained from explicit native display: %p",
            (void*)egl_display
        );

        TEST_LOG_SUCCESS(
            test_case,
            test_procedure
        );
    }
}

void GS_EGL10_IN_GD_TP_002_draw(void) {

}

void GS_EGL10_IN_GD_TP_002_close(void)
{
    egl_display = EGL_NO_DISPLAY;

    GS_EGL10_release_valid_native_display(native_display);
}