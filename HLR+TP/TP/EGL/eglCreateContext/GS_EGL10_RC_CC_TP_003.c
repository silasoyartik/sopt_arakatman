#include <EGL/egl.h>
#include "../../helpers.h"
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglCreateContext

Verify that each valid attrib_list form (NULL and an EGL_NONE-terminated
attribute list) creates a context.

Covered requirements:
    - GS-EGL10-RC-CC-003
*/

static const char* test_case = "GS_EGL10_RC_CC_TC_003";
static const char* test_procedure = "GS_EGL10_RC_CC_TP_003";

static EGLDisplay test_display = EGL_NO_DISPLAY;
static EGLContext null_list_context = EGL_NO_CONTEXT;
static EGLContext empty_list_context = EGL_NO_CONTEXT;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Tests both valid attrib_list forms with EGL_NO_CONTEXT sharing. */
void GS_EGL10_RC_CC_TP_003_init(void) {
    EGLConfig config;
    EGLint config_count = 0;
    EGLint error;
    const EGLint attrib_list[] = { EGL_NONE };

    if (GS_EGL10_initialize_display(&environment) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain and initialize EGL_DEFAULT_DISPLAY, error: 0x%x",
            eglGetError());
        return;
    }
    test_display = environment.display;

    if ((eglGetConfigs(test_display, &config, 1, &config_count) != EGL_TRUE) ||
        (config_count < 1)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain a valid EGLConfig, error: 0x%x", eglGetError());
        return;
    }

    (void)eglGetError();
    empty_list_context = eglCreateContext(test_display, config, EGL_NO_CONTEXT,
        attrib_list);
    error = eglGetError();
    if (empty_list_context == EGL_NO_CONTEXT) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglCreateContext failed for { EGL_NONE }, error: 0x%x", error);
        return;
    }

    (void)eglGetError();
    null_list_context = eglCreateContext(test_display, config, EGL_NO_CONTEXT,
        NULL);
    error = eglGetError();
    if (null_list_context == EGL_NO_CONTEXT) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglCreateContext failed for NULL attrib_list, error: 0x%x", error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this attribute-list test. */
void GS_EGL10_RC_CC_TP_003_draw(void) {

}

/* Releases both contexts created by the test. */
void GS_EGL10_RC_CC_TP_003_close(void) {
    if (null_list_context != EGL_NO_CONTEXT) {
        (void)eglDestroyContext(test_display, null_list_context);
        null_list_context = EGL_NO_CONTEXT;
    }

    if (empty_list_context != EGL_NO_CONTEXT) {
        (void)eglDestroyContext(test_display, empty_list_context);
        empty_list_context = EGL_NO_CONTEXT;
    }

    GS_EGL10_cleanup_environment(&environment);
    test_display = EGL_NO_DISPLAY;
}
