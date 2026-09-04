#include <EGL/egl.h>
#include "../../helpers.h"

/*
 * EGL10 - Initialization - eglInitialize
 *
 * Covered requirements:
 *   GS-EGL10-IN-INI-005
 *   GS-EGL10-IN-INI-006
 */
static const char *test_case1 = "GS_EGL10_IN_INI_TC_005";
static const char *test_case2 = "GS_EGL10_IN_INI_TC_006";
static const char *test_procedure = "GS_EGL10_IN_INI_TP_003";
static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;

void GS_EGL10_IN_INI_TP_003_init(void)
{
    EGLint major = -1;
    EGLint minor = -1;
    EGLBoolean result;
    EGLint error;

    (void)eglGetError();
    result = eglInitialize(EGL_NO_DISPLAY, &major, &minor);
    error = eglGetError();

    if (result != EGL_FALSE || major != -1 || minor != -1)
    {
        TEST_LOG_FAIL(test_case1, test_procedure,
            "Failure shall return EGL_FALSE without changing outputs; "
            "got %u/%d.%d",
            (unsigned int)result, major, minor);
        test_success1 = EGL_FALSE;
    }

    if (error != EGL_BAD_DISPLAY)
    {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "Expected EGL_BAD_DISPLAY, got 0x%x", error);
        test_success2 = EGL_FALSE;
    }

    if (test_success1)
        TEST_LOG_SUCCESS(test_case1, test_procedure);
    if (test_success2)
        TEST_LOG_SUCCESS(test_case2, test_procedure);
}

void GS_EGL10_IN_INI_TP_003_draw(void) { }
void GS_EGL10_IN_INI_TP_003_close(void) { }
