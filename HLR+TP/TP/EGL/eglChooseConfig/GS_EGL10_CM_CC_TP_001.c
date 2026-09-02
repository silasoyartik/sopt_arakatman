#include <stdio.h>
#include <stdint.h>
#include <EGL/egl.h>
#include "macros.h"

/*
EGL10 - ConfigurationManagement - ChooseConfig

The graphics library shall provide API function (eglChooseConfig)
that obtains EGLConfigs matching a list of required attributes.

The eglChooseConfig function shall return EGL_TRUE and write
the number of returned matching configurations to num_config
when the call succeeds and configs is not NULL.

The eglChooseConfig function shall write valid matching EGLConfig
handles into elements zero through num_config minus one of configs
and shall return no more than config_size handles.

The eglChooseConfig function shall return no configuration handles
and shall write the total number of matching configurations to
num_config when configs is NULL.

Covered requirements:
    - GS-EGL10-CM-CC-001
    - GS-EGL10-CM-CC-002
    - GS-EGL10-CM-CC-003
    - GS-EGL10-CM-CC-004
*/

static const char* test_case1 =
    "GS_EGL10_CM_CC_TC_001";

static const char* test_case2 =
    "GS_EGL10_CM_CC_TC_002";

static const char* test_case3 =
    "GS_EGL10_CM_CC_TC_003";

static const char* test_case4 =
    "GS_EGL10_CM_CC_TC_004";

static const char* test_procedure =
    "GS_EGL10_CM_CC_TP_001";


static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;
static EGLBoolean test_success3 = EGL_TRUE;
static EGLBoolean test_success4 = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLBoolean initialized = EGL_FALSE;


