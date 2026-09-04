#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - RenderingSurfaces - eglCreateWindowSurface
 * Covered requirement: GS-EGL10-RS-CWS-010
 * A deterministic allocation failure requires a platform test hook.
 */
static const char* test_case = "GS_EGL10_RS_CWS_TC_010";
static const char* test_procedure = "GS_EGL10_RS_CWS_TP_010";

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean fixture_prepared = EGL_FALSE;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
extern EGLBoolean GS_EGL10_prepare_window_surface_allocation_failure(
    EGLDisplay *display, EGLConfig *config, EGLNativeWindowType *window);
extern void GS_EGL10_cleanup_window_surface_allocation_failure(void);
#endif

void GS_EGL10_RS_CWS_TP_010_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    EGLConfig config = (EGLConfig)0;
    EGLNativeWindowType window = (EGLNativeWindowType)0;
    EGLint error;

    fixture_prepared = GS_EGL10_prepare_window_surface_allocation_failure(
        &display, &config, &window);
    if (!fixture_prepared)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Allocation-failure setup failed");
        return;
    }

    /* The hook supplies otherwise valid inputs and forces resource allocation
     * for the new window surface to fail. This avoids unsafe and unpredictable
     * attempts to exhaust all system memory during the test.
     */
    (void)eglGetError();
    surface = eglCreateWindowSurface(display, config, window, NULL);
    error = eglGetError();

    if (surface != EGL_NO_SURFACE || error != EGL_BAD_ALLOC)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_NO_SURFACE/EGL_BAD_ALLOC, got %p/0x%x",
            (void*)surface, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: GS_EGL_PLATFORM_TEST_HOOKS is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_RS_CWS_TP_010_draw(void) { }

void GS_EGL10_RS_CWS_TP_010_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (surface != EGL_NO_SURFACE && display != EGL_NO_DISPLAY)
        (void)eglDestroySurface(display, surface);
    if (fixture_prepared)
        GS_EGL10_cleanup_window_surface_allocation_failure();
#endif
}
