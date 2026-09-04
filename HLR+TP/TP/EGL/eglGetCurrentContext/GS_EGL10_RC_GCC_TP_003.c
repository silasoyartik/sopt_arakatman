#include <EGL/egl.h>
#include "../../helpers.h"
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglGetCurrentContext

Verify that eglGetCurrentContext returns EGL_NO_CONTEXT and generates no EGL
error when the calling thread has no current context.

Covered requirements:
    - GS-EGL10-RC-GCC-003
*/

static const char* test_case = "GS_EGL10_RC_GCC_TC_003";
static const char* test_procedure = "GS_EGL10_RC_GCC_TP_003";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;
static EGLBoolean context_detached = EGL_FALSE;

/* Detaches the context, then verifies the no-context and no-error results. */
void GS_EGL10_RC_GCC_TP_003_init(void)
{
    EGLContext current_context;
    EGLint error;

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

    /* Precondition for GCC-003: leave the thread without a current context. */
    (void)eglGetError();
    if (eglMakeCurrent(environment.display, EGL_NO_SURFACE, EGL_NO_SURFACE,
            EGL_NO_CONTEXT) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not detach the current EGLContext, error: 0x%x",
            eglGetError());
        return;
    }
    context_detached = EGL_TRUE;

    /* GCC-003: EGL_NO_CONTEXT is returned and this query is not an error. */
    (void)eglGetError();
    current_context = eglGetCurrentContext();
    error = eglGetError();

    if (current_context != EGL_NO_CONTEXT)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_NO_CONTEXT after detaching the current context");
        return;
    }

    if (error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglGetCurrentContext generated an unexpected error: 0x%x", error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this no-current-context test. */
void GS_EGL10_RC_GCC_TP_003_draw(void)
{
}

/* Restores the fixture binding before releasing the helper-owned resources. */
void GS_EGL10_RC_GCC_TP_003_close(void)
{
    if ((context_detached == EGL_TRUE) &&
        (environment.display != EGL_NO_DISPLAY))
    {
        (void)eglMakeCurrent(environment.display, environment.surface,
            environment.surface, environment.context);
    }

    context_detached = EGL_FALSE;
    GS_EGL10_cleanup_environment(&environment);
}
