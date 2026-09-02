#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include "macros.h"

/* OPTIONAL IMPLEMENTATION ADAPTATION
 *
 * EGL 1.0 defines the sorting priority of
 * EGL_NATIVE_VISUAL_TYPE as implementation-dependent.
 *
 * If matching configurations contain different native visual
 * types, the target implementation shall provide:
 *
 *     GS_EGL10_compare_native_visual_type(lhs, rhs)
 *
 * Return:
 *     -1 : lhs shall be sorted before rhs
 *      0 : lhs and rhs have equal sorting priority
 *      1 : lhs shall be sorted after rhs
 *
 * Define:
 *
 *     GS_EGL10_NATIVE_VISUAL_COMPARE_AVAILABLE
 *
 * when this comparator is provided by the target platform.
 *
 * If all returned configurations have the same native visual
 * type, no adaptation is required.
 */

#ifdef GS_EGL10_NATIVE_VISUAL_COMPARE_AVAILABLE

extern int GS_EGL10_compare_native_visual_type(
    EGLint lhs,
    EGLint rhs);

#else

static int GS_EGL10_compare_native_visual_type(
    EGLint lhs,
    EGLint rhs)
{
    if (lhs == rhs)
    {
        return 0;
    }

    /* 2 means that the target-specific ordering is unknown. */
    return 2;
}

#endif


/*
EGL10 - ConfigurationManagement - ChooseConfig

The eglChooseConfig function shall sort multiple matching
EGLConfigs according to the EGL 1.0 best-match precedence rules.

Covered requirements:
    - GS-EGL10-CM-CC-014
*/

static const char* test_case =
    "GS_EGL10_CM_CC_TC_014";

static const char* test_procedure =
    "GS_EGL10_CM_CC_TP_009";

static EGLBoolean test_success = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLBoolean initialized = EGL_FALSE;


/* Attributes used by the EGL 1.0 sorting rules. */
typedef struct
{
    EGLint config_caveat;

    EGLint red_size;
    EGLint green_size;
    EGLint blue_size;
    EGLint alpha_size;

    EGLint buffer_size;

    EGLint sample_buffers;
    EGLint samples;

    EGLint depth_size;
    EGLint stencil_size;

    EGLint native_visual_type;

    EGLint config_id;

} ConfigSortKey;


/* Convert EGL_CONFIG_CAVEAT to its EGL 1.0 sorting rank.
 * Sorting order:
 *     EGL_NONE
 *     EGL_SLOW_CONFIG
 *     EGL_NON_CONFORMANT_CONFIG
 */
static EGLint caveat_rank(EGLint caveat)
{
    if (caveat == EGL_NONE)
    {
        return 0;
    }

    if (caveat == EGL_SLOW_CONFIG)
    {
        return 1;
    }

    if (caveat == EGL_NON_CONFORMANT_CONFIG)
    {
        return 2;
    }

    return 3;
}


/* Read all attributes required for sorting. */
static EGLBoolean load_sort_key(
    EGLConfig config,
    ConfigSortKey *key)
{
    if (eglGetConfigAttrib(
            display,
            config,
            EGL_CONFIG_CAVEAT,
            &key->config_caveat) != EGL_TRUE ||

        eglGetConfigAttrib(
            display,
            config,
            EGL_RED_SIZE,
            &key->red_size) != EGL_TRUE ||

        eglGetConfigAttrib(
            display,
            config,
            EGL_GREEN_SIZE,
            &key->green_size) != EGL_TRUE ||

        eglGetConfigAttrib(
            display,
            config,
            EGL_BLUE_SIZE,
            &key->blue_size) != EGL_TRUE ||

        eglGetConfigAttrib(
            display,
            config,
            EGL_ALPHA_SIZE,
            &key->alpha_size) != EGL_TRUE ||

        eglGetConfigAttrib(
            display,
            config,
            EGL_BUFFER_SIZE,
            &key->buffer_size) != EGL_TRUE ||

        eglGetConfigAttrib(
            display,
            config,
            EGL_SAMPLE_BUFFERS,
            &key->sample_buffers) != EGL_TRUE ||

        eglGetConfigAttrib(
            display,
            config,
            EGL_SAMPLES,
            &key->samples) != EGL_TRUE ||

        eglGetConfigAttrib(
            display,
            config,
            EGL_DEPTH_SIZE,
            &key->depth_size) != EGL_TRUE ||

        eglGetConfigAttrib(
            display,
            config,
            EGL_STENCIL_SIZE,
            &key->stencil_size) != EGL_TRUE ||

        eglGetConfigAttrib(
            display,
            config,
            EGL_NATIVE_VISUAL_TYPE,
            &key->native_visual_type) != EGL_TRUE ||

        eglGetConfigAttrib(
            display,
            config,
            EGL_CONFIG_ID,
            &key->config_id) != EGL_TRUE)
    {
        return EGL_FALSE;
    }

    return EGL_TRUE;
}


/* Compare two EGLConfigs according to EGL 1.0 sorting rules.
 * Return:
 *     -1 : lhs is correctly before rhs
 *      0 : equivalent
 *      1 : lhs should be after rhs
 *      2 : native visual ordering requires platform adaptation
 */
