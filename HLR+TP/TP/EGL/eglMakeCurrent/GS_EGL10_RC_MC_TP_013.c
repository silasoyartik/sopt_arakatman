#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-013
 *
 * Define GS_EGL_PLATFORM_TEST_HOOKS when the target fixture can create the
 * required graphics-memory exhaustion condition.
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_013";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_013";
static EGLBoolean test_success = EGL_TRUE;

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
#endif

void GS_EGL10_RC_MC_TP_013_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    EGLDisplay display;
    EGLSurface draw;
    EGLSurface read;
    EGLContext context;
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_prepare_make_current_error(EGL_BAD_MATCH,
            &display, &draw, &read, &context))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Platform setup for graphics-memory exhaustion failed");
        test_success = EGL_FALSE;
        return;
    }

    // Test starts here: execute eglMakeCurrent in the injected condition.
    (void)eglGetError();
    result = eglMakeCurrent(display, draw, read, context);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_MATCH)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_MATCH, got %u/0x%x",
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

void GS_EGL10_RC_MC_TP_013_draw(void) { }

void GS_EGL10_RC_MC_TP_013_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    GS_EGL10_cleanup_make_current_error(EGL_BAD_MATCH);
#endif
}
