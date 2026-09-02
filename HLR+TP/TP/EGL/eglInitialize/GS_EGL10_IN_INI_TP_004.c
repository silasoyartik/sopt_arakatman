#include <stdio.h>
#include <EGL/egl.h>
#include "macros.h"

/* PLATFORM / TEST ENVIRONMENT ADAPTATION
 *
 * The target test environment shall provide a valid EGLDisplay
 * for which EGL initialization cannot be completed.
 *
 * The returned display shall be a valid EGLDisplay. It shall NOT
 * be EGL_NO_DISPLAY and shall NOT be an arbitrary fabricated
 * handle.
 *
 * The test environment shall provide:
 *
 *     GS_EGL10_get_noninitializable_display()
 *     GS_EGL10_release_noninitializable_display()
 *
 * Depending on the target platform, this condition may require
 * a controlled test fixture or fault-injection mechanism.
 *
 */

extern EGLDisplay
GS_EGL10_get_noninitializable_display(void);

extern void
GS_EGL10_release_noninitializable_display(
    EGLDisplay display);


/*
EGL10 - Initialization - Initialize

The eglInitialize function shall generate EGL_NOT_INITIALIZED
when EGL cannot be initialized for an otherwise valid dpy.

Covered requirements:
    - GS-EGL10-IN-INI-007
*/

static const char* test_case =
    "GS_EGL10_IN_INI_TC_007";

static const char* test_procedure =
    "GS_EGL10_IN_INI_TP_004";

static EGLBoolean test_success = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;


/* Initialization */
void GS_EGL10_IN_INI_TP_004_init(void)
{
    EGLBoolean result;
    EGLint error;

    EGLint major = -1;
    EGLint minor = -1;


    /* Obtain a valid EGLDisplay that the target test environment has arranged to be non-initializable. */
    display =
        GS_EGL10_get_noninitializable_display();


    /* The fixture shall provide a valid EGLDisplay. EGL_NO_DISPLAY would exercise EGL_BAD_DISPLAY instead. */
    if (display == EGL_NO_DISPLAY)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: platform fixture "
            "returned EGL_NO_DISPLAY"
        );

        test_success = EGL_FALSE;
        return;
    }


    /* Clear a possible previous EGL error. */
    (void)eglGetError();


    /* Call the function under test. */
    result = eglInitialize(
        display,
        &major,
        &minor
    );

    error = eglGetError();


    /* Initialization is expected to fail for the valid but non-initializable display. */
    if (result != EGL_FALSE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Expected EGL_FALSE when EGL could not be "
            "initialized for the valid display, got: %u",
            (unsigned int)result
        );

        test_success = EGL_FALSE;
    }


    /* The specific error required by INI-007 is EGL_NOT_INITIALIZED. */
    if (error != EGL_NOT_INITIALIZED)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Expected EGL_NOT_INITIALIZED, got: 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }


    if (test_success)
    {
        TEST_LOG_INFO(
            "eglInitialize correctly returned EGL_FALSE "
            "and generated EGL_NOT_INITIALIZED for a valid "
            "display that could not be initialized"
        );

        TEST_LOG_SUCCESS(
            test_case,
            test_procedure
        );
    }
}


void GS_EGL10_IN_INI_TP_004_draw(void) {

}

void GS_EGL10_IN_INI_TP_004_close(void) {
    if (display != EGL_NO_DISPLAY)
    {
        GS_EGL10_release_noninitializable_display(
            display
        );
    }

    display = EGL_NO_DISPLAY;
}