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
 * GS_GLES31_CS_DC_TP_002
 * HLR: GS-GLES31-CS-DC-009.
 * Dispatch (2,3,2), local (1,1,1): check group coordinates and atomic visits.
 *
 * Runner: current OpenGL ES >= 3.1 context, loaded entry points, clean GL
 * error state. Call init/draw/close in order, even after failed setup, and
 * keep the same context current through close. This TP uses no helper module.
 * It changes program/pipeline and any SSBO bindings used below; the next TP
 * must establish its own bindings. Only resources owned by this TP are deleted.
 */
#define TP002_GROUPS_X 2u
#define TP002_GROUPS_Y 3u
#define TP002_GROUPS_Z 2u
#define TP002_TOTAL_GROUPS (TP002_GROUPS_X * TP002_GROUPS_Y * TP002_GROUPS_Z)
#define TP002_VALUES_PER_GROUP 8u
#define TP002_BUFFER_VALUES (1u + TP002_TOTAL_GROUPS * TP002_VALUES_PER_GROUP)
#define TP002_MARKER_BASE 0xC0DE0000u

static const char *test_case = "GS_GLES31_CS_DC_TC_009";
static const char *test_procedure = "GS_GLES31_CS_DC_TP_002";
static GLuint shader, program, output_buffer;

static const char *compute_shader_source =
    "#version 310 es\n"
    "layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
    "layout(std430, binding = 0) buffer OutputBuffer { uint data[]; };\n"
    "void main()\n"
    "{\n"
    "    atomicAdd(data[0], 1u);\n"
    "    uvec3 id = gl_WorkGroupID;\n"
    "    if (id.x >= 2u || id.y >= 3u || id.z >= 2u) return;\n"
    "    uint linear_index = id.x + 2u * (id.y + 3u * id.z);\n"
    "    uint base = 1u + linear_index * 8u;\n"
    "    if (atomicAdd(data[base], 1u) != 0u) return;\n"
    "    data[base + 1u] = gl_NumWorkGroups.x;\n"
    "    data[base + 2u] = gl_NumWorkGroups.y;\n"
    "    data[base + 3u] = gl_NumWorkGroups.z;\n"
    "    data[base + 4u] = id.x;\n"
    "    data[base + 5u] = id.y;\n"
    "    data[base + 6u] = id.z;\n"
    "    data[base + 7u] = 0xC0DE0000u + linear_index;\n"
    "}\n";

void GS_GLES31_CS_DC_TP_002_init(void)
{
    GLenum error;
    GLboolean success = GL_FALSE;
    const char *failure = "Test setup did not complete";
    GLint status = GL_FALSE;
    char info_log[512] = {0};
    void *mapped;
    GLboolean unmapped;
    GLuint data[TP002_BUFFER_VALUES];
    GLuint x, y, z, index;

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

    for (index = 0; index < TP002_BUFFER_VALUES; ++index)
        data[index] = 0xFFFFFFFFu;
    data[0] = 0u;
    for (index = 0; index < TP002_TOTAL_GROUPS; ++index)
        data[1u + index * TP002_VALUES_PER_GROUP] = 0u;

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

    /* Test Case #009: verify dispatched group counts, coordinates and visits. */
    glDispatchCompute(TP002_GROUPS_X, TP002_GROUPS_Y, TP002_GROUPS_Z);
    error = glGetError();
    if (error != GL_NO_ERROR)
    {
        TEST_LOG_INFO("Dispatch: expected error 0x%x, got 0x%x", GL_NO_ERROR, error);
        failure = "Unexpected dispatch error result";
        goto report;
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
        goto report;
    }
    memcpy(data, mapped, sizeof(data));
    unmapped = glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    if (glGetError() != GL_NO_ERROR || unmapped != GL_TRUE)
    {
        failure = "Compute output became invalid during unmap";
        goto report;
    }

    success = GL_TRUE;
    failure = "Compute output did not match the expected invocation records";
    if (data[0] != TP002_TOTAL_GROUPS)
    {
        TEST_LOG_INFO("Expected %u executed groups, got %u", TP002_TOTAL_GROUPS, data[0]);
        success = GL_FALSE;
    }
    for (z = 0; z < TP002_GROUPS_Z; ++z)
    {
        for (y = 0; y < TP002_GROUPS_Y; ++y)
        {
            for (x = 0; x < TP002_GROUPS_X; ++x)
            {
                const GLuint linear_index = x + TP002_GROUPS_X * (y + TP002_GROUPS_Y * z);
                const GLuint base = 1u + linear_index * TP002_VALUES_PER_GROUP;
                const GLuint expected[] = {
                    1u, TP002_GROUPS_X, TP002_GROUPS_Y, TP002_GROUPS_Z,
                    x, y, z, TP002_MARKER_BASE + linear_index
                };
                static const char *fields[] = {
                    "visits", "NumWorkGroups.x", "NumWorkGroups.y", "NumWorkGroups.z",
                    "WorkGroupID.x", "WorkGroupID.y", "WorkGroupID.z", "marker"
                };

                for (index = 0; index < TP002_VALUES_PER_GROUP; ++index)
                {
                    if (data[base + index] != expected[index])
                    {
                        TEST_LOG_INFO("Group (%u,%u,%u) %s: expected %u, got %u", x, y, z, fields[index], expected[index], data[base + index]);
                        success = GL_FALSE;
                    }
                }
            }
        }
    }

report:
    if (success)
        TEST_LOG_SUCCESS(test_case, test_procedure);
    else
        TEST_LOG_FAIL(test_case, test_procedure, "%s", failure);
}

void GS_GLES31_CS_DC_TP_002_draw(void) {}

void GS_GLES31_CS_DC_TP_002_close(void)
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
