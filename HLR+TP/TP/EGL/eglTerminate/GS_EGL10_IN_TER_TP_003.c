#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - Initialization - eglTerminate

Verify that a successfully terminated display is left uninitialized.

Covered requirements:
    - GS-EGL10-IN-TER-003
*/

static const char* test_case = "GS_EGL10_IN_TER_TC_003";
static const char* test_procedure = "GS_EGL10_IN_TER_TP_003";

/* Terminates a display and verifies its subsequent uninitialized state. */
void GS_EGL10_IN_TER_TP_003_init(void) {
    EGLDisplay display;
    const char* query_result;
    EGLint error;

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain a valid EGLDisplay, error: 0x%x", eglGetError());
        return;
    }

    (void)eglGetError();
    if (eglInitialize(display, NULL, NULL) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not initialize the EGLDisplay, error: 0x%x", eglGetError());
        return;
    }

    (void)eglGetError();
    if (eglTerminate(display) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not terminate the initialized EGLDisplay, error: 0x%x",
            eglGetError());
        return;
    }

    /* eglQueryString requires initialization and shall fail after termination. */
    (void)eglGetError();
    query_result = eglQueryString(display, EGL_VENDOR);
    error = eglGetError();

    if (query_result != NULL) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglQueryString unexpectedly returned data after eglTerminate");
        return;
    }

    if (error != EGL_NOT_INITIALIZED) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_NOT_INITIALIZED after eglTerminate, got: 0x%x", error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this display-state test. */
void GS_EGL10_IN_TER_TP_003_draw(void) {

}

/* The test intentionally leaves the display in its terminated state. */
void GS_EGL10_IN_TER_TP_003_close(void) {

}
