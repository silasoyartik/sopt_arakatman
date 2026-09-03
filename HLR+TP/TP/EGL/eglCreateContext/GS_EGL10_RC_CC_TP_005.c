#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglCreateContext

Verify that an invalid share_context generates EGL_BAD_CONTEXT.

Covered requirements:
    - GS-EGL10-RC-CC-005
*/

static const char* test_case = "GS_EGL10_RC_CC_TC_005";
static const char* test_procedure = "GS_EGL10_RC_CC_TP_005";

/* Uses a destroyed context handle to verify EGL_BAD_CONTEXT reporting. */
void GS_EGL10_RC_CC_TP_005_init(void) {
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLContext invalid_share_context;
    EGLint config_count = 0;
    EGLint error;

    display = eglGetCurrentDisplay();
    if (display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "An initialized current EGLDisplay is required");
        return;
    }

    if ((eglGetConfigs(display, &config, 1, &config_count) != EGL_TRUE) ||
        (config_count < 1)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain a valid EGLConfig, error: 0x%x", eglGetError());
        return;
    }

    /*
     * Create and destroy a context before using its former handle. The stale
     * handle is guaranteed not to name a valid EGLContext at the call below.
     */
    (void)eglGetError();
    invalid_share_context = eglCreateContext(display, config, EGL_NO_CONTEXT,
        NULL);
    error = eglGetError();
    if (invalid_share_context == EGL_NO_CONTEXT) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create the context used to obtain an invalid handle, error: 0x%x",
            error);
        return;
    }

    if (eglDestroyContext(display, invalid_share_context) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not destroy the context used to obtain an invalid handle, error: 0x%x",
            eglGetError());
        return;
    }

    (void)eglGetError();
    context = eglCreateContext(display, config, invalid_share_context, NULL);
    error = eglGetError();

    if (context != EGL_NO_CONTEXT) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglCreateContext unexpectedly accepted an invalid share_context");
        (void)eglDestroyContext(display, context);
        return;
    }

    if (error != EGL_BAD_CONTEXT) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_BAD_CONTEXT, got: 0x%x", error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this error-condition test. */
void GS_EGL10_RC_CC_TP_005_draw(void) {

}

/* No EGL objects remain after the initialization procedure. */
void GS_EGL10_RC_CC_TP_005_close(void) {

}
