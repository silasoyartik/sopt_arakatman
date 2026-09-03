#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - RenderingSurfaces - eglDestroySurface

Verify that destruction is deferred while the surface remains current.

Covered requirement:
    - GS-EGL10-RS-DS-003
*/

static const char* test_case = "GS_EGL10_RS_DS_TC_003";
static const char* test_procedure = "GS_EGL10_RS_DS_TP_003";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Marks a current surface for deletion and verifies that it remains current. */
void GS_EGL10_RS_DS_TP_003_init(void) {
    EGLSurface marked_surface;
    EGLBoolean result;
    EGLint error;

    test_success = EGL_TRUE;

    /*
     * Create a complete pbuffer environment and make its surface current.
     * The surface remains bound when the destruction request is performed.
     */
    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16) ||
        !GS_EGL10_make_environment_current(&environment)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
        return;
    }

    marked_surface = environment.surface;

    /*
     * Request destruction while the surface is current. EGL shall accept the
     * request but retain the surface until its current binding is removed.
     */
    (void)eglGetError();
    result = eglDestroySurface(environment.display, marked_surface);
    error = eglGetError();

    if (result == EGL_TRUE) {
        environment.surface = EGL_NO_SURFACE;
    }

    if ((result != EGL_TRUE) || (error != EGL_SUCCESS)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_TRUE/EGL_SUCCESS, got: %u/0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
        return;
    }

    if ((eglGetCurrentSurface(EGL_DRAW) != marked_surface) ||
        (eglGetCurrentSurface(EGL_READ) != marked_surface)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "The surface was not retained while current");
        test_success = EGL_FALSE;
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* No drawing is required for this deferred-destruction test. */
void GS_EGL10_RS_DS_TP_003_draw(void) {

}

/* Releasing the current binding completes destruction, then cleanup continues. */
void GS_EGL10_RS_DS_TP_003_close(void) {
    GS_EGL10_cleanup_environment(&environment);
}
