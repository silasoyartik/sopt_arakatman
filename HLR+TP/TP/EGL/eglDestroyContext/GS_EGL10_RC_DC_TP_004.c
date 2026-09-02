#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - RenderingContexts - eglDestroyContext

Verify that a context marked for deletion remains valid only while current.

Covered requirement:
    - GS-EGL10-RC-DC-004
*/

static const char* test_case = "GS_EGL10_RC_DC_TC_004";
static const char* test_procedure = "GS_EGL10_RC_DC_TP_004";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Checks the validity boundary of a context marked for deletion. */
void GS_EGL10_RC_DC_TP_004_init(void) {
    EGLContext marked_context;
    EGLBoolean result;
    EGLint error;

    /*
     * Prepare and bind a context so it can remain valid after being marked.
     * All objects are created locally through the shared EGL test helper.
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

    if ((result != EGL_TRUE) || (error != EGL_SUCCESS) ||
        (eglGetCurrentContext() != marked_context)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "The marked context was not valid while current, result: %u, error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
        return;
    }

    /*
     * Release the current binding and reference the old handle again. Once it
     * is no longer current, the marked context shall be treated as invalid.
     */
    if (eglMakeCurrent(environment.display, EGL_NO_SURFACE,
            EGL_NO_SURFACE, EGL_NO_CONTEXT) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not release the current context, error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
        return;
    }

    (void)eglGetError();
    result = eglDestroyContext(environment.display, marked_context);
    error = eglGetError();

    if ((result != EGL_FALSE) || (error != EGL_BAD_CONTEXT)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Context remained valid after release, result: %u, error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* No drawing is required for this context-validity test. */
void GS_EGL10_RC_DC_TP_004_draw(void) {

}

/* Releases the remaining surface and display resources. */
void GS_EGL10_RC_DC_TP_004_close(void) {
    GS_EGL10_cleanup_environment(&environment);
}
