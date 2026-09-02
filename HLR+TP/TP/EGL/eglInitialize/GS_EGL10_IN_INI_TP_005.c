#include <stdio.h>
#include <EGL/egl.h>
#include "macros.h"

/*
EGL10 - Initialization - Initialize

The eglInitialize function shall return EGL_TRUE when dpy is
already initialized, and the only effect of the call shall be
to update the EGL version numbers through the provided version
output parameters.

Covered requirements:
    - GS-EGL10-IN-INI-008
*/

static const char* test_case =
    "GS_EGL10_IN_INI_TC_008";

static const char* test_procedure =
    "GS_EGL10_IN_INI_TP_005";

static EGLBoolean test_success = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLBoolean initialized = EGL_FALSE;


/* Initialization */
void GS_EGL10_IN_INI_TP_005_init(void)
{
    EGLBoolean result;
    EGLint error;

    EGLint first_major = -1;
    EGLint first_minor = -1;

    EGLint second_major = -1;
    EGLint second_minor = -1;


    /* Obtain an EGLDisplay. */
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (display == EGL_NO_DISPLAY)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: "
            "eglGetDisplay(EGL_DEFAULT_DISPLAY) returned "
            "EGL_NO_DISPLAY. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
        return;
    }


    /* Precondition:
     * Initialize the display for the first time.
     * This call establishes the already-initialized state
     * required by TC_008.
     */

    (void)eglGetError();

    result = eglInitialize(
        display,
        &first_major,
        &first_minor
    );

    error = eglGetError();

    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: first eglInitialize "
            "returned EGL_FALSE. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
        return;
    }

    initialized = EGL_TRUE;


    /* Sentinel values are used so that it can be verified that
     * the second eglInitialize call updates the output values.
     */
    second_major = -1;
    second_minor = -1;


    /* Clear a possible previous EGL error. */
    (void)eglGetError();


    // Test Case 008

    /* Call eglInitialize again on the already-initialized EGLDisplay. */
    result = eglInitialize(
        display,
        &second_major,
        &second_minor
    );

    error = eglGetError();


    /* Reinitializing an already-initialized display shall succeed. */
    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Expected EGL_TRUE when eglInitialize was called "
            "on an already-initialized display. "
            "eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }


    /* The provided version outputs shall be updated. */
    if (second_major == -1)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Major version output was not updated by the "
            "second eglInitialize call"
        );

        test_success = EGL_FALSE;
    }


    if (second_minor == -1)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Minor version output was not updated by the "
            "second eglInitialize call"
        );

        test_success = EGL_FALSE;
    }


    /* The EGL version reported by the second initialization shall be the same version reported by the first one. */
    if (second_major != first_major ||
        second_minor != first_minor)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "EGL version changed after repeated initialization. "
            "First: %d.%d, Second: %d.%d",
            first_major,
            first_minor,
            second_major,
            second_minor
        );

        test_success = EGL_FALSE;
    }


    if (test_success)
    {
        TEST_LOG_INFO(
            "Repeated eglInitialize succeeded. "
            "EGL version: %d.%d",
            second_major,
            second_minor
        );

        TEST_LOG_SUCCESS(
            test_case,
            test_procedure
        );
    }
}

void GS_EGL10_IN_INI_TP_005_draw(void) {

}

void GS_EGL10_IN_INI_TP_005_close(void)
{
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;
}