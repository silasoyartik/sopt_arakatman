#include <EGL/egl.h>
#include "../../helpers.h"
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglGetCurrentContext

Verify that eglGetCurrentContext returns the known EGLContext after it is
successfully bound to the calling thread.

Covered requirements:
    - GS-EGL10-RC-GCC-002
*/

static const char* test_case = "GS_EGL10_RC_GCC_TC_002";
static const char* test_procedure = "GS_EGL10_RC_GCC_TP_002";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;
static EGLContext test_context = EGL_NO_CONTEXT;
static EGLBoolean test_context_is_current = EGL_FALSE;

/* Creates a second context compatible with the helper-owned fixture. */
static EGLBoolean create_test_context(void)
{
    test_context = eglCreateContext(environment.display, environment.config,
        EGL_NO_CONTEXT, NULL);
    return test_context != EGL_NO_CONTEXT;
}

/* Binds a known context successfully, then verifies that it is returned. */
void GS_EGL10_RC_GCC_TP_002_init(void)
{
    EGLContext current_context;

    if (GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not prepare the EGL pbuffer fixture, error: 0x%x",
            eglGetError());
        return;
    }

    if (GS_EGL10_make_environment_current(&environment) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not make the helper fixture context current, error: 0x%x",
            eglGetError());
        return;
    }

    if (create_test_context() != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create a compatible EGL test context, error: 0x%x",
            eglGetError());
        return;
    }

    /* Precondition for GCC-002: bind the independently created context. */
    if (eglMakeCurrent(environment.display, environment.surface,
            environment.surface, test_context) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not bind the test EGL context, error: 0x%x", eglGetError());
        return;
    }
    test_context_is_current = EGL_TRUE;

    current_context = eglGetCurrentContext();
    if (current_context != test_context)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected the independently created EGLContext handle to be returned");
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this current-context test. */
void GS_EGL10_RC_GCC_TP_002_draw(void)
{
}

/* Restores the helper fixture binding and releases all test-owned objects. */
void GS_EGL10_RC_GCC_TP_002_close(void)
{
    if ((test_context_is_current == EGL_TRUE) &&
        (environment.display != EGL_NO_DISPLAY))
    {
        (void)eglMakeCurrent(environment.display, environment.surface,
            environment.surface, environment.context);
    }

    if ((environment.display != EGL_NO_DISPLAY) &&
        (test_context != EGL_NO_CONTEXT))
    {
        (void)eglDestroyContext(environment.display, test_context);
    }

    test_context = EGL_NO_CONTEXT;
    test_context_is_current = EGL_FALSE;
    GS_EGL10_cleanup_environment(&environment);
}
