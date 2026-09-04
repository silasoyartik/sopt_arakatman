#include <EGL/egl.h>
#include "../../helpers.h"
#include "../../macros.h"

/* EGL10 - ConfigurationManagement - eglGetConfigs
 * Covered requirements: GS-EGL10-CM-GCS-008 */
static const char* test_case = "GS_EGL10_CM_GCS_TC_008";
static const char* test_procedure = "GS_EGL10_CM_GCS_TP_008";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_CM_GCS_TP_008_init(void)
{
    EGLDisplay display;
    EGLBoolean result;
    EGLint error;
    if (GS_EGL10_initialize_display(&environment) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain and initialize EGL_DEFAULT_DISPLAY, error: 0x%x",
            eglGetError());
        return;
    }
    display = environment.display;
    (void)eglGetError();
    result = eglGetConfigs(display, NULL, 0, NULL);
    error = eglGetError();
    if ((result != EGL_FALSE) || (error != EGL_BAD_PARAMETER)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE and EGL_BAD_PARAMETER, got result %d, error: 0x%x", result, error);
        return;
    }
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_CM_GCS_TP_008_draw(void) {}
void GS_EGL10_CM_GCS_TP_008_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
