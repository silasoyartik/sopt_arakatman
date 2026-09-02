#include <EGL/egl.h>
#include <pthread.h>
#include "../../macros.h"

/*
EGL10 - Errors - eglGetError

Covered requirement:
    - GS-EGL10-ER-GE-004
*/

static const char* test_case = "GS_EGL10_ER_GE_TC_004";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_004";

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
static EGLContext context = EGL_NO_CONTEXT;
static EGLBoolean make_current_result = EGL_FALSE;
static EGLint worker_error = EGL_SUCCESS;

static EGLBoolean create_test_objects(void) {
    const EGLint config_attributes[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_NONE
    };
    const EGLint surface_attributes[] = {
        EGL_WIDTH, 1,
        EGL_HEIGHT, 1,
        EGL_NONE
    };
    EGLConfig config;
    EGLint config_count = 0;

    /*
     * Create and initialize the display used only by this test procedure.
     * No display, config, surface or context is expected from main.
     */
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if ((display == EGL_NO_DISPLAY) ||
        (eglInitialize(display, NULL, NULL) == EGL_FALSE)) {
        return EGL_FALSE;
    }

    /*
     * Select a pbuffer-capable configuration and create a one-pixel surface.
     * The small surface keeps the test fixture independent of a native window.
     */
    if ((eglChooseConfig(display, config_attributes, &config, 1,
                         &config_count) == EGL_FALSE) ||
        (config_count != 1)) {
        return EGL_FALSE;
    }

    surface = eglCreatePbufferSurface(
        display, config, surface_attributes);
    if (surface == EGL_NO_SURFACE) {
        return EGL_FALSE;
    }

    /*
     * Create the context and keep it current in the test thread. The worker
     * thread will attempt to access these same EGL objects.
     */
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
    if (context == EGL_NO_CONTEXT) {
        return EGL_FALSE;
    }

    return eglMakeCurrent(display, surface, surface, context);
}

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
        display, surface, surface, context);
    worker_error = eglGetError();

    return NULL;
}

void GS_EGL10_ER_GE_TP_004_init(void) {
    pthread_t worker;
    int thread_result;

    /*
     * Build every EGL object required by the test in this source file. The
     * test does not depend on EGL initialization performed by main.
     */
    if (create_test_objects() == EGL_FALSE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create the EGL test objects, error: 0x%x",
            eglGetError());
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
    /*
     * Release and destroy every EGL object created by this test procedure.
     * Each handle is checked so partial initialization can also be cleaned up.
     */
    if (display != EGL_NO_DISPLAY) {
        (void)eglMakeCurrent(
            display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (context != EGL_NO_CONTEXT) {
            (void)eglDestroyContext(display, context);
            context = EGL_NO_CONTEXT;
        }

        if (surface != EGL_NO_SURFACE) {
            (void)eglDestroySurface(display, surface);
            surface = EGL_NO_SURFACE;
        }

        (void)eglTerminate(display);
        display = EGL_NO_DISPLAY;
    }

}
