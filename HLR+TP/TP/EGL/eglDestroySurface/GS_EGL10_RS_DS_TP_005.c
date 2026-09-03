#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - RenderingSurfaces - eglDestroySurface

Verify the EGL_BAD_SURFACE result for an invalid rendering surface.

Covered requirement:
    - GS-EGL10-RS-DS-005
*/

static const char* test_case = "GS_EGL10_RS_DS_TC_005";
static const char* test_procedure = "GS_EGL10_RS_DS_TP_005";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

/* Calls eglDestroySurface with EGL_NO_SURFACE on an initialized display. */
void GS_EGL10_RS_DS_TP_005_init(void) {
    EGLBoolean result;
    EGLint error;

    /*
     * Initialize a valid display so the surface argument is the only invalid
     * input to the eglDestroySurface call performed by this test.
     */
    if (!GS_EGL10_initialize_display(&environment)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Display setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    (void)eglGetError();
    result = eglDestroySurface(environment.display, EGL_NO_SURFACE);
    error = eglGetError();

    if ((result != EGL_FALSE) || (error != EGL_BAD_SURFACE)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_SURFACE, got: %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this invalid-surface test. */
void GS_EGL10_RS_DS_TP_005_draw(void) {

}

/* Terminates the initialized test display. */
void GS_EGL10_RS_DS_TP_005_close(void) {
    GS_EGL10_cleanup_environment(&environment);
}
