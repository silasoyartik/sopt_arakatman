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
 * GS_GLES31_CS_BD_TP_002
 * Covered requirement: GS-GLES31-CS-BD-002.
 * Accept GL_DYNAMIC_COPY for the SSBO target and verify GL_BUFFER_USAGE.
 *
 * Runner: current GLES >= 3.1 context, loaded entry points and clean GL
 * error state. Call init/draw/close in order, including close after setup
 * failure, keeping the same context current. No helper or main is used.
 * Buffer bindings changed below are not restored; the next TP sets its own.
 */
static const char *test_case = "GS_GLES31_CS_BD_TC_002";
static const char *test_procedure = "GS_GLES31_CS_BD_TP_002";
static GLuint buffer;

void GS_GLES31_CS_BD_TP_002_init(void)
{
    const GLsizeiptr size = 64;
    GLint actual_usage = -1;
    GLenum error;

    glGenBuffers(1, &buffer);
    if (buffer != 0)
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    error = glGetError();
    if (buffer == 0 || error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "SSBO setup failed: 0x%x", error);
        return;
    }

    /* Test Case #002: accept GL_DYNAMIC_COPY and record it as GL_BUFFER_USAGE. */
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_COPY);
    error = glGetError();
    if (error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
                      "glBufferData(GL_DYNAMIC_COPY) generated 0x%x", error);
        return;
    }
    glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_USAGE, &actual_usage);
    error = glGetError();
    if (error != GL_NO_ERROR || actual_usage != (GLint)GL_DYNAMIC_COPY)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
                      "Expected usage 0x%x, got 0x%x; GL error 0x%x",
                      GL_DYNAMIC_COPY, (GLuint)actual_usage, error);
        return;
    }
    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_GLES31_CS_BD_TP_002_draw(void) {}

void GS_GLES31_CS_BD_TP_002_close(void)
{
    if (buffer != 0)
    {
        glDeleteBuffers(1, &buffer);
        buffer = 0;
    }
}
