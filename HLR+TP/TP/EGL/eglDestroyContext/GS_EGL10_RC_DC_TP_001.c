#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - RenderingContexts - eglDestroyContext

Verify that the graphics library provides the eglDestroyContext API function
and accepts a destruction request for a valid EGL rendering context.

Covered requirement:
    - GS-EGL10-RC-DC-001
*/

static const char* test_case = "GS_EGL10_RC_DC_TC_001";
static const char* test_procedure = "GS_EGL10_RC_DC_TP_001";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Verifies the API declaration, linked entry point and destruction request. */
void GS_EGL10_RC_DC_TP_001_init(void) {
    EGLBoolean (*destroy_context)(EGLDisplay, EGLContext) =
        eglDestroyContext;
    EGLBoolean result;
    EGLint error;

    /*
     * Create a valid display, config and non-current context within this test.
     * The helper also creates a pbuffer surface that cleanup will release.
     */
    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
        return;
    }

    /*
     * Call through the required EGL 1.0 function type using the valid context.
     * A successful result confirms that the destruction request was accepted.
     */
    (void)eglGetError();
    result = destroy_context(environment.display, environment.context);
    error = eglGetError();

    if (result == EGL_TRUE) {
        environment.context = EGL_NO_CONTEXT;
    }

    if ((result != EGL_TRUE) || (error != EGL_SUCCESS)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglDestroyContext failed, result: %u, EGL error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* No drawing is required for this context-destruction test. */
void GS_EGL10_RC_DC_TP_001_draw(void) {

}

/* Releases the pbuffer surface and terminates the test display. */
void GS_EGL10_RC_DC_TP_001_close(void) {
    GS_EGL10_cleanup_environment(&environment);
}
