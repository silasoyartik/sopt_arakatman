#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-004
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_004";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_004";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_RC_MC_TP_004_init(void)
{
    EGLBoolean result;

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16) ||
        !GS_EGL10_make_environment_current(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    // Test starts here: release all current bindings.
    result = eglMakeCurrent(environment.display, EGL_NO_SURFACE,
        EGL_NO_SURFACE, EGL_NO_CONTEXT);

    if (result != EGL_TRUE ||
        eglGetCurrentContext() != EGL_NO_CONTEXT ||
        eglGetCurrentSurface(EGL_DRAW) != EGL_NO_SURFACE ||
        eglGetCurrentSurface(EGL_READ) != EGL_NO_SURFACE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Current bindings were not released");
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_RC_MC_TP_004_draw(void) { }
void GS_EGL10_RC_MC_TP_004_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

