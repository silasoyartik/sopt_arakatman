#include <stdio.h>
#include <EGL/egl.h>
#include "macros.h"

/*
EGL10 - ConfigurationManagement - ChooseConfig

The eglChooseConfig function shall interpret attrib_list as
attribute/value pairs terminated by EGL_NONE.

The eglChooseConfig function shall apply the EGL 1.0 default
selection values when attrib_list is NULL, when its first
attribute is EGL_NONE, or for each attribute omitted from
the list.

Covered requirements:
    - GS-EGL10-CM-CC-005
    - GS-EGL10-CM-CC-006
*/

static const char* test_case1 =
    "GS_EGL10_CM_CC_TC_005";

static const char* test_case2 =
    "GS_EGL10_CM_CC_TC_006";

static const char* test_procedure =
    "GS_EGL10_CM_CC_TP_002";


static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLBoolean initialized = EGL_FALSE;


/* Helper function used only to obtain the number of configurations matching the supplied attribute list. */
static EGLBoolean get_matching_config_count(
    const EGLint *attrib_list,
    EGLint *count,
    EGLint *error)
{
    EGLBoolean result;

    *count = -1;

    /* Clear any previous EGL error. */
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
void GS_EGL10_CM_CC_TP_002_init(void)
{
    EGLBoolean result;
    EGLint error;


    /* Reset test state. */
    test_success1 = EGL_TRUE;
    test_success2 = EGL_TRUE;


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

        return;
    }

    initialized = EGL_TRUE;


    // TEST CASE 005
  
    /* attrib_list shall consist of attribute/value pairs
     * terminated by EGL_NONE.
     *
     * The second list contains deliberately invalid values
     * after EGL_NONE. They shall not be interpreted because
     * EGL_NONE terminates the attribute list.
     */
    const EGLint clean_attrib_list[] =
    {
        EGL_RED_SIZE, 0,
        EGL_NONE
    };


    const EGLint terminated_attrib_list[] =
    {
        EGL_RED_SIZE, 0,
        EGL_NONE,

        /* Values after EGL_NONE shall not be processed. */
        0x7fffffff,
        0x7fffffff
    };


    EGLint clean_count = -1;
    EGLint terminated_count = -1;

    EGLint clean_error;
    EGLint terminated_error;

    EGLBoolean clean_result;
    EGLBoolean terminated_result;


    clean_result =
        get_matching_config_count(
            clean_attrib_list,
            &clean_count,
            &clean_error
        );


    terminated_result =
        get_matching_config_count(
            terminated_attrib_list,
            &terminated_count,
            &terminated_error
        );


    if (clean_result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "eglChooseConfig failed for a valid attribute/value "
            "list. eglGetError(): 0x%x",
            clean_error
        );

        test_success1 = EGL_FALSE;
    }


    if (terminated_result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "eglChooseConfig processed data after EGL_NONE. "
            "eglGetError(): 0x%x",
            terminated_error
        );

        test_success1 = EGL_FALSE;
    }


    /* Since both lists are identical up to EGL_NONE, they shall produce the same matching configuration count. */
    if (clean_result == EGL_TRUE &&
        terminated_result == EGL_TRUE &&
        clean_count != terminated_count)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Attribute data after EGL_NONE affected the result. "
            "Clean count: %d, terminated-list count: %d",
            clean_count,
            terminated_count
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


    // TEST CASE 006

    /* Verify the default-selection behavior using:
     *   1. attrib_list == NULL
     *   2. first attribute == EGL_NONE
     *   3. EGL_RED_SIZE explicitly set to its default value 0
     *   4. EGL_SURFACE_TYPE explicitly set to its default
     *      value EGL_WINDOW_BIT
     *
     * Each form shall be equivalent with respect to the
     * matching configuration count.
     */

    const EGLint empty_attrib_list[] =
    {
        EGL_NONE
    };


    /* EGL_RED_SIZE has default value 0 in EGL 1.0 Table 3.3.
     * All omitted attributes shall implicitly use their
     * corresponding default values.
     */
    const EGLint explicit_red_default[] =
    {
        EGL_RED_SIZE, 0,
        EGL_NONE
    };


    /* EGL_SURFACE_TYPE has default value EGL_WINDOW_BIT in EGL 1.0 Table 3.3. */
    const EGLint explicit_surface_default[] =
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };


    EGLint null_count = -1;
    EGLint empty_count = -1;
    EGLint red_default_count = -1;
    EGLint surface_default_count = -1;

    EGLint null_error;
    EGLint empty_error;
    EGLint red_error;
    EGLint surface_error;

    EGLBoolean null_result;
    EGLBoolean empty_result;
    EGLBoolean red_result;
    EGLBoolean surface_result;


    /* Case 1:
     * attrib_list == NULL
     */
    null_result =
        get_matching_config_count(
            NULL,
            &null_count,
            &null_error
        );


    if (null_result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "eglChooseConfig failed when attrib_list was NULL. "
            "eglGetError(): 0x%x",
            null_error
        );

        test_success2 = EGL_FALSE;
    }


    /* Case 2:
     * The first attribute is EGL_NONE.
     */
    empty_result =
        get_matching_config_count(
            empty_attrib_list,
            &empty_count,
            &empty_error
        );


    if (empty_result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "eglChooseConfig failed when the first attribute "
            "was EGL_NONE. eglGetError(): 0x%x",
            empty_error
        );

        test_success2 = EGL_FALSE;
    }


    /* NULL and an empty attribute list shall use the same default selection criteria. */
    if (null_result == EGL_TRUE &&
        empty_result == EGL_TRUE &&
        null_count != empty_count)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "NULL and empty attribute lists produced different "
            "matching counts. NULL: %d, EGL_NONE: %d",
            null_count,
            empty_count
        );

        test_success2 = EGL_FALSE;
    }


    /* Case 3:
     * Explicitly specify the default EGL_RED_SIZE value.
     *
     * Since EGL_RED_SIZE would otherwise be omitted and
     * implicitly receive the same default value, the result
     * shall be equivalent to the default query.
     */
    red_result =
        get_matching_config_count(
            explicit_red_default,
            &red_default_count,
            &red_error
        );


    if (red_result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "eglChooseConfig failed when EGL_RED_SIZE was "
            "specified with its default value. "
            "eglGetError(): 0x%x",
            red_error
        );

        test_success2 = EGL_FALSE;
    }


    if (null_result == EGL_TRUE &&
        red_result == EGL_TRUE &&
        null_count != red_default_count)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Implicit and explicit EGL_RED_SIZE default values "
            "produced different matching counts. "
            "Implicit: %d, Explicit: %d",
            null_count,
            red_default_count
        );

        test_success2 = EGL_FALSE;
    }


    /* Case 4:
     * Explicitly specify the default EGL_SURFACE_TYPE value.
     */
    surface_result =
        get_matching_config_count(
            explicit_surface_default,
            &surface_default_count,
            &surface_error
        );


    if (surface_result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "eglChooseConfig failed when EGL_SURFACE_TYPE was "
            "specified with its default value. "
            "eglGetError(): 0x%x",
            surface_error
        );

        test_success2 = EGL_FALSE;
    }


    if (null_result == EGL_TRUE &&
        surface_result == EGL_TRUE &&
        null_count != surface_default_count)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Implicit and explicit EGL_SURFACE_TYPE default "
            "values produced different matching counts. "
            "Implicit: %d, Explicit: %d",
            null_count,
            surface_default_count
        );

        test_success2 = EGL_FALSE;
    }


    if (test_success2)
    {
        TEST_LOG_INFO(
            "Default configuration counts: "
            "NULL=%d, EGL_NONE=%d, RED_SIZE=0=%d, "
            "SURFACE_TYPE=EGL_WINDOW_BIT=%d",
            null_count,
            empty_count,
            red_default_count,
            surface_default_count
        );

        TEST_LOG_SUCCESS(
            test_case2,
            test_procedure
        );
    }
}

void GS_EGL10_CM_CC_TP_002_draw(void) {

}

void GS_EGL10_CM_CC_TP_002_close(void)
{
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;
}