static int compare_sort_keys(
    const ConfigSortKey *lhs,
    const ConfigSortKey *rhs,
    EGLBoolean consider_red,
    EGLBoolean consider_green,
    EGLBoolean consider_blue,
    EGLBoolean consider_alpha)
{
    EGLint lhs_value;
    EGLint rhs_value;


    /* Rule 1
     * EGL_CONFIG_CAVEAT
     */

    lhs_value = caveat_rank(lhs->config_caveat);
    rhs_value = caveat_rank(rhs->config_caveat);

    if (lhs_value != rhs_value)
    {
        return (lhs_value < rhs_value) ? -1 : 1;
    }


    /* Rule 2
     * Larger total number of considered RGBA bits.
     *
     * Components requested as 0 or EGL_DONT_CARE are not
     * considered in this calculation.
     */

    EGLint lhs_rgba = 0;
    EGLint rhs_rgba = 0;


    if (consider_red == EGL_TRUE)
    {
        lhs_rgba += lhs->red_size;
        rhs_rgba += rhs->red_size;
    }

    if (consider_green == EGL_TRUE)
    {
        lhs_rgba += lhs->green_size;
        rhs_rgba += rhs->green_size;
    }

    if (consider_blue == EGL_TRUE)
    {
        lhs_rgba += lhs->blue_size;
        rhs_rgba += rhs->blue_size;
    }

    if (consider_alpha == EGL_TRUE)
    {
        lhs_rgba += lhs->alpha_size;
        rhs_rgba += rhs->alpha_size;
    }


    if (lhs_rgba != rhs_rgba)
    {
        /* Larger RGBA total sorts first. */
        return (lhs_rgba > rhs_rgba) ? -1 : 1;
    }


    /* Rule 3
     * Smaller EGL_BUFFER_SIZE
     */

    if (lhs->buffer_size != rhs->buffer_size)
    {
        return
            (lhs->buffer_size < rhs->buffer_size) ? -1 : 1;
    }


    /* Rule 4
     * Smaller EGL_SAMPLE_BUFFERS
     */

    if (lhs->sample_buffers != rhs->sample_buffers)
    {
        return
            (lhs->sample_buffers < rhs->sample_buffers)
            ? -1 : 1;
    }


    /* Rule 5
     * Smaller EGL_SAMPLES
     */

    if (lhs->samples != rhs->samples)
    {
        return
            (lhs->samples < rhs->samples) ? -1 : 1;
    }


    /* Rule 6
     * Smaller EGL_DEPTH_SIZE
     */

    if (lhs->depth_size != rhs->depth_size)
    {
        return
            (lhs->depth_size < rhs->depth_size) ? -1 : 1;
    }


    /* Rule 7
     * Smaller EGL_STENCIL_SIZE
     */

    if (lhs->stencil_size != rhs->stencil_size)
    {
        return
            (lhs->stencil_size < rhs->stencil_size) ? -1 : 1;
    }


    /* Rule 8
     * EGL_NATIVE_VISUAL_TYPE
     *
     * Actual ordering is implementation-dependent.
     */

    if (lhs->native_visual_type !=
        rhs->native_visual_type)
    {
        int visual_compare =
            GS_EGL10_compare_native_visual_type(
                lhs->native_visual_type,
                rhs->native_visual_type
            );

        if (visual_compare == 2)
        {
            return 2;
        }

        if (visual_compare != 0)
        {
            return visual_compare;
        }
    }


    /* Rule 9
     * Smaller EGL_CONFIG_ID
     *
     * This is always the final sorting rule.
     */

    if (lhs->config_id != rhs->config_id)
    {
        return
            (lhs->config_id < rhs->config_id) ? -1 : 1;
    }


    return 0;
}


