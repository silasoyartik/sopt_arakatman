#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - Initialization - eglTerminate

Verify that eglTerminate succeeds for a valid initialized EGLDisplay and
leaves that display uninitialized.

Covered requirements:
    - GS-EGL10-IN-TER-002
*/

static const char* test_case = "GS_EGL10_IN_TER_TC_002";
static const char* test_procedure = "GS_EGL10_IN_TER_TP_002";

/* Verifies both the successful termination and the resulting display state. */
void GS_EGL10_IN_TER_TP_002_init(void) {
    EGLDisplay display;
    EGLBoolean result;
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
    result = eglTerminate(display);
    error = eglGetError();
    if (result != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_TRUE for a valid initialized display, error: 0x%x",
            error);
        return;
    }

    /* An EGL query requiring initialization must fail after termination. */
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

/* No drawing is required for this display-lifecycle test. */
void GS_EGL10_IN_TER_TP_002_draw(void) {

}

/* The display was terminated during initialization. */
void GS_EGL10_IN_TER_TP_002_close(void) {

}
