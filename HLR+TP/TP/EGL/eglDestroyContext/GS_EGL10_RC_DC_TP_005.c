#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - RenderingContexts - eglDestroyContext

Verify final destruction after an otherwise valid eglMakeCurrent call.

Covered requirement:
    - GS-EGL10-RC-DC-005
*/

static const char* test_case = "GS_EGL10_RC_DC_TC_005";
static const char* test_procedure = "GS_EGL10_RC_DC_TP_005";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Releases a marked current context and verifies final invalidation. */
void GS_EGL10_RC_DC_TP_005_init(void) {
    EGLContext marked_context;
    EGLBoolean result;
    EGLint error;

    /*
     * Create and bind the context that will be marked for deletion. The
     * binding is retained until the eglMakeCurrent call under test.
     */
    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16) ||
        !GS_EGL10_make_environment_current(&environment)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
        return;
    }

    marked_context = environment.context;
    (void)eglGetError();
    result = eglDestroyContext(environment.display, marked_context);
    error = eglGetError();

    if (result == EGL_TRUE) {
        environment.context = EGL_NO_CONTEXT;
    }

    if ((result != EGL_TRUE) || (error != EGL_SUCCESS)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not mark the context for deletion, result: %u, error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
        return;
    }

    /*
     * Make an otherwise valid call that removes the current binding. This
     * call shall complete destruction of the previously marked context.
     */
    (void)eglGetError();
    result = eglMakeCurrent(environment.display, EGL_NO_SURFACE,
        EGL_NO_SURFACE, EGL_NO_CONTEXT);
    error = eglGetError();

    if ((result != EGL_TRUE) || (error != EGL_SUCCESS)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Valid eglMakeCurrent failed, result: %u, error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
        return;
    }

    (void)eglGetError();
    result = eglDestroyContext(environment.display, marked_context);
    error = eglGetError();

    if ((result != EGL_FALSE) || (error != EGL_BAD_CONTEXT)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Context was not invalidated, result: %u, error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* No drawing is required for this final-destruction test. */
void GS_EGL10_RC_DC_TP_005_draw(void) {

}

/* Releases the remaining surface and display resources. */
void GS_EGL10_RC_DC_TP_005_close(void) {
    GS_EGL10_cleanup_environment(&environment);
}
