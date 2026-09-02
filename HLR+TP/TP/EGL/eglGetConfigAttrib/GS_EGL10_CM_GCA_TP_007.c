#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - ConfigurationManagement - eglGetConfigAttrib
Covered requirement: GS-EGL10-CM-GCA-007
*/

static const char* test_case = "GS_EGL10_CM_GCA_TC_007";
static const char* test_procedure = "GS_EGL10_CM_GCA_TP_007";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_CM_GCA_TP_007_init(void)
{
    EGLint value = -1;
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_initialize_display(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    // Test starts here: query a handle that cannot name an EGLConfig.
    (void)eglGetError();
    result = eglGetConfigAttrib(environment.display, (EGLConfig)0,
        EGL_CONFIG_ID, &value);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_CONFIG)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_CONFIG, got %u/0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success)
        TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_CM_GCA_TP_007_draw(void) { }

void GS_EGL10_CM_GCA_TP_007_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

