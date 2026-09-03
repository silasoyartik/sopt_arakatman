#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglCreateContext

Verify that an invalid config generates EGL_BAD_CONFIG.

Covered requirements:
    - GS-EGL10-RC-CC-006
*/

static const char* test_case = "GS_EGL10_RC_CC_TC_006";
static const char* test_procedure = "GS_EGL10_RC_CC_TP_006";

/* Uses a null EGLConfig handle to verify EGL_BAD_CONFIG reporting. */
void GS_EGL10_RC_CC_TP_006_init(void) {
    EGLDisplay display;
    EGLConfig invalid_config = (EGLConfig)0;
    EGLContext context;
    EGLint error;

    display = eglGetCurrentDisplay();
    if (display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "An initialized current EGLDisplay is required");
        return;
    }

    (void)eglGetError();
    context = eglCreateContext(display, invalid_config, EGL_NO_CONTEXT, NULL);
    error = eglGetError();

    if (context != EGL_NO_CONTEXT) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglCreateContext unexpectedly accepted an invalid EGLConfig");
        (void)eglDestroyContext(display, context);
        return;
    }

    if (error != EGL_BAD_CONFIG) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_BAD_CONFIG, got: 0x%x", error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this error-condition test. */
void GS_EGL10_RC_CC_TP_006_draw(void) {

}

/* No EGL objects are created by this negative test. */
void GS_EGL10_RC_CC_TP_006_close(void) {

}
