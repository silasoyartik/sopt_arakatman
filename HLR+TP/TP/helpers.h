#ifndef GS_EGL10_TEST_HELPERS_H
#define GS_EGL10_TEST_HELPERS_H

#include <EGL/egl.h>
#include "macros.h"

typedef struct
{
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface surface;
    EGLBoolean initialized;
} GS_EGL10_TestEnvironment;

#define GS_EGL10_ENV_INITIALIZER \
    { EGL_NO_DISPLAY, (EGLConfig)0, EGL_NO_CONTEXT, EGL_NO_SURFACE, EGL_FALSE }

/* CHECK_ERROR expects test_case and test_success in the test source file. */
#define CHECK_ERROR(test_procedure)                                         \
    do                                                                      \
    {                                                                       \
        EGLint gs_egl_check_error = eglGetError();                          \
        if (gs_egl_check_error != EGL_SUCCESS)                              \
        {                                                                   \
            TEST_LOG_FAIL(test_case, test_procedure,                        \
                "Unexpected EGL error: 0x%x", gs_egl_check_error);          \
            test_success = EGL_FALSE;                                       \
        }                                                                   \
    } while (0)

static EGLBoolean GS_EGL10_initialize_display(
    GS_EGL10_TestEnvironment *environment)
{
    environment->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (environment->display == EGL_NO_DISPLAY)
    {
        return EGL_FALSE;
    }

    if (eglInitialize(environment->display, NULL, NULL) != EGL_TRUE)
    {
        return EGL_FALSE;
    }

    environment->initialized = EGL_TRUE;
    return EGL_TRUE;
}

static EGLBoolean GS_EGL10_choose_config(
    GS_EGL10_TestEnvironment *environment,
    EGLint surface_type)
{
    const EGLint attributes[] = {
        EGL_SURFACE_TYPE, surface_type,
        EGL_NONE
    };
    EGLint count = 0;

    return eglChooseConfig(environment->display, attributes,
        &environment->config, 1, &count) == EGL_TRUE && count > 0;
}

/* Creates the objects but deliberately leaves the context non-current. */
static EGLBoolean GS_EGL10_prepare_pbuffer_environment(
    GS_EGL10_TestEnvironment *environment,
    EGLint width,
    EGLint height)
{
    const EGLint pbuffer_attributes[] = {
        EGL_WIDTH, width,
        EGL_HEIGHT, height,
        EGL_NONE
    };

    if (!GS_EGL10_initialize_display(environment) ||
        !GS_EGL10_choose_config(environment, EGL_PBUFFER_BIT))
    {
        return EGL_FALSE;
    }

    environment->context = eglCreateContext(environment->display,
        environment->config, EGL_NO_CONTEXT, NULL);
    if (environment->context == EGL_NO_CONTEXT)
    {
        return EGL_FALSE;
    }

    environment->surface = eglCreatePbufferSurface(environment->display,
        environment->config, pbuffer_attributes);
    return environment->surface != EGL_NO_SURFACE;
}

static EGLBoolean GS_EGL10_make_environment_current(
    GS_EGL10_TestEnvironment *environment)
{
    return eglMakeCurrent(environment->display, environment->surface,
        environment->surface, environment->context);
}

static void GS_EGL10_cleanup_environment(
    GS_EGL10_TestEnvironment *environment)
{
    if (environment->display != EGL_NO_DISPLAY && environment->initialized)
    {
        (void)eglMakeCurrent(environment->display, EGL_NO_SURFACE,
            EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (environment->surface != EGL_NO_SURFACE)
        {
            (void)eglDestroySurface(environment->display,
                environment->surface);
        }

        if (environment->context != EGL_NO_CONTEXT)
        {
            (void)eglDestroyContext(environment->display,
                environment->context);
        }

        (void)eglTerminate(environment->display);
    }

    environment->display = EGL_NO_DISPLAY;
    environment->config = (EGLConfig)0;
    environment->context = EGL_NO_CONTEXT;
    environment->surface = EGL_NO_SURFACE;
    environment->initialized = EGL_FALSE;
}

#endif
