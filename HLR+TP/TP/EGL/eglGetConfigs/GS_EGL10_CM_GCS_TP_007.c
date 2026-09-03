#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - ConfigurationManagement - eglGetConfigs
 * Covered requirements: GS-EGL10-CM-GCS-007 */
static const char* test_case = "GS_EGL10_CM_GCS_TC_007";
static const char* test_procedure = "GS_EGL10_CM_GCS_TP_007";

void GS_EGL10_CM_GCS_TP_007_init(void)
{
    EGLDisplay display = eglGetCurrentDisplay();
    EGLConfig config;
    EGLint total = 0;
    EGLint returned = -1;
    if ((display == EGL_NO_DISPLAY) ||
        (eglGetConfigs(display, NULL, 0, &total) != EGL_TRUE) || (total <= 0)) {
        TEST_LOG_FAIL(test_case, test_procedure, "Could not obtain the total EGLConfig count");
        return;
    }
    if ((eglGetConfigs(display, &config, 1, &returned) != EGL_TRUE) ||
        (returned != 1)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "num_config does not report the one handle returned to the one-element buffer (got %d)",
            returned);
        return;
    }
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_CM_GCS_TP_007_draw(void) {}
void GS_EGL10_CM_GCS_TP_007_close(void) {}
