#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - RenderingSurfaces - eglCreateWindowSurface
 * Covered requirement: GS-EGL10-RS-CWS-004
 */
static const char* test_case = "GS_EGL10_RS_CWS_TC_004";
static const char* test_procedure = "GS_EGL10_RS_CWS_TP_004";

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean fixture_prepared = EGL_FALSE;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
extern EGLBoolean GS_EGL10_prepare_native_window(EGLDisplay *display,
    EGLConfig *config, EGLNativeWindowType *window);
extern void GS_EGL10_cleanup_native_window(void);
#endif

void GS_EGL10_RS_CWS_TP_004_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    const EGLint empty_attributes[] = { EGL_NONE };
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

    /* First create the surface with a NULL attribute list, then destroy it
     * and reuse the available native window with an EGL_NONE-only list. Both
     * forms shall be accepted without an EGL error.
     */
    (void)eglGetError();
    surface = eglCreateWindowSurface(display, config, window, NULL);
    error = eglGetError();
    if (surface == EGL_NO_SURFACE || error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "NULL list failed, surface/error: %p/0x%x",
            (void*)surface, error);
        return;
    }

    if (eglDestroySurface(display, surface) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not release first surface, error: 0x%x", eglGetError());
        return;
    }
    surface = EGL_NO_SURFACE;

    (void)eglGetError();
    surface = eglCreateWindowSurface(display, config, window,
        empty_attributes);
    error = eglGetError();
    if (surface == EGL_NO_SURFACE || error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "EGL_NONE list failed, surface/error: %p/0x%x",
            (void*)surface, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: GS_EGL_PLATFORM_TEST_HOOKS is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_RS_CWS_TP_004_draw(void) { }

void GS_EGL10_RS_CWS_TP_004_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (surface != EGL_NO_SURFACE && display != EGL_NO_DISPLAY)
        (void)eglDestroySurface(display, surface);
    if (fixture_prepared)
        GS_EGL10_cleanup_native_window();
#endif
}
