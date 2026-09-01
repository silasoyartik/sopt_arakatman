#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
EGL10 - Errors - eglGetError

Verify that the graphics library provides the eglGetError API function.

Covered requirement:
    - GS-EGL10-ER-GE-001
*/

static const char* test_case = "GS_EGL10_ER_GE_TC_001";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_001";

void GS_EGL10_ER_GE_TP_001_init(void) {
    EGLint error;

    /*
     * Call eglGetError directly to verify that the API is declared and
     * provided by the graphics library. Successful compilation and linking
     * demonstrate API availability; error values are tested separately.
     */
    error = eglGetError();
    (void)error;

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_ER_GE_TP_001_draw(void) {

}

void GS_EGL10_ER_GE_TP_001_close(void) {

}
