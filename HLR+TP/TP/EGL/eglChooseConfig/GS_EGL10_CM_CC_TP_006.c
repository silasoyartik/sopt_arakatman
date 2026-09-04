#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - ConfigurationManagement - ChooseConfig

The eglChooseConfig function shall ignore EGL_NATIVE_VISUAL_TYPE
when the requested EGL_SURFACE_TYPE mask does not contain
EGL_WINDOW_BIT or when no native visual types exist.

Covered requirements:
    - GS-EGL10-CM-CC-019
*/

static const char* test_case =
    "GS_EGL10_CM_CC_TC_019";

static const char* test_procedure =
    "GS_EGL10_CM_CC_TP_006";

static EGLBoolean test_success = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLBoolean initialized = EGL_FALSE;


/* Return the number of configurations matching attrib_list. */
static EGLBoolean get_matching_count(
    const EGLint *attrib_list,
    EGLint *count,
    EGLint *error)
{
    EGLBoolean result;

    (void)eglGetError();
    result = GS_EGL10_get_matching_config_count(
        display, attrib_list, count);
    *error = eglGetError();

    return result;
}


/* Find a native visual type value that is not used by any
 * configuration in the supplied array.
 * EGL_NATIVE_VISUAL_TYPE is an integer attribute.
 */
static EGLBoolean find_unused_visual_type(
    EGLConfig *configs,
    EGLint config_count,
    EGLint *unused_type)
{
    for (EGLint candidate = 0;
         candidate <= config_count;
         ++candidate)
    {
        EGLBoolean found = EGL_FALSE;

        for (EGLint i = 0; i < config_count; ++i)
        {
            EGLint visual_type = EGL_NONE;

            if (eglGetConfigAttrib(
                    display,
                    configs[i],
                    EGL_NATIVE_VISUAL_TYPE,
                    &visual_type) != EGL_TRUE)
            {
                return EGL_FALSE;
            }

            if (visual_type == candidate)
            {
                found = EGL_TRUE;
                break;
            }
        }

        if (found == EGL_FALSE)
        {
            *unused_type = candidate;
            return EGL_TRUE;
        }
    }

    return EGL_FALSE;
}


/* Determine whether the EGLDisplay has no native visual types.
 * EGL 1.0 specifies EGL_NONE for EGL_NATIVE_VISUAL_TYPE when
 * a configuration has no associated native visual type.
 */
static EGLBoolean no_native_visual_types_exist(
    EGLBoolean *no_visual_types)
{
    EGLBoolean result;
    EGLint error;

    EGLint config_count = 0;
    EGLint returned_count = 0;

    EGLConfig *configs = NULL;

    *no_visual_types = EGL_TRUE;


    result = eglGetConfigs(
        display,
        NULL,
        0,
        &config_count
    );

    if (result != EGL_TRUE ||
        config_count <= 0)
    {
        return EGL_FALSE;
    }


    configs = (EGLConfig*)malloc(
        sizeof(EGLConfig) * (size_t)config_count
    );

    if (configs == NULL)
    {
        return EGL_FALSE;
    }


    result = eglGetConfigs(
        display,
        configs,
        config_count,
        &returned_count
    );

    if (result != EGL_TRUE)
    {
        free(configs);
        return EGL_FALSE;
    }


    for (EGLint i = 0; i < returned_count; ++i)
    {
        EGLint visual_type = EGL_NONE;

        (void)eglGetError();

        result = eglGetConfigAttrib(
            display,
            configs[i],
            EGL_NATIVE_VISUAL_TYPE,
            &visual_type
        );

        error = eglGetError();

        if (result != EGL_TRUE)
        {
            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "Unable to query EGL_NATIVE_VISUAL_TYPE "
                "for config %d. eglGetError(): 0x%x",
                i,
                error
            );

            free(configs);
            return EGL_FALSE;
        }


        if (visual_type != EGL_NONE)
        {
            *no_visual_types = EGL_FALSE;
            break;
        }
    }


    free(configs);

    return EGL_TRUE;
}


