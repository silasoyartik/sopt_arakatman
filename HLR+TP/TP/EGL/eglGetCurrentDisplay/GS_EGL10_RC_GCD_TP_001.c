#include <stdio.h>
#include <EGL/egl.h>
#include "macros.h"

/*
EGL10 - RenderingContexts - GetCurrentDisplay

Covered requirements:
    - GS-EGL10-RC-GCD-001
    - GS-EGL10-RC-GCD-003

TC_001:
    The graphics library shall provide eglGetCurrentDisplay,
    which obtains the EGLDisplay associated with the calling
    thread's current context.

TC_003:
    eglGetCurrentDisplay shall return EGL_NO_DISPLAY when
    no current context exists for the calling thread.
*/

static const char* test_case1 =
    "GS_EGL10_RC_GCD_TC_001";

static const char* test_case2 =
    "GS_EGL10_RC_GCD_TC_003";

static const char* test_procedure =
    "GS_EGL10_RC_GCD_TP_001";


static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLBoolean initialized = EGL_FALSE;


/* Initialization */
void GS_EGL10_RC_GCD_TP_001_init(void)
{
    EGLBoolean result;
    EGLDisplay current_display;
    EGLint error;


    test_success1 = EGL_TRUE;
    test_success2 = EGL_TRUE;


    // TEST CASE 001 / TEST CASE 003

     /* STATE 1:
     * No EGLDisplay has been initialized and no EGLContext
     * has been made current on the calling thread.
     * Expected:
     *
     *     eglGetCurrentDisplay() == EGL_NO_DISPLAY
     */

    (void)eglGetError();


    current_display = eglGetCurrentDisplay();


    error = eglGetError();


    /* The API function can be called without parameters and shall report the current display state of this thread. */
    if (current_display != EGL_NO_DISPLAY)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Expected EGL_NO_DISPLAY when no current context "
            "exists, got: %p",
            (void*)current_display
        );

        test_success2 = EGL_FALSE;
    }


    /* EGL_NO_DISPLAY is a normal state result here, not an EGL error. */
    if (error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "eglGetCurrentDisplay generated an unexpected "
            "EGL error. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
    }


    /* TEST PRECONDITION FOR STATE 2
     * Obtain and initialize an EGLDisplay, but deliberately
     * DO NOT:
     *     - create an EGLContext
     *     - create an EGLSurface
     *     - call eglMakeCurrent
     *
     * Initialization alone shall not establish a current
     * context/display state.
     */
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);


    if (display == EGL_NO_DISPLAY)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Test precondition failed: eglGetDisplay returned "
            "EGL_NO_DISPLAY. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;

        return;
    }


    result = eglInitialize(
        display,
        NULL,
        NULL
    );


    if (result != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Test precondition failed: eglInitialize returned "
            "EGL_FALSE. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;

        return;
    }


    initialized = EGL_TRUE;


    /* TEST CASE 003
     * STATE 2:
     * EGLDisplay exists and has been initialized, but there
     * is still no current EGLContext on the calling thread.
     * Expected:
     *     eglGetCurrentDisplay() == EGL_NO_DISPLAY
     */

    (void)eglGetError();


    current_display = eglGetCurrentDisplay();


    error = eglGetError();


    if (current_display != EGL_NO_DISPLAY)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Expected EGL_NO_DISPLAY after eglInitialize "
            "because no EGLContext is current. Got: %p",
            (void*)current_display
        );

        test_success2 = EGL_FALSE;
    }


    if (error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "eglGetCurrentDisplay generated an unexpected "
            "EGL error after initialization. "
            "eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
    }


    /* FINAL RESULTS */
    if (test_success1)
    {
        TEST_LOG_SUCCESS(
            test_case1,
            test_procedure
        );
    }


    if (test_success2)
    {
        TEST_LOG_INFO(
            "eglGetCurrentDisplay returned EGL_NO_DISPLAY "
            "both before and after eglInitialize while no "
            "EGLContext was current"
        );

        TEST_LOG_SUCCESS(
            test_case2,
            test_procedure
        );
    }
}


void GS_EGL10_RC_GCD_TP_001_draw(void) {

}

void GS_EGL10_RC_GCD_TP_001_close(void)
{
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;
}