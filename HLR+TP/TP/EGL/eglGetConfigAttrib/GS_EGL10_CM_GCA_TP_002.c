#include <EGL/egl.h>
#include "../../helper.h"

/*
EGL10 - ConfigurationManagement - eglGetConfigAttrib
Covered requirement: GS-EGL10-CM-GCA-002
*/

static const char* test_case = "GS_EGL10_CM_GCA_TC_002";
static const char* test_procedure = "GS_EGL10_CM_GCA_TP_002";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_CM_GCA_TP_002_init(void)
{
    EGLint value = -1;
    EGLBoolean result;

    if (!GS_EGL10_initialize_display(&environment) ||
        !GS_EGL10_choose_config(&environment, EGL_PBUFFER_BIT))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    // Test starts here: query one valid attribute and its output.
    (void)eglGetError();
    result = eglGetConfigAttrib(environment.display, environment.config,
        EGL_CONFIG_ID, &value);

    if (result != EGL_TRUE || value < 0 || eglGetError() != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Valid query failed or did not write value");
        test_success = EGL_FALSE;
    }

    if (test_success)
        TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_CM_GCA_TP_002_draw(void) { }

void GS_EGL10_CM_GCA_TP_002_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

