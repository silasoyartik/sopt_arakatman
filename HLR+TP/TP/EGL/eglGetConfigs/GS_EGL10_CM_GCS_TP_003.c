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
    EGLint total = 0;
    EGLint returned = 0;
    EGLint config_size;
    EGLint config_id;
    EGLint index;

    if ((display == EGL_NO_DISPLAY) ||
        (eglGetConfigs(display, NULL, 0, &total) != EGL_TRUE)) {
        TEST_LOG_FAIL(test_case, test_procedure, "An initialized current EGLDisplay is required");
        return;
    }

    if (total < 0) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Count-only eglGetConfigs returned an invalid count: %d", total);
        return;
    }

    /* A non-NULL buffer call is required even when no configs are available. */
    config_size = (total > 0) ? total : 1;
    configs = (EGLConfig*)malloc((size_t)config_size * sizeof(EGLConfig));
    if (configs == NULL) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not allocate the EGLConfig buffer");
        return;
    }

    (void)eglGetError();
    if ((eglGetConfigs(display, configs, config_size, &returned) != EGL_TRUE) ||
        (returned != total)) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not return the complete EGLConfig list, error: 0x%x",
            eglGetError());
        free(configs);
        return;
    }

    /* Every returned slot shall contain a valid EGLConfig for this display. */
    for (index = 0; index < returned; ++index) {
        if (eglGetConfigAttrib(display, configs[index], EGL_CONFIG_ID,
                &config_id) != EGL_TRUE) {
            TEST_LOG_FAIL(test_case, test_procedure,
                "Returned config at index %d is invalid, error: 0x%x",
                index, eglGetError());
            free(configs);
            return;
        }
    }

    free(configs);
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_CM_GCS_TP_003_draw(void) {}
void GS_EGL10_CM_GCS_TP_003_close(void) {}
