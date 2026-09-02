#include <EGL/egl.h>
#include "../../helper.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-002
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_002";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_002";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_RC_MC_TP_002_init(void)
{
    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    // Test starts here: bind and query the context, draw surface and read surface.
    if (eglMakeCurrent(environment.display, environment.surface,
            environment.surface, environment.context) != EGL_TRUE ||
        eglGetCurrentContext() != environment.context ||
        eglGetCurrentSurface(EGL_DRAW) != environment.surface ||
        eglGetCurrentSurface(EGL_READ) != environment.surface)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "The requested current bindings were not established");
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_RC_MC_TP_002_draw(void) { }
void GS_EGL10_RC_MC_TP_002_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

