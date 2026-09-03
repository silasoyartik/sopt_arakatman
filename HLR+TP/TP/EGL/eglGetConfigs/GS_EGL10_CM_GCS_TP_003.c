#include <EGL/egl.h>
#include <stdlib.h>
#include "../../macros.h"

/* EGL10 - ConfigurationManagement - eglGetConfigs
 * Covered requirements: GS-EGL10-CM-GCS-003 */
static const char* test_case = "GS_EGL10_CM_GCS_TC_003";
static const char* test_procedure = "GS_EGL10_CM_GCS_TP_003";

void GS_EGL10_CM_GCS_TP_003_init(void)
{
    EGLDisplay display = eglGetCurrentDisplay();
    EGLConfig* configs;
    EGLint total = -1;
    EGLint returned = -1;
    if (display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure, "An initialized current EGLDisplay is required");
        return;
    }
    if ((eglGetConfigs(display, NULL, 0, &total) != EGL_TRUE) || (total <= 0)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain the total EGLConfig count, error: 0x%x", eglGetError());
        return;
    }
    configs = (EGLConfig*)malloc((size_t)total * sizeof(EGLConfig));
    if (configs == NULL) {
        TEST_LOG_FAIL(test_case, test_procedure, "Could not allocate the EGLConfig buffer");
        return;
    }
    if ((eglGetConfigs(display, configs, total, &returned) != EGL_TRUE) ||
        (returned != total)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Count-only result %d does not equal complete-list result %d", total, returned);
        free(configs);
        return;
    }
    free(configs);
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_CM_GCS_TP_003_draw(void) {}
void GS_EGL10_CM_GCS_TP_003_close(void) {}
