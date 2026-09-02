#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-001
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_001";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_001";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_RC_MC_TP_001_init(void)
{
    EGLBoolean (*make_current)(EGLDisplay, EGLSurface, EGLSurface, EGLContext) =
        eglMakeCurrent;

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    // Test starts here: call through the required EGL 1.0 function type.
    if (make_current(environment.display, environment.surface,
            environment.surface, environment.context) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglMakeCurrent entry point failed, EGL error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_RC_MC_TP_001_draw(void) { }
void GS_EGL10_RC_MC_TP_001_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

