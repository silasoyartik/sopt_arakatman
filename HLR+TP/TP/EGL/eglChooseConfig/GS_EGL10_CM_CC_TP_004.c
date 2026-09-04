#include <stdio.h>
#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - ConfigurationManagement - ChooseConfig

The eglChooseConfig function shall ignore all attributes other
than EGL_CONFIG_ID when EGL_CONFIG_ID is specified and shall
return only the EGLConfig having the requested ID when it exists.

Covered requirements:
    - GS-EGL10-CM-CC-017
*/

static const char* test_case =
    "GS_EGL10_CM_CC_TC_017";

static const char* test_procedure =
    "GS_EGL10_CM_CC_TP_004";

static EGLBoolean test_success = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLBoolean initialized = EGL_FALSE;


/* Initialization */
void GS_EGL10_CM_CC_TP_004_init(void)
{
    EGLBoolean result;
    EGLint error;

    EGLint major = -1;
    EGLint minor = -1;

    EGLConfig reference_config = (EGLConfig)0;
    EGLConfig returned_config = (EGLConfig)0;

    EGLint num_config = 0;

    EGLint reference_config_id = -1;
    EGLint reference_caveat = -1;
    EGLint conflicting_caveat;


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
        &major,
        &minor
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


    /* Obtain one known valid EGLConfig. */

    result = eglGetConfigs(
        display,
        &reference_config,
        1,
        &num_config
    );

    if (result != EGL_TRUE ||
        num_config != 1 ||
        reference_config == (EGLConfig)0)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: unable to obtain "
            "a reference EGLConfig. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
        return;
    }


    /* Obtain the unique EGL_CONFIG_ID of the reference config. */
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
            test_case,
            test_procedure,
            "Test precondition failed: unable to query "
            "EGL_CONFIG_ID. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
        return;
    }


    /* Obtain EGL_CONFIG_CAVEAT.
     * We will deliberately request a different valid caveat
     * value so that it conflicts with the reference config.
     */
    result = eglGetConfigAttrib(
        display,
        reference_config,
        EGL_CONFIG_CAVEAT,
        &reference_caveat
    );

    if (result != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: unable to query "
            "EGL_CONFIG_CAVEAT. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
        return;
    }


    /* Select a valid EGL_CONFIG_CAVEAT value that is different
     * from the reference config's actual value.
     */
    if (reference_caveat != EGL_NONE)
    {
        conflicting_caveat = EGL_NONE;
    }
    else
    {
        conflicting_caveat = EGL_SLOW_CONFIG;
    }


    // TEST CASE 009

    /* The EGL_CONFIG_CAVEAT request deliberately conflicts
     * with the reference configuration.
     *
     * Because EGL_CONFIG_ID is also specified, the
     * EGL_CONFIG_CAVEAT attribute shall be ignored.
     */
    const EGLint attrib_list[] =
    {
        EGL_CONFIG_ID, reference_config_id,

        /* Deliberately conflicting attribute.
         * This shall be ignored because EGL_CONFIG_ID is specified.
         */
        EGL_CONFIG_CAVEAT, conflicting_caveat,

        EGL_NONE
    };


    num_config = -1;
    returned_config = (EGLConfig)0;

    (void)eglGetError();


    result = eglChooseConfig(
        display,
        attrib_list,
        &returned_config,
        1,
        &num_config
    );


    error = eglGetError();


    /* The function shall succeed. */
    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "eglChooseConfig returned EGL_FALSE when "
            "EGL_CONFIG_ID was specified. "
            "eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }


    /* EGL_CONFIG_ID is unique, therefore exactly one configuration shall be returned. */
    if (num_config != 1)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Expected exactly one EGLConfig for "
            "EGL_CONFIG_ID %d, got: %d",
            reference_config_id,
            num_config
        );

        test_success = EGL_FALSE;
    }


    if (returned_config == (EGLConfig)0)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "eglChooseConfig did not return an EGLConfig "
            "for the requested EGL_CONFIG_ID"
        );

        test_success = EGL_FALSE;
    }


    /* Verify the ID of the returned configuration. */
    if (returned_config != (EGLConfig)0)
    {
        EGLint returned_config_id = -1;


        result = eglGetConfigAttrib(
            display,
            returned_config,
            EGL_CONFIG_ID,
            &returned_config_id
        );


        if (result != EGL_TRUE)
        {
            error = eglGetError();

            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "Unable to query EGL_CONFIG_ID of the "
                "returned EGLConfig. eglGetError(): 0x%x",
                error
            );

            test_success = EGL_FALSE;
        }
        else if (returned_config_id != reference_config_id)
        {
            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "eglChooseConfig returned the wrong config. "
                "Expected EGL_CONFIG_ID: %d, got: %d",
                reference_config_id,
                returned_config_id
            );

            test_success = EGL_FALSE;
        }
    }


    /* Verify that the deliberately conflicting attribute was
     * actually different from the returned configuration.
     *
     * This proves that the configuration was returned despite
     * the conflicting EGL_CONFIG_CAVEAT request.
     */
    if (returned_config != (EGLConfig)0)
    {
        EGLint returned_caveat = -1;


        result = eglGetConfigAttrib(
            display,
            returned_config,
            EGL_CONFIG_CAVEAT,
            &returned_caveat
        );


        if (result != EGL_TRUE)
        {
            error = eglGetError();

            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "Unable to query EGL_CONFIG_CAVEAT of the "
                "returned EGLConfig. eglGetError(): 0x%x",
                error
            );

            test_success = EGL_FALSE;
        }
        else if (returned_caveat == conflicting_caveat)
        {
            /* This should not normally occur because the
             * conflicting value was selected specifically to
             * differ from the reference configuration.
             */
            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "Test precondition invalid: conflicting "
                "EGL_CONFIG_CAVEAT unexpectedly matches "
                "the returned configuration"
            );

            test_success = EGL_FALSE;
        }
    }


    if (test_success)
    {
        TEST_LOG_INFO(
            "EGL_CONFIG_ID %d returned its configuration "
            "while conflicting EGL_CONFIG_CAVEAT 0x%x "
            "was ignored",
            reference_config_id,
            conflicting_caveat
        );

        TEST_LOG_SUCCESS(
            test_case,
            test_procedure
        );
    }
}


void GS_EGL10_CM_CC_TP_004_draw(void) {

}

void GS_EGL10_CM_CC_TP_004_close(void)
{
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;
}
