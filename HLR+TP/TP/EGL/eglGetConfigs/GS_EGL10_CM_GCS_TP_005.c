#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - ConfigurationManagement - eglGetConfigs
 * Covered requirements: GS-EGL10-CM-GCS-005 */
static const char* test_case = "GS_EGL10_CM_GCS_TC_005";
static const char* test_procedure = "GS_EGL10_CM_GCS_TP_005";

void GS_EGL10_CM_GCS_TP_005_init(void)
{
    EGLDisplay display = eglGetCurrentDisplay();
    EGLConfig config;
    EGLint total = 0;
    EGLint returned = -1;
    EGLint expected_returned;

    if (display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "An initialized current EGLDisplay is required");
        return;
    }

    if ((eglGetConfigs(display, NULL, 0, &total) != EGL_TRUE) || (total < 0)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain a valid total EGLConfig count");
        return;
    }

    expected_returned = (total > 0) ? 1 : 0;
    if ((eglGetConfigs(display, &config, 1, &returned) != EGL_TRUE) ||
        (returned != expected_returned)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "num_config does not report the number of handles returned to the one-element buffer (got %d, expected %d)",
            returned, expected_returned);
        return;
    }
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_CM_GCS_TP_005_draw(void) {}
void GS_EGL10_CM_GCS_TP_005_close(void) {}
