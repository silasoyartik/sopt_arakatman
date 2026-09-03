#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - RenderingSurfaces - eglDestroySurface

Verify that a surface marked for deletion remains valid only while current.

Covered requirement:
    - GS-EGL10-RS-DS-004
*/

static const char* test_case = "GS_EGL10_RS_DS_TC_004";
static const char* test_procedure = "GS_EGL10_RS_DS_TP_004";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Checks the validity boundary of a surface marked for deletion. */
void GS_EGL10_RS_DS_TP_004_init(void) {
    EGLSurface marked_surface;
    EGLBoolean result;
    EGLint error;

    test_success = EGL_TRUE;

    /*
     * Prepare and bind a surface so it can remain valid after being marked.
     * All EGL objects are created locally through the shared test helper.
     */
    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16) ||
        !GS_EGL10_make_environment_current(&environment)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
        return;
    }

    marked_surface = environment.surface;
    (void)eglGetError();
    result = eglDestroySurface(environment.display, marked_surface);
    error = eglGetError();

    if (result == EGL_TRUE) {
        environment.surface = EGL_NO_SURFACE;
    }

    if ((result != EGL_TRUE) || (error != EGL_SUCCESS) ||
        (eglGetCurrentSurface(EGL_DRAW) != marked_surface) ||
        (eglGetCurrentSurface(EGL_READ) != marked_surface)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "The marked surface was not valid while current, result: %u, error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
        return;
    }

    /*
     * Release the current binding and reference the old handle again. Once it
     * is no longer current, the marked surface shall be treated as invalid.
     */
    if (eglMakeCurrent(environment.display, EGL_NO_SURFACE,
            EGL_NO_SURFACE, EGL_NO_CONTEXT) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not release the current surface, error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
        return;
    }

    (void)eglGetError();
    result = eglDestroySurface(environment.display, marked_surface);
    error = eglGetError();

    if ((result != EGL_FALSE) || (error != EGL_BAD_SURFACE)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Surface remained valid after release, result: %u, error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* No drawing is required for this surface-validity test. */
void GS_EGL10_RS_DS_TP_004_draw(void) {

}

/* Releases the remaining context and display resources. */
void GS_EGL10_RS_DS_TP_004_close(void) {
    GS_EGL10_cleanup_environment(&environment);
}
