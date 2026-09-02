#include <stdio.h>
#include <EGL/egl.h>
#include "macros.h"

/*
EGL10 - ConfigurationManagement - ChooseConfig

Covered requirements:
    - GS-EGL10-CM-CC-015
    - GS-EGL10-CM-CC-016
    - GS-EGL10-CM-CC-017
    - GS-EGL10-CM-CC-018

TC_015:
    eglChooseConfig shall return EGL_FALSE and generate
    EGL_BAD_ATTRIBUTE when attrib_list contains an undefined
    attribute or an unrecognized/out-of-range attribute value.

TC_016:
    eglChooseConfig shall return EGL_FALSE and generate
    EGL_NOT_INITIALIZED when EGL is not initialized on dpy.

TC_017:
    eglChooseConfig shall return EGL_FALSE and generate
    EGL_BAD_PARAMETER when num_config is NULL.

TC_018:
    eglChooseConfig shall return EGL_FALSE and generate
    EGL_BAD_DISPLAY when dpy is invalid.
*/

static const char* test_case1 =
    "GS_EGL10_CM_CC_TC_015";

static const char* test_case2 =
    "GS_EGL10_CM_CC_TC_016";

static const char* test_case3 =
    "GS_EGL10_CM_CC_TC_017";

static const char* test_case4 =
    "GS_EGL10_CM_CC_TC_018";

static const char* test_procedure =
    "GS_EGL10_CM_CC_TP_010";


static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;
static EGLBoolean test_success3 = EGL_TRUE;
static EGLBoolean test_success4 = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLBoolean initialized = EGL_FALSE;


