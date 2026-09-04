#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - Initialization - eglTerminate

Verify that an invalid EGLDisplay generates EGL_BAD_DISPLAY.

Covered requirements:
    - GS-EGL10-IN-TER-003
*/

static const char* test_case = "GS_EGL10_IN_TER_TC_003";
static const char* test_procedure = "GS_EGL10_IN_TER_TP_003";

/* Calls eglTerminate with EGL_NO_DISPLAY and verifies the reported error. */
void GS_EGL10_IN_TER_TP_003_init(void) {
    EGLBoolean result;
    EGLint error;

    (void)eglGetError();
    result = eglTerminate(EGL_NO_DISPLAY);
    error = eglGetError();

    if (result != EGL_FALSE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglTerminate unexpectedly succeeded for EGL_NO_DISPLAY");
        return;
    }

    if (error != EGL_BAD_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_BAD_DISPLAY, got: 0x%x", error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this error-condition test. */
void GS_EGL10_IN_TER_TP_003_draw(void) {

}

/* No EGL objects are created by this negative test. */
void GS_EGL10_IN_TER_TP_003_close(void) {

}
