#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - RenderingSurfaces - eglCreateWindowSurface
 * Covered requirement: GS-EGL10-RS-CWS-008
 */
static const char* test_case = "GS_EGL10_RS_CWS_TC_008";
static const char* test_procedure = "GS_EGL10_RS_CWS_TP_008";

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean fixture_prepared = EGL_FALSE;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
extern EGLBoolean GS_EGL10_prepare_invalid_native_window(
    EGLDisplay *display, EGLConfig *config, EGLNativeWindowType *window);
extern void GS_EGL10_cleanup_invalid_native_window(void);
#endif

void GS_EGL10_RS_CWS_TP_008_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    EGLConfig config = (EGLConfig)0;
    EGLNativeWindowType window = (EGLNativeWindowType)0;
    EGLint error;

    fixture_prepared = GS_EGL10_prepare_invalid_native_window(
        &display, &config, &window);
    if (!fixture_prepared)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Invalid native-window setup failed");
        return;
    }

    /* The hook supplies a native window handle that the target platform
     * identifies as invalid. The test verifies the required EGL return value
     * and error without assuming a platform-specific handle representation.
     */
    (void)eglGetError();
    surface = eglCreateWindowSurface(display, config, window, NULL);
    error = eglGetError();

    if (surface != EGL_NO_SURFACE || error != EGL_BAD_NATIVE_WINDOW)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_NO_SURFACE/EGL_BAD_NATIVE_WINDOW, got %p/0x%x",
            (void*)surface, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: GS_EGL_PLATFORM_TEST_HOOKS is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_RS_CWS_TP_008_draw(void) { }

void GS_EGL10_RS_CWS_TP_008_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (surface != EGL_NO_SURFACE && display != EGL_NO_DISPLAY)
        (void)eglDestroySurface(display, surface);
    if (fixture_prepared)
        GS_EGL10_cleanup_invalid_native_window();
#endif
}