/* Initialization */
void GS_EGL10_CM_CC_TP_010_init(void)
{
    EGLBoolean result;
    EGLint error;
    EGLint num_config;


    test_success1 = EGL_TRUE;
    test_success2 = EGL_TRUE;
    test_success3 = EGL_TRUE;
    test_success4 = EGL_TRUE;


    /* TEST PRECONDITION
     * Obtain a valid EGLDisplay.
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


    /* Initialize and then terminate the display deliberately.
     *
     * This leaves us with a valid EGLDisplay for which EGL
     * is not initialized. That state is required by TC_016.
     */
    result = eglInitialize(
        display,
        NULL,
        NULL
    );

    if (result != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case2,
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


    result = eglTerminate(display);

    if (result != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Test precondition failed: eglTerminate returned "
            "EGL_FALSE. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;
        test_success3 = EGL_FALSE;
        test_success4 = EGL_FALSE;

        return;
    }


    initialized = EGL_FALSE;


    // TEST CASE 016

    /* EGLDisplay is valid, but EGL is not initialized on it.
     *
     * Expected:
     *
     *     eglChooseConfig -> EGL_FALSE
     *     eglGetError     -> EGL_NOT_INITIALIZED
     */
    num_config = -1;

    (void)eglGetError();


    result = eglChooseConfig(
        display,
        NULL,
        NULL,
        0,
        &num_config
    );


    error = eglGetError();


    if (result != EGL_FALSE)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Expected EGL_FALSE for an uninitialized "
            "EGLDisplay"
        );

        test_success2 = EGL_FALSE;
    }


    if (error != EGL_NOT_INITIALIZED)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Expected EGL_NOT_INITIALIZED, got: 0x%x",
            error
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


    /* Reinitialize the valid display.
     * TC_015 and TC_017 require an initialized EGLDisplay so
     * that the intended parameter error is isolated from
     * EGL_NOT_INITIALIZED.
     */

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
            "Test precondition failed: display could not be "
            "reinitialized. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success3 = EGL_FALSE;
        test_success4 = EGL_FALSE;

        return;
    }


    initialized = EGL_TRUE;


    // TEST CASE 015

    /* Subcase A:
     * attrib_list contains an undefined EGL attribute.
     *
     * Expected:
     *
     *     eglChooseConfig -> EGL_FALSE
     *     eglGetError     -> EGL_BAD_ATTRIBUTE
     */

    const EGLint undefined_attribute_list[] =
    {
        /* Deliberately undefined EGL attribute. */
        0x7fffffff, 0,

        EGL_NONE
    };


    num_config = -1;

    (void)eglGetError();


    result = eglChooseConfig(
        display,
        undefined_attribute_list,
        NULL,
        0,
        &num_config
    );


    error = eglGetError();


    if (result != EGL_FALSE)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Expected EGL_FALSE for an undefined "
            "EGL attribute"
        );

        test_success1 = EGL_FALSE;
    }


    if (error != EGL_BAD_ATTRIBUTE)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Expected EGL_BAD_ATTRIBUTE for undefined "
            "attribute, got: 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
    }


    /* TC_015 - Subcase B
     * Use a valid EGL attribute with an invalid /
     * unrecognized value.
     * EGL_CONFIG_CAVEAT accepts defined caveat values.
     * 0x7fffffff is deliberately outside that value set.
     * Expected:
     *
     *     eglChooseConfig -> EGL_FALSE
     *     eglGetError     -> EGL_BAD_ATTRIBUTE
     */

    const EGLint invalid_value_list[] =
    {
        EGL_CONFIG_CAVEAT, 0x7fffffff,
        EGL_NONE
    };


    num_config = -1;

    (void)eglGetError();


    result = eglChooseConfig(
        display,
        invalid_value_list,
        NULL,
        0,
        &num_config
    );


    error = eglGetError();


    if (result != EGL_FALSE)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Expected EGL_FALSE for an invalid "
            "EGL_CONFIG_CAVEAT value"
        );

        test_success1 = EGL_FALSE;
    }


    if (error != EGL_BAD_ATTRIBUTE)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Expected EGL_BAD_ATTRIBUTE for invalid "
            "attribute value, got: 0x%x",
            error
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


    // TEST CASE 017

    /* num_config is NULL.
     * Expected:
     *
     *     eglChooseConfig -> EGL_FALSE
     *     eglGetError     -> EGL_BAD_PARAMETER
     */
    (void)eglGetError();


    result = eglChooseConfig(
        display,
        NULL,
        NULL,
        0,
        NULL
    );


    error = eglGetError();


    if (result != EGL_FALSE)
    {
        TEST_LOG_FAIL(
            test_case3,
            test_procedure,
            "Expected EGL_FALSE when num_config was NULL"
        );

        test_success3 = EGL_FALSE;
    }


    if (error != EGL_BAD_PARAMETER)
    {
        TEST_LOG_FAIL(
            test_case3,
            test_procedure,
            "Expected EGL_BAD_PARAMETER when num_config "
            "was NULL, got: 0x%x",
            error
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


    // TEST CASE 018

    /* EGL_NO_DISPLAY is not a valid EGLDisplay for
     * eglChooseConfig.
     * Expected:
     *
     *     eglChooseConfig -> EGL_FALSE
     *     eglGetError     -> EGL_BAD_DISPLAY
    */

    num_config = -1;

    (void)eglGetError();


    result = eglChooseConfig(
        EGL_NO_DISPLAY,
        NULL,
        NULL,
        0,
        &num_config
    );


    error = eglGetError();


    if (result != EGL_FALSE)
    {
        TEST_LOG_FAIL(
            test_case4,
            test_procedure,
            "Expected EGL_FALSE when dpy was "
            "EGL_NO_DISPLAY"
        );

        test_success4 = EGL_FALSE;
    }


    if (error != EGL_BAD_DISPLAY)
    {
        TEST_LOG_FAIL(
            test_case4,
            test_procedure,
            "Expected EGL_BAD_DISPLAY for invalid dpy, "
            "got: 0x%x",
            error
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
}

void GS_EGL10_CM_CC_TP_010_draw(void) {

}

void GS_EGL10_CM_CC_TP_010_close(void)
{
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;
}