/* Execute eglChooseConfig and verify the returned list using the EGL 1.0 sorting comparator above. */
static EGLBoolean verify_sorted_query(
    const EGLint *attrib_list,
    EGLBoolean consider_red,
    EGLBoolean consider_green,
    EGLBoolean consider_blue,
    EGLBoolean consider_alpha,
    const char *query_name)
{
    EGLBoolean result;
    EGLint error;

    EGLint total_count = -1;
    EGLint returned_count = -1;

    EGLConfig *configs = NULL;
    ConfigSortKey *keys = NULL;


    /* Query the number of matching configurations. */
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
            test_case,
            test_procedure,
            "%s count query failed. eglGetError(): 0x%x",
            query_name,
            error
        );

        return EGL_FALSE;
    }


    /* Sorting can only be observed if at least two matching configurations exist. */
    if (total_count < 2)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "%s test precondition failed: at least two "
            "matching EGLConfigs are required, got: %d",
            query_name,
            total_count
        );

        return EGL_FALSE;
    }


    configs = (EGLConfig*)malloc(
        sizeof(EGLConfig) * (size_t)total_count
    );

    keys = (ConfigSortKey*)malloc(
        sizeof(ConfigSortKey) * (size_t)total_count
    );


    if (configs == NULL ||
        keys == NULL)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "%s memory allocation failed",
            query_name
        );

        free(configs);
        free(keys);

        return EGL_FALSE;
    }


    /* Obtain all matching configurations. */
    (void)eglGetError();

    result = eglChooseConfig(
        display,
        attrib_list,
        configs,
        total_count,
        &returned_count
    );

    error = eglGetError();


    if (result != EGL_TRUE ||
        returned_count < 2)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "%s could not retrieve multiple matching "
            "EGLConfigs. Count: %d, eglGetError(): 0x%x",
            query_name,
            returned_count,
            error
        );

        free(configs);
        free(keys);

        return EGL_FALSE;
    }


    /* Load all attributes involved in sorting. */
    for (EGLint i = 0;
         i < returned_count;
         ++i)
    {
        if (load_sort_key(
                configs[i],
                &keys[i]) != EGL_TRUE)
        {
            error = eglGetError();

            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "%s could not query sorting attributes "
                "for config index %d. eglGetError(): 0x%x",
                query_name,
                i,
                error
            );

            free(configs);
            free(keys);

            return EGL_FALSE;
        }
    }


    /* Verify adjacent configurations.
     *
     * A correctly sorted list must never contain an adjacent
     * pair where the left configuration should sort after
     * the right configuration.
     */

    for (EGLint i = 0;
         i < returned_count - 1;
         ++i)
    {
        int comparison =
            compare_sort_keys(
                &keys[i],
                &keys[i + 1],
                consider_red,
                consider_green,
                consider_blue,
                consider_alpha
            );


        /* Criterion 8 is implementation-dependent. */
        if (comparison == 2)
        {
            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "%s requires target-specific "
                "EGL_NATIVE_VISUAL_TYPE ordering for "
                "config IDs %d and %d",
                query_name,
                keys[i].config_id,
                keys[i + 1].config_id
            );

            TEST_LOG_INFO(
                "Provide "
                "GS_EGL10_compare_native_visual_type() "
                "and define "
                "GS_EGL10_NATIVE_VISUAL_COMPARE_AVAILABLE"
            );

            free(configs);
            free(keys);

            return EGL_FALSE;
        }


        /* comparison > 0 means the left configuration should appear after the right configuration. */
        if (comparison > 0)
        {
            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "%s sorting violation between config IDs "
                "%d and %d at indices %d and %d",
                query_name,
                keys[i].config_id,
                keys[i + 1].config_id,
                i,
                i + 1
            );

            free(configs);
            free(keys);

            return EGL_FALSE;
        }
    }


    TEST_LOG_INFO(
        "%s sorting verified for %d matching EGLConfigs",
        query_name,
        returned_count
    );


    free(configs);
    free(keys);

    return EGL_TRUE;
}


/* Initialization */
void GS_EGL10_CM_CC_TP_009_init(void)
{
    EGLBoolean result;
    EGLint error;

    test_success = EGL_TRUE;


    /* TEST PRECONDITION
     *
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


    // TEST CASE 014

    /* Verify EGL 1.0 best-match sorting.*/

    /* Scenario 1
     * Request all surface types using EGL_DONT_CARE.
     *
     * RGBA components retain their default requested values
     * of 0, therefore they do not participate in rule 2.
     *
     * This scenario exercises the remaining sorting rules.
     */
    const EGLint general_attribs[] =
    {
        EGL_SURFACE_TYPE, EGL_DONT_CARE,
        EGL_NONE
    };


    if (verify_sorted_query(
            general_attribs,
            EGL_FALSE,
            EGL_FALSE,
            EGL_FALSE,
            EGL_FALSE,
            "General sorting") != EGL_TRUE)
    {
        test_success = EGL_FALSE;
    }


    /* Scenario 2
     * EGL_RED_SIZE is explicitly requested as 1.
     *
     * Therefore RED_SIZE participates in sorting rule 2.
     * GREEN, BLUE and ALPHA retain their default requested
     * values of 0 and are not considered.
     *
     * This verifies the "larger considered RGBA total"
     * sorting behavior.
     */

    const EGLint rgba_attribs[] =
    {
        EGL_RED_SIZE, 1,

        /* Do not restrict the test to one native surface type. */
        EGL_SURFACE_TYPE, EGL_DONT_CARE,

        EGL_NONE
    };


    if (verify_sorted_query(
            rgba_attribs,
            EGL_TRUE,
            EGL_FALSE,
            EGL_FALSE,
            EGL_FALSE,
            "RGBA sorting") != EGL_TRUE)
    {
        test_success = EGL_FALSE;
    }


    /* FINAL RESULT */
    if (test_success)
    {
        TEST_LOG_INFO(
            "eglChooseConfig returned matching EGLConfigs "
            "in EGL 1.0 best-match order"
        );

        TEST_LOG_SUCCESS(
            test_case,
            test_procedure
        );
    }
}


void GS_EGL10_CM_CC_TP_009_draw(void) {

}

void GS_EGL10_CM_CC_TP_009_close(void)
{
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;
}