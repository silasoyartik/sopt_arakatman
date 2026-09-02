#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-014
 *
 * Define GS_EGL_PLATFORM_TEST_HOOKS when the target fixture can create the
 * required invalid previous current surface condition.
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_014";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_014";
static EGLBoolean test_success = EGL_TRUE;

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
extern EGLBoolean GS_EGL10_prepare_make_current_error(
    EGLint expected_error, EGLDisplay *display, EGLSurface *draw,
    EGLSurface *read, EGLContext *context);
extern void GS_EGL10_cleanup_make_current_error(EGLint expected_error);
#endif

void GS_EGL10_RC_MC_TP_014_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    EGLDisplay display;
    EGLSurface draw;
    EGLSurface read;
    EGLContext context;
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_prepare_make_current_error(EGL_BAD_CURRENT_SURFACE,
            &display, &draw, &read, &context))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Platform setup for invalid previous current surface failed");
        test_success = EGL_FALSE;
        return;
    }

    // Test starts here: execute eglMakeCurrent in the injected condition.
    (void)eglGetError();
    result = eglMakeCurrent(display, draw, read, context);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_CURRENT_SURFACE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_CURRENT_SURFACE, got %u/0x%x",
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

void GS_EGL10_RC_MC_TP_014_draw(void) { }

void GS_EGL10_RC_MC_TP_014_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    GS_EGL10_cleanup_make_current_error(EGL_BAD_CURRENT_SURFACE);
#endif
}

