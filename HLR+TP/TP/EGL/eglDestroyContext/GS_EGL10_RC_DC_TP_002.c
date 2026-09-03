#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - RenderingContexts - eglDestroyContext

Verify that a successful eglDestroyContext call returns EGL_TRUE and causes
the context to become invalid when it is no longer current to any thread.

Covered requirement:
    - GS-EGL10-RC-DC-002
*/

static const char* test_case = "GS_EGL10_RC_DC_TC_002";
static const char* test_procedure = "GS_EGL10_RC_DC_TP_002";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Verifies that a destroyed context handle is no longer valid. */
static EGLBoolean verify_context_is_invalid(EGLContext context) {
    EGLBoolean result;
    EGLint error;

    (void)eglGetError();
    result = eglDestroyContext(environment.display, context);
    error = eglGetError();

    if ((result != EGL_FALSE) || (error != EGL_BAD_CONTEXT)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Context remained valid, result: %u, EGL error: 0x%x",
            (unsigned int)result, error);
        return EGL_FALSE;
    }

    return EGL_TRUE;
}

/* Tests invalidation for both non-current and current contexts. */
void GS_EGL10_RC_DC_TP_002_init(void) {
    EGLContext tested_context;
    EGLBoolean result;
    EGLint error;

    test_success = EGL_TRUE;

    /*
     * Create a valid non-current context owned entirely by this test. This
     * covers the case where no binding delays the context invalidation.
     */
    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
        return;
    }

    tested_context = environment.context;
    (void)eglGetError();
    result = eglDestroyContext(environment.display, tested_context);
    error = eglGetError();

    if (result == EGL_TRUE) {
        environment.context = EGL_NO_CONTEXT;
    }

    if ((result != EGL_TRUE) || (error != EGL_SUCCESS)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Non-current context destruction failed, result: %u, error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
        return;
    }

    if (!verify_context_is_invalid(tested_context)) {
        test_success = EGL_FALSE;
        return;
    }

    /*
     * Create another context and make it current before requesting deletion.
     * It shall become invalid only after its current binding is removed.
     */
    environment.context = eglCreateContext(environment.display,
        environment.config, EGL_NO_CONTEXT, NULL);
    if (environment.context == EGL_NO_CONTEXT) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create the current-context scenario, error: 0x%x",
            eglGetError());
        test_success = EGL_FALSE;
        return;
    }

    if (!GS_EGL10_make_environment_current(&environment)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not make the context current, error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
        return;
    }

    tested_context = environment.context;
    (void)eglGetError();
    result = eglDestroyContext(environment.display, tested_context);
    error = eglGetError();

    if (result == EGL_TRUE) {
        environment.context = EGL_NO_CONTEXT;
    }

    if ((result != EGL_TRUE) || (error != EGL_SUCCESS)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Current context destruction request failed, result: %u, error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
        return;
    }

    /*
     * Remove the current binding with an otherwise valid eglMakeCurrent call.
     * The pending destruction shall make the old context handle invalid.
     */
    (void)eglGetError();
    result = eglMakeCurrent(environment.display, EGL_NO_SURFACE,
        EGL_NO_SURFACE, EGL_NO_CONTEXT);
    error = eglGetError();

    if ((result != EGL_TRUE) || (error != EGL_SUCCESS)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not release the current context, result: %u, error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
        return;
    }

    if (!verify_context_is_invalid(tested_context)) {
        test_success = EGL_FALSE;
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* No drawing is required for this context-lifecycle test. */
void GS_EGL10_RC_DC_TP_002_draw(void) {

}

/* Releases the remaining pbuffer and display resources. */
void GS_EGL10_RC_DC_TP_002_close(void) {
    GS_EGL10_cleanup_environment(&environment);
}
