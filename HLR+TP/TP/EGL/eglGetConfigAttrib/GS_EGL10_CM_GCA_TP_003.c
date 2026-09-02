#include <EGL/egl.h>
#include "../../helpers.h"

/*
EGL10 - ConfigurationManagement - eglGetConfigAttrib
Covered requirement: GS-EGL10-CM-GCA-003
*/

static const char* test_case = "GS_EGL10_CM_GCA_TC_003";
static const char* test_procedure = "GS_EGL10_CM_GCA_TP_003";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_CM_GCA_TP_003_init(void)
{
    static const EGLint attributes[] = {
        EGL_BUFFER_SIZE, EGL_RED_SIZE, EGL_GREEN_SIZE, EGL_BLUE_SIZE,
        EGL_ALPHA_SIZE, EGL_CONFIG_CAVEAT, EGL_CONFIG_ID, EGL_DEPTH_SIZE,
        EGL_LEVEL, EGL_MAX_PBUFFER_WIDTH, EGL_MAX_PBUFFER_HEIGHT,
        EGL_MAX_PBUFFER_PIXELS, EGL_NATIVE_RENDERABLE, EGL_NATIVE_VISUAL_ID,
        EGL_NATIVE_VISUAL_TYPE, EGL_SAMPLE_BUFFERS, EGL_SAMPLES,
        EGL_STENCIL_SIZE, EGL_SURFACE_TYPE, EGL_TRANSPARENT_TYPE,
        EGL_TRANSPARENT_RED_VALUE, EGL_TRANSPARENT_GREEN_VALUE,
        EGL_TRANSPARENT_BLUE_VALUE
    };
    EGLint value;
    EGLint index;

    if (!GS_EGL10_initialize_display(&environment) ||
        !GS_EGL10_choose_config(&environment, EGL_PBUFFER_BIT))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    // Test starts here: query every EGL 1.0 configuration attribute.
    for (index = 0; index < (EGLint)(sizeof(attributes) / sizeof(attributes[0]));
         ++index)
    {
        (void)eglGetError();
        if (eglGetConfigAttrib(environment.display, environment.config,
                attributes[index], &value) != EGL_TRUE ||
            eglGetError() != EGL_SUCCESS)
        {
            TEST_LOG_FAIL(test_case, test_procedure,
                "Attribute 0x%x could not be queried", attributes[index]);
            test_success = EGL_FALSE;
        }
    }

    if (test_success)
        TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_CM_GCA_TP_003_draw(void) { }

void GS_EGL10_CM_GCA_TP_003_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

