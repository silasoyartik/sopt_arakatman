#include <EGL/egl.h>
#include "../../helpers.h"

/* EGL10 - RenderingContexts - eglMakeCurrent
 * Covered requirement: GS-EGL10-RC-MC-007
 */
static const char* test_case = "GS_EGL10_RC_MC_TC_007";
static const char* test_procedure = "GS_EGL10_RC_MC_TP_007";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;
static EGLSurface incompatible_surface = EGL_NO_SURFACE;

static EGLBoolean configs_are_incompatible(EGLConfig first, EGLConfig second)
{
    const EGLint attributes[] = {
        EGL_RED_SIZE, EGL_GREEN_SIZE, EGL_BLUE_SIZE, EGL_ALPHA_SIZE,
        EGL_DEPTH_SIZE, EGL_STENCIL_SIZE
    };
    EGLint first_value;
    EGLint second_value;
    EGLint index;

    for (index = 0; index < 6; ++index)
    {
        if (eglGetConfigAttrib(environment.display, first, attributes[index],
                &first_value) != EGL_TRUE ||
            eglGetConfigAttrib(environment.display, second, attributes[index],
                &second_value) != EGL_TRUE)
        {
            return EGL_FALSE;
        }

        if (first_value != second_value)
            return EGL_TRUE;
    }
    return EGL_FALSE;
}

void GS_EGL10_RC_MC_TP_007_init(void)
{
    EGLConfig configs[64];
    const EGLint pbuffer_attributes[] = {
        EGL_WIDTH, 16, EGL_HEIGHT, 16, EGL_NONE
    };
    EGLint count = 0;
    EGLint returned;
    EGLint index;
    EGLBoolean result;
    EGLint error;

    if (!GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    if (eglGetConfigs(environment.display, configs, 64, &count) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not enumerate EGLConfigs");
        return;
    }
    returned = count < 64 ? count : 64;

    for (index = 0; index < returned; ++index)
    {
        EGLint surface_type = 0;

        if (!configs_are_incompatible(environment.config, configs[index]) ||
            eglGetConfigAttrib(environment.display, configs[index],
                EGL_SURFACE_TYPE, &surface_type) != EGL_TRUE ||
            (surface_type & EGL_PBUFFER_BIT) == 0)
        {
            continue;
        }

        incompatible_surface = eglCreatePbufferSurface(environment.display,
            configs[index], pbuffer_attributes);
        if (incompatible_surface != EGL_NO_SURFACE)
            break;
    }

    if (incompatible_surface == EGL_NO_SURFACE)
    {
        TEST_LOG_INFO("[ %s ][ %s ] Not applicable: no incompatible pbuffer config.",
            test_case, test_procedure);
        return;
    }

    // Test starts here: bind a surface incompatible with the context.
    (void)eglGetError();
    result = eglMakeCurrent(environment.display, incompatible_surface,
        incompatible_surface, environment.context);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_MATCH)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_MATCH, got %u/0x%x",
            (unsigned int)result, error);
        test_success = EGL_FALSE;
    }

    if (test_success) TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_EGL10_RC_MC_TP_007_draw(void) { }
void GS_EGL10_RC_MC_TP_007_close(void)
{
    if (environment.display != EGL_NO_DISPLAY &&
        incompatible_surface != EGL_NO_SURFACE)
    {
        (void)eglDestroySurface(environment.display, incompatible_surface);
        incompatible_surface = EGL_NO_SURFACE;
    }
    GS_EGL10_cleanup_environment(&environment);
}

