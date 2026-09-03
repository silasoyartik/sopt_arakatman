#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - ConfigurationManagement - eglGetConfigs
 * Covered requirements: GS-EGL10-CM-GCS-002 */
static const char* test_case = "GS_EGL10_CM_GCS_TC_002";
static const char* test_procedure = "GS_EGL10_CM_GCS_TP_002";

void GS_EGL10_CM_GCS_TP_002_init(void)
{
    EGLDisplay display = eglGetCurrentDisplay();
    EGLint config_count = -1;
    if (display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure, "An initialized current EGLDisplay is required");
        return;
    }
    (void)eglGetError();
    if (eglGetConfigs(display, NULL, 0, &config_count) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Count-only eglGetConfigs failed, error: 0x%x", eglGetError());
        return;
    }
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_CM_GCS_TP_002_draw(void) {}
void GS_EGL10_CM_GCS_TP_002_close(void) {}
