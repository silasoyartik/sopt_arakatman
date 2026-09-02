#include <stdio.h>
#include <EGL/egl.h>
#include "macros.h"

/*
EGL10 - Initialization - Initialize

The eglInitialize function shall return EGL_FALSE and shall not
update major or minor when initialization fails.

The eglInitialize function shall generate EGL_BAD_DISPLAY when
dpy does not refer to a valid EGLDisplay.

Covered requirements:
    - GS-EGL10-IN-INI-005
    - GS-EGL10-IN-INI-006
*/

static const char* test_case1 =
    "GS_EGL10_IN_INI_TC_005";

static const char* test_case2 =
    "GS_EGL10_IN_INI_TC_006";

static const char* test_procedure =
    "GS_EGL10_IN_INI_TP_003";

static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;


/* Initialization */
void GS_EGL10_IN_INI_TP_003_init(void)
{
    EGLBoolean result;
    EGLint error;

    /* Sentinel values are used to verify that the output parameters are not modified when initialization fails. */
    EGLint major = -1;
    EGLint minor = -1;


    /* Clear a possible previous EGL error. */
    (void)eglGetError();


    /* Call eglInitialize with an invalid EGLDisplay.
     * EGL_NO_DISPLAY does not name a valid EGLDisplay and
     * therefore EGL_BAD_DISPLAY is expected.
     */
    result = eglInitialize(
        EGL_NO_DISPLAY,
        &major,
        &minor
    );

    error = eglGetError();


    // Test Case 005

    /* Initialization failure shall return EGL_FALSE and shall not modify major or minor. */
    if (result != EGL_FALSE)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Expected EGL_FALSE for failed initialization, "
            "got: %u",
            (unsigned int)result
        );

        test_success1 = EGL_FALSE;
    }


    if (major != -1)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Major version was modified after failed "
            "initialization. Expected: -1, got: %d",
            major
        );

        test_success1 = EGL_FALSE;
    }


    if (minor != -1)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Minor version was modified after failed "
            "initialization. Expected: -1, got: %d",
            minor
        );

        test_success1 = EGL_FALSE;
    }


    if (test_success1)
    {
        TEST_LOG_SUCCESS(
            test_case1,
            test_procedure
        );
    }


    // Test Case 006

    /* Invalid EGLDisplay shall generate EGL_BAD_DISPLAY. */
    if (error != EGL_BAD_DISPLAY)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Expected EGL_BAD_DISPLAY, got: 0x%x",
            error
        );

        test_success2 = EGL_FALSE;
    }


    if (test_success2)
    {
        TEST_LOG_SUCCESS(
            test_case2,
            test_procedure
        );
    }
}

void GS_EGL10_IN_INI_TP_003_draw(void) {

}

void GS_EGL10_IN_INI_TP_003_close(void) {

}