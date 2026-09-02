#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - Errors - eglGetError

Covered requirement:
    - GS-EGL10-ER-GE-003
*/

static const char* test_case = "GS_EGL10_ER_GE_TC_003";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_003";

void GS_EGL10_ER_GE_TP_003_init(void) {
    EGLDisplay display;
    EGLBoolean result;
    EGLint config_count = 0;
    EGLint error;

    /*
     * Obtain a valid EGLDisplay for the test, but intentionally do not call
     * eglInitialize. The display must remain uninitialized for this scenario.
     */
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain a valid EGLDisplay");
        return;
    }

    /*
     * Discard any error produced before the operation under test. The next
     * error read can then be associated with the eglGetConfigs call below.
     */
    (void)eglGetError();

    /*
     * Call an operation that requires an initialized display. It shall fail,
     * and eglGetError shall report EGL_NOT_INITIALIZED for this calling thread.
     */
    result = eglGetConfigs(display, NULL, 0, &config_count);
    error = eglGetError();

    if (result != EGL_FALSE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglGetConfigs unexpectedly succeeded");
        return;
    }

    if (error != EGL_NOT_INITIALIZED) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_NOT_INITIALIZED, got: 0x%x", error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_ER_GE_TP_003_draw(void) {

}

void GS_EGL10_ER_GE_TP_003_close(void) {

}
