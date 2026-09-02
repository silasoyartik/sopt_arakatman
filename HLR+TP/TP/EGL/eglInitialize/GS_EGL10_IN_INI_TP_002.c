#include <stdio.h>
#include <EGL/egl.h>
#include "macros.h"

/*
EGL10 - Initialization - Initialize

The eglInitialize function shall allow major and/or minor
to be NULL and shall not write the corresponding EGL version
number when an output pointer is NULL.

Either output pointer may independently be NULL.

Covered requirements:
    - GS-EGL10-IN-INI-004
*/

static const char* test_case =
    "GS_EGL10_IN_INI_TC_004";

static const char* test_procedure =
    "GS_EGL10_IN_INI_TP_002";

static EGLBoolean test_success = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;


/* Initialization */
void GS_EGL10_IN_INI_TP_002_init(void)
{
    EGLBoolean result;
    EGLint error;

    EGLint major;
    EGLint minor;


    /* Obtain the EGLDisplay required by eglInitialize. */
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

    // Test Case 004

    /* Case 1:
     * major = NULL
     * minor = valid output pointer
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
        /* The non-NULL output pointer shall still be updated. */
        if (minor == -1)
        {
            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "Minor version was not updated when "
                "major was NULL"
            );

            test_success = EGL_FALSE;
        }
    }


    /* Return the display to the uninitialized state before testing the next input combination. */
    if (result == EGL_TRUE)
    {
        eglTerminate(display);
    }


    /* Case 2:
     * major = valid output pointer
     * minor = NULL
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
        /*  The non-NULL output pointer shall still be updated. */
        if (major == -1)
        {
            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "Major version was not updated when "
                "minor was NULL"
            );

            test_success = EGL_FALSE;
        }
    }


    if (result == EGL_TRUE)
    {
        eglTerminate(display);
    }


    /* Case 3:
     * major = NULL
     * minor = NULL
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


    /* Final result of TC_004. */
    if (test_success)
    {
        TEST_LOG_INFO(
            "eglInitialize accepted all required NULL "
            "major/minor output combinations"
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
    if (display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    display = EGL_NO_DISPLAY;
}