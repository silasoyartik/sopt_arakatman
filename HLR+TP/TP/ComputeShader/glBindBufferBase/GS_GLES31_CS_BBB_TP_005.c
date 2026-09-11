#ifdef GS_GLES31_API_HEADER
#include GS_GLES31_API_HEADER
#else
#include <GLES3/gl31.h>
#endif
#include <stdint.h>
#include <string.h>
#include "../../macros.h"

/* Covered requirement: GS-GLES31-CS-BBB-007
 * Observe zero start, whole-store shader access, and resize without indexed rebinding.
 * Precondition: the harness supplies a fresh, isolated current context and
 * loaded entry points for this API. Context lifetime belongs to the harness.
 * This procedure creates and deletes only its own GL objects.
 */
static const char *test_case = "GS_GLES31_CS_BBB_TC_007";
static const char *test_procedure = "GS_GLES31_CS_BBB_TP_005";

/* Result values: -1 = not run, 0 = fail, 1 = pass. */
static int test_result = -1;

void GS_GLES31_CS_BBB_TP_005_init(void)
{
    GLenum actual_error, extra_error;
    GLuint buffers[4] = {0};
    GLuint shader = 0, program = 0;
    GLenum mapped_target = 0;
    int cleanup_ok = 1;
    test_result = 0; /* No early exit may report success. */
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Pre-existing GL error before test setup", (unsigned)GL_NO_ERROR,
                      (unsigned)actual_error, (unsigned)extra_error);
        goto finish;
    }

    const char *source =
        "#version 310 es\nlayout(local_size_x=1) in;\n"
        "layout(std430,binding=0) buffer Payload { uint words[]; };\n"
        "void main(){ uint n=uint(words.length()); words[0]=n; words[n-1u]=79u; }\n";
    GLuint initial[8] = {0}, observed[8] = {0};
    GLint64 start = -1, binding_size = -1;
    GLint alignment = 0;
    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &alignment);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    if (!(alignment > 0 && (size_t)alignment <= (size_t)PTRDIFF_MAX - sizeof(initial)))
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Invalid/overflowing SSBO alignment");
        goto finish;
    }
    /* Prepare storage through a non-indexed target. */
    glGenBuffers(1, &buffers[0]);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    if (buffers[0] == 0)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Buffer setup failed");
        goto finish;
    }
    glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[0]);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    glBufferData(GL_COPY_WRITE_BUFFER, (GLsizeiptr)alignment + sizeof(initial), NULL,
                 GL_STATIC_DRAW);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Buffer setup failed", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    /* Compile and link the compute program used by this requirement. */
    {
        const GLchar *shader_source = source;
        GLint compiled = GL_FALSE, linked = GL_FALSE;
        GLchar info_log[1024] = {0};
        shader = glCreateShader(GL_COMPUTE_SHADER);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        if (shader == 0)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Could not create compute shader");
            goto finish;
        }
        glShaderSource(shader, 1, &shader_source, NULL);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        if (compiled != GL_TRUE)
        {
            glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
            TEST_LOG_FAIL(test_case, test_procedure, "Shader compilation failed: %s", info_log);
            goto finish;
        }
        program = glCreateProgram();
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        if (program == 0)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Could not create program");
            goto finish;
        }
        glAttachShader(program, shader);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        if (linked != GL_TRUE)
        {
            glGetProgramInfoLog(program, sizeof(info_log), NULL, info_log);
            TEST_LOG_FAIL(test_case, test_procedure, "Program link failed: %s", info_log);
            goto finish;
        }
        glDetachShader(program, shader);
        glDeleteShader(shader);
        shader = 0;
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
    }
    if (program == 0)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Compute program setup failed");
        goto finish;
    }
    /* A restricted baseline detects implementations that retain a prior range. */
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, buffers[0], alignment, 2 * sizeof(GLuint));
    glUseProgram(program);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    glGetInteger64i_v(GL_SHADER_STORAGE_BUFFER_START, 0, &start);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    if (start != alignment)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Nonzero start baseline missing");
        goto finish;
    }
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[0]);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    glGetInteger64i_v(GL_SHADER_STORAGE_BUFFER_START, 0, &start);
    glGetInteger64i_v(GL_SHADER_STORAGE_BUFFER_SIZE, 0, &binding_size);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    /* ES 3.1 section 6.6.1: queried size 0 means whole current store. */
    if (!(start == 0 && binding_size == 0))
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s",
                      "Whole-buffer binding sentinel/start incorrect");
        goto finish;
    }
    for (unsigned count = 4; count <= 8; count += 4)
    {
        glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[0]);
        glBufferData(GL_COPY_WRITE_BUFFER, count * sizeof(GLuint), initial, GL_STATIC_DRAW);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        } /* No glBindBufferBase after resizing. */
        glDispatchCompute(1, 1, 1);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        /* Observe buffer bytes by mapping, independently of the function under test. */
        glBindBuffer(GL_COPY_READ_BUFFER, buffers[0]);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        {
            void *read_pointer =
                glMapBufferRange(GL_COPY_READ_BUFFER, 0, count * sizeof(GLuint), GL_MAP_READ_BIT);
            if (read_pointer != NULL)
                mapped_target = GL_COPY_READ_BUFFER;
            actual_error = glGetError();
            extra_error = glGetError();
            if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
            {
                TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                              "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                              (unsigned)extra_error);
                goto finish;
            }
            if (read_pointer == NULL)
            {
                TEST_LOG_FAIL(test_case, test_procedure, "%s", "Readback failed");
                goto finish;
            }
            memcpy(observed, read_pointer, (size_t)(count * sizeof(GLuint)));
            GLboolean intact = glUnmapBuffer(GL_COPY_READ_BUFFER);
            mapped_target = 0;
            actual_error = glGetError();
            extra_error = glGetError();
            if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
            {
                TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                              "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                              (unsigned)extra_error);
                goto finish;
            }
            if (intact != GL_TRUE)
            {
                TEST_LOG_FAIL(test_case, test_procedure, "%s",
                              "Buffer contents invalidated during unmap");
                goto finish;
            }
        }
        if (!(observed[0] == count && observed[count - 1] == 79u))
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s",
                          "Whole store/updated size not accessible");
            goto finish;
        }
        for (unsigned i = 1; i + 1 < count; ++i)
            if (!(observed[i] == 0))
            {
                TEST_LOG_FAIL(test_case, test_procedure, "%s",
                              "Unexpected write outside selected elements");
                goto finish;
            }
    }

    test_result = 1;
finish:
    /* Release test-owned GL objects while the harness context is still current. */
    if (mapped_target != 0)
    {
        if (glUnmapBuffer(mapped_target) != GL_TRUE)
        {
            cleanup_ok = 0;
            TEST_LOG_FAIL(test_case, test_procedure, "Cleanup unmap failed");
        }
    }
    glUseProgram(0);
    if (shader != 0)
        glDeleteShader(shader);
    if (program != 0)
        glDeleteProgram(program);
    glDeleteBuffers(4, buffers);
    {
        GLenum cleanup_error = glGetError();
        GLenum extra_error = glGetError();
        if (cleanup_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            cleanup_ok = 0;
            TEST_LOG_FAIL(test_case, test_procedure, "Cleanup GL error: 0x%x / 0x%x",
                          (unsigned)cleanup_error, (unsigned)extra_error);
        }
    }
    if (!cleanup_ok)
        test_result = 0;
    if (test_result == 1)
        TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_GLES31_CS_BBB_TP_005_draw(void) {}
/* init releases resources on both success and failure; repeated close is harmless. */
void GS_GLES31_CS_BBB_TP_005_close(void) {}
int GS_GLES31_CS_BBB_TP_005_result(void) { return test_result; }
