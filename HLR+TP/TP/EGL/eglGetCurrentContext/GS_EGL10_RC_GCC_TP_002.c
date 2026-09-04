#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglGetCurrentContext

Verify that eglGetCurrentContext returns the known EGLContext after it is
successfully bound to the calling thread.

Covered requirements:
    - GS-EGL10-RC-GCC-002
*/

static const char* test_case = "GS_EGL10_RC_GCC_TC_002";
static const char* test_procedure = "GS_EGL10_RC_GCC_TP_002";

static EGLDisplay framework_display = EGL_NO_DISPLAY;
static EGLSurface framework_draw_surface = EGL_NO_SURFACE;
static EGLSurface framework_read_surface = EGL_NO_SURFACE;
static EGLContext framework_context = EGL_NO_CONTEXT;
static EGLContext test_context = EGL_NO_CONTEXT;
static EGLBoolean framework_binding_replaced = EGL_FALSE;

/* Creates a context compatible with the framework's GBM/DRM/KMS surface. */
static EGLBoolean create_test_context(void) {
    EGLint config_id;
    EGLint config_count = 0;
    EGLint selected_attributes[] = {
        EGL_CONFIG_ID, 0,
        EGL_NONE
    };
    EGLConfig config;

    /*
     * Obtain the configuration of the framework context. The context handle
     * is used only to prepare a compatible test fixture, never as expected
     * output of the function under test.
     */
    if (eglQueryContext(framework_display, framework_context, EGL_CONFIG_ID,
            &config_id) != EGL_TRUE) {
        return EGL_FALSE;
    }

    selected_attributes[1] = config_id;
    if ((eglChooseConfig(framework_display, selected_attributes, &config, 1,
            &config_count) != EGL_TRUE) || (config_count != 1)) {
        return EGL_FALSE;
    }

    test_context = eglCreateContext(framework_display, config, EGL_NO_CONTEXT,
        NULL);
    return test_context != EGL_NO_CONTEXT;
}

/* Binds a known context successfully, then verifies that it is returned. */
void GS_EGL10_RC_GCC_TP_002_init(void) {
    EGLContext current_context;

    /*
     * The test framework has already created the GBM surface, associated it
     * with the DRM/KMS display path, and made this context current.
     */
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

    if (create_test_context() != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create a compatible EGL test context, error: 0x%x",
            eglGetError());
        return;
    }

    /*
     * Precondition for GCC-002: the successful binding below makes
     * test_context current to the calling thread.
     */
    if (eglMakeCurrent(framework_display, framework_draw_surface,
            framework_read_surface, test_context) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not bind the GBM/DRM/KMS EGL context, error: 0x%x",
            eglGetError());
        return;
    }
    framework_binding_replaced = EGL_TRUE;

    /* GCC-002: the queried handle shall be the context bound above. */
    current_context = eglGetCurrentContext();
    if (current_context != test_context) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected the independently created EGLContext handle to be returned");
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this current-context test. */
void GS_EGL10_RC_GCC_TP_002_draw(void) {

}

/* Restores the framework binding and destroys only the test-owned context. */
void GS_EGL10_RC_GCC_TP_002_close(void) {
    if ((framework_binding_replaced == EGL_TRUE) &&
        (framework_display != EGL_NO_DISPLAY)) {
        (void)eglMakeCurrent(framework_display, framework_draw_surface,
            framework_read_surface, framework_context);
    }

    if ((framework_display != EGL_NO_DISPLAY) &&
        (test_context != EGL_NO_CONTEXT)) {
        (void)eglDestroyContext(framework_display, test_context);
    }

    framework_display = EGL_NO_DISPLAY;
    framework_draw_surface = EGL_NO_SURFACE;
    framework_read_surface = EGL_NO_SURFACE;
    framework_context = EGL_NO_CONTEXT;
    test_context = EGL_NO_CONTEXT;
    framework_binding_replaced = EGL_FALSE;

}
