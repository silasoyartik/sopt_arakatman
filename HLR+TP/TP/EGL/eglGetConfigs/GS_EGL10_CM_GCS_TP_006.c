#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - ConfigurationManagement - eglGetConfigs
 * Covered requirements: GS-EGL10-CM-GCS-006 */
static const char* test_case = "GS_EGL10_CM_GCS_TC_006";
static const char* test_procedure = "GS_EGL10_CM_GCS_TP_006";

void GS_EGL10_CM_GCS_TP_006_init(void)
{
    EGLDisplay display = eglGetCurrentDisplay();
    EGLConfig guard = (EGLConfig)0;
    EGLConfig initial_guard = guard;
    EGLint returned = -1;
    if (display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure, "An initialized current EGLDisplay is required");
        return;
    }
    if (eglGetConfigs(display, &guard, 0, &returned) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Zero-capacity eglGetConfigs failed, error: 0x%x", eglGetError());
        return;
    }
    if ((returned != 0) || (guard != initial_guard)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglGetConfigs wrote beyond its zero-element config buffer");
        return;
    }
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_CM_GCS_TP_006_draw(void) {}
void GS_EGL10_CM_GCS_TP_006_close(void) {}
