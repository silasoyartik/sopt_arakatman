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

The eglInitialize function shall return EGL_FALSE and shall not
update major or minor when initialization fails.

The eglInitialize function shall generate EGL_NOT_INITIALIZED
when EGL cannot be initialized for an otherwise valid dpy.

Covered requirements:
    - GS-EGL10-IN-INI-005
    - GS-EGL10-IN-INI-007
*/

static const char* test_case1 =
    "GS_EGL10_IN_INI_TC_005";

static const char* test_case2 =
    "GS_EGL10_IN_INI_TC_007";

static const char* test_procedure =
    "GS_EGL10_IN_INI_TP_004";

static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;


/* Initialization */
void GS_EGL10_IN_INI_TP_004_init(void)
{
    EGLBoolean result;
    EGLint error;

    /* Sentinel values are used to verify that the version outputs are not modified when initialization fails. */
    EGLint major = -1;
    EGLint minor = -1;


    /* Obtain a valid EGLDisplay that the target test environment has arranged to be non-initializable. */
    display =
        GS_EGL10_get_noninitializable_display();


    /* The fixture itself shall provide a valid EGLDisplay. EGL_NO_DISPLAY would test EGL_BAD_DISPLAY instead. */
    if (display == EGL_NO_DISPLAY)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Test precondition failed: platform fixture "
            "returned EGL_NO_DISPLAY"
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
        return;
    }


    (void)eglGetError();

    result = eglInitialize(
        display,
        &major,
        &minor
    );

    error = eglGetError();


    // Test Case 005

    /* Failed initialization shall return EGL_FALSE and shall not update major or minor. */
    if (result != EGL_FALSE)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Expected EGL_FALSE when initialization cannot "
            "be completed, got: %u",
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


    // Test Case 007

    /* A valid display for which EGL cannot be initialized shall generate EGL_NOT_INITIALIZED. */
    if (error != EGL_NOT_INITIALIZED)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Expected EGL_NOT_INITIALIZED, got: 0x%x",
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