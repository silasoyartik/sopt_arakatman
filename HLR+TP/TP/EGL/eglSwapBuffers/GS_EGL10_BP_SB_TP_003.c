#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - BufferPosting - eglSwapBuffers
 * Covered requirement: GS-EGL10-BP-SB-003
 *
 * The pbuffer and pixmap subcases use separate fixtures. The pbuffer fixture
 * is fully released before the native pixmap fixture is prepared.
 */
static const char* test_case = "GS_EGL10_BP_SB_TC_003";
static const char* test_procedure = "GS_EGL10_BP_SB_TP_003";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment pbuffer_environment =
    GS_EGL10_ENV_INITIALIZER;

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
static EGLBoolean pixmap_fixture_prepared;
static EGLDisplay pixmap_display = EGL_NO_DISPLAY;
static EGLSurface pixmap_surface = EGL_NO_SURFACE;
#endif

void GS_EGL10_BP_SB_TP_003_init(void)
{
    EGLBoolean result;
    EGLint error;

    /* Subcase 1: swapping a pbuffer shall succeed without posting. */
    if (!GS_EGL10_prepare_pbuffer_environment(
            &pbuffer_environment, 16, 16) ||
        !GS_EGL10_make_environment_current(&pbuffer_environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Pbuffer setup failed, EGL error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
    }
    else
    {
        (void)eglGetError();
        result = eglSwapBuffers(pbuffer_environment.display,
            pbuffer_environment.surface);
        error = eglGetError();

        if (result != EGL_TRUE || error != EGL_SUCCESS)
        {
            TEST_LOG_FAIL(test_case, test_procedure,
                "Pbuffer swap result/error: %u/0x%x",
                (unsigned int)result, error);
            test_success = EGL_FALSE;
        }
    }

    /* Release all pbuffer EGL state before preparing the pixmap fixture. */
    GS_EGL10_cleanup_environment(&pbuffer_environment);

#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    /* Subcase 2: swapping a pixmap shall succeed without posting. */
    pixmap_fixture_prepared = GS_EGL10_prepare_current_pixmap_surface(
        &pixmap_display, &pixmap_surface);
    if (!pixmap_fixture_prepared)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Native pixmap setup failed");
        test_success = EGL_FALSE;
    }
    else
    {
        (void)eglGetError();
        result = eglSwapBuffers(pixmap_display, pixmap_surface);
        error = eglGetError();

        if (result != EGL_TRUE || error != EGL_SUCCESS ||
            !GS_EGL10_verify_pixmap_unchanged())
        {
            TEST_LOG_FAIL(test_case, test_procedure,
                "Pixmap swap result/error: %u/0x%x",
                (unsigned int)result, error);
            test_success = EGL_FALSE;
        }
    }

    if (test_success)
        TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: pixmap subcase requires GS_EGL_PLATFORM_TEST_HOOKS.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_BP_SB_TP_003_draw(void) { }

void GS_EGL10_BP_SB_TP_003_close(void)
{
    GS_EGL10_cleanup_environment(&pbuffer_environment);
#ifdef GS_EGL_PLATFORM_TEST_HOOKS
    if (pixmap_fixture_prepared)
        GS_EGL10_cleanup_current_pixmap_surface();
#endif
}
