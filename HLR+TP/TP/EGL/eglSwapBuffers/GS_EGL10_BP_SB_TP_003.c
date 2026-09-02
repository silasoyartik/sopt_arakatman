#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - BufferPosting - eglSwapBuffers
 * Covered requirement: GS-EGL10-BP-SB-003
 */
static const char* test_case = "GS_EGL10_BP_SB_TC_003";
static const char* test_procedure = "GS_EGL10_BP_SB_TP_003";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_BP_SB_TP_003_init(void)
{
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16) ||
        !GS_EGL10_make_environment_current(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    // Test starts here: swap a valid current pbuffer surface.
    (void)eglGetError();
    result = eglSwapBuffers(environment.display, environment.surface);
    error = eglGetError();

    if (result != EGL_TRUE || error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_TRUE/EGL_SUCCESS, got %u/0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_BP_SB_TP_003_draw(void) { }
void GS_EGL10_BP_SB_TP_003_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

