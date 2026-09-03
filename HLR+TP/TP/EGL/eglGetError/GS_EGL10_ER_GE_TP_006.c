#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - Errors - eglGetError
 * Covered requirement: GS-EGL10-ER-GE-006
 */
static const char* test_case = "GS_EGL10_ER_GE_TC_006";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_006";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_ER_GE_TP_006_init(void)
{
    const EGLint attributes[] = { (EGLint)0x7fffffff, 0, EGL_NONE };
    EGLint config_count = 0;
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_initialize_display(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Display setup failed, error: 0x%x", eglGetError());
        return;
    }

    /* Pass an undefined attribute in an otherwise terminated attribute list.
     * The failure is read immediately with eglGetError and must be reported
     * as EGL_BAD_ATTRIBUTE.
     */
    (void)eglGetError();
    result = eglChooseConfig(environment.display, attributes, NULL, 0,
        &config_count);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_ATTRIBUTE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_ATTRIBUTE, got %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_ER_GE_TP_006_draw(void) { }

void GS_EGL10_ER_GE_TP_006_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
