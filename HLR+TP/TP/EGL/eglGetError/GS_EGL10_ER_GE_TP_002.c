#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
EGL10 - Errors - eglGetError

Verify that eglGetError returns EGL_SUCCESS when the most recently called EGL
function in the calling thread completes without generating an error.

Covered requirement:
    - GS-EGL10-ER-GE-002
*/

static const char* test_case = "GS_EGL10_ER_GE_TC_002";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_002";

void GS_EGL10_ER_GE_TP_002_init(void) {
    EGLint error;

    /*
     * Read and discard any error left by an earlier EGL call. This provides
     * a clean error state before the successful call used by this test.
     */
    (void)eglGetError();

    /*
     * Query the current display without requiring one to be bound. This is a
     * valid EGL call and is expected to complete without generating an error.
     */
    (void)eglGetCurrentDisplay();
    error = eglGetError();

    if (error != EGL_SUCCESS) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_SUCCESS after a successful EGL call, got: 0x%x",
            error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_ER_GE_TP_002_draw(void) {

}

void GS_EGL10_ER_GE_TP_002_close(void) {

}
