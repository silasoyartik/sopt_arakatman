#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "../../helpers.h"
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
static GS_EGL10_TestEnvironment environment = GS_EGL10_ENV_INITIALIZER;
static EGLContext source_context = EGL_NO_CONTEXT;
static EGLContext shared_context = EGL_NO_CONTEXT;
static EGLBoolean test_binding_active = EGL_FALSE;

/* Creates two compatible contexts, with shared_context sharing source_context. */
static EGLBoolean create_shared_contexts(void)
{
    source_context = eglCreateContext(environment.display, environment.config,
        EGL_NO_CONTEXT, NULL);
    if (source_context == EGL_NO_CONTEXT)
    {
        return EGL_FALSE;
    }

    shared_context = eglCreateContext(environment.display, environment.config,
        source_context, NULL);
    return (shared_context != EGL_NO_CONTEXT) &&
        (shared_context != source_context);
}

/* Creates a texture in source_context and verifies it in shared_context. */
void GS_EGL10_RC_CC_TP_007_init(void)
{
    GLuint texture = 0;
    GLenum gl_error;

    if (GS_EGL10_prepare_pbuffer_environment(&environment, 16, 16) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not prepare the EGL pbuffer fixture, error: 0x%x",
            eglGetError());
        return;
    }

    if (GS_EGL10_make_environment_current(&environment) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not make the helper fixture context current, error: 0x%x",
            eglGetError());
        return;
    }

    if (create_shared_contexts() != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not create compatible shared EGLContexts, error: 0x%x",
            eglGetError());
        return;
    }

    if (eglMakeCurrent(environment.display, environment.surface,
            environment.surface, source_context) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not make source_context current, error: 0x%x", eglGetError());
        return;
    }
    test_binding_active = EGL_TRUE;

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
    if (eglMakeCurrent(environment.display, environment.surface,
            environment.surface, shared_context) != EGL_TRUE)
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

/* Restores the fixture binding and releases all test-owned contexts. */
void GS_EGL10_RC_CC_TP_007_close(void)
{
    if ((test_binding_active == EGL_TRUE) &&
        (environment.display != EGL_NO_DISPLAY))
    {
        (void)eglMakeCurrent(environment.display, environment.surface,
            environment.surface, environment.context);
    }

    if ((environment.display != EGL_NO_DISPLAY) &&
        (shared_context != EGL_NO_CONTEXT))
    {
        (void)eglDestroyContext(environment.display, shared_context);
    }

    if ((environment.display != EGL_NO_DISPLAY) &&
        (source_context != EGL_NO_CONTEXT))
    {
        (void)eglDestroyContext(environment.display, source_context);
    }

    source_context = EGL_NO_CONTEXT;
    shared_context = EGL_NO_CONTEXT;
    test_binding_active = EGL_FALSE;
    GS_EGL10_cleanup_environment(&environment);
}
