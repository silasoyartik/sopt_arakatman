#include <EGL/egl.h>
#include "../../helper.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-019
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_019";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_019";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;
static EGLContext second_context = EGL_NO_CONTEXT;

void GS_EGL10_RC_MC_TP_019_init(void)
{
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
            "Could not create the second context");
        return;
    }

    // Test starts here: replace the thread's current context.
    if (eglMakeCurrent(environment.display, environment.surface,
            environment.surface, second_context) != EGL_TRUE ||
        eglGetCurrentContext() != second_context)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "The thread did not retain exactly the new current context");
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_RC_MC_TP_019_draw(void) { }
void GS_EGL10_RC_MC_TP_019_close(void)
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

