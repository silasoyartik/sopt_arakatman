#if defined(GS_GLES31_USE_GLAD1) && defined(GS_GLES31_USE_GLAD2)
#error "Select only one GLAD loader."
#elif defined(GS_GLES31_USE_GLAD1)
#include <glad/glad.h>
#elif defined(GS_GLES31_USE_GLAD2)
#include <glad/gles2.h>
#else
#ifndef GL_GLES_PROTOTYPES
#define GL_GLES_PROTOTYPES 1
#endif
#include <GLES3/gl31.h>
#endif

#include "../../macros.h"

/*
 * GS_GLES31_CS_BB_TP_001
 * Covered requirement: GS-GLES31-CS-BB-001.
 * Bind a generated buffer to the shader-storage target and query the binding.
 *
 * Runner: current GLES >= 3.1 context, loaded entry points and clean GL
 * error state. Call init/draw/close in order, including close after setup
 * failure, keeping the same context current. No helper or main is used.
 * Buffer bindings changed below are not restored; the next TP sets its own.
 */
static const char *test_case = "GS_GLES31_CS_BB_TC_001";
static const char *test_procedure = "GS_GLES31_CS_BB_TP_001";
static GLuint buffer;

void GS_GLES31_CS_BB_TP_001_init(void)
{
    GLenum error;
    GLint binding = 0;

    glGenBuffers(1, &buffer);
    error = glGetError();
    if (buffer == 0 || error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "Buffer setup failed: 0x%x", error);
        return;
    }

    /* Test Case #001: support GL_SHADER_STORAGE_BUFFER and bind the supplied name. */
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    error = glGetError();
    if (error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
                      "glBindBuffer(GL_SHADER_STORAGE_BUFFER) generated 0x%x", error);
        return;
    }
    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, &binding);
    error = glGetError();
    if (error != GL_NO_ERROR || (GLuint)binding != buffer)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
                      "Expected binding %u, got %u; GL error 0x%x",
                      buffer, (GLuint)binding, error);
        return;
    }
    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_GLES31_CS_BB_TP_001_draw(void) {}

void GS_GLES31_CS_BB_TP_001_close(void)
{
    if (buffer != 0)
    {
        glDeleteBuffers(1, &buffer);
        buffer = 0;
    }
}
