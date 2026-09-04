#include <EGL/egl.h>
#include "../../helpers.h"

/*
 * EGL10 - RenderingContexts - eglGetCurrentDisplay
 *
 * Covered requirements:
 *   GS-EGL10-RC-GCD-002
 *   GS-EGL10-RC-GCD-003
 */
static const char *test_case1 = "GS_EGL10_RC_GCD_TC_002";
static const char *test_case2 = "GS_EGL10_RC_GCD_TC_003";
static const char *test_procedure = "GS_EGL10_RC_GCD_TP_002";
static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_RC_GCD_TP_002_init(void)
{
    EGLDisplay current_display;
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 1, 1) ||
        !GS_EGL10_make_environment_current(&environment))
    {
        TEST_LOG_FAIL(test_case1, test_procedure,
            "Could not prepare a current pbuffer context, EGL error: 0x%x",
            eglGetError());
        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
        return;
    }

    (void)eglGetError();
    current_display = eglGetCurrentDisplay();
    error = eglGetError();

    if (current_display != environment.display || error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(test_case1, test_procedure,
            "Expected current display %p, got %p/0x%x",
            (void *)environment.display, (void *)current_display, error);
        test_success1 = EGL_FALSE;
    }

    result = eglMakeCurrent(environment.display, EGL_NO_SURFACE,
        EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "Could not release the current context, EGL error: 0x%x",
            eglGetError());
        test_success2 = EGL_FALSE;
        return;
    }

    (void)eglGetError();
    current_display = eglGetCurrentDisplay();
    error = eglGetError();

    if (current_display != EGL_NO_DISPLAY || error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "Expected EGL_NO_DISPLAY after release, got %p/0x%x",
            (void *)current_display, error);
        test_success2 = EGL_FALSE;
    }

    if (test_success1)
        TEST_LOG_SUCCESS(test_case1, test_procedure);
    if (test_success2)
        TEST_LOG_SUCCESS(test_case2, test_procedure);
}

void GS_EGL10_RC_GCD_TP_002_draw(void) { }

void GS_EGL10_RC_GCD_TP_002_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
