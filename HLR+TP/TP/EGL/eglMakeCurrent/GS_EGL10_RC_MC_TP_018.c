#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "../../helpers.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-018
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_018";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_018";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_RC_MC_TP_018_init(void)
{
    GLint viewport[4];
    GLint scissor[4];

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16) ||
        !GS_EGL10_make_environment_current(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    glViewport(1, 2, 3, 4);
    glScissor(4, 3, 2, 1);
    (void)eglMakeCurrent(environment.display, EGL_NO_SURFACE,
        EGL_NO_SURFACE, EGL_NO_CONTEXT);

    // Test starts here: make the same context current again.
    if (!GS_EGL10_make_environment_current(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Second eglMakeCurrent failed, EGL error: 0x%x", eglGetError());
        return;
    }

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetIntegerv(GL_SCISSOR_BOX, scissor);
    if (glGetError() != GL_NO_ERROR ||
        viewport[0] != 1 || viewport[1] != 2 ||
        viewport[2] != 3 || viewport[3] != 4 ||
        scissor[0] != 4 || scissor[1] != 3 ||
        scissor[2] != 2 || scissor[3] != 1)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Viewport or scissor changed on a later current binding");
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_RC_MC_TP_018_draw(void) { }
void GS_EGL10_RC_MC_TP_018_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

