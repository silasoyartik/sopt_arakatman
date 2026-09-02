#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - BufferPosting - eglSwapBuffers
 * Covered requirement: GS-EGL10-BP-SB-008
 */
static const char* test_case = "GS_EGL10_BP_SB_TC_008";
static const char* test_procedure = "GS_EGL10_BP_SB_TP_008";
static EGLBoolean test_success = EGL_TRUE;

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean fixture_prepared;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
extern EGLBoolean GS_EGL10_prepare_current_window_surface(
    EGLDisplay *display, EGLSurface *surface);
extern EGLBoolean GS_EGL10_invalidate_native_window(void);
extern void GS_EGL10_cleanup_current_window_surface(void);
#endif

void GS_EGL10_BP_SB_TP_008_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    EGLBoolean result;
    EGLint error;

    fixture_prepared = GS_EGL10_prepare_current_window_surface(
        &display, &surface);
    if (!fixture_prepared || !GS_EGL10_invalidate_native_window())
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Invalid native-window setup failed");
        test_success = EGL_FALSE;
        return;
    }

    // Test starts here: swap the surface whose native window is invalid.
    (void)eglGetError();
    result = eglSwapBuffers(display, surface);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_NATIVE_WINDOW)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_NATIVE_WINDOW, got %u/0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    (void)test_success;
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: GS_EGL_PLATFORM_TEST_HOOKS is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_BP_SB_TP_008_draw(void) { }

void GS_EGL10_BP_SB_TP_008_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (fixture_prepared)
        GS_EGL10_cleanup_current_window_surface();
#endif
}

