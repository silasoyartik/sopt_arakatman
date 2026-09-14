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
 * GS_GLES31_CS_DC_TP_010
 * HLR: GS-GLES31-CS-DC-012.
 * Use two SSBO pairs at bindings 0/1; verify selected outputs and preserved inputs.
 *
 * Runner: current OpenGL ES >= 3.1 context, loaded entry points, clean GL
 * error state. Call init/draw/close in order, even after failed setup, and
 * keep the same context current through close. No helper module is used.
 * Program/pipeline and SSBO bindings used below are changed by this test;
 * the next TP must establish its own bindings. Only owned resources are deleted.
 */
static const char *test_case = "GS_GLES31_CS_DC_TC_012";
static const char *test_procedure = "GS_GLES31_CS_DC_TP_010";
static GLuint shader, program;
/* Input A, input B, output A, output B. */
static GLuint buffers[4];
static const GLuint input_values[2][4] = {{7u,11u,19u,23u}, {31u,37u,41u,47u}};
static const GLuint sentinels[4] = {
    0xAAAAAAAAu, 0xBBBBBBBBu, 0xCCCCCCCCu, 0xDDDDDDDDu
};

static const char *compute_shader_source =
    "#version 310 es\n"
    "layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
    "layout(std430, binding = 0) readonly buffer InputBuffer { uint src[]; };\n"
    "layout(std430, binding = 1) writeonly buffer OutputBuffer { uint dst[]; };\n"
    "void main() {\n"
    "    uint i = gl_GlobalInvocationID.x;\n"
    "    if (i < 4u) dst[i] = src[i] * 3u + 7u;\n"
    "}\n";


