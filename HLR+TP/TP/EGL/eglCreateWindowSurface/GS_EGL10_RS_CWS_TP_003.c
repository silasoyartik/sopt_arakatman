#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - RenderingSurfaces - eglCreateWindowSurface
 * Covered requirement: GS-EGL10-RS-CWS-003
 */
static const char* test_case = "GS_EGL10_RS_CWS_TC_003";
static const char* test_procedure = "GS_EGL10_RS_CWS_TP_003";

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean fixture_prepared = EGL_FALSE;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
static EGLContext context = EGL_NO_CONTEXT;
extern EGLBoolean GS_EGL10_prepare_native_window(EGLDisplay *display,
    EGLConfig *config, EGLNativeWindowType *window);
extern void GS_EGL10_cleanup_native_window(void);
#endif

void GS_EGL10_RS_CWS_TP_003_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    EGLConfig config = (EGLConfig)0;
    EGLNativeWindowType window = (EGLNativeWindowType)0;
    EGLBoolean result;
    EGLint error;

    fixture_prepared = GS_EGL10_prepare_native_window(
        &display, &config, &window);
    if (!fixture_prepared)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Native-window setup failed");
        return;
    }

    surface = eglCreateWindowSurface(display, config, window, NULL);
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
    if (surface == EGL_NO_SURFACE || context == EGL_NO_CONTEXT)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Surface or context creation failed, error: 0x%x", eglGetError());
        return;
    }

    /* Bind a context created from the same compatible EGLConfig to the new
     * window surface. A successful eglMakeCurrent call demonstrates that the
     * surface can be used as the context's rendering target.
     */
    (void)eglGetError();
    result = eglMakeCurrent(display, surface, surface, context);
    error = eglGetError();

    if (result != EGL_TRUE || error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_TRUE/EGL_SUCCESS, got %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: GS_EGL_PLATFORM_TEST_HOOKS is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_RS_CWS_TP_003_draw(void) { }

void GS_EGL10_RS_CWS_TP_003_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (display != EGL_NO_DISPLAY)
    {
        (void)eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE,
            EGL_NO_CONTEXT);
        if (context != EGL_NO_CONTEXT)
            (void)eglDestroyContext(display, context);
        if (surface != EGL_NO_SURFACE)
            (void)eglDestroySurface(display, surface);
    }
    if (fixture_prepared)
        GS_EGL10_cleanup_native_window();
#endif
}
