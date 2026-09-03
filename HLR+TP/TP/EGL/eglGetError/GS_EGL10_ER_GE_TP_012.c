#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - Errors - eglGetError
 * Covered requirement: GS-EGL10-ER-GE-012
 */
static const char* test_case = "GS_EGL10_ER_GE_TC_012";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_012";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_ER_GE_TP_012_init(void)
{
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 1, 1))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "EGL setup failed, error: 0x%x", eglGetError());
        return;
    }

    /* Supply a valid context and read surface, but no draw surface. These
     * inconsistent arguments shall make eglMakeCurrent generate EGL_BAD_MATCH,
     * which is retrieved immediately in the same thread.
     */
    (void)eglGetError();
    result = eglMakeCurrent(environment.display, EGL_NO_SURFACE,
        environment.surface, environment.context);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_MATCH)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_MATCH, got %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_ER_GE_TP_012_draw(void) { }

void GS_EGL10_ER_GE_TP_012_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
