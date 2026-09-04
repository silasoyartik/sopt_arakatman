#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - ConfigurationManagement - eglGetConfigs
 * Covered requirements: GS-EGL10-CM-GCS-007 */
static const char* test_case = "GS_EGL10_CM_GCS_TC_007";
static const char* test_procedure = "GS_EGL10_CM_GCS_TP_007";
static EGLDisplay test_display = EGL_NO_DISPLAY;
static EGLBoolean display_initialized = EGL_FALSE;

void GS_EGL10_CM_GCS_TP_007_init(void)
{
    EGLint count = -1;
    EGLBoolean result;
    EGLint error;
    test_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (test_display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure, "Could not obtain EGL_DEFAULT_DISPLAY");
        return;
    }

    if (eglInitialize(test_display, NULL, NULL) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not initialize EGL_DEFAULT_DISPLAY, error: 0x%x", eglGetError());
        return;
    }
    display_initialized = EGL_TRUE;

    if (eglTerminate(test_display) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not establish an uninitialized EGLDisplay, error: 0x%x", eglGetError());
        return;
    }
    display_initialized = EGL_FALSE;

    (void)eglGetError();
    result = eglGetConfigs(test_display, NULL, 0, &count);
    error = eglGetError();
    if ((result != EGL_FALSE) || (error != EGL_NOT_INITIALIZED)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE and EGL_NOT_INITIALIZED, got result %d, error: 0x%x", result, error);
        return;
    }
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_CM_GCS_TP_007_draw(void) {}
void GS_EGL10_CM_GCS_TP_007_close(void)
{
    if ((display_initialized == EGL_TRUE) &&
        (test_display != EGL_NO_DISPLAY)) {
        (void)eglTerminate(test_display);
    }

    display_initialized = EGL_FALSE;
    test_display = EGL_NO_DISPLAY;
}