/* Initialization */
void GS_EGL10_CM_CC_TP_001_init(void)
{
    EGLBoolean result;
    EGLint error;

    EGLint total_configs = 0;
    EGLint returned_configs = 0;

    EGLConfig reference_config = (EGLConfig)0;
    EGLint reference_config_id = -1;

    EGLConfig configs[1] = { (EGLConfig)0 };

    /* Reset test state in case the procedure is executed more than once. */
    test_success1 = EGL_TRUE;
    test_success2 = EGL_TRUE;
    test_success3 = EGL_TRUE;
    test_success4 = EGL_TRUE;


    /* TEST PRECONDITION
     * Obtain and initialize an EGLDisplay.
     */

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (display == EGL_NO_DISPLAY)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Test precondition failed: eglGetDisplay returned "
            "EGL_NO_DISPLAY. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
        test_success3 = EGL_FALSE;
        test_success4 = EGL_FALSE;

        return;
    }


    result = eglInitialize(
        display,
        NULL,
        NULL
    );

    if (result != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Test precondition failed: eglInitialize returned "
            "EGL_FALSE. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
        test_success3 = EGL_FALSE;
        test_success4 = EGL_FALSE;

        return;
    }

    initialized = EGL_TRUE;


    /* Obtain one existing EGLConfig.
     * eglGetConfigs is used only to establish a deterministic test precondition for eglChooseConfig.
     */

    result = eglGetConfigs(
        display,
        NULL,
        0,
        &total_configs
    );

    if (result != EGL_TRUE || total_configs <= 0)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Test precondition failed: no EGLConfig could be "
            "obtained. Count: %d, eglGetError(): 0x%x",
            total_configs,
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
        test_success3 = EGL_FALSE;
        test_success4 = EGL_FALSE;

        return;
    }


    returned_configs = 0;

    result = eglGetConfigs(
        display,
        &reference_config,
        1,
        &returned_configs
    );

    if (result != EGL_TRUE ||
        returned_configs != 1 ||
        reference_config == (EGLConfig)0)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Test precondition failed: unable to obtain a "
            "reference EGLConfig. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
        test_success3 = EGL_FALSE;
        test_success4 = EGL_FALSE;

        return;
    }


    /* Obtain the ID of the known valid reference config. */
    result = eglGetConfigAttrib(
        display,
        reference_config,
        EGL_CONFIG_ID,
        &reference_config_id
    );

    if (result != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Test precondition failed: EGL_CONFIG_ID could not "
            "be obtained. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
        test_success3 = EGL_FALSE;
        test_success4 = EGL_FALSE;

        return;
    }


    /* Choose exactly the known configuration. */
    const EGLint attrib_list[] =
    {
        EGL_CONFIG_ID, reference_config_id,
        EGL_NONE
    };


    // TEST CASE 004

    /* configs == NULL: no handles shall be returned, while num_config shall receive the total number of matching configurations. */
    EGLint matching_count = -1;

    (void)eglGetError();

    result = eglChooseConfig(
        display,
        attrib_list,
        NULL,
        0,
        &matching_count
    );

    error = eglGetError();


    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case4,
            test_procedure,
            "eglChooseConfig with configs == NULL returned "
            "EGL_FALSE. eglGetError(): 0x%x",
            error
        );

        test_success4 = EGL_FALSE;
    }


    /* EGL_CONFIG_ID identifies the known reference config, therefore exactly one matching configuration is expected. */
    if (matching_count != 1)
    {
        TEST_LOG_FAIL(
            test_case4,
            test_procedure,
            "Expected one matching configuration when configs "
            "was NULL, got: %d",
            matching_count
        );

        test_success4 = EGL_FALSE;
    }


    if (test_success4)
    {
        TEST_LOG_SUCCESS(
            test_case4,
            test_procedure
        );
    }


    // TEST CASE 001

    /* Verify basic successful use of eglChooseConfig. */
    EGLint num_config = -1;

    (void)eglGetError();

    result = eglChooseConfig(
        display,
        attrib_list,
        configs,
        1,
        &num_config
    );

    error = eglGetError();


    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "eglChooseConfig returned EGL_FALSE. "
            "eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
    }


    if (num_config <= 0)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "eglChooseConfig did not obtain a matching "
            "EGLConfig"
        );

        test_success1 = EGL_FALSE;
    }


    if (test_success1)
    {
        TEST_LOG_SUCCESS(
            test_case1,
            test_procedure
        );
    }


    // TEST CASE 002

    /* num_config shall contain the number of configurations returned in configs. */
    if (num_config != 1)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Expected num_config == 1, got: %d",
            num_config
        );

        test_success2 = EGL_FALSE;
    }


    if (test_success2)
    {
        TEST_LOG_SUCCESS(
            test_case2,
            test_procedure
        );
    }


    // TEST CASE 003

    /* Verify that:
     *   1. no more than config_size handles are returned,
     *   2. the returned handle is valid,
     *   3. the returned handle matches the requested config ID.
     */

    if (num_config > 1)
    {
        TEST_LOG_FAIL(
            test_case3,
            test_procedure,
            "eglChooseConfig returned %d configurations with "
            "config_size == 1",
            num_config
        );

        test_success3 = EGL_FALSE;
    }


    if (configs[0] == (EGLConfig)0)
    {
        TEST_LOG_FAIL(
            test_case3,
            test_procedure,
            "eglChooseConfig returned an invalid/null "
            "EGLConfig handle"
        );

        test_success3 = EGL_FALSE;
    }
    else
    {
        EGLint returned_config_id = -1;

        result = eglGetConfigAttrib(
            display,
            configs[0],
            EGL_CONFIG_ID,
            &returned_config_id
        );

        if (result != EGL_TRUE)
        {
            error = eglGetError();

            TEST_LOG_FAIL(
                test_case3,
                test_procedure,
                "Returned EGLConfig handle could not be queried. "
                "eglGetError(): 0x%x",
                error
            );

            test_success3 = EGL_FALSE;
        }
        else if (returned_config_id != reference_config_id)
        {
            TEST_LOG_FAIL(
                test_case3,
                test_procedure,
                "Returned EGLConfig does not match the requested "
                "EGL_CONFIG_ID. Expected: %d, got: %d",
                reference_config_id,
                returned_config_id
            );

            test_success3 = EGL_FALSE;
        }
    }


    /*  Additional config_size boundary check.
     * There is a known matching config, but config_size == 0.
     * Therefore no configuration handle shall be written and
     * num_config shall report zero returned handles.
     */
    EGLConfig sentinel_config =
        (EGLConfig)(uintptr_t)0x1;

    EGLint zero_size_count = -1;

    (void)eglGetError();

    result = eglChooseConfig(
        display,
        attrib_list,
        &sentinel_config,
        0,
        &zero_size_count
    );

    error = eglGetError();


    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case3,
            test_procedure,
            "eglChooseConfig with config_size == 0 returned "
            "EGL_FALSE. eglGetError(): 0x%x",
            error
        );

        test_success3 = EGL_FALSE;
    }


    if (zero_size_count != 0)
    {
        TEST_LOG_FAIL(
            test_case3,
            test_procedure,
            "Expected zero returned configurations when "
            "config_size == 0, got: %d",
            zero_size_count
        );

        test_success3 = EGL_FALSE;
    }


    if (sentinel_config !=
        (EGLConfig)(uintptr_t)0x1)
    {
        TEST_LOG_FAIL(
            test_case3,
            test_procedure,
            "configs memory was modified even though "
            "config_size == 0"
        );

        test_success3 = EGL_FALSE;
    }


    if (test_success3)
    {
        TEST_LOG_SUCCESS(
            test_case3,
            test_procedure
        );
    }


    TEST_LOG_INFO(
        "Reference EGL_CONFIG_ID: %d",
        reference_config_id
    );
}

void GS_EGL10_CM_CC_TP_001_draw(void) {

}

void GS_EGL10_CM_CC_TP_001_close(void)
{
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;
}