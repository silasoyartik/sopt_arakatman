#include <EGL/egl.h>
#include "../../helpers.h"

/*
 * EGL10 - ConfigurationManagement - eglChooseConfig
 *
 * Covered requirements:
 *   GS-EGL10-CM-CC-004
 *   GS-EGL10-CM-CC-005
 *   GS-EGL10-CM-CC-006
 */
static const char *test_case1 = "GS_EGL10_CM_CC_TC_004";
static const char *test_case2 = "GS_EGL10_CM_CC_TC_005";
static const char *test_case3 = "GS_EGL10_CM_CC_TC_006";
static const char *test_procedure = "GS_EGL10_CM_CC_TP_002";
static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;
static EGLBoolean test_success3 = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_CM_CC_TP_002_init(void)
{
    const EGLint empty_attributes[] = { EGL_NONE };
    const EGLint explicit_red_default[] = {
        EGL_RED_SIZE, 0,
        EGL_NONE
    };
    const EGLint explicit_surface_default[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };
    const EGLint ignored_red[] = {
        EGL_RED_SIZE, EGL_DONT_CARE,
        EGL_NONE
    };
    const EGLint invalid_level[] = {
        EGL_LEVEL, EGL_DONT_CARE,
        EGL_NONE
    };
    EGLint null_count = -1;
    EGLint empty_count = -1;
    EGLint red_default_count = -1;
    EGLint surface_default_count = -1;
    EGLint ignored_count = -1;
    EGLint invalid_count = -1;
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_prepare_config_environment(&environment))
    {
        TEST_LOG_FAIL(test_case1, test_procedure,
            "Could not prepare the initialized display precondition");
        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
        test_success3 = EGL_FALSE;
        return;
    }

    if (!GS_EGL10_get_matching_config_count(
            environment.display, NULL, &null_count) ||
        !GS_EGL10_get_matching_config_count(
            environment.display, empty_attributes, &empty_count) ||
        !GS_EGL10_get_matching_config_count(
            environment.display, explicit_red_default, &red_default_count) ||
        !GS_EGL10_get_matching_config_count(environment.display,
            explicit_surface_default, &surface_default_count) ||
        null_count != empty_count ||
        null_count != red_default_count ||
        null_count != surface_default_count)
    {
        TEST_LOG_FAIL(test_case1, test_procedure,
            "NULL, empty, omitted and explicit default criteria differ: "
            "%d/%d/%d/%d",
            null_count, empty_count, red_default_count,
            surface_default_count);
        test_success1 = EGL_FALSE;
    }

    if (!GS_EGL10_get_matching_config_count(
            environment.display, ignored_red, &ignored_count) ||
        ignored_count != null_count)
    {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "EGL_DONT_CARE did not ignore EGL_RED_SIZE: %d/%d",
            ignored_count, null_count);
        test_success2 = EGL_FALSE;
    }

    (void)eglGetError();
    result = eglChooseConfig(environment.display, invalid_level, NULL, 0,
        &invalid_count);
    error = eglGetError();
    if (result != EGL_FALSE || error != EGL_BAD_ATTRIBUTE)
    {
        TEST_LOG_FAIL(test_case3, test_procedure,
            "EGL_LEVEL=EGL_DONT_CARE shall fail with EGL_BAD_ATTRIBUTE; "
            "got %u/0x%x",
            (unsigned int)result, error);
        test_success3 = EGL_FALSE;
    }

    if (test_success1)
        TEST_LOG_SUCCESS(test_case1, test_procedure);
    if (test_success2)
        TEST_LOG_SUCCESS(test_case2, test_procedure);
    if (test_success3)
        TEST_LOG_SUCCESS(test_case3, test_procedure);
}

void GS_EGL10_CM_CC_TP_002_draw(void) { }

void GS_EGL10_CM_CC_TP_002_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
