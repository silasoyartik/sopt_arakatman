#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - ConfigurationManagement - ChooseConfig

The eglChooseConfig function shall ignore
EGL_MAX_PBUFFER_WIDTH,
EGL_MAX_PBUFFER_HEIGHT,
EGL_MAX_PBUFFER_PIXELS, and
EGL_NATIVE_VISUAL_ID
as selection criteria when they appear with attribute values
in attrib_list.

Covered requirements:
    - GS-EGL10-CM-CC-018
*/

static const char* test_case =
    "GS_EGL10_CM_CC_TC_018";

static const char* test_procedure =
    "GS_EGL10_CM_CC_TP_005";

static EGLBoolean test_success = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLBoolean initialized = EGL_FALSE;


/* Obtain only the number of configurations matching attrib_list. */
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


/* Find an integer value which is not used as EGL_NATIVE_VISUAL_ID by any of the supplied configurations. */
static EGLBoolean find_unused_native_visual_id(
    EGLConfig *configs,
    EGLint config_count,
    EGLint *unused_id)
{
    EGLint candidate;

    /* Among config_count + 1 candidate values, at least one shall not occur among config_count configurations. */
    for (candidate = 0;
         candidate <= config_count;
         ++candidate)
    {
        EGLBoolean found = EGL_FALSE;

        for (EGLint i = 0; i < config_count; ++i)
        {
            EGLint visual_id = -1;

            if (eglGetConfigAttrib(
                    display,
                    configs[i],
                    EGL_NATIVE_VISUAL_ID,
                    &visual_id) != EGL_TRUE)
            {
                return EGL_FALSE;
            }

            if (visual_id == candidate)
            {
                found = EGL_TRUE;
                break;
            }
        }

        if (found == EGL_FALSE)
        {
            *unused_id = candidate;
            return EGL_TRUE;
        }
    }

    return EGL_FALSE;
}


