#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - RenderingSurfaces - eglDestroySurface

Verify that eglDestroySurface returns EGL_TRUE and causes a surface to become
invalid when it is no longer current to any thread.

Covered requirement:
    - GS-EGL10-RS-DS-002
*/

static const char* test_case = "GS_EGL10_RS_DS_TC_002";
static const char* test_procedure = "GS_EGL10_RS_DS_TP_002";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Verifies that a destroyed surface handle is no longer valid. */
static EGLBoolean verify_surface_is_invalid(EGLSurface surface) {
    EGLBoolean result;
    EGLint error;

    (void)eglGetError();
    result = eglDestroySurface(environment.display, surface);
    error = eglGetError();

    if ((result != EGL_FALSE) || (error != EGL_BAD_SURFACE)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Surface remained valid, result: %u, EGL error: 0x%x",
            (unsigned int)result, error);
        return EGL_FALSE;
    }

    return EGL_TRUE;
}

/* Tests invalidation for both non-current and current surfaces. */
void GS_EGL10_RS_DS_TP_002_init(void) {
    const EGLint pbuffer_attributes[] = {
        EGL_WIDTH, 16,
        EGL_HEIGHT, 16,
        EGL_NONE
    };
    EGLSurface tested_surface;
    EGLBoolean result;
    EGLint error;

    test_success = EGL_TRUE;

    /*
     * Create a valid non-current pbuffer surface owned entirely by this test.
     * This covers invalidation when no current binding delays destruction.
     */
    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
        return;
    }

    tested_surface = environment.surface;
    (void)eglGetError();
    result = eglDestroySurface(environment.display, tested_surface);
    error = eglGetError();

    if (result == EGL_TRUE) {
        environment.surface = EGL_NO_SURFACE;
    }

    if ((result != EGL_TRUE) || (error != EGL_SUCCESS)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Non-current surface destruction failed, result: %u, error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
        return;
    }

    if (!verify_surface_is_invalid(tested_surface)) {
        test_success = EGL_FALSE;
        return;
    }

    /*
     * Create another pbuffer surface and bind it to the existing context.
     * This surface shall become invalid only after its binding is removed.
     */
    environment.surface = eglCreatePbufferSurface(environment.display,
        environment.config, pbuffer_attributes);
    if (environment.surface == EGL_NO_SURFACE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create the current-surface scenario, error: 0x%x",
            eglGetError());
        test_success = EGL_FALSE;
        return;
    }

    if (!GS_EGL10_make_environment_current(&environment)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not make the surface current, error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
        return;
    }

    tested_surface = environment.surface;
    (void)eglGetError();
    result = eglDestroySurface(environment.display, tested_surface);
    error = eglGetError();

    if (result == EGL_TRUE) {
        environment.surface = EGL_NO_SURFACE;
    }

    if ((result != EGL_TRUE) || (error != EGL_SUCCESS)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Current surface destruction request failed, result: %u, error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
        return;
    }

    /*
     * Remove the current binding with an otherwise valid eglMakeCurrent call.
     * The pending destruction shall make the old surface handle invalid.
     */
    (void)eglGetError();
    result = eglMakeCurrent(environment.display, EGL_NO_SURFACE,
        EGL_NO_SURFACE, EGL_NO_CONTEXT);
    error = eglGetError();

    if ((result != EGL_TRUE) || (error != EGL_SUCCESS)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not release the current surface, result: %u, error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
        return;
    }

    if (!verify_surface_is_invalid(tested_surface)) {
        test_success = EGL_FALSE;
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* No drawing is required for this surface-lifecycle test. */
void GS_EGL10_RS_DS_TP_002_draw(void) {

}

/* Releases the remaining context and display resources. */
void GS_EGL10_RS_DS_TP_002_close(void) {
    GS_EGL10_cleanup_environment(&environment);
}
