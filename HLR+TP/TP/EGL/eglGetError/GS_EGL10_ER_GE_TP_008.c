#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - Errors - eglGetError
 * Covered requirement: GS-EGL10-ER-GE-008
 */
static const char* test_case = "GS_EGL10_ER_GE_TC_008";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_008";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_ER_GE_TP_008_init(void)
{
    EGLint value = 0;
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_initialize_display(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Display setup failed, error: 0x%x", eglGetError());
        return;
    }

    /* A null EGLConfig handle cannot identify a configuration belonging to
     * the initialized display. The following eglGetError call must therefore
     * retrieve EGL_BAD_CONFIG.
     */
    (void)eglGetError();
    result = eglGetConfigAttrib(environment.display, (EGLConfig)0,
        EGL_CONFIG_ID, &value);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_CONFIG)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_CONFIG, got %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_ER_GE_TP_008_draw(void) { }

void GS_EGL10_ER_GE_TP_008_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
