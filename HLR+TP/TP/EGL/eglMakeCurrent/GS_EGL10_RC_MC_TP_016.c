#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "../../helper.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-016
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_016";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_016";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_RC_MC_TP_016_init(void)
{
    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16) ||
        !GS_EGL10_make_environment_current(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    if (eglDestroySurface(environment.display, environment.surface) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not mark the current draw surface for deletion");
        return;
    }
    environment.surface = EGL_NO_SURFACE;

    // Test starts here: context-state commands shall still be processed.
    glDisable(GL_BLEND);
    glEnable(GL_BLEND);
    if (glGetError() != GL_NO_ERROR || glIsEnabled(GL_BLEND) != GL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Context state was not updated after destroying the draw surface");
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_RC_MC_TP_016_draw(void) { }
void GS_EGL10_RC_MC_TP_016_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

