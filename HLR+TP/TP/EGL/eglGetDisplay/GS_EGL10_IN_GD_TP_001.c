#include <stdio.h>
#include <EGL/egl.h>
#include "macros.h"

/*
EGL10 - Initialization - GetDisplay

The graphics library shall provide API function (eglGetDisplay)
that obtains an EGL display associated with a native display.

The eglGetDisplay function shall return the default EGL display
when display_id is EGL_DEFAULT_DISPLAY and a default display
is available.

Covered requirements:
    - GS-EGL10-IN-GD-001
    - GS-EGL10-IN-GD-003
*/

static const char* test_case1 = "GS_EGL10_IN_GD_TC_001";
static const char* test_case2 = "GS_EGL10_IN_GD_TC_003";
static const char* test_procedure = "GS_EGL10_IN_GD_TP_001";

static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;
static EGLDisplay display = EGL_NO_DISPLAY;


/* Initialization */
void GS_EGL10_IN_GD_TP_001_init(void)
{
    /*
     * Call eglGetDisplay with EGL_DEFAULT_DISPLAY.
     *
     * The same function call is used to verify:
     *
     * TC_001:
     *     eglGetDisplay API can obtain an EGL display.
     *
     * TC_003:
     *     EGL_DEFAULT_DISPLAY returns the default EGL display
     *     when one is available.
     */
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);


    //Test Case 001

    /* Check if eglGetDisplay obtains an EGL display associated with a native display.*/
    if (display == EGL_NO_DISPLAY)
    {
        EGLint error = eglGetError();

        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "eglGetDisplay(EGL_DEFAULT_DISPLAY) did not obtain "
            "an EGLDisplay. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
    }

    if (test_success1)
    {
        TEST_LOG_SUCCESS(test_case1, test_procedure);
    }


    //Test Case 003

    /* Check if EGL_DEFAULT_DISPLAY returns the default EGL display when one is available. */
    if (display == EGL_NO_DISPLAY)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "EGL_DEFAULT_DISPLAY did not return an available "
            "default EGL display"
        );

        test_success2 = EGL_FALSE;
    }

    if (test_success2)
    {
        TEST_LOG_SUCCESS(test_case2, test_procedure);
    }


    if (display != EGL_NO_DISPLAY)
    {
        TEST_LOG_INFO(
            "Default EGLDisplay obtained: %p",
            (void*)display
        );
    }
}


void GS_EGL10_IN_GD_TP_001_draw(void) {

}

void GS_EGL10_IN_GD_TP_001_close(void) {
    display = EGL_NO_DISPLAY;
}