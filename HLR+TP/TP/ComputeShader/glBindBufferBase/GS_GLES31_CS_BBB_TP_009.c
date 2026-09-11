#ifdef GS_GLES31_API_HEADER
#include GS_GLES31_API_HEADER
#else
#include <GLES3/gl31.h>
#endif
#include <stdint.h>
#include <string.h>
#include "../../macros.h"

/* Covered requirement: GS-GLES31-CS-BBB-011
 * Query explicit and default block bindings immediately after link; no override is used.
 * Precondition: the harness supplies a fresh, isolated current context and
 * loaded entry points for this API. Context lifetime belongs to the harness.
 * This procedure creates and deletes only its own GL objects.
 */
static const char *test_case = "GS_GLES31_CS_BBB_TC_011";
static const char *test_procedure = "GS_GLES31_CS_BBB_TP_009";

/* Result values: -1 = not run, 0 = fail, 1 = pass. */
static int test_result = -1;

void GS_GLES31_CS_BBB_TP_009_init(void)
{
    GLenum actual_error, extra_error;
    GLuint shader = 0, program = 0;
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

    const char *sources[] = {"#version 310 es\nlayout(local_size_x=1) in;\n"
                             "layout(std430,binding=1) buffer Payload { uint value; };\n"
                             "void main(){ value=7u; }\n",
                             "#version 310 es\nlayout(local_size_x=1) in;\n"
                             "layout(std430) buffer Payload { uint value; };\n"
                             "void main(){ value=7u; }\n"};
    for (unsigned i = 0; i < 2; ++i)
    {
        GLint binding = -1;
        GLenum property = GL_BUFFER_BINDING;
        /* Compile and link the compute program used by this requirement. */
        {
            const GLchar *shader_source = sources[i];
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
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Program link failed");
            goto finish;
        }
        GLuint block = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "Payload");
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        if (block == GL_INVALID_INDEX)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Active storage block missing");
            goto finish;
        }
        glGetProgramResourceiv(program, GL_SHADER_STORAGE_BLOCK, block, 1, &property, 1, NULL,
                               &binding);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        if (!(binding == (i == 0 ? 1 : 0)))
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s",
                          "Linked block binding differs from explicit/default binding");
            goto finish;
        }
        glDeleteProgram(program);
        program = 0;
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

    test_result = 1;
finish:
    /* Release test-owned GL objects while the harness context is still current. */
    glUseProgram(0);
    if (shader != 0)
        glDeleteShader(shader);
    if (program != 0)
        glDeleteProgram(program);
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
void GS_GLES31_CS_BBB_TP_009_draw(void) {}
/* init releases resources on both success and failure; repeated close is harmless. */
void GS_GLES31_CS_BBB_TP_009_close(void) {}
int GS_GLES31_CS_BBB_TP_009_result(void) { return test_result; }
