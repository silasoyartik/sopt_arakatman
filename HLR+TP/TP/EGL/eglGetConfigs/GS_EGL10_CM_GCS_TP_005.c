#include <EGL/egl.h>
#include <stdlib.h>
#include "../../macros.h"

/* EGL10 - ConfigurationManagement - eglGetConfigs
 * Covered requirements: GS-EGL10-CM-GCS-005 */
static const char* test_case = "GS_EGL10_CM_GCS_TC_005";
static const char* test_procedure = "GS_EGL10_CM_GCS_TP_005";

void GS_EGL10_CM_GCS_TP_005_init(void)
{
    EGLDisplay display = eglGetCurrentDisplay();
    EGLConfig* configs;
    EGLint total = 0;
    EGLint returned = 0;
    EGLint value;
    EGLint index;
    if ((display == EGL_NO_DISPLAY) ||
        (eglGetConfigs(display, NULL, 0, &total) != EGL_TRUE) || (total <= 0)) {
        TEST_LOG_FAIL(test_case, test_procedure, "Could not establish a non-empty config list");
        return;
    }
    configs = (EGLConfig*)malloc((size_t)total * sizeof(EGLConfig));
    if (configs == NULL) {
        TEST_LOG_FAIL(test_case, test_procedure, "Could not allocate the EGLConfig buffer");
        return;
    }
    if ((eglGetConfigs(display, configs, total, &returned) != EGL_TRUE) ||
        (returned != total)) {
        TEST_LOG_FAIL(test_case, test_procedure, "Could not return the complete EGLConfig list");
        free(configs);
        return;
    }
    for (index = 0; index < returned; ++index) {
        if (eglGetConfigAttrib(display, configs[index], EGL_CONFIG_ID, &value) != EGL_TRUE) {
            TEST_LOG_FAIL(test_case, test_procedure,
                "Returned config at index %d is not valid, error: 0x%x", index, eglGetError());
            free(configs);
            return;
        }
    }
    free(configs);
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_CM_GCS_TP_005_draw(void) {}
void GS_EGL10_CM_GCS_TP_005_close(void) {}
