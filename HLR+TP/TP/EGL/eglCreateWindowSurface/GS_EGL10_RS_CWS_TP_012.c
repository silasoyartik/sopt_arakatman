#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - RenderingSurfaces - eglCreateWindowSurface
 * Covered requirement: GS-EGL10-RS-CWS-012
 */
static const char* test_case = "GS_EGL10_RS_CWS_TC_012";
static const char* test_procedure = "GS_EGL10_RS_CWS_TP_012";

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean fixture_prepared = EGL_FALSE;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
extern EGLBoolean GS_EGL10_prepare_native_window(EGLDisplay *display,
    EGLConfig *config, EGLNativeWindowType *window);
extern void GS_EGL10_cleanup_native_window(void);
#endif

void GS_EGL10_RS_CWS_TP_012_init(void)
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

    /* Terminate the valid display after the platform fixture has provided the
     * config and native window. The target call therefore receives a valid
     * EGLDisplay handle on which EGL is no longer initialized.
     */
    if (eglTerminate(display) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not terminate display, error: 0x%x", eglGetError());
        return;
    }

    (void)eglGetError();
    surface = eglCreateWindowSurface(display, config, window, NULL);
    error = eglGetError();

    if (surface != EGL_NO_SURFACE || error != EGL_NOT_INITIALIZED)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_NO_SURFACE/EGL_NOT_INITIALIZED, got %p/0x%x",
            (void*)surface, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: GS_EGL_PLATFORM_TEST_HOOKS is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_RS_CWS_TP_012_draw(void) { }

void GS_EGL10_RS_CWS_TP_012_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (surface != EGL_NO_SURFACE && display != EGL_NO_DISPLAY &&
        eglInitialize(display, NULL, NULL) == EGL_TRUE)
    {
        (void)eglDestroySurface(display, surface);
    }
    if (fixture_prepared)
        GS_EGL10_cleanup_native_window();
#endif
}
