#include <EGL/egl.h>
#include <pthread.h>
#include "../../helpers.h"

/* EGL10 - Errors - eglGetError
 * Covered requirement: GS-EGL10-ER-GE-004
 */
static const char* test_case = "GS_EGL10_ER_GE_TC_004";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_004";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;
static EGLBoolean worker_result = EGL_FALSE;
static EGLint worker_error = EGL_SUCCESS;

static void* bind_current_context_from_worker(void* argument)
{
    (void)argument;

    /* The context remains current in the test thread. This second thread
     * attempts to bind the same resources and reads only its own EGL error
     * state, which shall contain EGL_BAD_ACCESS.
     */
    (void)eglGetError();
    worker_result = eglMakeCurrent(environment.display, environment.surface,
        environment.surface, environment.context);
    worker_error = eglGetError();
    return NULL;
}

void GS_EGL10_ER_GE_TP_004_init(void)
{
    pthread_t worker;
    int thread_result;

    worker_result = EGL_FALSE;
    worker_error = EGL_SUCCESS;

    /* Create all EGL resources locally and keep the context current in this
     * thread. The shared helper performs only the common fixture setup; the
     * cross-thread access below is the operation under test.
     */
    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 1, 1) ||
        !GS_EGL10_make_environment_current(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "EGL setup failed, error: 0x%x", eglGetError());
        return;
    }

    thread_result = pthread_create(&worker, NULL,
        bind_current_context_from_worker, NULL);
    if (thread_result != 0)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create worker thread, error: %d", thread_result);
        return;
    }

    thread_result = pthread_join(worker, NULL);
    if (thread_result != 0)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not join worker thread, error: %d", thread_result);
        return;
    }

    if (worker_result != EGL_FALSE || worker_error != EGL_BAD_ACCESS)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_ACCESS, got %u/0x%x",
            (unsigned int)worker_result, worker_error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_ER_GE_TP_004_draw(void) { }

void GS_EGL10_ER_GE_TP_004_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
