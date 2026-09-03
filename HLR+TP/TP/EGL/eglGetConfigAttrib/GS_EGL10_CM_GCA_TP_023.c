#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - ConfigurationManagement - eglGetConfigAttrib
 * Covered requirement: GS-EGL10-CM-GCA-023
 */
static const char* test_case = "GS_EGL10_CM_GCA_TC_023";
static const char* test_procedure = "GS_EGL10_CM_GCA_TP_023";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_CM_GCA_TP_023_init(void)
{
    EGLint value = (EGLint)0x5a5a5a5a;
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_initialize_display(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Display setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    {
        const EGLint attributes[] = {
            EGL_TRANSPARENT_TYPE, EGL_TRANSPARENT_RGB,
            EGL_NONE
        };
        EGLint count = 0;

        if (eglChooseConfig(environment.display, attributes,
                &environment.config, 1, &count) != EGL_TRUE ||
            count < 1)
        {
            TEST_LOG_INFO("[ %s ][ %s ] Not applicable: no EGL_TRANSPARENT_RGB config.",
                test_case, test_procedure);
            return;
        }
    }

    // Test starts here: query EGL_TRANSPARENT_GREEN_VALUE.
    (void)eglGetError();
    result = eglGetConfigAttrib(environment.display, environment.config,
        EGL_TRANSPARENT_GREEN_VALUE, &value);
    error = eglGetError();

    if (result != EGL_TRUE || error != EGL_SUCCESS || (value < 0))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "EGL_TRANSPARENT_GREEN_VALUE query returned %u/0x%x with value %d",
            (unsigned int)result, error, value);
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_CM_GCA_TP_023_draw(void) { }

void GS_EGL10_CM_GCA_TP_023_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