void GS_GLES31_CS_DC_TP_010_init(void)
{
    const GLubyte *version;
    GLint major = 0, minor = 0, status = GL_FALSE;
    GLint active_program = 0, active_pipeline = 0;
    GLint limits[3] = {0, 0, 0};
    GLenum error;
    void *mapped;
    GLboolean unmapped;
    GLboolean success = GL_FALSE;
    const char *failure = "Test setup did not complete";
    char info_log[512] = {0};
    GLuint data[4];
    GLuint expected_outputs[2][4];
    GLuint index, pair, buffer_index;

    if (shader != 0 || program != 0 || buffers[0] != 0 || buffers[1] != 0 || buffers[2] != 0 || buffers[3] != 0)
    {
        failure = "Previous run must be closed before init is called again";
        goto report;
    }
#if defined(GS_GLES31_USE_GLAD1) || defined(GS_GLES31_USE_GLAD2)
    if (glAttachShader == NULL ||
        glBindBuffer == NULL ||
        glBindBufferBase == NULL ||
        glBindProgramPipeline == NULL ||
        glBufferData == NULL ||
        glCompileShader == NULL ||
        glCreateProgram == NULL ||
        glCreateShader == NULL ||
        glDeleteBuffers == NULL ||
        glDeleteProgram == NULL ||
        glDeleteShader == NULL ||
        glDispatchCompute == NULL ||
        glGenBuffers == NULL ||
        glGetError == NULL ||
        glGetIntegeri_v == NULL ||
        glGetIntegerv == NULL ||
        glGetProgramInfoLog == NULL ||
        glGetProgramiv == NULL ||
        glGetShaderInfoLog == NULL ||
        glGetShaderiv == NULL ||
        glGetString == NULL ||
        glLinkProgram == NULL ||
        glMapBufferRange == NULL ||
        glMemoryBarrier == NULL ||
        glShaderSource == NULL ||
        glUnmapBuffer == NULL ||
        glUseProgram == NULL)
    {
        failure = "Runner has not loaded all required GLES entry points";
        goto report;
    }
#endif

    version = glGetString(GL_VERSION);
    if (version == NULL || strncmp((const char *)version, "OpenGL ES ", 10) != 0)
    {
        failure = "Runner must provide a current OpenGL ES context";
        goto report;
    }
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    error = glGetError();
    if (error != GL_NO_ERROR || major < 3 || (major == 3 && minor < 1))
    {
        failure = "GLES 3.1 or later and a clean initial GL error state are required";
        goto report;
    }

    for (index = 0; index < 3; ++index)
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, index, &limits[index]);
    if (glGetError() != GL_NO_ERROR || limits[0] < 4 ||
        limits[1] < 1 || limits[2] < 1)
    {
        failure = "Could not establish valid dispatch limits";
        goto report;
    }

    shader = glCreateShader(GL_COMPUTE_SHADER);
    if (glGetError() != GL_NO_ERROR || shader == 0)
    {
        failure = "Could not create the compute shader";
        goto report;
    }
    glShaderSource(shader, 1, &compute_shader_source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (glGetError() != GL_NO_ERROR || status != GL_TRUE)
    {
        glGetShaderInfoLog(shader, (GLsizei)sizeof(info_log), NULL, info_log);
        TEST_LOG_INFO("Compute shader compilation: %s", info_log);
        failure = "Compute shader compilation failed";
        goto report;
    }

    program = glCreateProgram();
    if (glGetError() != GL_NO_ERROR || program == 0)
    {
        failure = "Could not create the compute program";
        goto report;
    }
    glAttachShader(program, shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (glGetError() != GL_NO_ERROR || status != GL_TRUE)
    {
        glGetProgramInfoLog(program, (GLsizei)sizeof(info_log), NULL, info_log);
        TEST_LOG_INFO("Compute program linking: %s", info_log);
        failure = "Compute program linking failed";
        goto report;
    }

    glBindProgramPipeline(0);
    glUseProgram(program);
    glGetIntegerv(GL_CURRENT_PROGRAM, &active_program);
    glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &active_pipeline);
    if (glGetError() != GL_NO_ERROR || (GLuint)active_program != program ||
        active_pipeline != 0)
    {
        failure = "The linked compute program could not be made active";
        goto report;
    }

    glGenBuffers(4, buffers);
    if (glGetError() != GL_NO_ERROR || buffers[0] == 0 || buffers[1] == 0 ||
        buffers[2] == 0 || buffers[3] == 0)
    {
        failure = "Could not create both input/output SSBO pairs";
        goto report;
    }
    for (buffer_index = 0; buffer_index < 4; ++buffer_index)
    {
        const GLuint *initial = buffer_index < 2 ? input_values[buffer_index] : sentinels;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[buffer_index]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(data), initial,
                     buffer_index < 2 ? GL_STATIC_READ : GL_DYNAMIC_READ);
        if (glGetError() != GL_NO_ERROR)
        {
            failure = "Could not initialize an input/output SSBO";
            goto report;
        }
    }
    for (pair = 0; pair < 2; ++pair)
        for (index = 0; index < 4; ++index)
            expected_outputs[pair][index] = input_values[pair][index] * 3u + 7u;

    for (pair = 0; pair < 2; ++pair)
    {
        /* Change both indexed bindings; merely changing the generic target
         * binding does not select the shader's input/output resources. */
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[pair]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffers[pair + 2u]);
        if (glGetError() != GL_NO_ERROR)
        {
            failure = "Could not bind the input/output SSBO pair";
            goto report;
        }
        /* Test Case #012: verify access to the selected SSBO pair and preserved buffers. */
        glDispatchCompute(4, 1, 1);
        error = glGetError();
        if (error != GL_NO_ERROR)
        {
            TEST_LOG_INFO("Pair %u: dispatch error 0x%x", pair, error);
            failure = "Resource-access dispatch generated an error";
            goto report;
        }
        /* Check both inputs and outputs after each dispatch, including the
         * unused output B after A and preserved output A after B. */
        for (buffer_index = 0; buffer_index < 4; ++buffer_index)
        {
            const GLuint *expected = buffer_index < 2 ? input_values[buffer_index] :
                (buffer_index == 3 && pair == 0 ? sentinels : expected_outputs[buffer_index - 2u]);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[buffer_index]);
            glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
            if (glGetError() != GL_NO_ERROR)
            {
                failure = "Readback memory barrier failed";
                goto report;
            }
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

            for (index = 0; index < 4; ++index)
            {
                if (data[index] != expected[index])
                {
                    TEST_LOG_INFO("Pair %u, buffer %u, value %u: expected %u, got %u",
                                  pair, buffer_index, index, expected[index], data[index]);
                    failure = "Incorrect SSBO access or an unselected/input buffer was modified";
                    goto report;
                }
            }
        }
    }
    success = GL_TRUE;

report:
    if (success)
        TEST_LOG_SUCCESS(test_case, test_procedure);
    else
        TEST_LOG_FAIL(test_case, test_procedure, "%s", failure);
}

void GS_GLES31_CS_DC_TP_010_draw(void) {}

void GS_GLES31_CS_DC_TP_010_close(void)
{
    GLenum error;
    GLuint index;
    if (shader == 0 && program == 0 && buffers[0] == 0 && buffers[1] == 0 &&
        buffers[2] == 0 && buffers[3] == 0)
        return;
    for (index = 0; index < 4; ++index)
    {
        if (buffers[index] != 0)
        {
            glDeleteBuffers(1, &buffers[index]);
            buffers[index] = 0;
        }
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
    while ((error = glGetError()) != GL_NO_ERROR)
        TEST_LOG_FAIL(test_case, test_procedure, "GL error during cleanup: 0x%x", error);
}
