#include <EGL/egl.h>
#include "../../helpers.h"

/*
 * EGL10 - ConfigurationManagement - eglChooseConfig
 *
 * Covered requirements:
 *   GS-EGL10-CM-CC-001
 *   GS-EGL10-CM-CC-002
 *   GS-EGL10-CM-CC-003
 */
static const char *test_case1 = "GS_EGL10_CM_CC_TC_001";
static const char *test_case2 = "GS_EGL10_CM_CC_TC_002";
static const char *test_case3 = "GS_EGL10_CM_CC_TC_003";
static const char *test_procedure = "GS_EGL10_CM_CC_TP_001";
static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;
static EGLBoolean test_success3 = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_CM_CC_TP_001_init(void)
{
    EGLBoolean (*choose_config)(EGLDisplay, const EGLint *, EGLConfig *,
        EGLint, EGLint *) = eglChooseConfig;
    EGLConfig configs[2] = { (EGLConfig)0, (EGLConfig)0 };
    EGLint attributes[3];
    EGLint config_id = -1;
    EGLint returned = -1;
    EGLint queried_id = -1;
    EGLint total = -1;
    EGLBoolean result;

    if (!GS_EGL10_prepare_config_environment(&environment) ||
        eglGetConfigAttrib(environment.display, environment.config,
            EGL_CONFIG_ID, &config_id) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case1, test_procedure,
            "Could not prepare an initialized display and reference config");
        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
        test_success3 = EGL_FALSE;
        return;
    }

    attributes[0] = EGL_CONFIG_ID;
    attributes[1] = config_id;
    attributes[2] = EGL_NONE;

    result = choose_config(environment.display, attributes, configs, 1,
        &returned);

    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case1, test_procedure,
            "eglChooseConfig entry point returned EGL_FALSE, EGL error: 0x%x",
            eglGetError());
        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
    }

    if (result != EGL_TRUE || returned != 1 ||
        configs[0] == (EGLConfig)0 || configs[1] != (EGLConfig)0 ||
        eglGetConfigAttrib(environment.display, configs[0], EGL_CONFIG_ID,
            &queried_id) != EGL_TRUE ||
        queried_id != config_id)
    {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "Expected one valid matching handle without exceeding config_size");
        test_success2 = EGL_FALSE;
    }

    result = eglChooseConfig(environment.display, attributes, NULL, 0, &total);
    if (result != EGL_TRUE || total != 1)
    {
        TEST_LOG_FAIL(test_case3, test_procedure,
            "configs == NULL shall return the total match count; got %u/%d",
            (unsigned int)result, total);
        test_success3 = EGL_FALSE;
    }

    if (test_success1)
        TEST_LOG_SUCCESS(test_case1, test_procedure);
    if (test_success2)
        TEST_LOG_SUCCESS(test_case2, test_procedure);
    if (test_success3)
        TEST_LOG_SUCCESS(test_case3, test_procedure);
}

void GS_EGL10_CM_CC_TP_001_draw(void) { }

void GS_EGL10_CM_CC_TP_001_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
