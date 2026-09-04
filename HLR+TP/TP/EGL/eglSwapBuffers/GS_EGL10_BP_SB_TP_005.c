#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - BufferPosting - eglSwapBuffers
 * Covered requirement: GS-EGL10-BP-SB-005
 */
static const char* test_case = "GS_EGL10_BP_SB_TC_005";
static const char* test_procedure = "GS_EGL10_BP_SB_TP_005";
static EGLBoolean test_success = EGL_TRUE;

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean fixture_prepared;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
#endif

void GS_EGL10_BP_SB_TP_005_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    EGLint expected_width;
    EGLint expected_height;
    EGLint actual_width = -1;
    EGLint actual_height = -1;

    fixture_prepared = GS_EGL10_prepare_current_window_surface(
        &display, &surface);
    if (!fixture_prepared ||
        !GS_EGL10_resize_native_window(&expected_width, &expected_height))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Native resize setup failed");
        test_success = EGL_FALSE;
        return;
    }

    // Test starts here: swap shall synchronize surface and window dimensions.
    if (eglSwapBuffers(display, surface) != EGL_TRUE ||
        eglQuerySurface(display, surface, EGL_WIDTH, &actual_width) != EGL_TRUE ||
        eglQuerySurface(display, surface, EGL_HEIGHT, &actual_height) != EGL_TRUE ||
        actual_width != expected_width || actual_height != expected_height)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Surface size %dx%d, expected %dx%d",
            actual_width, actual_height, expected_width, expected_height);
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    (void)test_success;
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: GS_EGL_PLATFORM_TEST_HOOKS is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_BP_SB_TP_005_draw(void) { }

void GS_EGL10_BP_SB_TP_005_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (fixture_prepared)
        GS_EGL10_cleanup_current_window_surface();
#endif
}
