#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "../../helper.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-017
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_017";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_017";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_RC_MC_TP_017_init(void)
{
    GLint viewport[4];
    GLint scissor[4];

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    // Test starts here: make the context current for the first time.
    if (!GS_EGL10_make_environment_current(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglMakeCurrent failed, EGL error: 0x%x", eglGetError());
        return;
    }

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetIntegerv(GL_SCISSOR_BOX, scissor);
    if (glGetError() != GL_NO_ERROR ||
        viewport[0] != 0 || viewport[1] != 0 ||
        viewport[2] != 16 || viewport[3] != 16 ||
        scissor[0] != 0 || scissor[1] != 0 ||
        scissor[2] != 16 || scissor[3] != 16)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Initial viewport or scissor does not match the draw surface");
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_RC_MC_TP_017_draw(void) { }
void GS_EGL10_RC_MC_TP_017_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

