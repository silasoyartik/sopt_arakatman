#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - RenderingContexts - eglDestroyContext

Verify the EGL_NOT_INITIALIZED result for an uninitialized EGLDisplay.

Covered requirement:
    - GS-EGL10-RC-DC-007
*/

static const char* test_case = "GS_EGL10_RC_DC_TC_007";
static const char* test_procedure = "GS_EGL10_RC_DC_TP_007";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Uses a valid display handle without calling eglInitialize. */
void GS_EGL10_RC_DC_TP_007_init(void) {
    EGLBoolean result;
    EGLint error;

    /*
     * Obtain a valid display handle but intentionally leave it uninitialized.
     * The handle is checked before it is used by the negative test.
     */
    environment.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (environment.display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain a valid EGLDisplay, error: 0x%x", eglGetError());
        return;
    }

    (void)eglGetError();
    result = eglDestroyContext(environment.display, EGL_NO_CONTEXT);
    error = eglGetError();

    if ((result != EGL_FALSE) || (error != EGL_NOT_INITIALIZED)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_NOT_INITIALIZED, got: %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this uninitialized-display test. */
void GS_EGL10_RC_DC_TP_007_draw(void) {

}

/* Resets the local environment; no initialized display must be terminated. */
void GS_EGL10_RC_DC_TP_007_close(void) {
    GS_EGL10_cleanup_environment(&environment);
}
