#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglCreateContext

Verify that the graphics library provides eglCreateContext with the EGL 1.0
function interface.

Covered requirements:
    - GS-EGL10-RC-CC-001
*/

static const char* test_case = "GS_EGL10_RC_CC_TC_001";
static const char* test_procedure = "GS_EGL10_RC_CC_TP_001";

/* Verifies the API declaration and linked entry point. */
void GS_EGL10_RC_CC_TP_001_init(void) {
    EGLContext (*create_context)(EGLDisplay, EGLConfig, EGLContext,
        const EGLint*) = eglCreateContext;
    EGLContext context;

    /*
     * Assigning the function to a pointer with the required EGL 1.0 type,
     * then calling it, verifies both the declaration and the linked symbol.
     * The arguments are deliberately not a creation scenario; the return
     * value is not evaluated by this interface-availability test.
     */
    context = create_context(EGL_NO_DISPLAY, (EGLConfig)0,
        EGL_NO_CONTEXT, NULL);
    (void)context;
    (void)eglGetError();

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this API availability test. */
void GS_EGL10_RC_CC_TP_001_draw(void) {

}

/* No EGL objects are created by this test. */
void GS_EGL10_RC_CC_TP_001_close(void) {

}
