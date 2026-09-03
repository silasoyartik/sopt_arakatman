#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include "macros.h"

/*
EGL10 - ConfigurationManagement - ChooseConfig

The eglChooseConfig function shall ignore
EGL_TRANSPARENT_RED_VALUE,
EGL_TRANSPARENT_GREEN_VALUE, and
EGL_TRANSPARENT_BLUE_VALUE
when EGL_TRANSPARENT_TYPE is requested as EGL_NONE.

Covered requirements:
    - GS-EGL10-CM-CC-012
*/

static const char* test_case =
    "GS_EGL10_CM_CC_TC_012";

static const char* test_procedure =
    "GS_EGL10_CM_CC_TP_007";

static EGLBoolean test_success = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLBoolean initialized = EGL_FALSE;


/* Obtain only the number of configurations matching the supplied attribute list. */
static EGLBoolean get_matching_count(
    const EGLint *attrib_list,
    EGLint *count,
    EGLint *error)
{
    EGLBoolean result;

    *count = -1;

    (void)eglGetError();

    result = eglChooseConfig(
        display,
        attrib_list,
        NULL,
        0,
        count
    );

    *error = eglGetError();

    return result;
}


/* Initialization */
void GS_EGL10_CM_CC_TP_007_init(void)
{
    EGLBoolean result;
    EGLint error;

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


    /* BASELINE
     * Select configurations for which transparent type is
     * EGL_NONE.
     * At least one bit is requested for each RGB component so
     * that 0 and 1 can be used as valid distinguishable
     * transparent color values during the test.
     */

    const EGLint baseline_attribs[] =
    {
        EGL_TRANSPARENT_TYPE, EGL_NONE,

        EGL_RED_SIZE,   1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE,  1,

        EGL_NONE
    };


    EGLint baseline_count = -1;
    EGLint baseline_error;


    result = get_matching_count(
        baseline_attribs,
        &baseline_count,
        &baseline_error
    );


    if (result != EGL_TRUE ||
        baseline_count <= 0)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: no suitable EGLConfig "
            "with EGL_TRANSPARENT_TYPE == EGL_NONE was found. "
            "Count: %d, eglGetError(): 0x%x",
            baseline_count,
            baseline_error
        );

        test_success = EGL_FALSE;
        return;
    }


    /* Obtain one known matching configuration. */
    EGLConfig reference_config = (EGLConfig)0;
    EGLint returned_count = 0;


    result = eglChooseConfig(
        display,
        baseline_attribs,
        &reference_config,
        1,
        &returned_count
    );


    if (result != EGL_TRUE ||
        returned_count != 1 ||
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


    /* Obtain the actual transparent RGB values of the reference configuration. */

    EGLint reference_red = -1;
    EGLint reference_green = -1;
    EGLint reference_blue = -1;


    if (eglGetConfigAttrib(
            display,
            reference_config,
            EGL_TRANSPARENT_RED_VALUE,
            &reference_red) != EGL_TRUE ||
        eglGetConfigAttrib(
            display,
            reference_config,
            EGL_TRANSPARENT_GREEN_VALUE,
            &reference_green) != EGL_TRUE ||
        eglGetConfigAttrib(
            display,
            reference_config,
            EGL_TRANSPARENT_BLUE_VALUE,
            &reference_blue) != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: transparent RGB "
            "attributes could not be queried. "
            "eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
        return;
    }


    /* Construct values deliberately different from the
     * reference configuration.
     *
     * Since each requested RGB component size is at least
     * one bit, both 0 and 1 are representable values.
     */
    EGLint test_red =
        (reference_red == 0) ? 1 : 0;

    EGLint test_green =
        (reference_green == 0) ? 1 : 0;

    EGLint test_blue =
        (reference_blue == 0) ? 1 : 0;


    // TEST CASE 012

     /* EGL_TRANSPARENT_TYPE is EGL_NONE.
     *
     * Therefore the three transparent RGB attributes below
     * shall not participate in configuration selection.
     */

    const EGLint test_attribs[] =
    {
        EGL_TRANSPARENT_TYPE, EGL_NONE,

        EGL_RED_SIZE,   1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE,  1,

        /* These values deliberately differ from the values
         * reported by the reference configuration.
         *
         * They shall be ignored because
         * EGL_TRANSPARENT_TYPE == EGL_NONE.
         */
        EGL_TRANSPARENT_RED_VALUE,   test_red,
        EGL_TRANSPARENT_GREEN_VALUE, test_green,
        EGL_TRANSPARENT_BLUE_VALUE,  test_blue,

        EGL_NONE
    };


    EGLint test_count = -1;
    EGLint test_error;


    result = get_matching_count(
        test_attribs,
        &test_count,
        &test_error
    );


    /* The call itself shall succeed. */
    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "eglChooseConfig failed while transparent RGB "
            "attributes should have been ignored. "
            "eglGetError(): 0x%x",
            test_error
        );

        test_success = EGL_FALSE;
    }


    /* Since the transparent RGB values shall be ignored,
     * adding them shall not change the number of matching
     * configurations.
     */
    if (test_count != baseline_count)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Transparent RGB values affected configuration "
            "selection although EGL_TRANSPARENT_TYPE was "
            "EGL_NONE. Baseline: %d, result: %d",
            baseline_count,
            test_count
        );

        test_success = EGL_FALSE;
    }


    /* Verify that the deliberately supplied values really do
     * differ from the reference configuration.
     *
     * This ensures that the test stimulus is distinguishable.
     */
    if (test_red == reference_red ||
        test_green == reference_green ||
        test_blue == reference_blue)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test stimulus is not distinguishable from the "
            "reference transparent RGB values"
        );

        test_success = EGL_FALSE;
    }


    if (test_success)
    {
        TEST_LOG_INFO(
            "Transparent RGB attributes were correctly ignored. "
            "Reference RGB: (%d, %d, %d), "
            "requested RGB: (%d, %d, %d), "
            "matching configs: %d",
            reference_red,
            reference_green,
            reference_blue,
            test_red,
            test_green,
            test_blue,
            test_count
        );

        TEST_LOG_SUCCESS(
            test_case,
            test_procedure
        );
    }
}


void GS_EGL10_CM_CC_TP_007_draw(void) {

}

void GS_EGL10_CM_CC_TP_007_close(void)
{
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;
}