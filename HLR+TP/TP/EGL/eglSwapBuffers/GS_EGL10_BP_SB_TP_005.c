#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - BufferPosting - eglSwapBuffers
 * Covered requirement: GS-EGL10-BP-SB-005
 *
 * This test needs implementation instrumentation that can observe glFlush.
 */
static const char* test_case = "GS_EGL10_BP_SB_TC_005";
static const char* test_procedure = "GS_EGL10_BP_SB_TP_005";
static EGLBoolean test_success = EGL_TRUE;

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean fixture_prepared;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
extern EGLBoolean GS_EGL10_prepare_current_window_surface(
    EGLDisplay *display, EGLSurface *surface);
extern void GS_EGL10_begin_flush_observation(void);
extern EGLBoolean GS_EGL10_implicit_flush_observed(void);
extern void GS_EGL10_cleanup_current_window_surface(void);
#endif

void GS_EGL10_BP_SB_TP_005_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    fixture_prepared = GS_EGL10_prepare_current_window_surface(
        &display, &surface);
    if (!fixture_prepared)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Instrumented window setup failed");
        test_success = EGL_FALSE;
        return;
    }

    GS_EGL10_begin_flush_observation();

    // Test starts here: eglSwapBuffers shall issue an implicit glFlush.
    if (eglSwapBuffers(display, surface) != EGL_TRUE ||
        !GS_EGL10_implicit_flush_observed())
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Implicit glFlush was not observed");
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    (void)test_success;
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: flush instrumentation is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_BP_SB_TP_005_draw(void) { }

void GS_EGL10_BP_SB_TP_005_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (fixture_prepared)
        GS_EGL10_cleanup_current_window_surface();
#endif
}

