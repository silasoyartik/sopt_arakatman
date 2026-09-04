#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - Initialization - eglTerminate

Verify that eglTerminate succeeds for a valid, uninitialized EGLDisplay.

Covered requirements:
    - GS-EGL10-IN-TER-004
*/

static const char* test_case = "GS_EGL10_IN_TER_TC_004";
static const char* test_procedure = "GS_EGL10_IN_TER_TP_004";

/* Establishes an uninitialized display state, then verifies termination. */
void GS_EGL10_IN_TER_TP_004_init(void) {
    EGLDisplay display;
    EGLBoolean result;
    EGLint error;

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain a valid EGLDisplay, error: 0x%x", eglGetError());
        return;
    }

    /*
     * A successful termination leaves a valid display uninitialized. This
     * establishes the TER-005 precondition independently of prior test state.
     */
    (void)eglGetError();
    if (eglInitialize(display, NULL, NULL) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not initialize the EGLDisplay, error: 0x%x", eglGetError());
        return;
    }

    (void)eglGetError();
    if (eglTerminate(display) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not establish an uninitialized EGLDisplay, error: 0x%x",
            eglGetError());
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
void GS_EGL10_IN_TER_TP_004_draw(void) {

}

/* No EGL objects are created by this test. */
void GS_EGL10_IN_TER_TP_004_close(void) {

}
