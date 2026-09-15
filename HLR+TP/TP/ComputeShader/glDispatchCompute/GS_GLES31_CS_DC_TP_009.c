#include <string.h>

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
 * GS_GLES31_CS_DC_TP_009
 * HLR: GS-GLES31-CS-DC-008.
 * Prove execution with (1,1,1), then verify zero X/Y/Z preserves sentinels.
 *
 * Runner: current OpenGL ES >= 3.1 context, loaded entry points, clean GL
 * error state. Call init/draw/close in order, even after failed setup, and
 * keep the same context current through close. No helper module is used.
 * Program/pipeline and SSBO bindings used below are changed by this test;
 * the next TP must establish its own bindings. Only owned resources are deleted.
 */
static const char *test_case = "GS_GLES31_CS_DC_TC_008";
static const char *test_procedure = "GS_GLES31_CS_DC_TP_009";
static GLuint shader, program, output_buffer;
static const GLuint sentinels[4] = {
    0xAAAAAAAAu, 0xBBBBBBBBu, 0xCCCCCCCCu, 0xDDDDDDDDu
};
static const GLuint executed_values[4] = {0x12345678u, 0u, 0u, 0u};

static const char *compute_shader_source =
    "#version 310 es\n"
    "layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
    "layout(std430, binding = 0) buffer OutputBuffer { uint data[]; };\n"
    "void main() {\n"
    "    data[0] = 0x12345678u;\n"
    "    data[1] = gl_GlobalInvocationID.x;\n"
    "    data[2] = gl_GlobalInvocationID.y;\n"
    "    data[3] = gl_GlobalInvocationID.z;\n"
    "}\n";

void GS_GLES31_CS_DC_TP_009_init(void)
{
    GLint status = GL_FALSE;
    GLenum error;
    void *mapped;
    GLboolean unmapped;
    GLboolean success = GL_FALSE;
    const char *failure = "Test setup did not complete";
    char info_log[512] = {0};
    const GLuint groups[4][3] = {{1u,1u,1u}, {0u,1u,1u}, {1u,0u,1u}, {1u,1u,0u}};
    const char *labels[] = {"positive control", "zero X", "zero Y", "zero Z"};
    GLuint data[4];
    GLuint index, case_index;

    shader = glCreateShader(GL_COMPUTE_SHADER);
    if (shader == 0)
    {
        failure = "Could not create the compute shader";
        goto report;
    }
    glShaderSource(shader, 1, &compute_shader_source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        glGetShaderInfoLog(shader, (GLsizei)sizeof(info_log), NULL, info_log);
        TEST_LOG_INFO("Compute shader compilation: %s", info_log);
        failure = "Compute shader compilation failed";
        goto report;
    }

    program = glCreateProgram();
    if (program == 0)
    {
        failure = "Could not create the compute program";
        goto report;
    }
    glAttachShader(program, shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        glGetProgramInfoLog(program, (GLsizei)sizeof(info_log), NULL, info_log);
        TEST_LOG_INFO("Compute program linking: %s", info_log);
        failure = "Compute program linking failed";
        goto report;
    }

    glBindProgramPipeline(0);
    glUseProgram(program);

    memcpy(data, sentinels, sizeof(data));
    glGenBuffers(1, &output_buffer);
    if (output_buffer == 0)
    {
        failure = "Could not create the output buffer";
        goto report;
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, output_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizeiptr)sizeof(data),
                 data, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, output_buffer);
    if (glGetError() != GL_NO_ERROR)
    {
        failure = "Program/SSBO setup failed";
        goto report;
    }

    success = GL_TRUE;
    for (case_index = 0; case_index < 4; ++case_index)
    {
        GLboolean case_success = GL_FALSE;
        const GLuint *expected = case_index == 0 ? executed_values : sentinels;

        /* Synchronize before replacing data written by the preceding case. */
        glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, output_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, output_buffer);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(sentinels), sentinels);
        if (glGetError() != GL_NO_ERROR)
        {
            failure = "SSBO reset failed";
            goto case_end;
        }
        /* Test Case #008: positive control first, then no execution for zero X/Y/Z. */
        glDispatchCompute(groups[case_index][0], groups[case_index][1], groups[case_index][2]);
        error = glGetError();
        if (error != GL_NO_ERROR)
        {
            TEST_LOG_INFO("%s: dispatch error 0x%x", labels[case_index], error);
            failure = "Valid or zero-count dispatch generated an error";
            goto case_end;
        }
        glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
        mapped = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0,
                                 (GLsizeiptr)sizeof(data), GL_MAP_READ_BIT);
        error = glGetError();
        if (mapped == NULL || error != GL_NO_ERROR)
        {
            if (mapped != NULL)
                (void)glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            failure = "Could not map the compute output";
            goto case_end;
        }
        memcpy(data, mapped, sizeof(data));
        unmapped = glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        if (glGetError() != GL_NO_ERROR || unmapped != GL_TRUE)
        {
            failure = "Compute output became invalid during unmap";
            goto case_end;
        }

        case_success = GL_TRUE;
        for (index = 0; index < 4; ++index)
        {
            if (data[index] != expected[index])
            {
                TEST_LOG_INFO("%s: value %u expected 0x%x, got 0x%x",
                              labels[case_index], index, expected[index], data[index]);
                failure = "Compute output did not match the dispatch scenario";
                case_success = GL_FALSE;
            }
        }
case_end:
        while ((error = glGetError()) != GL_NO_ERROR)
        {
            case_success = GL_FALSE;
            failure = "Additional GL error in the dispatch scenario";
            TEST_LOG_INFO("%s: additional error 0x%x", labels[case_index], error);
        }
        if (!case_success)
        {
            success = GL_FALSE;
            TEST_LOG_INFO("%s failed: %s", labels[case_index], failure);
            /* A no-op shader must never validate the zero-work tests. */
            if (case_index == 0)
                goto report;
        }
    }

report:
    if (success)
        TEST_LOG_SUCCESS(test_case, test_procedure);
    else
        TEST_LOG_FAIL(test_case, test_procedure, "%s", failure);
}

void GS_GLES31_CS_DC_TP_009_draw(void) {}

void GS_GLES31_CS_DC_TP_009_close(void)
{

    if (shader == 0 && program == 0 && output_buffer == 0)
        return;
    if (output_buffer != 0)
    {
        glDeleteBuffers(1, &output_buffer);
        output_buffer = 0;
    }
    if (program != 0)
    {
        glUseProgram(0);
        glDeleteProgram(program);
        program = 0;
    }
    if (shader != 0)
    {
        glDeleteShader(shader);
        shader = 0;
    }
}