/* Initialization */
void GS_EGL10_CM_CC_TP_006_init(void)
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

    // TEST CASE 011

    /* CASE 1
     * EGL_SURFACE_TYPE does NOT contain EGL_WINDOW_BIT.
     * EGL_NATIVE_VISUAL_TYPE shall therefore be ignored.
     */

    /* First determine a supported non-window surface type.
     * Prefer EGL_PBUFFER_BIT. If no configuration supports it,
     * try EGL_PIXMAP_BIT.
     */
    EGLint non_window_surface_bit = EGL_PBUFFER_BIT;

    EGLint baseline_count = -1;
    EGLint baseline_error;


    const EGLint pbuffer_attribs[] =
    {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_NONE
    };


    result = get_matching_count(
        pbuffer_attribs,
        &baseline_count,
        &baseline_error
    );


    if (result != EGL_TRUE ||
        baseline_count <= 0)
    {
        const EGLint pixmap_attribs[] =
        {
            EGL_SURFACE_TYPE, EGL_PIXMAP_BIT,
            EGL_NONE
        };

        result = get_matching_count(
            pixmap_attribs,
            &baseline_count,
            &baseline_error
        );

        if (result != EGL_TRUE ||
            baseline_count <= 0)
        {
            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "Test precondition failed: no configuration "
                "supports a non-window surface type"
            );

            test_success = EGL_FALSE;
            return;
        }

        non_window_surface_bit = EGL_PIXMAP_BIT;
    }


    /* Retrieve the configurations matching the selected non-window surface type. */
    EGLConfig *baseline_configs =
        (EGLConfig*)malloc(
            sizeof(EGLConfig) * (size_t)baseline_count
        );


    if (baseline_configs == NULL)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Memory allocation failed"
        );

        test_success = EGL_FALSE;
        return;
    }


    EGLint returned_count = 0;


    const EGLint non_window_attribs[] =
    {
        EGL_SURFACE_TYPE, non_window_surface_bit,
        EGL_NONE
    };


    result = eglChooseConfig(
        display,
        non_window_attribs,
        baseline_configs,
        baseline_count,
        &returned_count
    );


    if (result != EGL_TRUE ||
        returned_count <= 0)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Unable to retrieve non-window EGLConfigs. "
            "eglGetError(): 0x%x",
            error
        );

        free(baseline_configs);

        test_success = EGL_FALSE;
        return;
    }


    /* Find a visual type that does not match any of these
     * configurations.
     *
     * If EGL_NATIVE_VISUAL_TYPE were incorrectly used as
     * a selection criterion, the matching count would change.
     */
    EGLint unused_visual_type = -1;


    if (find_unused_visual_type(
            baseline_configs,
            returned_count,
            &unused_visual_type) != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Unable to construct an unused native visual type. "
            "eglGetError(): 0x%x",
            error
        );

        free(baseline_configs);

        test_success = EGL_FALSE;
        return;
    }


    free(baseline_configs);


    EGLint ignored_count = -1;


    /* Important:
     * non_window_surface_bit does NOT contain EGL_WINDOW_BIT.
     * Therefore EGL_NATIVE_VISUAL_TYPE shall be ignored.
     */
    const EGLint ignored_visual_attribs[] =
    {
        EGL_SURFACE_TYPE, non_window_surface_bit,
        EGL_NATIVE_VISUAL_TYPE, unused_visual_type,
        EGL_NONE
    };


    result = get_matching_count(
        ignored_visual_attribs,
        &ignored_count,
        &error
    );


    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "eglChooseConfig failed when "
            "EGL_NATIVE_VISUAL_TYPE should have been ignored. "
            "eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;
    }


    /* Because EGL_NATIVE_VISUAL_TYPE shall be ignored, the result shall be identical to the baseline query. */
    if (ignored_count != baseline_count)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "EGL_NATIVE_VISUAL_TYPE affected selection even "
            "though EGL_WINDOW_BIT was not requested. "
            "Baseline: %d, with visual type: %d",
            baseline_count,
            ignored_count
        );

        test_success = EGL_FALSE;
    }


    /* CASE 2
     * If the test environment has NO native visual types,
     * EGL_NATIVE_VISUAL_TYPE shall also be ignored.
     *
     * This condition is inherently platform/environment
     * dependent. It is executed when the current EGLDisplay
     * actually satisfies the required precondition.
     */

    EGLBoolean no_visual_types = EGL_FALSE;


    if (no_native_visual_types_exist(
            &no_visual_types) != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Unable to determine whether native visual "
            "types exist"
        );

        test_success = EGL_FALSE;
    }
    else if (no_visual_types == EGL_TRUE)
    {
        EGLint window_baseline_count = -1;
        EGLint window_visual_count = -1;

        EGLint window_baseline_error;
        EGLint window_visual_error;


        const EGLint window_baseline_attribs[] =
        {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_NONE
        };


        const EGLint window_visual_attribs[] =
        {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,

            /* No native visual types exist.
             * This value therefore cannot identify an
             * existing native visual type and shall be ignored.
             */
            EGL_NATIVE_VISUAL_TYPE, 1,

            EGL_NONE
        };


        result = get_matching_count(
            window_baseline_attribs,
            &window_baseline_count,
            &window_baseline_error
        );


        if (result != EGL_TRUE)
        {
            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "Window baseline query failed. "
                "eglGetError(): 0x%x",
                window_baseline_error
            );

            test_success = EGL_FALSE;
        }


        result = get_matching_count(
            window_visual_attribs,
            &window_visual_count,
            &window_visual_error
        );


        if (result != EGL_TRUE)
        {
            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "eglChooseConfig failed while testing "
                "EGL_NATIVE_VISUAL_TYPE with no native "
                "visual types. eglGetError(): 0x%x",
                window_visual_error
            );

            test_success = EGL_FALSE;
        }


        if (window_baseline_count !=
            window_visual_count)
        {
            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "EGL_NATIVE_VISUAL_TYPE affected selection "
                "although no native visual types exist. "
                "Baseline: %d, with visual type: %d",
                window_baseline_count,
                window_visual_count
            );

            test_success = EGL_FALSE;
        }


        TEST_LOG_INFO(
            "No native visual types detected; "
            "second CC-019 condition was exercised"
        );
    }
    else
    {
        /* The second branch of CC-019 requires a target
         * environment that has no native visual types.
         * The first branch has still been exercised above.
         */
        TEST_LOG_INFO(
            "Native visual types exist on this test platform. "
            "The 'no native visual types' branch requires "
            "a target environment satisfying that precondition"
        );
    }


    /* FINAL RESULT */
    if (test_success)
    {
        TEST_LOG_INFO(
            "EGL_NATIVE_VISUAL_TYPE was correctly ignored "
            "when EGL_WINDOW_BIT was not requested"
        );

        TEST_LOG_SUCCESS(
            test_case,
            test_procedure
        );
    }
}

void GS_EGL10_CM_CC_TP_006_draw(void) {

}

void GS_EGL10_CM_CC_TP_006_close(void)
{
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;
}