/* Initialization */
void GS_EGL10_CM_CC_TP_005_init(void)
{
    EGLBoolean result;
    EGLint error;

    EGLint major = -1;
    EGLint minor = -1;

    EGLint config_count = 0;
    EGLConfig *all_configs = NULL;

    EGLint max_pbuffer_width = 0;
    EGLint max_pbuffer_height = 0;
    EGLint max_pbuffer_pixels = 0;

    EGLint unused_visual_id = -1;

    EGLint baseline_count = -1;


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


    /* Obtain all EGLConfigs.
     * They are used to construct attribute values which do not
     * correspond to the actual configuration capabilities.
     */
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


    all_configs =
        (EGLConfig*)malloc(
            sizeof(EGLConfig) * (size_t)config_count
        );


    if (all_configs == NULL)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: unable to allocate "
            "configuration array"
        );

        test_success = EGL_FALSE;
        return;
    }


    EGLint returned_configs = 0;

    result = eglGetConfigs(
        display,
        all_configs,
        config_count,
        &returned_configs
    );

    if (result != EGL_TRUE ||
        returned_configs <= 0)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: eglGetConfigs could not "
            "return EGLConfig handles. eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;

        free(all_configs);
        return;
    }


    /* Determine maximum Pbuffer capabilities across all configs. */
    for (EGLint i = 0; i < returned_configs; ++i)
    {
        EGLint width;
        EGLint height;
        EGLint pixels;


        if (eglGetConfigAttrib(
                display,
                all_configs[i],
                EGL_MAX_PBUFFER_WIDTH,
                &width) != EGL_TRUE ||
            eglGetConfigAttrib(
                display,
                all_configs[i],
                EGL_MAX_PBUFFER_HEIGHT,
                &height) != EGL_TRUE ||
            eglGetConfigAttrib(
                display,
                all_configs[i],
                EGL_MAX_PBUFFER_PIXELS,
                &pixels) != EGL_TRUE)
        {
            error = eglGetError();

            TEST_LOG_FAIL(
                test_case,
                test_procedure,
                "Test precondition failed: Pbuffer attributes "
                "could not be queried. eglGetError(): 0x%x",
                error
            );

            test_success = EGL_FALSE;

            free(all_configs);
            return;
        }


        if (width > max_pbuffer_width)
        {
            max_pbuffer_width = width;
        }

        if (height > max_pbuffer_height)
        {
            max_pbuffer_height = height;
        }

        if (pixels > max_pbuffer_pixels)
        {
            max_pbuffer_pixels = pixels;
        }
    }


    /* Values one greater than the maximum supported values are used to make the test distinguishable. */
    if (max_pbuffer_width == INT_MAX ||
        max_pbuffer_height == INT_MAX ||
        max_pbuffer_pixels == INT_MAX)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: unable to construct "
            "a value greater than a reported Pbuffer maximum"
        );

        test_success = EGL_FALSE;

        free(all_configs);
        return;
    }


    const EGLint test_width =
        max_pbuffer_width + 1;

    const EGLint test_height =
        max_pbuffer_height + 1;

    const EGLint test_pixels =
        max_pbuffer_pixels + 1;


    /* Find a native visual ID which is not used by any available configuration. */
    if (find_unused_native_visual_id(
            all_configs,
            returned_configs,
            &unused_visual_id) != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Test precondition failed: unable to construct "
            "an unused EGL_NATIVE_VISUAL_ID. "
            "eglGetError(): 0x%x",
            error
        );

        test_success = EGL_FALSE;

        free(all_configs);
        return;
    }


    free(all_configs);


    /* BASELINE
     * Use the normal EGL 1.0 default selection criteria.
     */

    EGLint baseline_error;

    result = get_matching_count(
        NULL,
        &baseline_count,
        &baseline_error
    );


    if (result != EGL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Baseline eglChooseConfig query failed. "
            "eglGetError(): 0x%x",
            baseline_error
        );

        test_success = EGL_FALSE;
        return;
    }


    // TEST CASE 010

     /* Each of the following attributes shall be ignored by
     * eglChooseConfig as a selection criterion.
     */


    /* Case 1: EGL_MAX_PBUFFER_WIDTH */
    const EGLint width_attribs[] =
    {
        EGL_MAX_PBUFFER_WIDTH, test_width,
        EGL_NONE
    };

    EGLint width_count;
    EGLint width_error;


    result = get_matching_count(
        width_attribs,
        &width_count,
        &width_error
    );


    if (result != EGL_TRUE ||
        width_error != EGL_SUCCESS ||
        width_count != baseline_count)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "EGL_MAX_PBUFFER_WIDTH affected configuration "
            "selection. Baseline: %d, result: %d, error: 0x%x",
            baseline_count,
            width_count,
            width_error
        );

        test_success = EGL_FALSE;
    }


    /* Case 2: EGL_MAX_PBUFFER_HEIGHT */
    const EGLint height_attribs[] =
    {
        EGL_MAX_PBUFFER_HEIGHT, test_height,
        EGL_NONE
    };

    EGLint height_count;
    EGLint height_error;


    result = get_matching_count(
        height_attribs,
        &height_count,
        &height_error
    );


    if (result != EGL_TRUE ||
        height_error != EGL_SUCCESS ||
        height_count != baseline_count)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "EGL_MAX_PBUFFER_HEIGHT affected configuration "
            "selection. Baseline: %d, result: %d, error: 0x%x",
            baseline_count,
            height_count,
            height_error
        );

        test_success = EGL_FALSE;
    }


    /* Case 3: EGL_MAX_PBUFFER_PIXELS */

    const EGLint pixels_attribs[] =
    {
        EGL_MAX_PBUFFER_PIXELS, test_pixels,
        EGL_NONE
    };

    EGLint pixels_count;
    EGLint pixels_error;


    result = get_matching_count(
        pixels_attribs,
        &pixels_count,
        &pixels_error
    );


    if (result != EGL_TRUE ||
        pixels_error != EGL_SUCCESS ||
        pixels_count != baseline_count)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "EGL_MAX_PBUFFER_PIXELS affected configuration "
            "selection. Baseline: %d, result: %d, error: 0x%x",
            baseline_count,
            pixels_count,
            pixels_error
        );

        test_success = EGL_FALSE;
    }


    /* Case 4: EGL_NATIVE_VISUAL_ID */

    const EGLint visual_attribs[] =
    {
        EGL_NATIVE_VISUAL_ID, unused_visual_id,
        EGL_NONE
    };

    EGLint visual_count;
    EGLint visual_error;


    result = get_matching_count(
        visual_attribs,
        &visual_count,
        &visual_error
    );


    if (result != EGL_TRUE ||
        visual_error != EGL_SUCCESS ||
        visual_count != baseline_count)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "EGL_NATIVE_VISUAL_ID affected configuration "
            "selection. Baseline: %d, result: %d, error: 0x%x",
            baseline_count,
            visual_count,
            visual_error
        );

        test_success = EGL_FALSE;
    }


    /* Combined case:
     * All ignored attributes appear together.
     */

    const EGLint combined_attribs[] =
    {
        EGL_MAX_PBUFFER_WIDTH,  test_width,
        EGL_MAX_PBUFFER_HEIGHT, test_height,
        EGL_MAX_PBUFFER_PIXELS, test_pixels,
        EGL_NATIVE_VISUAL_ID,   unused_visual_id,
        EGL_NONE
    };

    EGLint combined_count;
    EGLint combined_error;


    result = get_matching_count(
        combined_attribs,
        &combined_count,
        &combined_error
    );


    if (result != EGL_TRUE ||
        combined_error != EGL_SUCCESS ||
        combined_count != baseline_count)
    {
        TEST_LOG_FAIL(
            test_case,
            test_procedure,
            "Ignored attributes affected configuration "
            "selection when specified together. "
            "Baseline: %d, result: %d, error: 0x%x",
            baseline_count,
            combined_count,
            combined_error
        );

        test_success = EGL_FALSE;
    }


    if (test_success)
    {
        TEST_LOG_INFO(
            "eglChooseConfig ignored all required attributes. "
            "Matching configuration count remained: %d",
            baseline_count
        );

        TEST_LOG_SUCCESS(
            test_case,
            test_procedure
        );
    }
}


void GS_EGL10_CM_CC_TP_005_draw(void) {

}

void GS_EGL10_CM_CC_TP_005_close(void)
{
    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }

    initialized = EGL_FALSE;
    display = EGL_NO_DISPLAY;
}
