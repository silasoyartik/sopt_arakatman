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
static EGLDisplay test_display = EGL_NO_DISPLAY;
static EGLBoolean display_initialized = EGL_FALSE;

/* Verifies both the successful termination and the resulting display state. */
void GS_EGL10_IN_TER_TP_002_init(void) {
    EGLBoolean result;
    const char* query_result;
    EGLint error;

    test_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (test_display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain a valid EGLDisplay, error: 0x%x", eglGetError());
        return;
    }

    (void)eglGetError();
    if (eglInitialize(test_display, NULL, NULL) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not initialize the EGLDisplay, error: 0x%x", eglGetError());
        return;
    }
    display_initialized = EGL_TRUE;

    (void)eglGetError();
    result = eglTerminate(test_display);
    error = eglGetError();
    if (result != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_TRUE for a valid initialized display, error: 0x%x",
            error);
        return;
    }
    display_initialized = EGL_FALSE;

    /* An EGL query requiring initialization must fail after termination. */
    (void)eglGetError();
    query_result = eglQueryString(test_display, EGL_VENDOR);
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

/* Cleans up only when termination failed before it could uninitialize dpy. */
void GS_EGL10_IN_TER_TP_002_close(void) {
    if ((display_initialized == EGL_TRUE) &&
        (test_display != EGL_NO_DISPLAY)) {
        (void)eglTerminate(test_display);
    }

    display_initialized = EGL_FALSE;
    test_display = EGL_NO_DISPLAY;
}
