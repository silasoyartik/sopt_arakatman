#include <stdio.h>
#include <EGL/egl.h>
#include "macros.h"

/*
EGL10 - Initialization - Initialize

The graphics library shall provide API function (eglInitialize)
that initializes EGL for a specified display.

The eglInitialize function shall initialize EGL for dpy and
return EGL_TRUE when initialization succeeds.

The eglInitialize function shall write the EGL implementation
major version number to major and the minor version number to
minor on success when the corresponding output pointer is not NULL.

Covered requirements:
    - GS-EGL10-IN-INI-001
    - GS-EGL10-IN-INI-002
    - GS-EGL10-IN-INI-003
*/

static const char* test_case1 =
    "GS_EGL10_IN_INI_TC_001";

static const char* test_case2 =
    "GS_EGL10_IN_INI_TC_002";

static const char* test_case3 =
    "GS_EGL10_IN_INI_TC_003";

static const char* test_procedure =
    "GS_EGL10_IN_INI_TP_001";


static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;
static EGLBoolean test_success3 = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;

static EGLint major = -1;
static EGLint minor = -1;

static EGLBoolean initialized = EGL_FALSE;


/* Initialization */
void GS_EGL10_IN_INI_TP_001_init(void)
{
    EGLBoolean result;
    EGLint error;

    /* Obtain an EGLDisplay required as a precondition for eglInitialize. */
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (display == EGL_NO_DISPLAY)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Test precondition failed: "
            "eglGetDisplay(EGL_DEFAULT_DISPLAY) returned "
            "EGL_NO_DISPLAY. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
        test_success3 = EGL_FALSE;

        return;
    }


    /* Initialize sentinel values so that it can be verified that eglInitialize writes the output parameters. */
    major = -1;
    minor = -1;


    /* Clear a possible previous EGL error. */
    (void)eglGetError();


    /* Call the function under test. */
    result = eglInitialize(
        display,
        &major,
        &minor
    );

    error = eglGetError();


    // Test Case 001

    /* Verify that the eglInitialize API can initialize EGL for the specified display.*/
    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "eglInitialize returned EGL_FALSE. "
            "eglGetError(): 0x%x",
            error
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


    // Test Case 002

    /* Successful initialization shall return EGL_TRUE.*/
    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Expected EGL_TRUE from eglInitialize, got: %u",
            (unsigned int)result
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


    // Test Case 003

    /* Verify that major and minor are updated on success.*/
    if (result == EGL_TRUE)
    {
        if (major == -1)
        {
            TEST_LOG_FAIL(
                test_case3,
                test_procedure,
                "Major EGL version output was not updated"
            );

            test_success3 = EGL_FALSE;
        }

        if (minor == -1)
        {
            TEST_LOG_FAIL(
                test_case3,
                test_procedure,
                "Minor EGL version output was not updated"
            );

            test_success3 = EGL_FALSE;
        }
    }
    else
    {
        /* TC_003 cannot pass if initialization itself failed. */
        test_success3 = EGL_FALSE;
    }


    if (test_success3)
    {
        TEST_LOG_INFO(
            "EGL initialized successfully. Version: %d.%d",
            major,
            minor
        );

        TEST_LOG_SUCCESS(
            test_case3,
            test_procedure
        );
    }


    if (result == EGL_TRUE)
    {
        initialized = EGL_TRUE;
    }
}


void GS_EGL10_IN_INI_TP_001_draw(void) {

}

void GS_EGL10_IN_INI_TP_001_close(void)
{
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;

    major = -1;
    minor = -1;
}