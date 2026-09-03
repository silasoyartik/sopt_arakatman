#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglGetCurrentContext

Verify that eglGetCurrentContext returns EGL_NO_CONTEXT and generates no EGL
error when the calling thread has no current context.

Covered requirements:
    - GS-EGL10-RC-GCC-003
*/

static const char* test_case = "GS_EGL10_RC_GCC_TC_003";
static const char* test_procedure = "GS_EGL10_RC_GCC_TP_003";

static EGLDisplay framework_display = EGL_NO_DISPLAY;
static EGLSurface framework_draw_surface = EGL_NO_SURFACE;
static EGLSurface framework_read_surface = EGL_NO_SURFACE;
static EGLContext framework_context = EGL_NO_CONTEXT;
static EGLBoolean context_detached = EGL_FALSE;

/* Detaches the context, then verifies the no-context and no-error results. */
void GS_EGL10_RC_GCC_TP_003_init(void) {
    EGLContext current_context;
    EGLint error;

    /* Capture the current binding created by the GBM/DRM/KMS test framework. */
    framework_display = eglGetCurrentDisplay();
    framework_draw_surface = eglGetCurrentSurface(EGL_DRAW);
    framework_read_surface = eglGetCurrentSurface(EGL_READ);
    framework_context = eglGetCurrentContext();

    if ((framework_display == EGL_NO_DISPLAY) ||
        (framework_draw_surface == EGL_NO_SURFACE) ||
        (framework_read_surface == EGL_NO_SURFACE) ||
        (framework_context == EGL_NO_CONTEXT)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "A current GBM/DRM/KMS EGL display, surfaces and context are required");
        return;
    }

    /*
     * Precondition for GCC-003: a successful no-context eglMakeCurrent call
     * leaves the calling thread with no current EGLContext.
     */
    (void)eglGetError();
    if (eglMakeCurrent(framework_display, EGL_NO_SURFACE, EGL_NO_SURFACE,
            EGL_NO_CONTEXT) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not detach the current GBM/DRM/KMS EGLContext, error: 0x%x",
            eglGetError());
        return;
    }
    context_detached = EGL_TRUE;

    /* GCC-003: EGL_NO_CONTEXT is returned and this query is not an error. */
    (void)eglGetError();
    current_context = eglGetCurrentContext();
    error = eglGetError();

    if (current_context != EGL_NO_CONTEXT) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_NO_CONTEXT after detaching the current context");
        return;
    }

    if (error != EGL_SUCCESS) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglGetCurrentContext generated an unexpected error: 0x%x", error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this no-current-context test. */
void GS_EGL10_RC_GCC_TP_003_draw(void) {

}

/* Restores the GBM/DRM/KMS binding owned by the test framework. */
void GS_EGL10_RC_GCC_TP_003_close(void) {
    if ((context_detached == EGL_TRUE) &&
        (framework_display != EGL_NO_DISPLAY)) {
        (void)eglMakeCurrent(framework_display, framework_draw_surface,
            framework_read_surface, framework_context);
    }

    framework_display = EGL_NO_DISPLAY;
    framework_draw_surface = EGL_NO_SURFACE;
    framework_read_surface = EGL_NO_SURFACE;
    framework_context = EGL_NO_CONTEXT;
    context_detached = EGL_FALSE;
}
