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
 * GS_GLES31_CS_BD_TP_001
 * Covered requirement: GS-GLES31-CS-BD-001.
 * Allocate the requested size in the generic SSBO binding, not the indexed binding.
 *
 * Runner: current GLES >= 3.1 context, loaded entry points and clean GL
 * error state. Call init/draw/close in order, including close after setup
 * failure, keeping the same context current. No helper or main is used.
 * Buffer bindings changed below are not restored; the next TP sets its own.
 */
static const char *test_case = "GS_GLES31_CS_BD_TC_001";
static const char *test_procedure = "GS_GLES31_CS_BD_TP_001";
static GLuint buffers[2];

void GS_GLES31_CS_BD_TP_001_init(void)
{
    const GLint initial_sizes[2] = {64, 32};
    const GLint requested_size = 256; /* Exactly representable by GLint. */
    GLint size = -1, indexed_binding = 0, generic_binding = 0;
    GLenum error;
    GLuint index;

    glGenBuffers(2, buffers);
    error = glGetError();
    if (buffers[0] == 0 || buffers[1] == 0 || buffers[0] == buffers[1] ||
        error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "Buffer setup failed: 0x%x", error);
        return;
    }
    /* Distinct initial sizes make an ignored allocation detectable. */
    for (index = 0; index < 2; ++index)
    {
        glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[index]);
        glBufferData(GL_COPY_WRITE_BUFFER, initial_sizes[index], NULL, GL_STATIC_DRAW);
    }
    /* BindBufferBase also sets the generic binding: select B after indexing A. */
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[0]);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[1]);
    glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, 0, &indexed_binding);
    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, &generic_binding);
    error = glGetError();
    if (error != GL_NO_ERROR || (GLuint)indexed_binding != buffers[0] ||
        (GLuint)generic_binding != buffers[1])
    {
        TEST_LOG_FAIL(test_case, test_procedure,
                      "Could not establish distinct indexed/generic bindings: 0x%x", error);
        return;
    }

    /* Test Case #001: allocate size bytes in the generically bound SSBO (B). */
    glBufferData(GL_SHADER_STORAGE_BUFFER, requested_size, NULL, GL_STATIC_DRAW);
    error = glGetError();
    if (error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "SSBO allocation generated 0x%x", error);
        return;
    }
    glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &size);
    error = glGetError();
    if (error != GL_NO_ERROR || size != requested_size)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
                      "Generic buffer size: expected %d, got %d; GL error 0x%x",
                      requested_size, size, error);
        return;
    }

    /* The other object (indexed A) must retain its original allocation. */
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[0]);
    size = -1;
    glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &size);
    error = glGetError();
    if (error != GL_NO_ERROR || size != initial_sizes[0])
    {
        TEST_LOG_FAIL(test_case, test_procedure,
                      "Indexed buffer size: expected %d, got %d; GL error 0x%x",
                      initial_sizes[0], size, error);
        return;
    }
    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_GLES31_CS_BD_TP_001_draw(void) {}

void GS_GLES31_CS_BD_TP_001_close(void)
{
    GLuint index;
    for (index = 0; index < 2; ++index)
    {
        if (buffers[index] != 0)
        {
            glDeleteBuffers(1, &buffers[index]);
            buffers[index] = 0;
        }
    }
}
