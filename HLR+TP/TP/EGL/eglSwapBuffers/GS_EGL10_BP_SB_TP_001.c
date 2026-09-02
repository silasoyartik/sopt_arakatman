#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - BufferPosting - eglSwapBuffers
 * Covered requirement: GS-EGL10-BP-SB-001
 */
static const char* test_case = "GS_EGL10_BP_SB_TC_001";
static const char* test_procedure = "GS_EGL10_BP_SB_TP_001";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_BP_SB_TP_001_init(void)
{
    EGLBoolean (*swap_buffers)(EGLDisplay, EGLSurface) = eglSwapBuffers;

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16) ||
        !GS_EGL10_make_environment_current(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    // Test starts here: call through the required EGL 1.0 function type.
    if (swap_buffers(environment.display, environment.surface) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglSwapBuffers entry point failed, EGL error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_BP_SB_TP_001_draw(void) { }
void GS_EGL10_BP_SB_TP_001_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

