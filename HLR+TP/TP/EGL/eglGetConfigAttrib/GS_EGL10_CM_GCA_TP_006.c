#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - ConfigurationManagement - eglGetConfigAttrib
Covered requirement: GS-EGL10-CM-GCA-006
*/

static const char* test_case = "GS_EGL10_CM_GCA_TC_006";
static const char* test_procedure = "GS_EGL10_CM_GCA_TP_006";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_CM_GCA_TP_006_init(void)
{
    EGLint value = -1;
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_initialize_display(&environment) ||
        !GS_EGL10_choose_config(&environment, EGL_PBUFFER_BIT))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    // Test starts here: pass an undefined attribute token.
    (void)eglGetError();
    result = eglGetConfigAttrib(environment.display, environment.config,
        (EGLint)0x7fffffff, &value);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_ATTRIBUTE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_ATTRIBUTE, got %u/0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success)
        TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_CM_GCA_TP_006_draw(void) { }

void GS_EGL10_CM_GCA_TP_006_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

