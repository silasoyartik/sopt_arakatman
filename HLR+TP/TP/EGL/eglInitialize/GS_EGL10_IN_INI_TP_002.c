#include <stdio.h>
#include <EGL/egl.h>
#include "macros.h"

/*
EGL10 - Initialization - Initialize

The eglInitialize function shall allow major, minor, or both
to be NULL without causing initialization to fail solely
because an output pointer is NULL.

A NULL output pointer is not updated. Each non-NULL output
pointer shall receive the corresponding EGL version number
on successful initialization.

Covered requirements:
    - GS-EGL10-IN-INI-004
*/

static const char* test_case =
    "GS_EGL10_IN_INI_TC_004";

static const char* test_procedure =
    "GS_EGL10_IN_INI_TP_002";

static EGLBoolean test_success = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLBoolean initialized = EGL_FALSE;


/* Initialization */
void GS_EGL10_IN_INI_TP_002_init(void)
{
    EGLBoolean result;
    EGLint error;

    EGLint major;
    EGLint minor;


    /* Obtain an EGLDisplay required by eglInitialize. */
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


    /* Case 1
     * major = NULL
     * minor = valid output pointer
     * Expected:
     *     - eglInitialize returns EGL_TRUE
     *     - minor is updated
     */

    minor = -1;

    (void)eglGetError();

    result = eglInitialize(
        display,
        NULL,
        &minor
    );

    error = eglGetError();

    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "eglInitialize(display, NULL, &minor) returned "
            "EGL_FALSE. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }
    else
    {
        initialized = EGL_TRUE;

        if (minor == -1)
        {
            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "Minor version was not updated when major "
                "was NULL"
            );

            test_success = EGL_FALSE;
        }
    }


    /* Return the display to the uninitialized state before testing the next combination. */
    if (initialized == EGL_TRUE)
    {
        eglTerminate(display);
        initialized = EGL_FALSE;
    }


    /* Case 2
     * major = valid output pointer
     * minor = NULL
     * Expected:
     *     - eglInitialize returns EGL_TRUE
     *     - major is updated
     */

    major = -1;

    (void)eglGetError();

    result = eglInitialize(
        display,
        &major,
        NULL
    );

    error = eglGetError();

    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "eglInitialize(display, &major, NULL) returned "
            "EGL_FALSE. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }
    else
    {
        initialized = EGL_TRUE;

        if (major == -1)
        {
            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "Major version was not updated when minor "
                "was NULL"
            );

            test_success = EGL_FALSE;
        }
    }


    /* Return the display to the uninitialized state before testing the next combination. */
    if (initialized == EGL_TRUE)
    {
        eglTerminate(display);
        initialized = EGL_FALSE;
    }


    /* Case 3
     * major = NULL
     * minor = NULL
     * Expected:
     *     - eglInitialize returns EGL_TRUE
     *     - no version output is requested
     */

    (void)eglGetError();

    result = eglInitialize(
        display,
        NULL,
        NULL
    );

    error = eglGetError();

    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "eglInitialize(display, NULL, NULL) returned "
            "EGL_FALSE. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }
    else
    {
        initialized = EGL_TRUE;
    }


    /* Final Test Result */
    if (test_success)
    {
        TEST_LOG_INFO(
            "eglInitialize accepted all required NULL "
            "major/minor combinations and updated each "
            "provided version output"
        );

        TEST_LOG_SUCCESS(
            test_case,
            test_procedure
        );
    }
}

void GS_EGL10_IN_INI_TP_002_draw(void) {

}

void GS_EGL10_IN_INI_TP_002_close(void) {
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;
}