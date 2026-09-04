#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - RenderingSurfaces - eglCreateWindowSurface
 * Covered requirement: GS-EGL10-RS-CWS-011
 */
static const char* test_case = "GS_EGL10_RS_CWS_TC_011";
static const char* test_procedure = "GS_EGL10_RS_CWS_TP_011";

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean fixture_prepared = EGL_FALSE;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
#endif

void GS_EGL10_RS_CWS_TP_011_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    const EGLint invalid_attributes[] = {
        (EGLint)0x7fffffff, 0, EGL_NONE
    };
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

    /* All primary objects are valid, while attrib_list contains one undefined
     * attribute followed by EGL_NONE. The target function shall reject only
     * this attribute-list condition with EGL_BAD_ATTRIBUTE.
     */
    (void)eglGetError();
    surface = eglCreateWindowSurface(display, config, window,
        invalid_attributes);
    error = eglGetError();

    if (surface != EGL_NO_SURFACE || error != EGL_BAD_ATTRIBUTE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_NO_SURFACE/EGL_BAD_ATTRIBUTE, got %p/0x%x",
            (void*)surface, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: GS_EGL_PLATFORM_TEST_HOOKS is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_RS_CWS_TP_011_draw(void) { }

void GS_EGL10_RS_CWS_TP_011_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (surface != EGL_NO_SURFACE && display != EGL_NO_DISPLAY)
        (void)eglDestroySurface(display, surface);
    if (fixture_prepared)
        GS_EGL10_cleanup_native_window();
#endif
}
