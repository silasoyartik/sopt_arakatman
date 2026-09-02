#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - RenderingContexts - eglDestroyContext

Verify the EGL_BAD_CONTEXT result for an invalid rendering context.

Covered requirement:
    - GS-EGL10-RC-DC-006
*/

static const char* test_case = "GS_EGL10_RC_DC_TC_006";
static const char* test_procedure = "GS_EGL10_RC_DC_TP_006";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Calls eglDestroyContext with EGL_NO_CONTEXT on an initialized display. */
void GS_EGL10_RC_DC_TP_006_init(void) {
    EGLBoolean result;
    EGLint error;

    /*
     * Initialize a valid display so the context argument is the only invalid
     * input to the eglDestroyContext call performed by this test.
     */
    if (!GS_EGL10_initialize_display(&environment)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Display setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    (void)eglGetError();
    result = eglDestroyContext(environment.display, EGL_NO_CONTEXT);
    error = eglGetError();

    if ((result != EGL_FALSE) || (error != EGL_BAD_CONTEXT)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_CONTEXT, got: %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this invalid-context test. */
void GS_EGL10_RC_DC_TP_006_draw(void) {

}

/* Terminates the initialized test display. */
void GS_EGL10_RC_DC_TP_006_close(void) {
    GS_EGL10_cleanup_environment(&environment);
}
