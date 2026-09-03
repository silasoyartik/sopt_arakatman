#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - ConfigurationManagement - eglGetConfigs
 * Covered requirements: GS-EGL10-CM-GCS-006 */
static const char* test_case = "GS_EGL10_CM_GCS_TC_006";
static const char* test_procedure = "GS_EGL10_CM_GCS_TP_006";

void GS_EGL10_CM_GCS_TP_006_init(void)
{
    EGLDisplay display = eglGetCurrentDisplay();
    EGLConfig output[2] = { (EGLConfig)0, (EGLConfig)0 };
    EGLint total = 0;
    EGLint returned = -1;
    if (display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure, "An initialized current EGLDisplay is required");
        return;
    }

    /* Establish that one element is insufficient for the available list. */
    if ((eglGetConfigs(display, NULL, 0, &total) != EGL_TRUE) || (total < 2)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "At least two EGLConfigs are required to establish the capacity boundary");
        return;
    }

    /*
     * output[1] immediately follows the one-element logical output buffer. A
     * write to output[1] would prove that eglGetConfigs exceeded config_size.
     */
    if (eglGetConfigs(display, output, 1, &returned) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "One-element eglGetConfigs failed, error: 0x%x", eglGetError());
        return;
    }
    if ((returned > 1) || (output[1] != (EGLConfig)0)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglGetConfigs wrote beyond its one-element config buffer");
        return;
    }
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_CM_GCS_TP_006_draw(void) {}
void GS_EGL10_CM_GCS_TP_006_close(void) {}
