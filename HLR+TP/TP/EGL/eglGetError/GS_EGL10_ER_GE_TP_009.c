#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - Errors - eglGetError
 * Covered requirement: GS-EGL10-ER-GE-009
 * Invalidating a current native surface requires a platform test hook.
 */
static const char* test_case = "GS_EGL10_ER_GE_TC_009";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_009";

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean fixture_prepared = EGL_FALSE;
extern EGLBoolean GS_EGL10_prepare_make_current_error(
    EGLint expected_error, EGLDisplay *display, EGLSurface *draw,
    EGLSurface *read, EGLContext *context);
extern void GS_EGL10_cleanup_make_current_error(EGLint expected_error);
#endif

void GS_EGL10_ER_GE_TP_009_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    EGLDisplay display;
    EGLSurface draw;
    EGLSurface read;
    EGLContext context;
    EGLBoolean result;
    EGLint error;

    fixture_prepared = GS_EGL10_prepare_make_current_error(
        EGL_BAD_CURRENT_SURFACE, &display, &draw, &read, &context);
    if (!fixture_prepared)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Platform setup for EGL_BAD_CURRENT_SURFACE failed");
        return;
    }

    /* The platform hook invalidates the surface that was previously current.
     * eglMakeCurrent triggers the defined error, which is then retrieved in
     * the same thread by eglGetError.
     */
    (void)eglGetError();
    result = eglMakeCurrent(display, draw, read, context);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_CURRENT_SURFACE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_CURRENT_SURFACE, got %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: GS_EGL_PLATFORM_TEST_HOOKS is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_ER_GE_TP_009_draw(void) { }

void GS_EGL10_ER_GE_TP_009_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (fixture_prepared)
        GS_EGL10_cleanup_make_current_error(EGL_BAD_CURRENT_SURFACE);
#endif
}
