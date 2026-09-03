#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - ConfigurationManagement - eglGetConfigAttrib
 * Covered requirement: GS-EGL10-CM-GCA-016
 */
static const char* test_case = "GS_EGL10_CM_GCA_TC_016";
static const char* test_procedure = "GS_EGL10_CM_GCA_TP_016";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_CM_GCA_TP_016_init(void)
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

    // Test starts here: query EGL_NATIVE_VISUAL_TYPE.
    (void)eglGetError();
    result = eglGetConfigAttrib(environment.display, environment.config,
        EGL_NATIVE_VISUAL_TYPE, &value);
    error = eglGetError();

    if (result != EGL_TRUE || error != EGL_SUCCESS || (0))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "EGL_NATIVE_VISUAL_TYPE query returned %u/0x%x with value %d",
            (unsigned int)result, error, value);
        test_success = EGL_FALSE;
    }

    {
        EGLint surface_type = 0;
        if (eglGetConfigAttrib(environment.display, environment.config,
                EGL_SURFACE_TYPE, &surface_type) != EGL_TRUE ||
            ((surface_type & EGL_WINDOW_BIT) == 0 && value != EGL_NONE))
        {
            TEST_LOG_FAIL(test_case, test_procedure,
                "A config without window support returned visual type 0x%x",
                value);
            test_success = EGL_FALSE;
        }
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_CM_GCA_TP_016_draw(void) { }

void GS_EGL10_CM_GCA_TP_016_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
