#include <EGL/egl.h>
#include "../../helper.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-010
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_010";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_010";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_RC_MC_TP_010_init(void)
{
    EGLContext invalid_context;
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    invalid_context = environment.context;
    if (eglDestroyContext(environment.display, invalid_context) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create an invalid context handle");
        return;
    }
    environment.context = EGL_NO_CONTEXT;

    // Test starts here: pass a destroyed context handle.
    (void)eglGetError();
    result = eglMakeCurrent(environment.display, environment.surface,
        environment.surface, invalid_context);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_CONTEXT)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_CONTEXT, got %u/0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_RC_MC_TP_010_draw(void) { }
void GS_EGL10_RC_MC_TP_010_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

