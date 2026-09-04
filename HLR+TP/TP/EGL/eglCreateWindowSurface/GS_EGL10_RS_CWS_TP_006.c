#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - RenderingSurfaces - eglCreateWindowSurface
 * Covered requirement: GS-EGL10-RS-CWS-006
 */
static const char* test_case = "GS_EGL10_RS_CWS_TC_006";
static const char* test_procedure = "GS_EGL10_RS_CWS_TP_006";

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean fixture_prepared = EGL_FALSE;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
#endif

void GS_EGL10_RS_CWS_TP_006_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    EGLConfig config = (EGLConfig)0;
    EGLNativeWindowType window = (EGLNativeWindowType)0;
    EGLint surface_type = 0;
    EGLint error;

    fixture_prepared = GS_EGL10_prepare_non_window_config(
        &display, &config, &window);
    if (!fixture_prepared)
    {
        TEST_LOG_INFO("[ %s ][ %s ] Not applicable: no non-window EGLConfig is available.",
            test_case, test_procedure);
        return;
    }

    if (eglGetConfigAttrib(display, config, EGL_SURFACE_TYPE,
            &surface_type) != EGL_TRUE || (surface_type & EGL_WINDOW_BIT) != 0)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Platform hook did not supply a non-window EGLConfig");
        return;
    }

    /* The selected config is valid but its EGL_SURFACE_TYPE was verified not
     * to contain EGL_WINDOW_BIT. It therefore cannot be used to create a
     * window surface for the supplied valid native window.
     */
    (void)eglGetError();
    surface = eglCreateWindowSurface(display, config, window, NULL);
    error = eglGetError();

    if (surface != EGL_NO_SURFACE || error != EGL_BAD_MATCH)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_NO_SURFACE/EGL_BAD_MATCH, got %p/0x%x",
            (void*)surface, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: GS_EGL_PLATFORM_TEST_HOOKS is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_RS_CWS_TP_006_draw(void) { }

void GS_EGL10_RS_CWS_TP_006_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (surface != EGL_NO_SURFACE && display != EGL_NO_DISPLAY)
        (void)eglDestroySurface(display, surface);
    if (fixture_prepared)
        GS_EGL10_cleanup_non_window_config();
#endif
}
