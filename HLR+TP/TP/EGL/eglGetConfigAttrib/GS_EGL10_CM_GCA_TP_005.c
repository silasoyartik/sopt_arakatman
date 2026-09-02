#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - ConfigurationManagement - eglGetConfigAttrib
Covered requirement: GS-EGL10-CM-GCA-005
*/

static const char* test_case = "GS_EGL10_CM_GCA_TC_005";
static const char* test_procedure = "GS_EGL10_CM_GCA_TP_005";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_CM_GCA_TP_005_init(void)
{
    EGLConfig configs[64];
    EGLint count = 0;
    EGLint returned;
    EGLint index;

    if (!GS_EGL10_initialize_display(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    if (eglGetConfigs(environment.display, configs, 64, &count) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not enumerate EGLConfigs");
        return;
    }
    returned = count < 64 ? count : 64;

    // Test starts here: find a config without window support and query its visual ID.
    for (index = 0; index < returned; ++index)
    {
        EGLint surface_type;
        EGLint visual_id = -1;

        if (eglGetConfigAttrib(environment.display, configs[index],
                EGL_SURFACE_TYPE, &surface_type) == EGL_TRUE &&
            (surface_type & EGL_WINDOW_BIT) == 0)
        {
            if (eglGetConfigAttrib(environment.display, configs[index],
                    EGL_NATIVE_VISUAL_ID, &visual_id) != EGL_TRUE ||
                visual_id != 0)
            {
                TEST_LOG_FAIL(test_case, test_procedure,
                    "Expected EGL_NATIVE_VISUAL_ID 0, got: %d", visual_id);
                test_success = EGL_FALSE;
            }
            if (test_success)
                TEST_LOG_SUCCESS(test_case, test_procedure);
            return;
        }
    }

    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: no config without window support.",
        test_case, test_procedure);
}

void GS_EGL10_CM_GCA_TP_005_draw(void) { }

void GS_EGL10_CM_GCA_TP_005_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

