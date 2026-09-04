#include <EGL/egl.h>
#include "../../helpers.h"
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglCreateContext

Verify that an invalid config generates EGL_BAD_CONFIG.

Covered requirements:
    - GS-EGL10-RC-CC-005
*/

static const char* test_case = "GS_EGL10_RC_CC_TC_005";
static const char* test_procedure = "GS_EGL10_RC_CC_TP_005";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Uses a null EGLConfig handle to verify EGL_BAD_CONFIG reporting. */
void GS_EGL10_RC_CC_TP_005_init(void) {
    EGLDisplay display;
    EGLConfig invalid_config = (EGLConfig)0;
    EGLContext context;
    EGLint error;

    if (GS_EGL10_initialize_display(&environment) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain and initialize EGL_DEFAULT_DISPLAY, error: 0x%x",
            eglGetError());
        return;
    }
    display = environment.display;

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
void GS_EGL10_RC_CC_TP_005_draw(void) {

}

/* Releases the display initialization established by this independent test. */
void GS_EGL10_RC_CC_TP_005_close(void) {
    GS_EGL10_cleanup_environment(&environment);
}
