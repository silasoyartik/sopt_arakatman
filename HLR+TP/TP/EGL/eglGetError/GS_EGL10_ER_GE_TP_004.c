#include <pthread.h>
#include "../../macros.h"
#include <EGL/egl.h>

/*
EGL10 - Errors - eglGetError

Covered requirement:
    - GS-EGL10-ER-GE-004
*/

static const char* test_case = "GS_EGL10_ER_GE_TC_004";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_004";

static EGLDisplay display;
static EGLSurface draw_surface;
static EGLSurface read_surface;
static EGLContext context;
static EGLBoolean make_current_result;
static EGLint worker_error;

static void* access_context_from_worker(void* argument) {
    (void)argument;

    /*
     * Remove any error previously stored for this worker thread. The error
     * read below can then be associated with its eglMakeCurrent call.
     */
    (void)eglGetError();

    /*
     * Try to bind resources that remain current in the main test thread.
     * The worker cannot access them and shall receive EGL_BAD_ACCESS.
     */
    make_current_result = eglMakeCurrent(
        display, draw_surface, read_surface, context);
    worker_error = eglGetError();

    return NULL;
}

void GS_EGL10_ER_GE_TP_004_init(void) {
    pthread_t worker;
    int thread_result;

    /*
     * Obtain the EGL objects currently bound by the test framework. They must
     * stay current in this thread while the worker attempts to bind them.
     */
    display = eglGetCurrentDisplay();
    draw_surface = eglGetCurrentSurface(EGL_DRAW);
    read_surface = eglGetCurrentSurface(EGL_READ);
    context = eglGetCurrentContext();

    if ((display == EGL_NO_DISPLAY) ||
        (draw_surface == EGL_NO_SURFACE) ||
        (read_surface == EGL_NO_SURFACE) ||
        (context == EGL_NO_CONTEXT)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "A current EGL display, surface and context are required");
        return;
    }

    /*
     * Run the access attempt in another thread and wait for its result. The
     * main thread keeps ownership of the current EGL context while waiting.
     */
    thread_result = pthread_create(
        &worker, NULL, access_context_from_worker, NULL);
    if (thread_result != 0) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create the worker thread, error: %d", thread_result);
        return;
    }

    thread_result = pthread_join(worker, NULL);
    if (thread_result != 0) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not join the worker thread, error: %d", thread_result);
        return;
    }

    if (make_current_result != EGL_FALSE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglMakeCurrent unexpectedly succeeded in the worker thread");
        return;
    }

    if (worker_error != EGL_BAD_ACCESS) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_BAD_ACCESS, got: 0x%x", worker_error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_ER_GE_TP_004_draw(void) {

}

void GS_EGL10_ER_GE_TP_004_close(void) {

}
