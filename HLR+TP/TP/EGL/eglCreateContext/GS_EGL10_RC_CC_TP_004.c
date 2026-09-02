#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglCreateContext

Verify that a NULL context attribute list is accepted.

Covered requirements:
    - GS-EGL10-RC-CC-004
*/

static const char* test_case = "GS_EGL10_RC_CC_TC_004";
static const char* test_procedure = "GS_EGL10_RC_CC_TP_004";

static EGLDisplay test_display = EGL_NO_DISPLAY;
static EGLContext test_context = EGL_NO_CONTEXT;

/* Creates a context without supplying an attribute list. */
void GS_EGL10_RC_CC_TP_004_init(void) {
    EGLConfig config;
    EGLint config_count = 0;
    EGLint error;

    test_display = eglGetCurrentDisplay();
    if (test_display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "An initialized current EGLDisplay is required");
        return;
    }

    if ((eglGetConfigs(test_display, &config, 1, &config_count) != EGL_TRUE) ||
        (config_count < 1)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain a valid EGLConfig, error: 0x%x", eglGetError());
        return;
    }

    (void)eglGetError();
    test_context = eglCreateContext(test_display, config, EGL_NO_CONTEXT, NULL);
    error = eglGetError();
    if (test_context == EGL_NO_CONTEXT) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglCreateContext failed for NULL attrib_list, error: 0x%x", error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this NULL attribute-list test. */
void GS_EGL10_RC_CC_TP_004_draw(void) {

}

/* Releases the context created by the test. */
void GS_EGL10_RC_CC_TP_004_close(void) {
    if (test_context != EGL_NO_CONTEXT) {
        (void)eglDestroyContext(test_display, test_context);
        test_context = EGL_NO_CONTEXT;
    }
}
