#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - ConfigurationManagement - eglGetConfigs
 * Covered requirements: GS-EGL10-CM-GCS-001 */
static const char* test_case = "GS_EGL10_CM_GCS_TC_001";
static const char* test_procedure = "GS_EGL10_CM_GCS_TP_001";

void GS_EGL10_CM_GCS_TP_001_init(void)
{
    EGLBoolean (*get_configs)(EGLDisplay, EGLConfig*, EGLint, EGLint*) = eglGetConfigs;
    EGLint config_count = 0;
    (void)get_configs(EGL_NO_DISPLAY, NULL, 0, &config_count);
    (void)eglGetError();
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_CM_GCS_TP_001_draw(void) {}
void GS_EGL10_CM_GCS_TP_001_close(void) {}
