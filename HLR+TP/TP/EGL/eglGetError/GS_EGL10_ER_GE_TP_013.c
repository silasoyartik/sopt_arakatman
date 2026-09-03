#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - Errors - eglGetError
 * Covered requirement: GS-EGL10-ER-GE-013
 */
static const char* test_case = "GS_EGL10_ER_GE_TC_013";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_013";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_ER_GE_TP_013_init(void)
{
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_initialize_display(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Display setup failed, error: 0x%x", eglGetError());
        return;
    }

    /* eglChooseConfig requires a non-null num_config output parameter.
     * Supplying NULL creates a deterministic invalid-parameter failure whose
     * error is retrieved immediately by eglGetError.
     */
    (void)eglGetError();
    result = eglChooseConfig(environment.display, NULL, NULL, 0, NULL);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_PARAMETER)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_PARAMETER, got %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_ER_GE_TP_013_draw(void) { }

void GS_EGL10_ER_GE_TP_013_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
