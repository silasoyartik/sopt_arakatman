#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - Initialization - eglTerminate

Verify that EGL defers release of a current context and surface until the
thread makes an otherwise valid eglMakeCurrent call.

Covered requirements:
    - GS-EGL10-IN-TER-006
*/

static const char* test_case = "GS_EGL10_IN_TER_TC_006";
static const char* test_procedure = "GS_EGL10_IN_TER_TP_006";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;
static EGLBoolean display_terminated = EGL_FALSE;

/* Releases a fixture whose resources were marked for deletion by eglTerminate. */
static void release_terminated_fixture(void)
{
    if (environment.display == EGL_NO_DISPLAY ||
        display_terminated != EGL_TRUE)
    {
        return;
    }

    if (eglInitialize(environment.display, NULL, NULL) == EGL_TRUE)
    {
        (void)eglMakeCurrent(environment.display, EGL_NO_SURFACE,
            EGL_NO_SURFACE, EGL_NO_CONTEXT);
        (void)eglTerminate(environment.display);
    }

    (void)eglGetError();
    environment.display = EGL_NO_DISPLAY;
    environment.config = (EGLConfig)0;
    environment.context = EGL_NO_CONTEXT;
    environment.surface = EGL_NO_SURFACE;
    environment.initialized = EGL_FALSE;
    display_terminated = EGL_FALSE;
}

/* Verifies deferred release while the context and surface remain current. */
void GS_EGL10_IN_TER_TP_006_init(void)
{
    EGLContext current_context;
    EGLSurface current_draw_surface;
    EGLSurface current_read_surface;

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
            "Could not make the test context current, error: 0x%x",
            eglGetError());
        return;
    }

    if (eglTerminate(environment.display) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglTerminate failed for the initialized test display, error: 0x%x",
            eglGetError());
        return;
    }
    environment.initialized = EGL_FALSE;
    display_terminated = EGL_TRUE;

    current_context = eglGetCurrentContext();
    if (current_context != environment.context)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "The current context was released before eglMakeCurrent detached it");
        return;
    }

    current_draw_surface = eglGetCurrentSurface(EGL_DRAW);
    current_read_surface = eglGetCurrentSurface(EGL_READ);
    if (current_draw_surface != environment.surface ||
        current_read_surface != environment.surface)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "A current surface was released before eglMakeCurrent detached it");
        return;
    }

    /* Reinitialize dpy so that the following detach call is otherwise valid. */
    if (eglInitialize(environment.display, NULL, NULL) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not reinitialize the terminated display, error: 0x%x",
            eglGetError());
        return;
    }
    environment.initialized = EGL_TRUE;

    if (eglMakeCurrent(environment.display, EGL_NO_SURFACE, EGL_NO_SURFACE,
            EGL_NO_CONTEXT) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not detach the current context after reinitialization, error: 0x%x",
            eglGetError());
        return;
    }

    if (eglGetCurrentContext() != EGL_NO_CONTEXT)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "The context remained current after eglMakeCurrent detached it");
        return;
    }

    if (eglGetCurrentSurface(EGL_DRAW) != EGL_NO_SURFACE ||
        eglGetCurrentSurface(EGL_READ) != EGL_NO_SURFACE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "A surface remained current after eglMakeCurrent detached it");
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this resource-lifetime test. */
void GS_EGL10_IN_TER_TP_006_draw(void)
{
}

/* Terminates the reinitialized display and clears the local fixture state. */
void GS_EGL10_IN_TER_TP_006_close(void)
{
    if (display_terminated == EGL_TRUE)
    {
        release_terminated_fixture();
    }
    else
    {
        GS_EGL10_cleanup_environment(&environment);
    }
}
