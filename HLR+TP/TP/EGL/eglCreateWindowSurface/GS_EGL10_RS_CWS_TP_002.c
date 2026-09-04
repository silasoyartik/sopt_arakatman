#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - RenderingSurfaces - eglCreateWindowSurface
 * Covered requirement: GS-EGL10-RS-CWS-002
 */
static const char* test_case = "GS_EGL10_RS_CWS_TC_002";
static const char* test_procedure = "GS_EGL10_RS_CWS_TP_002";

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean fixture_prepared = EGL_FALSE;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
#endif

void GS_EGL10_RS_CWS_TP_002_init(void)
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

    /* The hook supplies an initialized display, a compatible window-capable
     * config and an available native window. Only eglCreateWindowSurface is
     * exercised after the error state is cleared.
     */
    (void)eglGetError();
    surface = eglCreateWindowSurface(display, config, window, NULL);
    error = eglGetError();

    if (surface == EGL_NO_SURFACE || error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected a valid EGLSurface/EGL_SUCCESS, got %p/0x%x",
            (void*)surface, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: GS_EGL_PLATFORM_TEST_HOOKS is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_RS_CWS_TP_002_draw(void) { }

void GS_EGL10_RS_CWS_TP_002_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (surface != EGL_NO_SURFACE && display != EGL_NO_DISPLAY)
        (void)eglDestroySurface(display, surface);
    if (fixture_prepared)
        GS_EGL10_cleanup_native_window();
#endif
}
