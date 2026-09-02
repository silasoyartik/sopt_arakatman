#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - ConfigurationManagement - eglGetConfigAttrib
Covered requirement: GS-EGL10-CM-GCA-001
*/

static const char* test_case = "GS_EGL10_CM_GCA_TC_001";
static const char* test_procedure = "GS_EGL10_CM_GCA_TP_001";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_CM_GCA_TP_001_init(void)
{
    EGLBoolean (*get_config_attrib)(EGLDisplay, EGLConfig, EGLint, EGLint*) =
        eglGetConfigAttrib;
    EGLint value = -1;

    if (!GS_EGL10_initialize_display(&environment) ||
        !GS_EGL10_choose_config(&environment, EGL_PBUFFER_BIT))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    // Test starts here: call through the required EGL 1.0 function type.
    (void)eglGetError();
    if (get_config_attrib(environment.display, environment.config,
            EGL_CONFIG_ID, &value) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglGetConfigAttrib entry point failed, EGL error: 0x%x",
            eglGetError());
        test_success = EGL_FALSE;
    }

    if (test_success)
        TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_CM_GCA_TP_001_draw(void) { }

void GS_EGL10_CM_GCA_TP_001_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

