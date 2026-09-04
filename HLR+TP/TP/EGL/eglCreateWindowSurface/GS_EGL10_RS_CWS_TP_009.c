#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - RenderingSurfaces - eglCreateWindowSurface
 * Covered requirement: GS-EGL10-RS-CWS-009
 */
static const char* test_case = "GS_EGL10_RS_CWS_TC_009";
static const char* test_procedure = "GS_EGL10_RS_CWS_TP_009";

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean fixture_prepared = EGL_FALSE;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface first_surface = EGL_NO_SURFACE;
static EGLSurface second_surface = EGL_NO_SURFACE;
extern EGLBoolean GS_EGL10_prepare_native_window(EGLDisplay *display,
    EGLConfig *config, EGLNativeWindowType *window);
extern void GS_EGL10_cleanup_native_window(void);
#endif

void GS_EGL10_RS_CWS_TP_009_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    EGLConfig config = (EGLConfig)0;
    EGLNativeWindowType window = (EGLNativeWindowType)0;
    EGLint error;

    fixture_prepared = GS_EGL10_prepare_native_window(
        &display, &config, &window);
    if (!fixture_prepared)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Native-window setup failed");
        return;
    }

    first_surface = eglCreateWindowSurface(display, config, window, NULL);
    if (first_surface == EGL_NO_SURFACE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "First window surface creation failed, error: 0x%x",
            eglGetError());
        return;
    }

    /* Keep the first EGLSurface alive so the native window remains associated
     * with an EGLConfig. A second creation request for the same window shall
     * then fail with EGL_BAD_ALLOC.
     */
    (void)eglGetError();
    second_surface = eglCreateWindowSurface(display, config, window, NULL);
    error = eglGetError();

    if (second_surface != EGL_NO_SURFACE || error != EGL_BAD_ALLOC)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_NO_SURFACE/EGL_BAD_ALLOC, got %p/0x%x",
            (void*)second_surface, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: GS_EGL_PLATFORM_TEST_HOOKS is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_RS_CWS_TP_009_draw(void) { }

void GS_EGL10_RS_CWS_TP_009_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (display != EGL_NO_DISPLAY)
    {
        if (second_surface != EGL_NO_SURFACE)
            (void)eglDestroySurface(display, second_surface);
        if (first_surface != EGL_NO_SURFACE)
            (void)eglDestroySurface(display, first_surface);
    }
    if (fixture_prepared)
        GS_EGL10_cleanup_native_window();
#endif
}
