#include <stdio.h>
#include <EGL/egl.h>
#include <wayland-client.h>
#include "macros.h"

/*
EGL10 - Initialization - GetDisplay

The eglGetDisplay function shall return an EGLDisplay associated
with the native display specified by display_id when a matching
display is available.

Covered requirements:
    - GS-EGL10-IN-GD-002
*/

static const char* test_case = "GS_EGL10_IN_GD_TC_002";
static const char* test_procedure = "GS_EGL10_IN_GD_TP_002";

static EGLBoolean test_success = EGL_TRUE;

static struct wl_display* native_display = NULL;
static EGLDisplay egl_display = EGL_NO_DISPLAY;


/* Initialization */
void GS_EGL10_IN_GD_TP_002_init(void)
{
    /* Obtain a valid native Wayland display. */
    native_display = wl_display_connect(NULL);

    if (native_display == NULL)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "wl_display_connect(NULL) failed; a valid native "
            "Wayland display could not be obtained"
        );

        test_success = EGL_FALSE;
        return;
    }

    TEST_LOG_INFO(
        "Native Wayland display obtained: %p",
        (void*)native_display
    );


    /* Request an EGLDisplay associated with the explicit native Wayland display. */
    egl_display =
        eglGetDisplay((EGLNativeDisplayType)native_display);


    // Test Case 002
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

void GS_EGL10_IN_GD_TP_002_close(void) {
    egl_display = EGL_NO_DISPLAY;

    if (native_display != NULL)
    {
        wl_display_disconnect(native_display);
        native_display = NULL;
    }
}