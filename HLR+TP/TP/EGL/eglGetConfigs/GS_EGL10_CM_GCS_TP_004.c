#include <EGL/egl.h>
#include "../../helpers.h"
#include "../../macros.h"

/* EGL10 - ConfigurationManagement - eglGetConfigs
 * Covered requirements: GS-EGL10-CM-GCS-004 */
static const char* test_case = "GS_EGL10_CM_GCS_TC_004";
static const char* test_procedure = "GS_EGL10_CM_GCS_TP_004";
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_CM_GCS_TP_004_init(void)
{
    EGLDisplay display;
    struct {
        EGLConfig config[1];
        EGLConfig guard;
    } output = { { (EGLConfig)0 }, (EGLConfig)0 };
    EGLint returned = -1;

    if (GS_EGL10_initialize_display(&environment) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain and initialize EGL_DEFAULT_DISPLAY, error: 0x%x",
            eglGetError());
        return;
    }
    display = environment.display;

    /*
     * output.guard immediately follows the one-element logical output buffer.
     * A write to the guard would prove that eglGetConfigs exceeded config_size.
     */
    if (eglGetConfigs(display, output.config, 1, &returned) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "One-element eglGetConfigs failed, error: 0x%x", eglGetError());
        return;
    }
    if ((returned < 0) || (returned > 1) ||
        (output.guard != (EGLConfig)0)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglGetConfigs wrote beyond its one-element config buffer");
        return;
    }
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_CM_GCS_TP_004_draw(void) {}
void GS_EGL10_CM_GCS_TP_004_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
