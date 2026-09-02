#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - RenderingContexts - eglDestroyContext

Verify that a successful eglDestroyContext call marks a valid non-current
context for deletion and returns EGL_TRUE.

Covered requirement:
    - GS-EGL10-RC-DC-002
*/

static const char* test_case = "GS_EGL10_RC_DC_TC_002";
static const char* test_procedure = "GS_EGL10_RC_DC_TP_002";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Verifies successful deletion and invalidation of a non-current context. */
void GS_EGL10_RC_DC_TP_002_init(void) {
    EGLContext destroyed_context;
    EGLBoolean result;
    EGLint error;

    /*
     * Create a valid non-current context owned entirely by this test. A
     * non-current context can be deleted without deferred-destruction rules.
     */
    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
        return;
    }

    destroyed_context = environment.context;

    /*
     * Destroy the valid context and verify the successful result. The stored
     * environment handle is cleared to prevent cleanup from destroying it again.
     */
    (void)eglGetError();
    result = eglDestroyContext(environment.display, destroyed_context);
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

    /*
     * Reuse the destroyed handle to confirm that it no longer names a valid
     * context. This provides observable evidence of the deletion request.
     */
    (void)eglGetError();
    result = eglDestroyContext(environment.display, destroyed_context);
    error = eglGetError();

    if ((result != EGL_FALSE) || (error != EGL_BAD_CONTEXT)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Destroyed context remained valid, result: %u, EGL error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* No drawing is required for this context-destruction test. */
void GS_EGL10_RC_DC_TP_002_draw(void) {

}

/* Releases the pbuffer surface and terminates the test display. */
void GS_EGL10_RC_DC_TP_002_close(void) {
    GS_EGL10_cleanup_environment(&environment);
}
