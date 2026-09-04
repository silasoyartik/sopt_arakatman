#include <EGL/egl.h>
#include "../../helpers.h"

/*
 * EGL10 - ConfigurationManagement - eglChooseConfig
 *
 * Covered requirements:
 *   GS-EGL10-CM-CC-007
 *   GS-EGL10-CM-CC-008
 *   GS-EGL10-CM-CC-009
 *   GS-EGL10-CM-CC-010
 *   GS-EGL10-CM-CC-011
 *   GS-EGL10-CM-CC-012
 *   GS-EGL10-CM-CC-013
 *   GS-EGL10-CM-CC-014
 *   GS-EGL10-CM-CC-015
 *   GS-EGL10-CM-CC-016
 */
static const char *test_cases[] = {
    "GS_EGL10_CM_CC_TC_007",
    "GS_EGL10_CM_CC_TC_008",
    "GS_EGL10_CM_CC_TC_009",
    "GS_EGL10_CM_CC_TC_010",
    "GS_EGL10_CM_CC_TC_011",
    "GS_EGL10_CM_CC_TC_012",
    "GS_EGL10_CM_CC_TC_013",
    "GS_EGL10_CM_CC_TC_014",
    "GS_EGL10_CM_CC_TC_015",
    "GS_EGL10_CM_CC_TC_016"
};
static const char *test_procedure = "GS_EGL10_CM_CC_TP_003";
static EGLBoolean test_success[] = {
    EGL_TRUE, EGL_TRUE, EGL_TRUE, EGL_TRUE, EGL_TRUE,
    EGL_TRUE, EGL_TRUE, EGL_TRUE, EGL_TRUE, EGL_TRUE
};
static EGLBoolean test_executed[] = {
    EGL_TRUE, EGL_TRUE, EGL_TRUE, EGL_TRUE, EGL_TRUE,
    EGL_TRUE, EGL_FALSE, EGL_TRUE, EGL_TRUE, EGL_FALSE
};
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

static EGLBoolean read_attribute(
    EGLConfig config,
    EGLint attribute,
    EGLint *value)
{
    return eglGetConfigAttrib(
        environment.display, config, attribute, value);
}

static void verify_case(
    EGLint case_index,
    const EGLint *attributes,
    const GS_EGL10_ConfigExpectation *expectations,
    EGLint expectation_count)
{
    EGLint matching_count = 0;

    (void)eglGetError();
    if (!GS_EGL10_verify_config_selection(environment.display, attributes,
            expectations, expectation_count, &matching_count))
    {
        TEST_LOG_FAIL(test_cases[case_index], test_procedure,
            "Returned configs did not satisfy the requested criteria; "
            "EGL error: 0x%x",
            eglGetError());
        test_success[case_index] = EGL_FALSE;
        return;
    }

    TEST_LOG_INFO("[ %s ][ %s ] validated %d matching configuration(s)",
        test_cases[case_index], test_procedure, matching_count);
}

