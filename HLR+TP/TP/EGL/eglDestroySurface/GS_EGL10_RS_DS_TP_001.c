#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - RenderingSurfaces - eglDestroySurface

Verify that the graphics library provides the eglDestroySurface API function
and accepts a destruction request for a valid EGLSurface.

Covered requirement:
    - GS-EGL10-RS-DS-001
*/

static const char* test_case = "GS_EGL10_RS_DS_TC_001";
static const char* test_procedure = "GS_EGL10_RS_DS_TP_001";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Verifies the API declaration, linked entry point and destruction request. */
void GS_EGL10_RS_DS_TP_001_init(void) {
    EGLBoolean (*destroy_surface)(EGLDisplay, EGLSurface) =
        eglDestroySurface;
    EGLBoolean result;
    EGLint error;

    /*
     * Create a valid display, config and non-current pbuffer surface within
     * this test. No EGL objects are expected from an external main function.
     */
    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
        return;
    }

    /*
     * Call through the required EGL 1.0 function type using the valid surface.
     * A successful result confirms that the destruction request was accepted.
     */
    (void)eglGetError();
    result = destroy_surface(environment.display, environment.surface);
    error = eglGetError();

    if (result == EGL_TRUE) {
        environment.surface = EGL_NO_SURFACE;
    }

    if ((result != EGL_TRUE) || (error != EGL_SUCCESS)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglDestroySurface failed, result: %u, EGL error: 0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* No drawing is required for this API availability test. */
void GS_EGL10_RS_DS_TP_001_draw(void) {

}

/* Releases the context and terminates the test display. */
void GS_EGL10_RS_DS_TP_001_close(void) {
    GS_EGL10_cleanup_environment(&environment);
}
