#include <stdio.h>
#include <EGL/egl.h>
#include "macros.h"

/* PLATFORM ADAPTATION
 *
 * The target platform shall provide a native display identifier
 * for which no matching EGL display is available.
 *
 * IMPORTANT:
 * Do NOT use an arbitrary invalid pointer or fabricated handle.
 *
 * The platform shall provide implementations of:
 *
 *     GS_EGL10_get_unmatched_native_display()
 *     GS_EGL10_release_unmatched_native_display()
 *
 */

extern EGLNativeDisplayType
GS_EGL10_get_unmatched_native_display(void);

extern void
GS_EGL10_release_unmatched_native_display(
    EGLNativeDisplayType native_display);


/*
EGL10 - Initialization - GetDisplay

The eglGetDisplay function shall return EGL_NO_DISPLAY when
no display matching display_id is available.

This result shall not raise an EGL error condition.

Covered requirements:
    - GS-EGL10-IN-GD-004
*/

static const char* test_case =
    "GS_EGL10_IN_GD_TC_004";

static const char* test_procedure =
    "GS_EGL10_IN_GD_TP_003";

static EGLBoolean test_success = EGL_TRUE;

static EGLNativeDisplayType native_display;
static EGLDisplay egl_display = EGL_NO_DISPLAY;


/* Initialization */
void GS_EGL10_IN_GD_TP_003_init(void)
{
    EGLint error;

    /* Obtain a platform-specific native display identifier for which no matching EGL display is available. */
    native_display =
        GS_EGL10_get_unmatched_native_display();


    /* Clear a possible previous EGL error so that the error checked below belongs to this test. */
    (void)eglGetError();


    // Test Case 004

     /* eglGetDisplay shall return EGL_NO_DISPLAY when no display matching display_id is available. */
    egl_display =
        eglGetDisplay(native_display);

    error = eglGetError();


    /* EGL_NO_DISPLAY is expected. */
    if (egl_display != EGL_NO_DISPLAY)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Expected EGL_NO_DISPLAY when no matching display "
            "is available, got: %p",
            (void*)egl_display
        );

        test_success = EGL_FALSE;
    }


    /* No EGL error condition shall be generated. */
    if (error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Expected EGL_SUCCESS after eglGetDisplay, "
            "got error: 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }


    if (test_success)
    {
        TEST_LOG_INFO(
            "eglGetDisplay correctly returned EGL_NO_DISPLAY "
            "without generating an EGL error"
        );

        TEST_LOG_SUCCESS(
            test_case,
            test_procedure
        );
    }
}

void GS_EGL10_IN_GD_TP_003_draw(void) {

}

void GS_EGL10_IN_GD_TP_003_close(void) {
    egl_display = EGL_NO_DISPLAY;

    GS_EGL10_release_unmatched_native_display(
        native_display
    );
}