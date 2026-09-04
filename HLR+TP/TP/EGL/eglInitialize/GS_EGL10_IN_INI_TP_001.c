#include <EGL/egl.h>
#include "../../helpers.h"

/*
 * EGL10 - Initialization - eglInitialize
 *
 * Covered requirements:
 *   GS-EGL10-IN-INI-001
 *   GS-EGL10-IN-INI-002
 */
static const char *test_case1 = "GS_EGL10_IN_INI_TC_001";
static const char *test_case2 = "GS_EGL10_IN_INI_TC_002";
static const char *test_procedure = "GS_EGL10_IN_INI_TP_001";
static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_IN_INI_TP_001_init(void)
{
    EGLBoolean (*initialize)(EGLDisplay, EGLint *, EGLint *) = eglInitialize;
    EGLBoolean result;
    EGLint error;
    EGLint major = -1;
    EGLint minor = -1;

    if (!GS_EGL10_get_default_display(&environment))
    {
        TEST_LOG_FAIL(test_case1, test_procedure,
            "Could not obtain the display required by the test");
        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
        return;
    }

    (void)eglGetError();
    result = initialize(environment.display, &major, &minor);
    error = eglGetError();

    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case1, test_procedure,
            "eglInitialize entry point returned EGL_FALSE, EGL error: 0x%x",
            error);
        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
    }
    else
    {
        environment.initialized = EGL_TRUE;
    }

    if (result != EGL_TRUE || major < 0 || minor < 0)
    {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "Expected EGL_TRUE and both version outputs, got %u/%d.%d",
            (unsigned int)result, major, minor);
        test_success2 = EGL_FALSE;
    }

    if (test_success1)
        TEST_LOG_SUCCESS(test_case1, test_procedure);
    if (test_success2)
        TEST_LOG_SUCCESS(test_case2, test_procedure);
}

void GS_EGL10_IN_INI_TP_001_draw(void) { }

void GS_EGL10_IN_INI_TP_001_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
