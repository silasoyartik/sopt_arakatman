#ifdef GS_GLES31_API_HEADER
#include GS_GLES31_API_HEADER
#else
#include <GLES3/gl31.h>
#endif
#include <stdint.h>
#include <string.h>
#include "../../macros.h"

/* Covered requirement: GS-GLES31-CS-BBB-012
 * Route shader reads/writes to indexed buffer, with a distinct general binding as a decoy.
 * Precondition: the harness supplies a fresh, isolated current context and
 * loaded entry points for this API. Context lifetime belongs to the harness.
 * This procedure creates and deletes only its own GL objects.
 */
static const char *test_case = "GS_GLES31_CS_BBB_TC_012";
static const char *test_procedure = "GS_GLES31_CS_BBB_TP_010";

/* Result values: -1 = not run, 0 = fail, 1 = pass. */
static int test_result = -1;

void GS_GLES31_CS_BBB_TP_010_init(void)
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

    const char *source = "#version 310 es\nlayout(local_size_x=1) in;\n"
                         "layout(std430,binding=1) buffer Payload { uint words[4]; };\n"
                         "void main(){ words[1]=words[0]+37u; }\n";
    const GLuint first[4] = {101, 0, 301, 401}, second[4] = {202, 0, 302, 402};
    GLuint observed[4] = {0};
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
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "First buffer setup failed");
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
    glBufferData(GL_COPY_WRITE_BUFFER, sizeof(first), first, GL_STATIC_DRAW);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "First buffer setup failed", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    /* Prepare storage through a non-indexed target. */
    glGenBuffers(1, &buffers[1]);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    if (buffers[1] == 0)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Second buffer setup failed");
        goto finish;
    }
    glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[1]);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    glBufferData(GL_COPY_WRITE_BUFFER, sizeof(second), second, GL_STATIC_DRAW);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Second buffer setup failed", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
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
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Program setup failed");
        goto finish;
    }
    GLuint block = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "Payload");
    GLenum property = GL_BUFFER_DATA_SIZE;
    GLint required_size = 0;
    if (block == GL_INVALID_INDEX)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Active block missing");
        goto finish;
    }
    glGetProgramResourceiv(program, GL_SHADER_STORAGE_BLOCK, block, 1, &property, 1, NULL,
                           &required_size);
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
    if (!(required_size > 0 && required_size <= (GLint)sizeof(first)))
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s",
                      "Fixture smaller than shader block requirement");
        goto finish;
    }
    for (unsigned selected = 0; selected < 2; ++selected)
    {
        /* Restore known content without modifying indexed bindings. */
        glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[0]);
        glBufferSubData(GL_COPY_WRITE_BUFFER, 0, sizeof(first), first);
        glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[1]);
        glBufferSubData(GL_COPY_WRITE_BUFFER, 0, sizeof(second), second);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffers[selected]);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[1 - selected]);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
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
        for (unsigned i = 0; i < 2; ++i)
        {
            GLuint expected[4];
            memcpy(expected, i == 0 ? first : second, sizeof(expected));
            if (i == selected)
                expected[1] = expected[0] + 37u;
            /* Observe buffer bytes by mapping, independently of the function under test. */
            glBindBuffer(GL_COPY_READ_BUFFER, buffers[i]);
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
                    glMapBufferRange(GL_COPY_READ_BUFFER, 0, sizeof(observed), GL_MAP_READ_BIT);
                if (read_pointer != NULL)
                    mapped_target = GL_COPY_READ_BUFFER;
                actual_error = glGetError();
                extra_error = glGetError();
                if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
                {
                    TEST_LOG_FAIL(test_case, test_procedure,
                                  "%s (expected 0x%x, got 0x%x, extra 0x%x)", "Unexpected GL error",
                                  (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                                  (unsigned)extra_error);
                    goto finish;
                }
                if (read_pointer == NULL)
                {
                    TEST_LOG_FAIL(test_case, test_procedure, "%s", "Readback failed");
                    goto finish;
                }
                memcpy(observed, read_pointer, (size_t)(sizeof(observed)));
                GLboolean intact = glUnmapBuffer(GL_COPY_READ_BUFFER);
                mapped_target = 0;
                actual_error = glGetError();
                extra_error = glGetError();
                if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
                {
                    TEST_LOG_FAIL(test_case, test_procedure,
                                  "%s (expected 0x%x, got 0x%x, extra 0x%x)", "Unexpected GL error",
                                  (unsigned)GL_NO_ERROR, (unsigned)actual_error,
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
            if (!(memcmp(expected, observed, sizeof(expected)) == 0))
            {
                TEST_LOG_FAIL(test_case, test_procedure, "%s",
                              "Shader used wrong binding/buffer or changed unrelated data");
                goto finish;
            }
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
void GS_GLES31_CS_BBB_TP_010_draw(void) {}
/* init releases resources on both success and failure; repeated close is harmless. */
void GS_GLES31_CS_BBB_TP_010_close(void) {}
int GS_GLES31_CS_BBB_TP_010_result(void) { return test_result; }
