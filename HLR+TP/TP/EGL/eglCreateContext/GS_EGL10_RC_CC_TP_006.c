#include <EGL/egl.h>
#include "../../helpers.h"
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglCreateContext

Verify that an invalid EGLDisplay generates EGL_BAD_DISPLAY.

Covered requirements:
    - GS-EGL10-RC-CC-006
*/

static const char* test_case = "GS_EGL10_RC_CC_TC_006";
static const char* test_procedure = "GS_EGL10_RC_CC_TP_006";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Uses a valid config with EGL_NO_DISPLAY to isolate the invalid display input. */
void GS_EGL10_RC_CC_TP_006_init(void) {
    EGLDisplay valid_display;
    EGLConfig valid_config;
    EGLContext context;
    EGLint config_count = 0;
    EGLint error;

    if (GS_EGL10_initialize_display(&environment) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain and initialize EGL_DEFAULT_DISPLAY, error: 0x%x",
            eglGetError());
        return;
    }
    valid_display = environment.display;

    if ((eglGetConfigs(valid_display, &valid_config, 1, &config_count) !=
            EGL_TRUE) || (config_count < 1)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain a valid EGLConfig, error: 0x%x", eglGetError());
        return;
    }

    /* CC-006: EGL_NO_DISPLAY is the only deliberately invalid argument. */
    (void)eglGetError();
    context = eglCreateContext(EGL_NO_DISPLAY, valid_config, EGL_NO_CONTEXT,
        NULL);
    error = eglGetError();

    if (context != EGL_NO_CONTEXT) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglCreateContext unexpectedly accepted EGL_NO_DISPLAY");
        (void)eglDestroyContext(valid_display, context);
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
void GS_EGL10_RC_CC_TP_006_draw(void) {

}

/* Releases the display initialization established by this independent test. */
void GS_EGL10_RC_CC_TP_006_close(void) {
    GS_EGL10_cleanup_environment(&environment);
}
