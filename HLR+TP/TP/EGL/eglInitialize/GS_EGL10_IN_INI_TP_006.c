#include <stdio.h>
#include <EGL/egl.h>
#include "../../helpers.h"

/* OPTIONAL PLATFORM ADAPTATION - POSIX/PTHREAD EXAMPLE
 *
 * Define GS_EGL_USE_PTHREAD if POSIX threads are available.
 *
 * If the target system does not use pthreads, the target test
 * environment shall provide its own implementation of:
 *
 *     GS_EGL10_create_and_join_thread()
 *
 * The platform-specific function shall:
 *
 *     1. Create a new thread.
 *     2. Execute the supplied thread function in that thread.
 *     3. Wait until the thread completes.
 *     4. Return EGL_TRUE when thread execution succeeds.
 *
 */

#ifdef GS_EGL_USE_PTHREAD

#include <pthread.h>

typedef struct
{
    GS_EGL10_ThreadFunction function;
    void *argument;

} GS_EGL10_ThreadData;


static void *GS_EGL10_pthread_entry(void *arg)
{
    GS_EGL10_ThreadData *thread_data =
        (GS_EGL10_ThreadData *)arg;

    thread_data->function(
        thread_data->argument
    );

    return NULL;
}


static EGLBoolean GS_EGL10_create_and_join_thread(
    GS_EGL10_ThreadFunction function,
    void *argument)
{
    pthread_t thread;

    GS_EGL10_ThreadData thread_data;

    thread_data.function = function;
    thread_data.argument = argument;


    if (pthread_create(
            &thread,
            NULL,
            GS_EGL10_pthread_entry,
            &thread_data) != 0)
    {
        return EGL_FALSE;
    }


    if (pthread_join(thread, NULL) != 0)
    {
        return EGL_FALSE;
    }


    return EGL_TRUE;
}


#endif


/*
EGL10 - Initialization - Initialize

The graphics library shall allow an initialized EGLDisplay
to be used by other threads in the same address space without
initializing that display again in those threads.

Covered requirements:
    - GS-EGL10-IN-INI-009
*/

static const char* test_case =
    "GS_EGL10_IN_INI_TC_009";

static const char* test_procedure =
    "GS_EGL10_IN_INI_TP_006";

static EGLBoolean test_success = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;

static EGLBoolean initialized = EGL_FALSE;


/* Results written by the second thread. */
static EGLBoolean thread_egl_result = EGL_FALSE;

static EGLint thread_egl_error = EGL_SUCCESS;

static EGLint thread_num_config = -1;


/* SECOND THREAD
 *
 * IMPORTANT:
 *
 * eglInitialize() shall NOT be called in this function.
 *
 * The purpose of the test is to verify that the EGLDisplay
 * initialized by the first thread can be used directly by
 * another thread in the same address space.
 *
 */
static void GS_EGL10_IN_INI_TP_006_thread(void *argument)
{
    EGLDisplay thread_display =
        *((EGLDisplay *)argument);


    /* Clear the EGL error state of this thread. */
    (void)eglGetError();


    /* Use the display without calling eglInitialize().
     *
     * eglGetConfigs requires an initialized EGLDisplay.
     * Therefore successful execution demonstrates that
     * initialization performed in the first thread is also
     * effective for this thread.
     */
    thread_egl_result =
        eglGetConfigs(
            thread_display,
            NULL,
            0,
            &thread_num_config
        );


    thread_egl_error =
        eglGetError();
}


/* Initialization */
void GS_EGL10_IN_INI_TP_006_init(void)
{
    EGLBoolean result;
    EGLint error;

    EGLint major = -1;
    EGLint minor = -1;


    /* Precondition 1:  Obtain an EGLDisplay in the first thread. */

    display =
        eglGetDisplay(EGL_DEFAULT_DISPLAY);


    if (display == EGL_NO_DISPLAY)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: "
            "eglGetDisplay(EGL_DEFAULT_DISPLAY) returned "
            "EGL_NO_DISPLAY. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
        return;
    }


    /* Precondition 2: Initialize the display in the first thread. */

    (void)eglGetError();


    result =
        eglInitialize(
            display,
            &major,
            &minor
        );


    error =
        eglGetError();


    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: eglInitialize "
            "returned EGL_FALSE. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
        return;
    }


    initialized = EGL_TRUE;


    /* Reset outputs written by the second thread. */
    thread_egl_result = EGL_FALSE;
    thread_egl_error = EGL_SUCCESS;
    thread_num_config = -1;


    // Test Case 009

    /* Create another thread and use the already-initialized EGLDisplay without calling eglInitialize in that thread. */
    result =
        GS_EGL10_create_and_join_thread(
            GS_EGL10_IN_INI_TP_006_thread,
            &display
        );


    /* Thread creation/execution is a test-environment precondition. */
    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test environment failed to create or join "
            "the secondary thread"
        );

        test_success = EGL_FALSE;
        return;
    }


    /* eglGetConfigs shall succeed in the second thread. */
    if (thread_egl_result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "The initialized EGLDisplay could not be used "
            "by the secondary thread. eglGetConfigs "
            "returned EGL_FALSE. eglGetError(): 0x%x",
            thread_egl_error
        );

        test_success = EGL_FALSE;
    }


    /*
     * EGL_NOT_INITIALIZED would specifically indicate that
     * the display initialization performed in the first
     * thread was not effective for the second thread.
     */
    if (thread_egl_error == EGL_NOT_INITIALIZED)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Secondary thread reported EGL_NOT_INITIALIZED "
            "for a display initialized by the first thread"
        );

        test_success = EGL_FALSE;
    }


    /* The output parameter shall also have been written by eglGetConfigs when the operation succeeds. */
    if (thread_egl_result == EGL_TRUE &&
        thread_num_config < 0)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "eglGetConfigs did not update num_config "
            "in the secondary thread"
        );

        test_success = EGL_FALSE;
    }


    if (test_success)
    {
        TEST_LOG_INFO(
            "Secondary thread successfully used the "
            "initialized EGLDisplay without calling "
            "eglInitialize again. Config count: %d",
            thread_num_config
        );

        TEST_LOG_SUCCESS(
            test_case,
            test_procedure
        );
    }
}

void GS_EGL10_IN_INI_TP_006_draw(void) {

}

void GS_EGL10_IN_INI_TP_006_close(void) {
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;

    thread_egl_result = EGL_FALSE;
    thread_num_config = -1;
}
