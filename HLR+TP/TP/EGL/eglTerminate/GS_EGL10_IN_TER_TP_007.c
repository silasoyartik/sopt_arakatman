#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - Initialization - eglTerminate

Verify that a display can be initialized again after successful termination.

Covered requirements:
    - GS-EGL10-IN-TER-007
*/

static const char* test_case = "GS_EGL10_IN_TER_TC_007";
static const char* test_procedure = "GS_EGL10_IN_TER_TP_007";

static EGLDisplay test_display = EGL_NO_DISPLAY;
static EGLBoolean reinitialized = EGL_FALSE;

/* Terminates an initialized display and verifies that it can be reinitialized. */
void GS_EGL10_IN_TER_TP_007_init(void) {
    EGLint error;

    test_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (test_display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain a valid EGLDisplay, error: 0x%x", eglGetError());
        return;
    }

    (void)eglGetError();
    if (eglInitialize(test_display, NULL, NULL) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not initialize the EGLDisplay, error: 0x%x", eglGetError());
        return;
    }

    (void)eglGetError();
    if (eglTerminate(test_display) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not terminate the initialized EGLDisplay, error: 0x%x",
            eglGetError());
        return;
    }

    (void)eglGetError();
    if (eglInitialize(test_display, NULL, NULL) != EGL_TRUE) {
        error = eglGetError();
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglInitialize failed after eglTerminate, error: 0x%x", error);
        return;
    }

    reinitialized = EGL_TRUE;
    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this display-lifecycle test. */
void GS_EGL10_IN_TER_TP_007_draw(void) {

}

/* Releases the display initialization established for the reinitialization check. */
void GS_EGL10_IN_TER_TP_007_close(void) {
    if ((reinitialized == EGL_TRUE) && (test_display != EGL_NO_DISPLAY)) {
        (void)eglTerminate(test_display);
    }

    reinitialized = EGL_FALSE;
    test_display = EGL_NO_DISPLAY;
}
