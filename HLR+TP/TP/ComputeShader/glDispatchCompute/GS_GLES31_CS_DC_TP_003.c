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
 * GS_GLES31_CS_DC_TP_003
 * HLR: GS-GLES31-CS-DC-010.
 * Dispatch (2,1,1), local (4,2,1): check linked size and all local invocations.
 *
 * Runner: current OpenGL ES >= 3.1 context, loaded entry points, clean GL
 * error state. Call init/draw/close in order, even after failed setup, and
 * keep the same context current through close. This TP uses no helper module.
 * It changes program/pipeline and any SSBO bindings used below; the next TP
 * must establish its own bindings. Only resources owned by this TP are deleted.
 */
#define TP003_LOCAL_X 4u
#define TP003_LOCAL_Y 2u
#define TP003_LOCAL_Z 1u
#define TP003_GROUPS_X 2u
#define TP003_GROUPS_Y 1u
#define TP003_GROUPS_Z 1u
#define TP003_GLOBAL_X (TP003_GROUPS_X * TP003_LOCAL_X)
#define TP003_GLOBAL_Y (TP003_GROUPS_Y * TP003_LOCAL_Y)
#define TP003_GLOBAL_Z (TP003_GROUPS_Z * TP003_LOCAL_Z)
#define TP003_TOTAL_INVOCATIONS (TP003_GLOBAL_X * TP003_GLOBAL_Y * TP003_GLOBAL_Z)
#define TP003_VALUES_PER_INVOCATION 5u
#define TP003_BUFFER_VALUES (TP003_TOTAL_INVOCATIONS * TP003_VALUES_PER_INVOCATION)
#define TP003_MARKER_BASE 0x31000000u

static const char *test_case = "GS_GLES31_CS_DC_TC_010";
static const char *test_procedure = "GS_GLES31_CS_DC_TP_003";
static GLuint shader, program, output_buffer;

static const char *compute_shader_source =
    "#version 310 es\n"
    "layout(local_size_x = 4, local_size_y = 2, local_size_z = 1) in;\n"
    "layout(std430, binding = 0) buffer OutputBuffer { uint data[]; };\n"
    "void main()\n"
    "{\n"
    "    uvec3 id = gl_GlobalInvocationID;\n"
    "    if (id.x >= 8u || id.y >= 2u || id.z >= 1u) return;\n"
    "    uint linear_index = id.x + 8u * id.y;\n"
    "    uint base = linear_index * 5u;\n"
    "    data[base + 0u] = gl_LocalInvocationID.x;\n"
    "    data[base + 1u] = gl_LocalInvocationID.y;\n"
    "    data[base + 2u] = gl_LocalInvocationID.z;\n"
    "    data[base + 3u] = gl_WorkGroupID.x;\n"
    "    data[base + 4u] = 0x31000000u + linear_index;\n"
    "}\n";


void GS_GLES31_CS_DC_TP_003_init(void)
{
    const GLubyte *version;
    GLint major = 0, minor = 0;
    GLenum error;
    GLboolean success = GL_FALSE;
    const char *failure = "Test setup did not complete";
    GLint status = GL_FALSE;
    GLint active_program = 0, active_pipeline = 0;
    char info_log[512] = {0};
    void *mapped;
    GLboolean unmapped;
    GLint limits[3] = {0, 0, 0};
    GLuint data[TP003_BUFFER_VALUES];
    GLint reported_local_size[3] = {0, 0, 0};
    GLuint group_x, local_x, local_y, local_z, index;


    if (shader != 0 || program != 0 || output_buffer != 0)
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
    if (glGetError() != GL_NO_ERROR || limits[0] < (GLint)(TP003_GROUPS_X) ||
        limits[1] < (GLint)(TP003_GROUPS_Y) || limits[2] < (GLint)(TP003_GROUPS_Z))
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

    /* Test Case #010: verify the linked local size, then actual local invocations. */
    glGetProgramiv(program, GL_COMPUTE_WORK_GROUP_SIZE, reported_local_size);
    if (glGetError() != GL_NO_ERROR ||
        reported_local_size[0] != (GLint)TP003_LOCAL_X ||
        reported_local_size[1] != (GLint)TP003_LOCAL_Y ||
        reported_local_size[2] != (GLint)TP003_LOCAL_Z)
    {
        failure = "Linked compute local size does not match (4,2,1)";
        goto report;
    }

    for (index = 0; index < TP003_BUFFER_VALUES; ++index)
        data[index] = 0xFFFFFFFFu;

    glGenBuffers(1, &output_buffer);
    if (glGetError() != GL_NO_ERROR || output_buffer == 0)
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
        failure = "Could not initialize and bind the output SSBO";
        goto report;
    }

    glDispatchCompute(TP003_GROUPS_X, TP003_GROUPS_Y, TP003_GROUPS_Z);
    error = glGetError();
    if (error != GL_NO_ERROR)
    {
        TEST_LOG_INFO("Dispatch: expected error 0x%x, got 0x%x", GL_NO_ERROR, error);
        failure = "Unexpected dispatch error result";
        goto report;
    }
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

    success = GL_TRUE;
    failure = "Compute output did not match the expected invocation records";
    for (group_x = 0; group_x < TP003_GROUPS_X; ++group_x)
    {
        for (local_z = 0; local_z < TP003_LOCAL_Z; ++local_z)
        {
            for (local_y = 0; local_y < TP003_LOCAL_Y; ++local_y)
            {
                for (local_x = 0; local_x < TP003_LOCAL_X; ++local_x)
                {
                    const GLuint global_x = group_x * TP003_LOCAL_X + local_x;
                    const GLuint linear_index = global_x +
                        TP003_GLOBAL_X * (local_y + TP003_GLOBAL_Y * local_z);
                    const GLuint base = linear_index * TP003_VALUES_PER_INVOCATION;
                    const GLuint expected[] = {
                        local_x, local_y, local_z, group_x, TP003_MARKER_BASE + linear_index
                    };
                    static const char *fields[] = {
                        "LocalInvocationID.x", "LocalInvocationID.y", "LocalInvocationID.z",
                        "WorkGroupID.x", "marker"
                    };

                    for (index = 0; index < TP003_VALUES_PER_INVOCATION; ++index)
                    {
                        if (data[base + index] != expected[index])
                        {
                            TEST_LOG_INFO("Invocation %u %s: expected %u, got %u", linear_index, fields[index], expected[index], data[base + index]);
                            success = GL_FALSE;
                        }
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

void GS_GLES31_CS_DC_TP_003_draw(void) {}

void GS_GLES31_CS_DC_TP_003_close(void)
{
    GLenum error;

    /* Partial setup and repeated close are safe. No calls before loading
     * the API: nonzero handles exist only after entry-point validation. */
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
    while ((error = glGetError()) != GL_NO_ERROR)
        TEST_LOG_FAIL(test_case, test_procedure,
                      "GL error during cleanup: 0x%x", error);
}
