#include <EGL/egl.h>
#include "../../helpers.h"

/*
 * EGL10 - RenderingContexts - eglGetCurrentDisplay
 *
 * Covered requirements:
 *   GS-EGL10-RC-GCD-001
 *   GS-EGL10-RC-GCD-003
 */
static const char *test_case1 = "GS_EGL10_RC_GCD_TC_001";
static const char *test_case2 = "GS_EGL10_RC_GCD_TC_003";
static const char *test_procedure = "GS_EGL10_RC_GCD_TP_001";
static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

static void verify_no_current_display(const char *state)
{
    EGLDisplay (*get_current_display)(void) = eglGetCurrentDisplay;
    EGLDisplay current_display;
    EGLint error;

    (void)eglGetError();
    current_display = get_current_display();
    error = eglGetError();

    if (current_display != EGL_NO_DISPLAY)
    {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "Expected EGL_NO_DISPLAY %s, got %p",
            state, (void *)current_display);
        test_success2 = EGL_FALSE;
    }

    if (error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(test_case1, test_procedure,
            "eglGetCurrentDisplay generated 0x%x %s", error, state);
        test_success1 = EGL_FALSE;
    }
}

void GS_EGL10_RC_GCD_TP_001_init(void)
{
    verify_no_current_display("before display initialization");

    if (!GS_EGL10_initialize_display(&environment))
    {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "Could not initialize the display precondition");
        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
        return;
    }

    verify_no_current_display(
        "after display initialization without a current context");

    if (test_success1)
        TEST_LOG_SUCCESS(test_case1, test_procedure);
    if (test_success2)
        TEST_LOG_SUCCESS(test_case2, test_procedure);
}

void GS_EGL10_RC_GCD_TP_001_draw(void) { }

void GS_EGL10_RC_GCD_TP_001_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
