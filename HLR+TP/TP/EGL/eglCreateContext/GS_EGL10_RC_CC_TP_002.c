#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglCreateContext

Verify that a valid, compatible share_context creates a distinct second
context.

Covered requirements:
    - GS-EGL10-RC-CC-002
*/

static const char* test_case = "GS_EGL10_RC_CC_TC_002";
static const char* test_procedure = "GS_EGL10_RC_CC_TP_002";

static EGLDisplay test_display = EGL_NO_DISPLAY;
static EGLContext source_context = EGL_NO_CONTEXT;
static EGLContext shared_context = EGL_NO_CONTEXT;

/* Obtains the initialized display and one valid configuration for the test. */
static EGLBoolean get_test_display_and_config(EGLConfig* config) {
    EGLint config_count = 0;

    test_display = eglGetCurrentDisplay();
    if (test_display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "An initialized current EGLDisplay is required");
        return EGL_FALSE;
    }

    if ((eglGetConfigs(test_display, config, 1, &config_count) != EGL_TRUE) ||
        (config_count < 1)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain a valid EGLConfig, error: 0x%x", eglGetError());
        return EGL_FALSE;
    }

    return EGL_TRUE;
}

/* Creates a source context and a distinct context that shares its state. */
void GS_EGL10_RC_CC_TP_002_init(void) {
    EGLConfig config;
    EGLint error;
    const EGLint attrib_list[] = { EGL_NONE };

    if (!get_test_display_and_config(&config)) {
        return;
    }

    (void)eglGetError();
    source_context = eglCreateContext(test_display, config, EGL_NO_CONTEXT,
        attrib_list);
    error = eglGetError();
    if (source_context == EGL_NO_CONTEXT) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create the valid source context, error: 0x%x", error);
        return;
    }

    (void)eglGetError();
    shared_context = eglCreateContext(test_display, config, source_context,
        attrib_list);
    error = eglGetError();
    if (shared_context == EGL_NO_CONTEXT) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglCreateContext failed with a valid compatible share_context, error: 0x%x",
            error);
        return;
    }

    if (shared_context == source_context) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglCreateContext returned share_context instead of a new EGLContext");
        (void)eglDestroyContext(test_display, shared_context);
        shared_context = EGL_NO_CONTEXT;
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this context-sharing test. */
void GS_EGL10_RC_CC_TP_002_draw(void) {

}

/* Destroys the shared context before its source context. */
void GS_EGL10_RC_CC_TP_002_close(void) {
    if (shared_context != EGL_NO_CONTEXT) {
        (void)eglDestroyContext(test_display, shared_context);
        shared_context = EGL_NO_CONTEXT;
    }

    if (source_context != EGL_NO_CONTEXT) {
        (void)eglDestroyContext(test_display, source_context);
        source_context = EGL_NO_CONTEXT;
    }
}
