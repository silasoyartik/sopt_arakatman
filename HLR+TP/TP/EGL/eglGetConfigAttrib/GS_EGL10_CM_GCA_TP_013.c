#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - ConfigurationManagement - eglGetConfigAttrib
 * Covered requirement: GS-EGL10-CM-GCA-013
 */
static const char* test_case = "GS_EGL10_CM_GCA_TC_013";
static const char* test_procedure = "GS_EGL10_CM_GCA_TP_013";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_CM_GCA_TP_013_init(void)
{
    EGLint value = (EGLint)0x5a5a5a5a;
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_initialize_display(&environment) ||
        !GS_EGL10_choose_config(&environment, EGL_PBUFFER_BIT))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    // Test starts here: query EGL_MAX_PBUFFER_PIXELS.
    (void)eglGetError();
    result = eglGetConfigAttrib(environment.display, environment.config,
        EGL_MAX_PBUFFER_PIXELS, &value);
    error = eglGetError();

    if (result != EGL_TRUE || error != EGL_SUCCESS || (value < 0))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "EGL_MAX_PBUFFER_PIXELS query returned %u/0x%x with value %d",
            (unsigned int)result, error, value);
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_CM_GCA_TP_013_draw(void) { }

void GS_EGL10_CM_GCA_TP_013_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
