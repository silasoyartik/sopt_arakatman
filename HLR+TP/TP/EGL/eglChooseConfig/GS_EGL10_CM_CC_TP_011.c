#include <stdio.h>
#include <EGL/egl.h>
#include "macros.h"

/*
EGL10 - ConfigurationManagement - ChooseConfig

Configuration handles returned by eglChooseConfig shall remain
valid as long as the EGLDisplay from which they were obtained
has not been terminated.

Covered requirements:
    - GS-EGL10-CM-CC-019
*/

static const char* test_case =
    "GS_EGL10_CM_CC_TC_019";

static const char* test_procedure =
    "GS_EGL10_CM_CC_TP_011";

static EGLBoolean test_success = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLBoolean initialized = EGL_FALSE;


/* Initialization */
void GS_EGL10_CM_CC_TP_011_init(void)
{
    EGLBoolean result;
    EGLint error;

    EGLConfig config = (EGLConfig)0;
    EGLint num_config = 0;

    EGLint original_config_id = -1;
    EGLint queried_config_id = -1;

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


    // TEST CASE 019

    /* Obtain an EGLConfig handle using eglChooseConfig.
     * The handle shall remain valid until the EGLDisplay is terminated.
     */
    (void)eglGetError();


    result = eglChooseConfig(
        display,
        NULL,
        &config,
        1,
        &num_config
    );


    error = eglGetError();


    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "eglChooseConfig returned EGL_FALSE. "
            "eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
        return;
    }


    if (num_config != 1 ||
        config == (EGLConfig)0)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: no usable EGLConfig "
            "was returned. num_config: %d",
            num_config
        );

        test_success = EGL_FALSE;
        return;
    }


    /* STEP 1
     * Use the returned EGLConfig immediately.
     * Obtain its EGL_CONFIG_ID. This value will be used to
     * verify that the same handle remains usable later.
     */

    (void)eglGetError();


    result = eglGetConfigAttrib(
        display,
        config,
        EGL_CONFIG_ID,
        &original_config_id
    );


    error = eglGetError();


    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Returned EGLConfig was not valid immediately "
            "after eglChooseConfig. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
        return;
    }


    TEST_LOG_INFO(
        "EGLConfig obtained. EGL_CONFIG_ID: %d",
        original_config_id
    );


    /* STEP 2
     * Perform additional EGL operations while keeping the
     * original EGLConfig handle.
     * These operations must not invalidate the previously
     * returned configuration handle.
     */

    EGLConfig another_config = (EGLConfig)0;
    EGLint another_count = 0;


    result = eglChooseConfig(
        display,
        NULL,
        &another_config,
        1,
        &another_count
    );


    if (result != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Intermediate eglChooseConfig call failed. "
            "eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
        return;
    }


    /* STEP 3
     * Use the ORIGINAL EGLConfig handle again.
     * The display has not been terminated, therefore the
     * original handle shall still be valid.
     */

    queried_config_id = -1;

    (void)eglGetError();


    result = eglGetConfigAttrib(
        display,
        config,
        EGL_CONFIG_ID,
        &queried_config_id
    );


    error = eglGetError();


    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Previously returned EGLConfig became invalid "
            "before eglTerminate. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }
    else if (queried_config_id != original_config_id)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "EGLConfig did not preserve its configuration "
            "identity. Original ID: %d, queried ID: %d",
            original_config_id,
            queried_config_id
        );

        test_success = EGL_FALSE;
    }


    /* STEP 4
     * Query several additional attributes through the same
     * original EGLConfig handle.
     * This provides further evidence that the configuration
     * handle remains usable while the EGLDisplay remains
     * initialized.
     */

    EGLint buffer_size = -1;
    EGLint red_size = -1;
    EGLint surface_type = -1;


    if (eglGetConfigAttrib(
            display,
            config,
            EGL_BUFFER_SIZE,
            &buffer_size) != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Original EGLConfig could not be used to query "
            "EGL_BUFFER_SIZE before termination. "
            "eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }


    if (eglGetConfigAttrib(
            display,
            config,
            EGL_RED_SIZE,
            &red_size) != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Original EGLConfig could not be used to query "
            "EGL_RED_SIZE before termination. "
            "eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }


    if (eglGetConfigAttrib(
            display,
            config,
            EGL_SURFACE_TYPE,
            &surface_type) != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Original EGLConfig could not be used to query "
            "EGL_SURFACE_TYPE before termination. "
            "eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }


    /* STEP 5
     * Final validity check immediately before eglTerminate.
     */

    queried_config_id = -1;

    (void)eglGetError();


    result = eglGetConfigAttrib(
        display,
        config,
        EGL_CONFIG_ID,
        &queried_config_id
    );


    error = eglGetError();


    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "EGLConfig was not valid immediately before "
            "eglTerminate. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }
    else if (queried_config_id != original_config_id)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "EGLConfig identity changed before termination. "
            "Expected ID: %d, got: %d",
            original_config_id,
            queried_config_id
        );

        test_success = EGL_FALSE;
    }


    /* Report TC result before terminating the display.*/
    if (test_success)
    {
        TEST_LOG_INFO(
            "EGLConfig ID %d remained valid while its "
            "EGLDisplay was initialized",
            original_config_id
        );

        TEST_LOG_INFO(
            "Queried attributes: "
            "BUFFER_SIZE=%d, RED_SIZE=%d, SURFACE_TYPE=0x%x",
            buffer_size,
            red_size,
            surface_type
        );

        TEST_LOG_SUCCESS(
            test_case,
            test_procedure
        );
    }


    /* LIFETIME BOUNDARY
     * Terminate the EGLDisplay.
     * CC-019 guarantees validity only while the display has
     * not been terminated. Therefore this TP deliberately does
     * NOT require any particular behavior from the EGLConfig
     * handle after eglTerminate.
     */

    result = eglTerminate(display);


    if (result != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "eglTerminate failed while completing the test. "
            "eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }
    else
    {
        initialized = EGL_FALSE;
    }
}


void GS_EGL10_CM_CC_TP_011_draw(void) {

}

void GS_EGL10_CM_CC_TP_011_close(void)
{
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;
}