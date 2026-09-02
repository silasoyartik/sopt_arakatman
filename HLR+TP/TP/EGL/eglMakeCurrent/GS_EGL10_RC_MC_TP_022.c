#include <EGL/egl.h>
#include "../../helper.h"

#ifdef GS_EGL_USE_PTHREAD
#include <pthread.h>
#endif

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-022
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_022";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_022";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

#ifdef GS_EGL_USE_PTHREAD
static EGLSurface second_surface = EGL_NO_SURFACE;
static EGLContext second_context = EGL_NO_CONTEXT;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
static int worker_ready;
static int worker_release;
static EGLBoolean worker_bound;

static void* worker(void* argument)
{
    (void)argument;
    worker_bound = eglMakeCurrent(environment.display, environment.surface,
        environment.surface, environment.context);

    (void)pthread_mutex_lock(&mutex);
    worker_ready = 1;
    (void)pthread_cond_signal(&condition);
    while (!worker_release)
        (void)pthread_cond_wait(&condition, &mutex);
    (void)pthread_mutex_unlock(&mutex);

    if (worker_bound == EGL_TRUE)
    {
        (void)eglMakeCurrent(environment.display, EGL_NO_SURFACE,
            EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
    return NULL;
}
#endif

void GS_EGL10_RC_MC_TP_022_init(void)
{
#ifdef GS_EGL_USE_PTHREAD
    pthread_t thread;
    EGLBoolean result;
    EGLint error;
    const EGLint attributes[] = { EGL_WIDTH, 8, EGL_HEIGHT, 8, EGL_NONE };

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    second_surface = eglCreatePbufferSurface(environment.display,
        environment.config, attributes);
    second_context = eglCreateContext(environment.display, environment.config,
        EGL_NO_CONTEXT, NULL);
    if (second_surface == EGL_NO_SURFACE ||
        second_context == EGL_NO_CONTEXT ||
        pthread_create(&thread, NULL, worker, NULL) != 0)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Thread test setup failed");
        test_success = EGL_FALSE;
        return;
    }

    (void)pthread_mutex_lock(&mutex);
    while (!worker_ready)
        (void)pthread_cond_wait(&condition, &mutex);
    (void)pthread_mutex_unlock(&mutex);

    if (worker_bound != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Worker could not establish the test precondition");
        test_success = EGL_FALSE;
    }
    else
    {
        // Test starts here: attempt to bind a surface bound to another thread.
        (void)eglGetError();
        result = eglMakeCurrent(environment.display, environment.surface,
            environment.surface, second_context);
        error = eglGetError();

        if (result != EGL_FALSE || error != EGL_BAD_ACCESS ||
            eglGetCurrentContext() != EGL_NO_CONTEXT)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "A second context was bound to a surface already in use");
        test_success = EGL_FALSE;
    }
    }

    (void)pthread_mutex_lock(&mutex);
    worker_release = 1;
    (void)pthread_cond_signal(&condition);
    (void)pthread_mutex_unlock(&mutex);
    (void)pthread_join(thread, NULL);

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
#else
    (void)test_success;
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: GS_EGL_USE_PTHREAD is not enabled.",
        test_case, test_procedure);
#endif
}

void GS_EGL10_RC_MC_TP_022_draw(void) { }

void GS_EGL10_RC_MC_TP_022_close(void)
{
#ifdef GS_EGL_USE_PTHREAD
    if (environment.display != EGL_NO_DISPLAY)
    {
        if (second_surface != EGL_NO_SURFACE)
            (void)eglDestroySurface(environment.display, second_surface);
        if (second_context != EGL_NO_CONTEXT)
            (void)eglDestroyContext(environment.display, second_context);
    }
#endif
    GS_EGL10_cleanup_environment(&environment);
}
