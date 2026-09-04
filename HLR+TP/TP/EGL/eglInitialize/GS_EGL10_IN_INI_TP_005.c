#include <EGL/egl.h>
#include "../../helpers.h"

/*
 * EGL10 - Initialization - eglInitialize
 *
 * Covered requirement: GS-EGL10-IN-INI-008
 */
static const char *test_case = "GS_EGL10_IN_INI_TC_008";
static const char *test_procedure = "GS_EGL10_IN_INI_TP_005";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_IN_INI_TP_005_init(void)
{
    EGLint before_count = -1;
    EGLint after_count = -1;
    EGLint major = -1;
    EGLint minor = -1;
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_initialize_display(&environment) ||
        eglGetConfigs(environment.display, NULL, 0, &before_count) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not establish the initialized-display precondition");
        test_success = EGL_FALSE;
        return;
    }

    (void)eglGetError();
    result = eglInitialize(environment.display, &major, &minor);
    error = eglGetError();

    if (result != EGL_TRUE || major < 0 || minor < 0)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Repeated initialization did not return version outputs: "
            "%u/%d.%d, EGL error: 0x%x",
            (unsigned int)result, major, minor, error);
        test_success = EGL_FALSE;
    }

    if (eglGetConfigs(environment.display, NULL, 0, &after_count) != EGL_TRUE ||
        after_count != before_count)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Repeated initialization changed the observable display state");
        test_success = EGL_FALSE;
    }

    if (test_success)
        TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_IN_INI_TP_005_draw(void) { }

void GS_EGL10_IN_INI_TP_005_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
