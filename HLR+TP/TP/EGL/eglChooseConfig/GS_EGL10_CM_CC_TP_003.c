#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include "macros.h"

/*
EGL10 - ConfigurationManagement - ChooseConfig

The eglChooseConfig function shall not check an attribute whose
requested value is EGL_DONT_CARE, except that EGL_DONT_CARE
shall not be accepted for EGL_LEVEL.

The eglChooseConfig function shall match an EGLConfig only when
each requested attribute satisfies its EGL 1.0 selection criterion.

Selection criteria:
    - Smaller
    - Larger
    - Exact
    - Mask

Covered requirements:
    - GS-EGL10-CM-CC-007
    - GS-EGL10-CM-CC-008
*/

static const char* test_case1 =
    "GS_EGL10_CM_CC_TC_007";

static const char* test_case2 =
    "GS_EGL10_CM_CC_TC_008";

static const char* test_procedure =
    "GS_EGL10_CM_CC_TP_003";


static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLBoolean initialized = EGL_FALSE;


/* Matching modes used internally by this test procedure. */
#define MATCH_AT_LEAST  1
#define MATCH_EXACT     2
#define MATCH_MASK      3


/* Helper:
 * Obtain all configurations matching attrib_list and verify
 * that the selected attribute satisfies the expected matching
 * criterion.
 */
static EGLBoolean verify_selection(
    const EGLint *attrib_list,
    EGLint attribute,
    EGLint requested_value,
    EGLint match_mode,
    const char *criterion_name)
{
    EGLBoolean result;
    EGLint error;

    EGLint total_count = -1;
    EGLint returned_count = -1;

    EGLConfig *configs = NULL;


    /* First query the number of matching configurations. */
    (void)eglGetError();

    result = eglChooseConfig(
        display,
        attrib_list,
        NULL,
        0,
        &total_count
    );

    error = eglGetError();


    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "%s selection count query failed. "
            "eglGetError(): 0x%x",
            criterion_name,
            error
        );

        return EGL_FALSE;
    }


    /* The test input is derived from an existing EGLConfig, therefore at least one matching configuration is expected. */
    if (total_count <= 0)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "%s selection returned no matching configurations",
            criterion_name
        );

        return EGL_FALSE;
    }


    configs = (EGLConfig*)malloc(
        sizeof(EGLConfig) * (size_t)total_count
    );


    if (configs == NULL)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Memory allocation failed while testing %s "
            "selection",
            criterion_name
        );

        return EGL_FALSE;
    }


    /* Retrieve all matching configurations. */
    (void)eglGetError();

    result = eglChooseConfig(
        display,
        attrib_list,
        configs,
        total_count,
        &returned_count
    );

    error = eglGetError();


    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "%s selection failed while retrieving configs. "
            "eglGetError(): 0x%x",
            criterion_name,
            error
        );

        free(configs);
        return EGL_FALSE;
    }


    if (returned_count <= 0 ||
        returned_count > total_count)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Invalid returned configuration count for %s "
            "selection. Expected range: 1..%d, got: %d",
            criterion_name,
            total_count,
            returned_count
        );

        free(configs);
        return EGL_FALSE;
    }


    /* Verify every returned EGLConfig against the requested selection criterion. */
    for (EGLint i = 0; i < returned_count; ++i)
    {
        EGLint actual_value = -1;


        result = eglGetConfigAttrib(
            display,
            configs[i],
            attribute,
            &actual_value
        );


        if (result != EGL_TRUE)
        {
            error = eglGetError();

            TEST_LOG_FAIL(
                test_case2,
                test_procedure,
                "eglGetConfigAttrib failed while checking "
                "%s selection at config index %d. "
                "eglGetError(): 0x%x",
                criterion_name,
                i,
                error
            );

            free(configs);
            return EGL_FALSE;
        }


        /* Smaller and Larger have the same selection test in
         * EGL 1.0: the attribute value must meet or exceed the
         * requested value.
         *
         * Their difference affects sorting, not selection.
         */
        if (match_mode == MATCH_AT_LEAST)
        {
            if (actual_value < requested_value)
            {
                TEST_LOG_FAIL(
                    test_case2,
                    test_procedure,
                    "%s criterion violated at config %d. "
                    "Requested minimum: %d, actual: %d",
                    criterion_name,
                    i,
                    requested_value,
                    actual_value
                );

                free(configs);
                return EGL_FALSE;
            }
        }


        /* Exact:
         * actual value must equal requested value.
         */
        else if (match_mode == MATCH_EXACT)
        {
            if (actual_value != requested_value)
            {
                TEST_LOG_FAIL(
                    test_case2,
                    test_procedure,
                    "%s criterion violated at config %d. "
                    "Expected exactly: %d, actual: %d",
                    criterion_name,
                    i,
                    requested_value,
                    actual_value
                );

                free(configs);
                return EGL_FALSE;
            }
        }


        /* Mask:
         * every requested bit must also be present in the
         * EGLConfig attribute.
         */
        else if (match_mode == MATCH_MASK)
        {
            if ((actual_value & requested_value) !=
                requested_value)
            {
                TEST_LOG_FAIL(
                    test_case2,
                    test_procedure,
                    "%s criterion violated at config %d. "
                    "Requested mask: 0x%x, actual: 0x%x",
                    criterion_name,
                    i,
                    requested_value,
                    actual_value
                );

                free(configs);
                return EGL_FALSE;
            }
        }
    }


    free(configs);

    return EGL_TRUE;
}


