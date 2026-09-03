#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - BufferPosting - eglSwapBuffers
 * Covered requirement: GS-EGL10-BP-SB-008
 */
static const char* test_case = "GS_EGL10_BP_SB_TC_008";
static const char* test_procedure = "GS_EGL10_BP_SB_TP_008";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_BP_SB_TP_008_init(void)
{
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    // Test starts here: the valid surface is deliberately not current.
    (void)eglGetError();
    result = eglSwapBuffers(environment.display, environment.surface);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_SURFACE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_SURFACE, got %u/0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_BP_SB_TP_008_draw(void) { }
void GS_EGL10_BP_SB_TP_008_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
