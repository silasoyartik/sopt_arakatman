#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - ConfigurationManagement - eglGetConfigs
 * Covered requirements: GS-EGL10-CM-GCS-006 */
static const char* test_case = "GS_EGL10_CM_GCS_TC_006";
static const char* test_procedure = "GS_EGL10_CM_GCS_TP_006";

void GS_EGL10_CM_GCS_TP_006_init(void)
{
    EGLint count = -1;
    EGLBoolean result;
    EGLint error;
    (void)eglGetError();
    result = eglGetConfigs(EGL_NO_DISPLAY, NULL, 0, &count);
    error = eglGetError();
    if ((result != EGL_FALSE) || (error != EGL_BAD_DISPLAY)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE and EGL_BAD_DISPLAY, got result %d, error: 0x%x", result, error);
        return;
    }
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_CM_GCS_TP_006_draw(void) {}
void GS_EGL10_CM_GCS_TP_006_close(void) {}