/* Initialization */
void GS_EGL10_CM_CC_TP_003_init(void)
{
    EGLBoolean result;
    EGLint error;


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


    /* Obtain one EGLConfig selected using the EGL 1.0 default
     * criteria.
     *
     * The attribute values of this known matching config are
     * later used to construct deterministic selection tests.
     */

    EGLConfig reference_config = (EGLConfig)0;
    EGLint reference_count = 0;


    result = eglChooseConfig(
        display,
        NULL,
        &reference_config,
        1,
        &reference_count
    );


    if (result != EGL_TRUE ||
        reference_count != 1 ||
        reference_config == (EGLConfig)0)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Test precondition failed: unable to obtain "
            "a reference EGLConfig. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;

        return;
    }


    /* Obtain reference values required by TC_008. */
    EGLint red_size = -1;
    EGLint buffer_size = -1;
    EGLint level = -1;
    EGLint surface_type = -1;


    if (eglGetConfigAttrib(
            display,
            reference_config,
            EGL_RED_SIZE,
            &red_size) != EGL_TRUE ||
        eglGetConfigAttrib(
            display,
            reference_config,
            EGL_BUFFER_SIZE,
            &buffer_size) != EGL_TRUE ||
        eglGetConfigAttrib(
            display,
            reference_config,
            EGL_LEVEL,
            &level) != EGL_TRUE ||
        eglGetConfigAttrib(
            display,
            reference_config,
            EGL_SURFACE_TYPE,
            &surface_type) != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Test precondition failed: reference EGLConfig "
            "attributes could not be queried. "
            "eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;

        return;
    }


    // TEST CASE 007

     /* EGL_DONT_CARE shall cause the corresponding attribute to be ignored, except for EGL_LEVEL. */

    /*Case 1:
     * Compare the default selection result with
     * EGL_RED_SIZE = EGL_DONT_CARE.
     *
     * EGL_RED_SIZE shall not be checked when EGL_DONT_CARE
     * is requested.
     */

    EGLint default_count = -1;
    EGLint dont_care_count = -1;


    const EGLint dont_care_attribs[] =
    {
        EGL_RED_SIZE, EGL_DONT_CARE,
        EGL_NONE
    };


    (void)eglGetError();

    result = eglChooseConfig(
        display,
        NULL,
        NULL,
        0,
        &default_count
    );


    if (result != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Default eglChooseConfig query failed. "
            "eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
    }


    (void)eglGetError();

    result = eglChooseConfig(
        display,
        dont_care_attribs,
        NULL,
        0,
        &dont_care_count
    );


    if (result != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "eglChooseConfig rejected EGL_DONT_CARE for "
            "EGL_RED_SIZE. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
    }


    /* EGL_RED_SIZE has default selection value 0.
     *
     * Therefore ignoring EGL_RED_SIZE with EGL_DONT_CARE
     * should produce the same selection result as the
     * default query.
     */
    if (default_count != dont_care_count)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "EGL_DONT_CARE affected EGL_RED_SIZE selection. "
            "Default count: %d, DONT_CARE count: %d",
            default_count,
            dont_care_count
        );

        test_success1 = EGL_FALSE;
    }


    // Case 2:

    /* EGL_DONT_CARE shall NOT be accepted for EGL_LEVEL.  */
    const EGLint invalid_level_attribs[] =
    {
        EGL_LEVEL, EGL_DONT_CARE,
        EGL_NONE
    };


    EGLint level_count = -1;


    /* Clear previous EGL error. */
    (void)eglGetError();


    result = eglChooseConfig(
        display,
        invalid_level_attribs,
        NULL,
        0,
        &level_count
    );


    error = eglGetError();


    if (result != EGL_FALSE)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "eglChooseConfig accepted EGL_DONT_CARE "
            "for EGL_LEVEL"
        );

        test_success1 = EGL_FALSE;
    }


    if (error != EGL_BAD_ATTRIBUTE)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Expected EGL_BAD_ATTRIBUTE when EGL_DONT_CARE "
            "was specified for EGL_LEVEL, got: 0x%x",
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


    // TEST CASE 008

    /* Verify each EGL 1.0 selection criterion:
     *     Larger
     *     Smaller
     *     Exact
     *     Mask
     */


    /* LARGER
     * EGL_RED_SIZE uses the Larger criterion.
     * For selection, matching configs shall have:
     *      EGL_RED_SIZE >= requested red size
     */

    const EGLint larger_attribs[] =
    {
        EGL_RED_SIZE, red_size,
        EGL_NONE
    };


    if (verify_selection(
            larger_attribs,
            EGL_RED_SIZE,
            red_size,
            MATCH_AT_LEAST,
            "Larger") != EGL_TRUE)
    {
        test_success2 = EGL_FALSE;
    }


    /* SMALLER
     * EGL_BUFFER_SIZE uses the Smaller criterion.
     * For selection, matching configs shall have:
     *    EGL_BUFFER_SIZE >= requested buffer size
     * In EGL 1.0 the distinction between Smaller and Larger
     * affects sorting, not the selection comparison.
     */

    const EGLint smaller_attribs[] =
    {
        EGL_BUFFER_SIZE, buffer_size,
        EGL_NONE
    };


    if (verify_selection(
            smaller_attribs,
            EGL_BUFFER_SIZE,
            buffer_size,
            MATCH_AT_LEAST,
            "Smaller") != EGL_TRUE)
    {
        test_success2 = EGL_FALSE;
    }


    /* EXACT
     * EGL_LEVEL uses the Exact criterion.
     * Matching configs shall have:
     *      EGL_LEVEL == requested level
     */

    const EGLint exact_attribs[] =
    {
        EGL_LEVEL, level,
        EGL_NONE
    };


    if (verify_selection(
            exact_attribs,
            EGL_LEVEL,
            level,
            MATCH_EXACT,
            "Exact") != EGL_TRUE)
    {
        test_success2 = EGL_FALSE;
    }


    /* MASK
     * EGL_SURFACE_TYPE uses the Mask criterion.
     * The reference config was selected using the default
     * EGL 1.0 criteria, which require EGL_WINDOW_BIT.
     * Every returned configuration shall therefore contain
     * EGL_WINDOW_BIT in EGL_SURFACE_TYPE.
     */

    const EGLint mask_attribs[] =
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };


    if ((surface_type & EGL_WINDOW_BIT) == 0)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Test precondition failed: reference config "
            "does not contain EGL_WINDOW_BIT"
        );

        test_success2 = EGL_FALSE;
    }
    else
    {
        if (verify_selection(
                mask_attribs,
                EGL_SURFACE_TYPE,
                EGL_WINDOW_BIT,
                MATCH_MASK,
                "Mask") != EGL_TRUE)
        {
            test_success2 = EGL_FALSE;
        }
    }


    if (test_success2)
    {
        TEST_LOG_INFO(
            "Selection criteria verified: "
            "Larger, Smaller, Exact, Mask"
        );

        TEST_LOG_SUCCESS(
            test_case2,
            test_procedure
        );
    }
}


void GS_EGL10_CM_CC_TP_003_draw(void) {

}

void GS_EGL10_CM_CC_TP_003_close(void)
{
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;
}