#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - ConfigurationManagement - ChooseConfig

The eglChooseConfig function shall return EGL_TRUE and set
num_config to zero when no EGLConfig matches attrib_list.

Absence of a matching EGLConfig is not a function failure.

Covered requirements:
    - GS-EGL10-CM-CC-021
*/

static const char* test_case =
    "GS_EGL10_CM_CC_TC_021";

static const char* test_procedure =
    "GS_EGL10_CM_CC_TP_008";

static EGLBoolean test_success = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLBoolean initialized = EGL_FALSE;


/* Find an EGL_CONFIG_ID which is not used by any of the available EGLConfigs. */
static EGLBoolean find_unused_config_id(
    EGLConfig *configs,
    EGLint config_count,
    EGLint *unused_id)
{
    /* EGL_CONFIG_ID values are unique.
     *
     * There are config_count existing configurations.
     * Therefore, among config_count + 1 candidate integer
     * values, at least one value must be unused.
     */
    for (EGLint candidate = 1;
         candidate <= config_count + 1;
         ++candidate)
    {
        EGLBoolean used = EGL_FALSE;

        for (EGLint i = 0; i < config_count; ++i)
        {
            EGLint config_id = -1;

            if (eglGetConfigAttrib(
                    display,
                    configs[i],
                    EGL_CONFIG_ID,
                    &config_id) != EGL_TRUE)
            {
                return EGL_FALSE;
            }

            if (config_id == candidate)
            {
                used = EGL_TRUE;
                break;
            }
        }

        if (used == EGL_FALSE)
        {
            *unused_id = candidate;
            return EGL_TRUE;
        }
    }

    return EGL_FALSE;
}


/* Initialization */
void GS_EGL10_CM_CC_TP_008_init(void)
{
    EGLBoolean result;
    EGLint error;

    EGLint config_count = 0;
    EGLint returned_count = 0;

    EGLConfig *configs = NULL;

    EGLint unused_config_id = -1;


    test_success = EGL_TRUE;


    /* TEST PRECONDITION
     * Obtain and initialize an EGLDisplay.
     */
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (display == EGL_NO_DISPLAY)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: eglGetDisplay returned "
            "EGL_NO_DISPLAY. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
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
            test_case,
            test_procedure,
            "Test precondition failed: eglInitialize returned "
            "EGL_FALSE. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
        return;
    }

    initialized = EGL_TRUE;


    /* Obtain all available EGLConfigs. */
    result = eglGetConfigs(
        display,
        NULL,
        0,
        &config_count
    );

    if (result != EGL_TRUE ||
        config_count <= 0)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: no EGLConfigs available. "
            "Count: %d, eglGetError(): 0x%x",
            config_count,
            error
        );

        test_success = EGL_FALSE;
        return;
    }


    configs = (EGLConfig*)malloc(
        sizeof(EGLConfig) * (size_t)config_count
    );

    if (configs == NULL)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: unable to allocate "
            "EGLConfig array"
        );

        test_success = EGL_FALSE;
        return;
    }


    result = eglGetConfigs(
        display,
        configs,
        config_count,
        &returned_count
    );

    if (result != EGL_TRUE ||
        returned_count <= 0)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: eglGetConfigs could not "
            "return EGLConfig handles. eglGetError(): 0x%x",
            error
        );

        free(configs);

        test_success = EGL_FALSE;
        return;
    }


    /* Find a configuration ID which does not exist. */
    if (find_unused_config_id(
            configs,
            returned_count,
            &unused_config_id) != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: unable to determine "
            "an unused EGL_CONFIG_ID. eglGetError(): 0x%x",
            error
        );

        free(configs);

        test_success = EGL_FALSE;
        return;
    }


    free(configs);


    // TEST CASE 013

    /* Request an EGL_CONFIG_ID which does not correspond to
     * any available EGLConfig.
     * Expected:
     *
     *     eglChooseConfig -> EGL_TRUE
     *     num_config      -> 0
     *     EGL error       -> EGL_SUCCESS
     */
    const EGLint attrib_list[] =
    {
        EGL_CONFIG_ID, unused_config_id,
        EGL_NONE
    };


    EGLint num_config = -1;


    /* Clear any previous EGL error. */
    (void)eglGetError();


    result = eglChooseConfig(
        display,
        attrib_list,
        NULL,
        0,
        &num_config
    );


    error = eglGetError();


    /* No matching configuration is NOT a function failure. */
    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Expected EGL_TRUE when no EGLConfig matched, "
            "got EGL_FALSE. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }


    /* No configuration matches the unused EGL_CONFIG_ID. */
    if (num_config != 0)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Expected num_config == 0 for unused "
            "EGL_CONFIG_ID %d, got: %d",
            unused_config_id,
            num_config
        );

        test_success = EGL_FALSE;
    }


    /* The absence of a match shall not generate an EGL error. */
    if (error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "No EGL error expected when no configuration "
            "matches. Got: 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }


    if (test_success)
    {
        TEST_LOG_INFO(
            "No EGLConfig matched unused EGL_CONFIG_ID %d. "
            "eglChooseConfig returned EGL_TRUE and "
            "num_config was set to 0",
            unused_config_id
        );

        TEST_LOG_SUCCESS(
            test_case,
            test_procedure
        );
    }
}


void GS_EGL10_CM_CC_TP_008_draw(void) {

}

void GS_EGL10_CM_CC_TP_008_close(void)
{
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;
}
