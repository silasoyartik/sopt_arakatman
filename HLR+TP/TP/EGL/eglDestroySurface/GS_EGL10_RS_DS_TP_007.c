#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - RenderingSurfaces - eglDestroySurface

Verify the EGL_BAD_DISPLAY result for an invalid EGLDisplay.

Covered requirement:
    - GS-EGL10-RS-DS-007
*/

static const char* test_case = "GS_EGL10_RS_DS_TC_007";
static const char* test_procedure = "GS_EGL10_RS_DS_TP_007";

/* Calls eglDestroySurface with the invalid EGL_NO_DISPLAY handle. */
void GS_EGL10_RS_DS_TP_007_init(void) {
    EGLBoolean result;
    EGLint error;

    /*
     * EGL_NO_DISPLAY deliberately represents the invalid display condition.
     * No EGL setup is required because the display argument is under test.
     */
    (void)eglGetError();
    result = eglDestroySurface(EGL_NO_DISPLAY, EGL_NO_SURFACE);
    error = eglGetError();

    if ((result != EGL_FALSE) || (error != EGL_BAD_DISPLAY)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_DISPLAY, got: %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this invalid-display test. */
void GS_EGL10_RS_DS_TP_007_draw(void) {

}

/* No EGL objects are created by this negative test. */
void GS_EGL10_RS_DS_TP_007_close(void) {

}
