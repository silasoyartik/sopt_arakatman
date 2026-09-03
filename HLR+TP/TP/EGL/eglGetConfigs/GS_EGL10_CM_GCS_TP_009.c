#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - ConfigurationManagement - eglGetConfigs
 * Covered requirements: GS-EGL10-CM-GCS-009 */
static const char* test_case = "GS_EGL10_CM_GCS_TC_009";
static const char* test_procedure = "GS_EGL10_CM_GCS_TP_009";

void GS_EGL10_CM_GCS_TP_009_init(void)
{
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLint count = -1;
    EGLBoolean result;
    EGLint error;
    if (display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure, "Could not obtain EGL_DEFAULT_DISPLAY");
        return;
    }
    if ((eglInitialize(display, NULL, NULL) != EGL_TRUE) ||
        (eglTerminate(display) != EGL_TRUE)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not establish an uninitialized EGLDisplay, error: 0x%x", eglGetError());
        return;
    }
    (void)eglGetError();
    result = eglGetConfigs(display, NULL, 0, &count);
    error = eglGetError();
    if ((result != EGL_FALSE) || (error != EGL_NOT_INITIALIZED)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE and EGL_NOT_INITIALIZED, got result %d, error: 0x%x", result, error);
        return;
    }
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_CM_GCS_TP_009_draw(void) {}
void GS_EGL10_CM_GCS_TP_009_close(void) {}
