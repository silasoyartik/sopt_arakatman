#include <EGL/egl.h>
#include "../../helpers.h"

/* Define GS_EGL_USE_PTHREAD when POSIX threads are available. */
#ifdef GS_EGL_USE_PTHREAD
#include <pthread.h>

typedef struct
{
    GS_EGL10_ThreadFunction function;
    void *argument;
} GS_EGL10_ThreadData;

static void *GS_EGL10_pthread_entry(void *argument)
{
    GS_EGL10_ThreadData *data = (GS_EGL10_ThreadData *)argument;
    data->function(data->argument);
    return NULL;
}

static EGLBoolean GS_EGL10_create_and_join_thread(
    GS_EGL10_ThreadFunction function,
    void *argument)
{
    pthread_t thread;
    GS_EGL10_ThreadData data;

    if (function == NULL)
        return EGL_FALSE;

    data.function = function;
    data.argument = argument;

    if (pthread_create(&thread, NULL, GS_EGL10_pthread_entry, &data) != 0)
        return EGL_FALSE;
    return pthread_join(thread, NULL) == 0;
}
#endif

/*
 * EGL10 - Initialization - eglInitialize
 *
 * Covered requirement: GS-EGL10-IN-INI-009
 */
static const char *test_case = "GS_EGL10_IN_INI_TC_009";
static const char *test_procedure = "GS_EGL10_IN_INI_TP_006";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;
static EGLBoolean thread_result = EGL_FALSE;
static EGLint thread_error = EGL_SUCCESS;
static EGLint thread_config_count = -1;

static void GS_EGL10_IN_INI_TP_006_thread(void *argument)
{
    EGLDisplay display = *((EGLDisplay *)argument);

    /* Deliberately do not call eglInitialize in this thread. */
    (void)eglGetError();
    thread_result = eglGetConfigs(display, NULL, 0, &thread_config_count);
    thread_error = eglGetError();
}

void GS_EGL10_IN_INI_TP_006_init(void)
{
    if (!GS_EGL10_initialize_display(&environment))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not initialize the display in the calling thread");
        test_success = EGL_FALSE;
        return;
    }

    thread_result = EGL_FALSE;
    thread_error = EGL_SUCCESS;
    thread_config_count = -1;

    if (!GS_EGL10_create_and_join_thread(
            GS_EGL10_IN_INI_TP_006_thread, &environment.display))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create and join the secondary thread");
        test_success = EGL_FALSE;
        return;
    }

    if (thread_result != EGL_TRUE || thread_error != EGL_SUCCESS ||
        thread_config_count < 0)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "The second thread could not use the initialized display: "
            "%u/0x%x/%d",
            (unsigned int)thread_result, thread_error, thread_config_count);
        test_success = EGL_FALSE;
    }

    if (test_success)
        TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_IN_INI_TP_006_draw(void) { }

void GS_EGL10_IN_INI_TP_006_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
    thread_result = EGL_FALSE;
    thread_error = EGL_SUCCESS;
    thread_config_count = -1;
}
