#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglCreateContext

Verify creation with a valid EGLConfig whose EGL_DEPTH_SIZE is zero.

Covered requirements:
    - GS-EGL10-RC-CC-005
*/

static const char* test_case = "GS_EGL10_RC_CC_TC_005";
static const char* test_procedure = "GS_EGL10_RC_CC_TP_005";

static EGLDisplay test_display = EGL_NO_DISPLAY;
static EGLContext test_context = EGL_NO_CONTEXT;

/* Finds a valid configuration that has no depth buffer. */
static EGLBoolean find_zero_depth_config(EGLConfig* selected_config) {
    EGLConfig configs[64];
    EGLint config_count = 0;
    EGLint returned_count = 0;
    EGLint index;
    EGLint depth_size;

    if (eglGetConfigs(test_display, configs, 64, &config_count) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglGetConfigs failed, error: 0x%x", eglGetError());
        return EGL_FALSE;
    }

    returned_count = (config_count < 64) ? config_count : 64;
    for (index = 0; index < returned_count; ++index) {
        if ((eglGetConfigAttrib(test_display, configs[index], EGL_DEPTH_SIZE,
                &depth_size) == EGL_TRUE) && (depth_size == 0)) {
            *selected_config = configs[index];
            return EGL_TRUE;
        }
    }

    TEST_LOG_FAIL(test_case, test_procedure,
        "No valid EGLConfig with EGL_DEPTH_SIZE equal to 0 is available");
    return EGL_FALSE;
}

/* Creates a context with a configuration whose depth size is zero. */
void GS_EGL10_RC_CC_TP_005_init(void) {
    EGLConfig config;
    EGLint depth_size;
    EGLint error;

    test_display = eglGetCurrentDisplay();
    if (test_display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "An initialized current EGLDisplay is required");
        return;
    }

    if (!find_zero_depth_config(&config)) {
        return;
    }

    if ((eglGetConfigAttrib(test_display, config, EGL_DEPTH_SIZE, &depth_size) !=
            EGL_TRUE) || (depth_size != 0)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Selected EGLConfig does not report EGL_DEPTH_SIZE equal to 0");
        return;
    }

    (void)eglGetError();
    test_context = eglCreateContext(test_display, config, EGL_NO_CONTEXT, NULL);
    error = eglGetError();
    if (test_context == EGL_NO_CONTEXT) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglCreateContext failed for EGL_DEPTH_SIZE 0, error: 0x%x", error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this configuration test. */
void GS_EGL10_RC_CC_TP_005_draw(void) {

}

/* Releases the context created by the test. */
void GS_EGL10_RC_CC_TP_005_close(void) {
    if (test_context != EGL_NO_CONTEXT) {
        (void)eglDestroyContext(test_display, test_context);
        test_context = EGL_NO_CONTEXT;
    }
}
