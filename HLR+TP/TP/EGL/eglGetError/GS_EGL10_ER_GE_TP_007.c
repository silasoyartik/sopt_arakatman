#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - Errors - eglGetError
 * Covered requirement: GS-EGL10-ER-GE-007
 */
static const char* test_case = "GS_EGL10_ER_GE_TC_007";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_007";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_ER_GE_TP_007_init(void)
{
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_initialize_display(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Display setup failed, error: 0x%x", eglGetError());
        return;
    }

    /* EGL_NO_CONTEXT does not name a valid context for destruction.
     * Read the error directly after the failed operation to verify that
     * eglGetError returns EGL_BAD_CONTEXT.
     */
    (void)eglGetError();
    result = eglDestroyContext(environment.display, EGL_NO_CONTEXT);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_CONTEXT)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_CONTEXT, got %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_ER_GE_TP_007_draw(void) { }

void GS_EGL10_ER_GE_TP_007_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
