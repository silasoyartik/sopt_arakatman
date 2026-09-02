#include <EGL/egl.h>
#include "../../helper.h"

/*
EGL10 - ConfigurationManagement - eglGetConfigAttrib
Covered requirement: GS-EGL10-CM-GCA-004
*/

static const char* test_case = "GS_EGL10_CM_GCA_TC_004";
static const char* test_procedure = "GS_EGL10_CM_GCA_TP_004";
static EGLBoolean test_success = EGL_TRUE;
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;

void GS_EGL10_CM_GCA_TP_004_init(void)
{
    const EGLint valid_bits =
        EGL_WINDOW_BIT | EGL_PIXMAP_BIT | EGL_PBUFFER_BIT;
    EGLint surface_type = 0;

    if (!GS_EGL10_initialize_display(&environment) ||
        !GS_EGL10_choose_config(&environment, EGL_PBUFFER_BIT))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Setup failed, EGL error: 0x%x", eglGetError());
        return;
    }

    // Test starts here: validate the returned surface-type mask.
    if (eglGetConfigAttrib(environment.display, environment.config,
            EGL_SURFACE_TYPE, &surface_type) != EGL_TRUE ||
        (surface_type & ~valid_bits) != 0 ||
        (surface_type & valid_bits) == 0)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Invalid EGL_SURFACE_TYPE mask: 0x%x", surface_type);
        test_success = EGL_FALSE;
    }

    if (test_success)
        TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_CM_GCA_TP_004_draw(void) { }

void GS_EGL10_CM_GCA_TP_004_close(void)
{
    GS_EGL10_cleanup_environment(&environment);
}

