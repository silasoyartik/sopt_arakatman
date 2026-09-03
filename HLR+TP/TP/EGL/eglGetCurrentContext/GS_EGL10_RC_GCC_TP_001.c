#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglGetCurrentContext

Verify that the graphics library provides eglGetCurrentContext with the EGL
1.0 function interface.

Covered requirements:
    - GS-EGL10-RC-GCC-001
*/

static const char* test_case = "GS_EGL10_RC_GCC_TC_001";
static const char* test_procedure = "GS_EGL10_RC_GCC_TP_001";

/* Verifies the API declaration and linked entry point. */
void GS_EGL10_RC_GCC_TP_001_init(void) {
    EGLContext (*get_current_context)(void) = eglGetCurrentContext;
    EGLContext context;

    /*
     * Assigning the function to a pointer with the required EGL 1.0 type,
     * then calling it, verifies both the declaration and the linked symbol.
     * The return value is not evaluated by this interface-availability test.
     */
    context = get_current_context();
    (void)context;

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this API availability test. */
void GS_EGL10_RC_GCC_TP_001_draw(void) {

}

/* No EGL objects are created by this test. */
void GS_EGL10_RC_GCC_TP_001_close(void) {

}
