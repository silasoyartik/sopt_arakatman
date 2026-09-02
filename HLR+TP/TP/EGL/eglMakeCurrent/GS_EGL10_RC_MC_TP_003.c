#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "../../helpers.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-003
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_003";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_003";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;
static EGLContext second_context = EGL_NO_CONTEXT;

void GS_EGL10_RC_MC_TP_003_init(void)
{
    GLubyte pixel[4] = { 0, 0, 0, 0 };

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16) ||
        !GS_EGL10_make_environment_current(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    second_context = eglCreateContext(environment.display, environment.config,
        EGL_NO_CONTEXT, NULL);
    if (second_context == EGL_NO_CONTEXT)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create the replacement context");
        return;
    }

    /* Queue rendering in the old context without an explicit glFlush. */
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Test starts here: changing context shall flush the previous context.
    if (eglMakeCurrent(environment.display, environment.surface,
            environment.surface, second_context) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Context switch failed, EGL error: 0x%x", eglGetError());
        test_success = EGL_FALSE;
    }
    else
    {
        glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
        if (glGetError() != GL_NO_ERROR || pixel[0] == 0)
        {
            TEST_LOG_FAIL(test_case, test_procedure,
                "Rendering queued by the previous context was not visible");
            test_success = EGL_FALSE;
        }
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_RC_MC_TP_003_draw(void) { }
void GS_EGL10_RC_MC_TP_003_close(void)
{
    if (environment.display != EGL_NO_DISPLAY &&
        second_context != EGL_NO_CONTEXT)
    {
        (void)eglMakeCurrent(environment.display, EGL_NO_SURFACE,
            EGL_NO_SURFACE, EGL_NO_CONTEXT);
        (void)eglDestroyContext(environment.display, second_context);
        second_context = EGL_NO_CONTEXT;
    }
    GS_EGL10_cleanup_environment(&environment);
}

