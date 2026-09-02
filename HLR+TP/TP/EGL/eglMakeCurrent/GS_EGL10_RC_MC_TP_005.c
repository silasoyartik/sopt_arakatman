#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-005
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_005";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_005";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_RC_MC_TP_005_init(void)
{
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    // Test starts here: use a surface while ctx is EGL_NO_CONTEXT.
    (void)eglGetError();
    result = eglMakeCurrent(environment.display, environment.surface,
        EGL_NO_SURFACE, EGL_NO_CONTEXT);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_MATCH)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_MATCH, got %u/0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_RC_MC_TP_005_draw(void) { }
void GS_EGL10_RC_MC_TP_005_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

