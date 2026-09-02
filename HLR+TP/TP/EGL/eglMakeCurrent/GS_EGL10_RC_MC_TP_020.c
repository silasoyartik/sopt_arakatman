#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-020
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_020";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_020";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_RC_MC_TP_020_init(void)
{
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    (void)eglTerminate(environment.display);
    environment.initialized = EGL_FALSE;

    // Test starts here: call on a valid but uninitialized display.
    (void)eglGetError();
    result = eglMakeCurrent(environment.display, environment.surface,
        environment.surface, environment.context);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_NOT_INITIALIZED)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_NOT_INITIALIZED, got %u/0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_RC_MC_TP_020_draw(void) { }
void GS_EGL10_RC_MC_TP_020_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