void GS_EGL10_CM_CC_TP_003_init(void)
{
    const GS_EGL10_ConfigExpectation default_config_rules[] = {
        { EGL_LEVEL, 0, GS_EGL10_CONFIG_MATCH_EXACT },
        { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, GS_EGL10_CONFIG_MATCH_MASK },
        { EGL_TRANSPARENT_TYPE, EGL_NONE, GS_EGL10_CONFIG_MATCH_EXACT }
    };
    EGLint buffer_size;
    EGLint red_size;
    EGLint green_size;
    EGLint blue_size;
    EGLint alpha_size;
    EGLint depth_size;
    EGLint sample_buffers;
    EGLint samples;
    EGLint stencil_size;
    EGLint level;
    EGLint config_caveat;
    EGLint native_renderable;
    EGLint surface_type;
    EGLint transparent_type;
    EGLint index;

    for (index = 0; index < 10; ++index)
    {
        test_success[index] = EGL_TRUE;
        test_executed[index] = EGL_TRUE;
    }
    test_executed[6] = EGL_FALSE;
    test_executed[9] = EGL_FALSE;

    if (!GS_EGL10_initialize_display(&environment) ||
        !GS_EGL10_find_config_matching(environment.display,
            default_config_rules, 3, &environment.config))
    {
        TEST_LOG_FAIL(test_cases[0], test_procedure,
            "Could not find a config satisfying EGL 1.0 defaults");
        for (index = 0; index < 10; ++index)
            test_success[index] = EGL_FALSE;
        return;
    }

    if (!read_attribute(environment.config, EGL_BUFFER_SIZE, &buffer_size) ||
        !read_attribute(environment.config, EGL_RED_SIZE, &red_size) ||
        !read_attribute(environment.config, EGL_GREEN_SIZE, &green_size) ||
        !read_attribute(environment.config, EGL_BLUE_SIZE, &blue_size) ||
        !read_attribute(environment.config, EGL_ALPHA_SIZE, &alpha_size) ||
        !read_attribute(environment.config, EGL_DEPTH_SIZE, &depth_size) ||
        !read_attribute(environment.config, EGL_SAMPLE_BUFFERS,
            &sample_buffers) ||
        !read_attribute(environment.config, EGL_SAMPLES, &samples) ||
        !read_attribute(environment.config, EGL_STENCIL_SIZE, &stencil_size) ||
        !read_attribute(environment.config, EGL_LEVEL, &level) ||
        !read_attribute(environment.config, EGL_CONFIG_CAVEAT,
            &config_caveat) ||
        !read_attribute(environment.config, EGL_NATIVE_RENDERABLE,
            &native_renderable) ||
        !read_attribute(environment.config, EGL_SURFACE_TYPE, &surface_type) ||
        !read_attribute(environment.config, EGL_TRANSPARENT_TYPE,
            &transparent_type))
    {
        TEST_LOG_FAIL(test_cases[0], test_procedure,
            "Could not read the reference configuration attributes");
        for (index = 0; index < 10; ++index)
            test_success[index] = EGL_FALSE;
        return;
    }

    /* CC-008: EGL_BUFFER_SIZE uses the at-least selection rule. */
    {
        const EGLint attributes[] = {
            EGL_BUFFER_SIZE, buffer_size,
            EGL_NONE
        };
        const GS_EGL10_ConfigExpectation expectations[] = {
            { EGL_BUFFER_SIZE, buffer_size,
                GS_EGL10_CONFIG_MATCH_AT_LEAST }
        };
        verify_case(1, attributes, expectations, 1);
    }

    /* CC-009: every requested RGBA component uses the at-least rule. */
    {
        const EGLint attributes[] = {
            EGL_RED_SIZE, red_size,
            EGL_GREEN_SIZE, green_size,
            EGL_BLUE_SIZE, blue_size,
            EGL_ALPHA_SIZE, alpha_size,
            EGL_NONE
        };
        const GS_EGL10_ConfigExpectation expectations[] = {
            { EGL_RED_SIZE, red_size, GS_EGL10_CONFIG_MATCH_AT_LEAST },
            { EGL_GREEN_SIZE, green_size, GS_EGL10_CONFIG_MATCH_AT_LEAST },
            { EGL_BLUE_SIZE, blue_size, GS_EGL10_CONFIG_MATCH_AT_LEAST },
            { EGL_ALPHA_SIZE, alpha_size, GS_EGL10_CONFIG_MATCH_AT_LEAST }
        };
        verify_case(2, attributes, expectations, 4);
    }

    /* CC-010: depth, multisample and stencil values use at-least rules. */
    {
        const EGLint attributes[] = {
            EGL_DEPTH_SIZE, depth_size,
            EGL_SAMPLE_BUFFERS, sample_buffers,
            EGL_SAMPLES, samples,
            EGL_STENCIL_SIZE, stencil_size,
            EGL_NONE
        };
        const GS_EGL10_ConfigExpectation expectations[] = {
            { EGL_DEPTH_SIZE, depth_size, GS_EGL10_CONFIG_MATCH_AT_LEAST },
            { EGL_SAMPLE_BUFFERS, sample_buffers,
                GS_EGL10_CONFIG_MATCH_AT_LEAST },
            { EGL_SAMPLES, samples, GS_EGL10_CONFIG_MATCH_AT_LEAST },
            { EGL_STENCIL_SIZE, stencil_size,
                GS_EGL10_CONFIG_MATCH_AT_LEAST }
        };
        verify_case(3, attributes, expectations, 4);
    }

    /* CC-011: EGL_LEVEL uses exact matching. */
    {
        const EGLint attributes[] = {
            EGL_LEVEL, level,
            EGL_NONE
        };
        const GS_EGL10_ConfigExpectation expectations[] = {
            { EGL_LEVEL, level, GS_EGL10_CONFIG_MATCH_EXACT }
        };
        verify_case(4, attributes, expectations, 1);
    }

    /* CC-012: concrete caveat and native-renderable values match exactly. */
    {
        const EGLint attributes[] = {
            EGL_CONFIG_CAVEAT, config_caveat,
            EGL_NATIVE_RENDERABLE, native_renderable,
            EGL_NONE
        };
        const GS_EGL10_ConfigExpectation expectations[] = {
            { EGL_CONFIG_CAVEAT, config_caveat,
                GS_EGL10_CONFIG_MATCH_EXACT },
            { EGL_NATIVE_RENDERABLE, native_renderable,
                GS_EGL10_CONFIG_MATCH_EXACT }
        };
        verify_case(5, attributes, expectations, 2);
    }

    /* CC-013: native visual type is exact when it participates. */
    {
        const GS_EGL10_ConfigExpectation candidate_rules[] = {
            { EGL_LEVEL, 0, GS_EGL10_CONFIG_MATCH_EXACT },
            { EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                GS_EGL10_CONFIG_MATCH_MASK },
            { EGL_TRANSPARENT_TYPE, EGL_NONE,
                GS_EGL10_CONFIG_MATCH_EXACT },
            { EGL_NATIVE_VISUAL_TYPE, EGL_NONE,
                GS_EGL10_CONFIG_MATCH_NOT_EQUAL }
        };
        EGLConfig native_config = (EGLConfig)0;
        EGLint native_visual_type = EGL_NONE;

        if (GS_EGL10_find_config_matching(environment.display,
                candidate_rules, 4, &native_config) &&
            read_attribute(native_config, EGL_NATIVE_VISUAL_TYPE,
                &native_visual_type))
        {
            const EGLint attributes[] = {
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_NATIVE_VISUAL_TYPE, native_visual_type,
                EGL_NONE
            };
            const GS_EGL10_ConfigExpectation expectations[] = {
                { EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                    GS_EGL10_CONFIG_MATCH_MASK },
                { EGL_NATIVE_VISUAL_TYPE, native_visual_type,
                    GS_EGL10_CONFIG_MATCH_EXACT }
            };
            test_executed[6] = EGL_TRUE;
            verify_case(6, attributes, expectations, 2);
        }
        else
        {
            TEST_LOG_INFO("[ %s ][ %s ] Not applicable: no participating "
                "native visual type is exposed",
                test_cases[6], test_procedure);
        }
    }

    /* CC-014: every requested surface-type bit must be present. */
    {
        const EGLint attributes[] = {
            EGL_SURFACE_TYPE, surface_type,
            EGL_NONE
        };
        const GS_EGL10_ConfigExpectation expectations[] = {
            { EGL_SURFACE_TYPE, surface_type, GS_EGL10_CONFIG_MATCH_MASK }
        };
        verify_case(7, attributes, expectations, 1);
    }

    /* CC-015: EGL_TRANSPARENT_TYPE uses exact matching. */
    {
        const EGLint attributes[] = {
            EGL_TRANSPARENT_TYPE, transparent_type,
            EGL_NONE
        };
        const GS_EGL10_ConfigExpectation expectations[] = {
            { EGL_TRANSPARENT_TYPE, transparent_type,
                GS_EGL10_CONFIG_MATCH_EXACT }
        };
        verify_case(8, attributes, expectations, 1);
    }

    /* CC-016: transparent RGB components are exact when they participate. */
    {
        const GS_EGL10_ConfigExpectation candidate_rules[] = {
            { EGL_LEVEL, 0, GS_EGL10_CONFIG_MATCH_EXACT },
            { EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                GS_EGL10_CONFIG_MATCH_MASK },
            { EGL_TRANSPARENT_TYPE, EGL_TRANSPARENT_RGB,
                GS_EGL10_CONFIG_MATCH_EXACT }
        };
        EGLConfig transparent_config = (EGLConfig)0;
        EGLint transparent_red = 0;
        EGLint transparent_green = 0;
        EGLint transparent_blue = 0;

        if (GS_EGL10_find_config_matching(environment.display,
                candidate_rules, 3, &transparent_config) &&
            read_attribute(transparent_config, EGL_TRANSPARENT_RED_VALUE,
                &transparent_red) &&
            read_attribute(transparent_config, EGL_TRANSPARENT_GREEN_VALUE,
                &transparent_green) &&
            read_attribute(transparent_config, EGL_TRANSPARENT_BLUE_VALUE,
                &transparent_blue))
        {
            const EGLint attributes[] = {
                EGL_TRANSPARENT_TYPE, EGL_TRANSPARENT_RGB,
                EGL_TRANSPARENT_RED_VALUE, transparent_red,
                EGL_TRANSPARENT_GREEN_VALUE, transparent_green,
                EGL_TRANSPARENT_BLUE_VALUE, transparent_blue,
                EGL_NONE
            };
            const GS_EGL10_ConfigExpectation expectations[] = {
                { EGL_TRANSPARENT_TYPE, EGL_TRANSPARENT_RGB,
                    GS_EGL10_CONFIG_MATCH_EXACT },
                { EGL_TRANSPARENT_RED_VALUE, transparent_red,
                    GS_EGL10_CONFIG_MATCH_EXACT },
                { EGL_TRANSPARENT_GREEN_VALUE, transparent_green,
                    GS_EGL10_CONFIG_MATCH_EXACT },
                { EGL_TRANSPARENT_BLUE_VALUE, transparent_blue,
                    GS_EGL10_CONFIG_MATCH_EXACT }
            };
            test_executed[9] = EGL_TRUE;
            verify_case(9, attributes, expectations, 4);
        }
        else
        {
            TEST_LOG_INFO("[ %s ][ %s ] Not applicable: no transparent RGB "
                "configuration is exposed",
                test_cases[9], test_procedure);
        }
    }

    /* CC-007 is demonstrated by all concrete selection-rule checks above. */
    for (index = 1; index < 10; ++index)
    {
        if (test_success[index] != EGL_TRUE)
            test_success[0] = EGL_FALSE;
    }

    for (index = 0; index < 10; ++index)
    {
        if (test_success[index] == EGL_TRUE &&
            test_executed[index] == EGL_TRUE)
        {
            TEST_LOG_SUCCESS(test_cases[index], test_procedure);
        }
    }
}

void GS_EGL10_CM_CC_TP_003_draw(void) { }

void GS_EGL10_CM_CC_TP_003_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}
