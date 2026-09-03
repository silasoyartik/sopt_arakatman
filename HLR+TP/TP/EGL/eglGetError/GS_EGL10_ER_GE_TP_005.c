#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - Errors - eglGetError
 * Covered requirement: GS-EGL10-ER-GE-005
 * This resource-exhaustion condition requires a platform test hook.
 */
static const char* test_case = "GS_EGL10_ER_GE_TC_005";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_005";

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean fixture_prepared = EGL_FALSE;
extern EGLBoolean GS_EGL10_prepare_make_current_error(
    EGLint expected_error, EGLDisplay *display, EGLSurface *draw,
    EGLSurface *read, EGLContext *context);
extern void GS_EGL10_cleanup_make_current_error(EGLint expected_error);
#endif

void GS_EGL10_ER_GE_TP_005_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    EGLDisplay display;
    EGLSurface draw;
    EGLSurface read;
    EGLContext context;
    EGLBoolean result;
    EGLint error;

    fixture_prepared = GS_EGL10_prepare_make_current_error(EGL_BAD_ALLOC,
        &display, &draw, &read, &context);
    if (!fixture_prepared)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Platform setup for EGL_BAD_ALLOC failed");
        return;
    }

    /* Force the platform-provided allocation failure in eglMakeCurrent.
     * eglGetError is called immediately afterwards, so the returned value
     * belongs to that failed operation in the calling thread.
     */
    (void)eglGetError();
    result = eglMakeCurrent(display, draw, read, context);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_ALLOC)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_ALLOC, got %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: GS_EGL_PLATFORM_TEST_HOOKS is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_ER_GE_TP_005_draw(void) { }

void GS_EGL10_ER_GE_TP_005_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (fixture_prepared)
        GS_EGL10_cleanup_make_current_error(EGL_BAD_ALLOC);
#endif
}
