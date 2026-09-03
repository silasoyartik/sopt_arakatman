#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - BufferPosting - eglSwapBuffers
 * Covered requirement: GS-EGL10-BP-SB-003
 *
 * A native pixmap fixture is required to establish and observe the no-posting
 * condition. The fixture owns its EGL initialization and termination.
 */
static const char* test_case = "GS_EGL10_BP_SB_TC_003";
static const char* test_procedure = "GS_EGL10_BP_SB_TP_003";
static EGLBoolean test_success = EGL_TRUE;

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean fixture_prepared;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
extern EGLBoolean GS_EGL10_prepare_current_pixmap_surface(
    EGLDisplay *display, EGLSurface *surface);
extern EGLBoolean GS_EGL10_verify_pixmap_unchanged(void);
extern void GS_EGL10_cleanup_current_pixmap_surface(void);
#endif

void GS_EGL10_BP_SB_TP_003_init(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    EGLBoolean result;
    EGLint error;

    fixture_prepared = GS_EGL10_prepare_current_pixmap_surface(
        &display, &surface);
    if (!fixture_prepared)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Native pixmap setup failed");
        test_success = EGL_FALSE;
        return;
    }

    // Test starts here: swapping a pixmap shall succeed without posting.
    (void)eglGetError();
    result = eglSwapBuffers(display, surface);
    error = eglGetError();

    if (result != EGL_TRUE || error != EGL_SUCCESS ||
        !GS_EGL10_verify_pixmap_unchanged())
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Pixmap swap result/error: %u/0x%x",
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

void GS_EGL10_BP_SB_TP_003_draw(void) { }

void GS_EGL10_BP_SB_TP_003_close(void)
{
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (fixture_prepared)
        GS_EGL10_cleanup_current_pixmap_surface();
#endif
}

