#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - Initialization - eglTerminate

Verify that the graphics library provides eglTerminate with the EGL 1.0
function interface.

Covered requirements:
    - GS-EGL10-IN-TER-001
*/

static const char* test_case = "GS_EGL10_IN_TER_TC_001";
static const char* test_procedure = "GS_EGL10_IN_TER_TP_001";

/* Verifies the API declaration and linked entry point. */
void GS_EGL10_IN_TER_TP_001_init(void) {
    EGLBoolean (*terminate_display)(EGLDisplay) = eglTerminate;
    EGLBoolean result;

    /*
     * Assigning the function to a pointer with the required EGL 1.0 type,
     * then calling it, verifies both the declaration and the linked symbol.
     * The return value is not evaluated by this interface-availability test.
     */
    result = terminate_display(EGL_NO_DISPLAY);
    (void)result;
    (void)eglGetError();

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this API availability test. */
void GS_EGL10_IN_TER_TP_001_draw(void) {

}

/* No EGL objects are created by this test. */
void GS_EGL10_IN_TER_TP_001_close(void) {

}
