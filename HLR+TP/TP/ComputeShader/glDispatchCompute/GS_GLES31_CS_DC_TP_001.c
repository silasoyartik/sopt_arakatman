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
 * GS_GLES31_CS_DC_TP_001
 * Covered requirements:
 *   GS-GLES31-CS-DC-001: launch work groups and execute the compute shader.
 *   GS-GLES31-CS-DC-002: execute the active compute shader program.
 *   GS-GLES31-CS-DC-013: accept valid dispatch counts without a GL error.
 *
 * Runner preconditions: a current OpenGL ES >= 3.1 context and loaded GLES
 * entry points. No helpers_gles31 implementation or GLFW backend is used.
 * Call init/draw/close in order, including close after failed initialization;
 * keep the same context current through close. The TP owns only its shader,
 * program and buffer. It changes the program, pipeline and SSBO binding 0;
 * the runner/next TP must establish its own bindings afterwards.
 *
 * Dispatch (4,1,1), local size (1,1,1), initial output all 0xFFFFFFFF:
 * the active executable must write exactly {1,2,3,4}.
 */
static const char *test_cases[] = {
    "GS_GLES31_CS_DC_TC_001",
    "GS_GLES31_CS_DC_TC_002",
    "GS_GLES31_CS_DC_TC_013"
};
static const char *test_procedure = "GS_GLES31_CS_DC_TP_001";
static GLuint shader;
static GLuint program;
static GLuint output_buffer;

static const char *compute_shader_source =
    "#version 310 es\n"
    "layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
    "layout(std430, binding = 0) buffer OutputBuffer { uint data[]; };\n"
    "void main()\n"
    "{\n"
    "    uvec3 id = gl_GlobalInvocationID;\n"
    "    if (id.x < 4u && id.y == 0u && id.z == 0u)\n"
    "        data[id.x] = id.x + 1u;\n"
    "}\n";

void GS_GLES31_CS_DC_TP_001_init(void)
{
    const GLuint expected[] = {1u, 2u, 3u, 4u};
    GLuint data[] = {0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu};
    GLint status = GL_FALSE;
    GLuint index;
    GLenum error;
    void *mapped;
    GLboolean unmapped;
    GLboolean dispatch_success = GL_FALSE;
    GLboolean execution_success = GL_FALSE;
    const char *failure = "Test setup did not complete";
    char info_log[512] = {0};

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

    /* HLR-013: inspect the dispatch error before any other GL operation. */
    /* Test Case #001: launch work groups and verify execution below. */
    /* Test Case #002: execute the active compute shader and verify its output. */
    /* Test Case #013: valid dispatch must generate no GL error. */
    glDispatchCompute(4, 1, 1);
    error = glGetError();
    if (error != GL_NO_ERROR)
    {
        TEST_LOG_INFO("glDispatchCompute(4,1,1) error: 0x%x", error);
        failure = "Valid compute dispatch generated a GL error";
        goto report;
    }
    dispatch_success = GL_TRUE;

    /* Make shader writes visible to CPU buffer mapping. Mapping waits for
     * the pending operations; no glFinish or desktop glGetBufferSubData. */
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
    mapped = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0,
                             (GLsizeiptr)sizeof(data), GL_MAP_READ_BIT);
    error = glGetError();
    if (mapped == NULL || error != GL_NO_ERROR)
    {
        if (mapped != NULL)
            (void)glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        failure = "Could not map the compute output";
        goto report;
    }
    memcpy(data, mapped, sizeof(data));
    unmapped = glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    if (glGetError() != GL_NO_ERROR || unmapped != GL_TRUE)
    {
        failure = "Compute output became invalid during unmap";
        goto report;
    }

    /* HLR-001/002: an error-free no-op cannot pass the output checks. */
    execution_success = GL_TRUE;
    for (index = 0; index < 4; ++index)
    {
        if (data[index] != expected[index])
        {
            TEST_LOG_INFO("Output[%u]: expected %u, got %u",
                          index, expected[index], data[index]);
            execution_success = GL_FALSE;
        }
    }
    failure = "The active compute executable did not produce {1,2,3,4}";

report:
    for (index = 0; index < 3; ++index)
    {
        /* A readback failure does not undo the independent HLR-013 check. */
        if ((index == 2 && dispatch_success) ||
            (index < 2 && execution_success))
            TEST_LOG_SUCCESS(test_cases[index], test_procedure);
        else
            TEST_LOG_FAIL(test_cases[index], test_procedure, "%s", failure);
    }
}

void GS_GLES31_CS_DC_TP_001_draw(void) {}

void GS_GLES31_CS_DC_TP_001_close(void)
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
