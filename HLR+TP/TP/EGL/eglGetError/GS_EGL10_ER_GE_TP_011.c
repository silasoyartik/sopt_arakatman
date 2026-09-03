#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - Errors - eglGetError
 * Covered requirement: GS-EGL10-ER-GE-011
 */
static const char* test_case = "GS_EGL10_ER_GE_TC_011";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_011";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_ER_GE_TP_011_init(void)
{
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_initialize_display(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Display setup failed, error: 0x%x", eglGetError());
        return;
    }

    /* EGL_NO_SURFACE does not name a valid rendering surface for destruction.
     * Reading the error immediately after eglDestroySurface associates
     * EGL_BAD_SURFACE with that failed operation.
     */
    (void)eglGetError();
    result = eglDestroySurface(environment.display, EGL_NO_SURFACE);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_SURFACE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_SURFACE, got %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_ER_GE_TP_011_draw(void) { }

void GS_EGL10_ER_GE_TP_011_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
