#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - Initialization - eglTerminate

Verify that eglTerminate succeeds for a valid, uninitialized EGLDisplay.

Covered requirements:
    - GS-EGL10-IN-TER-005
*/

static const char* test_case = "GS_EGL10_IN_TER_TC_005";
static const char* test_procedure = "GS_EGL10_IN_TER_TP_005";

/* Terminates a valid display obtained without calling eglInitialize. */
void GS_EGL10_IN_TER_TP_005_init(void) {
    EGLDisplay display;
    EGLBoolean result;
    EGLint error;

    /* Do not call eglInitialize: the display must remain uninitialized. */
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain a valid EGLDisplay, error: 0x%x", eglGetError());
        return;
    }

    (void)eglGetError();
    result = eglTerminate(display);
    error = eglGetError();

    if (result != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_TRUE for a valid uninitialized display, error: 0x%x",
            error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this uninitialized-display test. */
void GS_EGL10_IN_TER_TP_005_draw(void) {

}

/* No EGL objects are created by this test. */
void GS_EGL10_IN_TER_TP_005_close(void) {

}
