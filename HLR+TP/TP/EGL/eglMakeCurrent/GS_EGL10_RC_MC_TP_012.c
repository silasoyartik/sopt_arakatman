#include <EGL/egl.h>
#include "../../helper.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-012
 *
 * Define GS_EGL_PLATFORM_TEST_HOOKS when the target fixture can create the
 * required invalid native window condition.
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_012";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_012";
static EGLBoolean test_success = EGL_TRUE;

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
extern EGLBoolean GS_EGL10_prepare_make_current_error(
    EGLint expected_error, EGLDisplay *display, EGLSurface *draw,
    EGLSurface *read, EGLContext *context);
extern void GS_EGL10_cleanup_make_current_error(EGLint expected_error);
#endif

void GS_EGL10_RC_MC_TP_012_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    EGLDisplay display;
    EGLSurface draw;
    EGLSurface read;
    EGLContext context;
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_prepare_make_current_error(EGL_BAD_NATIVE_WINDOW,
            &display, &draw, &read, &context))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Platform setup for invalid native window failed");
        test_success = EGL_FALSE;
        return;
    }

    // Test starts here: execute eglMakeCurrent in the injected condition.
    (void)eglGetError();
    result = eglMakeCurrent(display, draw, read, context);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_NATIVE_WINDOW)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_NATIVE_WINDOW, got %u/0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    (void)test_success;
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: GS_EGL_PLATFORM_TEST_HOOKS is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_RC_MC_TP_012_draw(void) { }

void GS_EGL10_RC_MC_TP_012_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    GS_EGL10_cleanup_make_current_error(EGL_BAD_NATIVE_WINDOW);
#endif
}

