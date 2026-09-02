#include <EGL/egl.h>
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglCreateContext

Verify EGL_BAD_MATCH when a valid share_context is incompatible with the
context being created. This procedure is applicable only on targets exposing
at least one incompatible pair of valid EGL configurations.

Covered requirements:
    - GS-EGL10-RC-CC-009
*/

static const char* test_case = "GS_EGL10_RC_CC_TC_009";
static const char* test_procedure = "GS_EGL10_RC_CC_TP_009";

/* Checks whether two configurations have different framebuffer attributes. */
static EGLBoolean configs_differ(EGLDisplay display, EGLConfig first,
    EGLConfig second) {
    const EGLint attributes[] = {
        EGL_RED_SIZE, EGL_GREEN_SIZE, EGL_BLUE_SIZE, EGL_ALPHA_SIZE,
        EGL_DEPTH_SIZE, EGL_STENCIL_SIZE
    };
    EGLint first_value;
    EGLint second_value;
    EGLint index;

    for (index = 0; index < 6; ++index) {
        if ((eglGetConfigAttrib(display, first, attributes[index],
                &first_value) != EGL_TRUE) ||
            (eglGetConfigAttrib(display, second, attributes[index],
                &second_value) != EGL_TRUE)) {
            return EGL_FALSE;
        }

        if (first_value != second_value) {
            return EGL_TRUE;
        }
    }

    return EGL_FALSE;
}

/* Searches for a valid context pair that is incompatible for state sharing. */
void GS_EGL10_RC_CC_TP_009_init(void) {
    EGLConfig configs[64];
    EGLDisplay display;
    EGLContext source_context;
    EGLContext attempted_context;
    EGLint config_count = 0;
    EGLint returned_count;
    EGLint source_index;
    EGLint target_index;
    EGLint error;
    const EGLint attrib_list[] = { EGL_NONE };

    display = eglGetCurrentDisplay();
    if (display == EGL_NO_DISPLAY) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "An initialized current EGLDisplay is required");
        return;
    }

    if (eglGetConfigs(display, configs, 64, &config_count) != EGL_TRUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "eglGetConfigs failed, error: 0x%x", eglGetError());
        return;
    }

    returned_count = (config_count < 64) ? config_count : 64;
    for (source_index = 0; source_index < returned_count; ++source_index) {
        (void)eglGetError();
        source_context = eglCreateContext(display, configs[source_index],
            EGL_NO_CONTEXT, attrib_list);
        if (source_context == EGL_NO_CONTEXT) {
            (void)eglGetError();
            continue;
        }

        for (target_index = 0; target_index < returned_count; ++target_index) {
            if ((target_index == source_index) || !configs_differ(display,
                    configs[source_index], configs[target_index])) {
                continue;
            }

            (void)eglGetError();
            attempted_context = eglCreateContext(display, configs[target_index],
                source_context, attrib_list);
            error = eglGetError();

            if ((attempted_context == EGL_NO_CONTEXT) &&
                (error == EGL_BAD_MATCH)) {
                (void)eglDestroyContext(display, source_context);
                TEST_LOG_SUCCESS(test_case, test_procedure);
                return;
            }

            if (attempted_context != EGL_NO_CONTEXT) {
                (void)eglDestroyContext(display, attempted_context);
            }
        }

        (void)eglDestroyContext(display, source_context);
    }

    /*
     * EGL 1.0 permits implementations where every creatable configuration
     * shares context state. HLR marks this verification conditional, so this
     * target has no applicable incompatible pair rather than a test failure.
     */
    TEST_LOG_INFO("[ %s ][ %s ] Not applicable: no incompatible valid EGLContext pair is available.",
        test_case, test_procedure);
}

/* No drawing is required for this conditional error-condition test. */
void GS_EGL10_RC_CC_TP_009_draw(void) {

}

/* All temporary contexts are released during initialization. */
void GS_EGL10_RC_CC_TP_009_close(void) {

}
