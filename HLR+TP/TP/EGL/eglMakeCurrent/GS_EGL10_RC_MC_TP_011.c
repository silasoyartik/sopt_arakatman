#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-011
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_011";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_011";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_RC_MC_TP_011_init(void)
{
    EGLSurface invalid_surface;
    EGLBoolean result;
    EGLint error;
    const EGLint attributes[] = { EGL_WIDTH, 8, EGL_HEIGHT, 8, EGL_NONE };

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    invalid_surface = eglCreatePbufferSurface(environment.display,
        environment.config, attributes);
    if (invalid_surface == EGL_NO_SURFACE ||
        eglDestroySurface(environment.display, invalid_surface) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create an invalid surface handle");
        return;
    }

    // Test starts here: pass a destroyed draw surface.
    (void)eglGetError();
    result = eglMakeCurrent(environment.display, invalid_surface,
        environment.surface, environment.context);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_SURFACE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_SURFACE, got %u/0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_RC_MC_TP_011_draw(void) { }
void GS_EGL10_RC_MC_TP_011_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

