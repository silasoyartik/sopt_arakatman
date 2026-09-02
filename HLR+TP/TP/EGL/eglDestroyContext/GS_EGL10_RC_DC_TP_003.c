#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - RenderingContexts - eglDestroyContext

Verify that destruction is deferred while the context remains current.

Covered requirement:
    - GS-EGL10-RC-DC-003
*/

static const char* test_case = "GS_EGL10_RC_DC_TC_003";
static const char* test_procedure = "GS_EGL10_RC_DC_TP_003";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Marks a current context for deletion and verifies that it remains current. */
void GS_EGL10_RC_DC_TP_003_init(void) {
    EGLContext marked_context;
    EGLBoolean result;
    EGLint error;

    /*
     * Create a complete pbuffer environment and make its context current.
     * The context must remain bound when the destruction request is made.
     */
    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16) ||
        !GS_EGL10_make_environment_current(&environment)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
        return;
    }

    marked_context = environment.context;

    /*
     * Request destruction while the context is current. EGL shall accept the
     * request but defer the actual destruction until the binding is changed.
     */
    (void)eglGetError();
    result = eglDestroyContext(environment.display, marked_context);
    error = eglGetError();

    if (result == EGL_TRUE) {
        environment.context = EGL_NO_CONTEXT;
    }

    if ((result != EGL_TRUE) || (error != EGL_SUCCESS)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_TRUE/EGL_SUCCESS, got: %u/0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
        return;
    }

    if (eglGetCurrentContext() != marked_context) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "The context was not retained while current");
        test_success = EGL_FALSE;
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* No drawing is required for this deferred-destruction test. */
void GS_EGL10_RC_DC_TP_003_draw(void) {

}

/* Releasing the current binding completes destruction, then cleanup continues. */
void GS_EGL10_RC_DC_TP_003_close(void) {
    GS_EGL10_cleanup_environment(&environment);
}
