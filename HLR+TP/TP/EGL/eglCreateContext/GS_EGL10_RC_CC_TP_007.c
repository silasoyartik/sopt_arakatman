#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "../../macros.h"

/*
EGL10 - RenderingContexts - eglCreateContext

Verify that a texture object created in share_context is available to the
context created with that share_context.

Covered requirements:
    - GS-EGL10-RC-CC-007
*/

static const char* test_case = "GS_EGL10_RC_CC_TC_007";
static const char* test_procedure = "GS_EGL10_RC_CC_TP_007";

static EGLDisplay framework_display = EGL_NO_DISPLAY;
static EGLSurface framework_draw_surface = EGL_NO_SURFACE;
static EGLSurface framework_read_surface = EGL_NO_SURFACE;
static EGLContext framework_context = EGL_NO_CONTEXT;
static EGLContext source_context = EGL_NO_CONTEXT;
static EGLContext shared_context = EGL_NO_CONTEXT;
static EGLBoolean framework_binding_replaced = EGL_FALSE;

/* Obtains the EGLConfig used by the current framework context. */
static EGLBoolean get_framework_config(EGLConfig* config)
{
    EGLint config_id;
    EGLint config_count = 0;
    EGLint selected_attributes[] = {
        EGL_CONFIG_ID, 0,
        EGL_NONE
    };

    if (eglQueryContext(framework_display, framework_context, EGL_CONFIG_ID,
            &config_id) != EGL_TRUE)
    {
        return EGL_FALSE;
    }

    selected_attributes[1] = config_id;
    return (eglChooseConfig(framework_display, selected_attributes, config, 1,
        &config_count) == EGL_TRUE) && (config_count == 1);
}

/* Creates two compatible contexts, with shared_context sharing source_context. */
static EGLBoolean create_shared_contexts(EGLConfig config)
{
    source_context = eglCreateContext(framework_display, config, EGL_NO_CONTEXT,
        NULL);
    if (source_context == EGL_NO_CONTEXT)
    {
        return EGL_FALSE;
    }

    shared_context = eglCreateContext(framework_display, config, source_context,
        NULL);
    return (shared_context != EGL_NO_CONTEXT) &&
        (shared_context != source_context);
}

/* Creates a texture in source_context and verifies it in shared_context. */
void GS_EGL10_RC_CC_TP_007_init(void)
{
    EGLConfig config;
    GLuint texture = 0;
    GLenum gl_error;

    framework_display = eglGetCurrentDisplay();
    framework_draw_surface = eglGetCurrentSurface(EGL_DRAW);
    framework_read_surface = eglGetCurrentSurface(EGL_READ);
    framework_context = eglGetCurrentContext();

    if ((framework_display == EGL_NO_DISPLAY) ||
        (framework_draw_surface == EGL_NO_SURFACE) ||
        (framework_read_surface == EGL_NO_SURFACE) ||
        (framework_context == EGL_NO_CONTEXT))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "A current GBM/DRM/KMS EGL display, surfaces and context are required");
        return;
    }

    if (get_framework_config(&config) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not obtain the framework EGLConfig, error: 0x%x", eglGetError());
        return;
    }

    if (create_shared_contexts(config) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create compatible shared EGLContexts, error: 0x%x",
            eglGetError());
        return;
    }

    /* Create and bind the texture object while source_context is current. */
    if (eglMakeCurrent(framework_display, framework_draw_surface,
            framework_read_surface, source_context) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not make source_context current, error: 0x%x", eglGetError());
        return;
    }
    framework_binding_replaced = EGL_TRUE;

    (void)glGetError();
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    gl_error = glGetError();
    if ((texture == 0) || (gl_error != GL_NO_ERROR) ||
        (glIsTexture(texture) != GL_TRUE))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create a texture object in source_context, GL error: 0x%x",
            gl_error);
        return;
    }

    /* CC-007: the texture created above shall be visible in shared_context. */
    if (eglMakeCurrent(framework_display, framework_draw_surface,
            framework_read_surface, shared_context) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not make shared_context current, error: 0x%x", eglGetError());
        return;
    }

    (void)glGetError();
    if ((glIsTexture(texture) != GL_TRUE) ||
        (glGetError() != GL_NO_ERROR))
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "The texture object from source_context is not available in shared_context");
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

/* No drawing is required for this shared-texture visibility test. */
void GS_EGL10_RC_CC_TP_007_draw(void)
{
}

/* Restores the framework binding and destroys the test-owned contexts. */
void GS_EGL10_RC_CC_TP_007_close(void)
{
    if ((framework_binding_replaced == EGL_TRUE) &&
        (framework_display != EGL_NO_DISPLAY))
    {
        (void)eglMakeCurrent(framework_display, framework_draw_surface,
            framework_read_surface, framework_context);
    }

    if ((framework_display != EGL_NO_DISPLAY) &&
        (shared_context != EGL_NO_CONTEXT))
    {
        (void)eglDestroyContext(framework_display, shared_context);
    }

    if ((framework_display != EGL_NO_DISPLAY) &&
        (source_context != EGL_NO_CONTEXT))
    {
        (void)eglDestroyContext(framework_display, source_context);
    }

    framework_display = EGL_NO_DISPLAY;
    framework_draw_surface = EGL_NO_SURFACE;
    framework_read_surface = EGL_NO_SURFACE;
    framework_context = EGL_NO_CONTEXT;
    source_context = EGL_NO_CONTEXT;
    shared_context = EGL_NO_CONTEXT;
    framework_binding_replaced = EGL_FALSE;
}
