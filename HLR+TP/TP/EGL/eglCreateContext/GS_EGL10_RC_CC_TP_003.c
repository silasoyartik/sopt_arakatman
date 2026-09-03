#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglCreateContext

Verify that an EGL_NONE-terminated empty attribute list creates a context.

Covered requirements:
    - GS-EGL10-RC-CC-003
*/

static const char* test_case = "GS_EGL10_RC_CC_TC_003";
static const char* test_procedure = "GS_EGL10_RC_CC_TP_003";

static EGLDisplay test_display = EGL_NO_DISPLAY;
static EGLContext test_context = EGL_NO_CONTEXT;

/* Uses EGL_NO_CONTEXT and an empty EGL_NONE-terminated attribute list. */
void GS_EGL10_RC_CC_TP_003_init(void) {
    EGLConfig config;
    EGLint config_count = 0;
    EGLint error;
    const EGLint attrib_list[] = { EGL_NONE };

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
    test_context = eglCreateContext(test_display, config, EGL_NO_CONTEXT,
        attrib_list);
    error = eglGetError();
    if (test_context == EGL_NO_CONTEXT) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglCreateContext failed for { EGL_NONE }, error: 0x%x", error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this attribute-list test. */
void GS_EGL10_RC_CC_TP_003_draw(void) {

}

/* Releases the context created by the test. */
void GS_EGL10_RC_CC_TP_003_close(void) {
    if (test_context != EGL_NO_CONTEXT) {
        (void)eglDestroyContext(test_display, test_context);
        test_context = EGL_NO_CONTEXT;
    }
}
