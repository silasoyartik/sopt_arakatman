#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglCreateContext

Verify creation with a valid EGLConfig whose EGL_DEPTH_SIZE is at least 16.

Covered requirements:
    - GS-EGL10-RC-CC-006
*/

static const char* test_case = "GS_EGL10_RC_CC_TC_006";
static const char* test_procedure = "GS_EGL10_RC_CC_TP_006";

static EGLDisplay test_display = EGL_NO_DISPLAY;
static EGLContext test_context = EGL_NO_CONTEXT;

/* Selects and verifies a configuration with at least 16 depth bits. */
void GS_EGL10_RC_CC_TP_006_init(void) {
    const EGLint config_attrib_list[] = {
        EGL_DEPTH_SIZE, 16,
        EGL_NONE
    };
    EGLConfig config;
    EGLint config_count = 0;
    EGLint depth_size;
    EGLint error;

    test_display = eglGetCurrentDisplay();
    if (test_display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "An initialized current EGLDisplay is required");
        return;
    }

    if ((eglChooseConfig(test_display, config_attrib_list, &config, 1,
            &config_count) != EGL_TRUE) || (config_count < 1)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "No EGLConfig satisfying EGL_DEPTH_SIZE >= 16 is available, error: 0x%x",
            eglGetError());
        return;
    }

    if ((eglGetConfigAttrib(test_display, config, EGL_DEPTH_SIZE, &depth_size) !=
            EGL_TRUE) || (depth_size < 16)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Selected EGLConfig does not report EGL_DEPTH_SIZE >= 16");
        return;
    }

    (void)eglGetError();
    test_context = eglCreateContext(test_display, config, EGL_NO_CONTEXT, NULL);
    error = eglGetError();
    if (test_context == EGL_NO_CONTEXT) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglCreateContext failed for EGL_DEPTH_SIZE >= 16, error: 0x%x", error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this configuration test. */
void GS_EGL10_RC_CC_TP_006_draw(void) {

}

/* Releases the context created by the test. */
void GS_EGL10_RC_CC_TP_006_close(void) {
    if (test_context != EGL_NO_CONTEXT) {
        (void)eglDestroyContext(test_display, test_context);
        test_context = EGL_NO_CONTEXT;
    }
}
