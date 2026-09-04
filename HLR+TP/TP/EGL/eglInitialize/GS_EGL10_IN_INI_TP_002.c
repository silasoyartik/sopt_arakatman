#include <EGL/egl.h>
#include "../../helpers.h"

/*
 * EGL10 - Initialization - eglInitialize
 *
 * Covered requirements:
 *   GS-EGL10-IN-INI-003
 *   GS-EGL10-IN-INI-004
 */
static const char *test_case1 = "GS_EGL10_IN_INI_TC_003";
static const char *test_case2 = "GS_EGL10_IN_INI_TC_004";
static const char *test_procedure = "GS_EGL10_IN_INI_TP_002";
static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

static EGLBoolean terminate_between_cases(void)
{
    if (environment.initialized == EGL_TRUE)
    {
        if (eglTerminate(environment.display) != EGL_TRUE)
        {
            return EGL_FALSE;
        }
        environment.initialized = EGL_FALSE;
    }
    return EGL_TRUE;
}

void GS_EGL10_IN_INI_TP_002_init(void)
{
    EGLBoolean result;
    EGLint major;
    EGLint minor;

    if (!GS_EGL10_get_default_display(&environment))
    {
        TEST_LOG_FAIL(test_case1, test_procedure,
            "Could not obtain the display required by the test");
        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
        return;
    }

    /* INI-003: both output pointers may be NULL. */
    result = eglInitialize(environment.display, NULL, NULL);
    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case1, test_procedure,
            "eglInitialize(display, NULL, NULL) failed, EGL error: 0x%x",
            eglGetError());
        test_success1 = EGL_FALSE;
    }
    else
    {
        environment.initialized = EGL_TRUE;
    }

    if (!terminate_between_cases())
    {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "Could not restore the uninitialized display precondition");
        test_success2 = EGL_FALSE;
        return;
    }

    /* INI-004, case A: major is NULL and minor is non-NULL. */
    minor = -1;
    result = eglInitialize(environment.display, NULL, &minor);
    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "eglInitialize failed when major was NULL, EGL error: 0x%x",
            eglGetError());
        test_success2 = EGL_FALSE;
    }
    if (result == EGL_TRUE)
        environment.initialized = EGL_TRUE;

    if (!terminate_between_cases())
    {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "Could not restore the display before the second pointer case");
        test_success2 = EGL_FALSE;
        return;
    }

    /* INI-004, case B: minor is NULL and major is non-NULL. */
    major = -1;
    result = eglInitialize(environment.display, &major, NULL);
    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "eglInitialize failed when minor was NULL, EGL error: 0x%x",
            eglGetError());
        test_success2 = EGL_FALSE;
    }
    if (result == EGL_TRUE)
        environment.initialized = EGL_TRUE;

    if (test_success1)
        TEST_LOG_SUCCESS(test_case1, test_procedure);
    if (test_success2)
        TEST_LOG_SUCCESS(test_case2, test_procedure);
}

void GS_EGL10_IN_INI_TP_002_draw(void) { }

void GS_EGL10_IN_INI_TP_002_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
