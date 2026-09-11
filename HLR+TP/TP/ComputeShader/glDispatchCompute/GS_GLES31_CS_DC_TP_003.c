#include "../../helpers_gles31.h"

/*
 * GS_GLES31_CS_DC_TP_003
 *
 * HLR: GS-GLES31-CS-DC-010.
 *
 * Verify the linked local size (4,2,1), then dispatch (2,1,1) and observe
 * all eight local invocation coordinates independently in both work groups.
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

static GS_GLES31_TestResult result =
    GS_GLES31_RESULT("GS_GLES31_CS_DC_TC_010", "GS_GLES31_CS_DC_TP_003");
static GS_GLES31_TestFixture fixture = GS_GLES31_FIXTURE_INITIALIZER;

/* Each record contains LocalInvocationID.xyz, WorkGroupID.x, marker. */
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
    GLuint data[TP003_BUFFER_VALUES];
    GLuint output_buffer;
    GLint reported_local_size[3] = {0, 0, 0};
    GLuint group_x, local_x, local_y, local_z, index;

    GS_GLES31_reset_results(&result, 1);
    if (!GS_GLES31_begin_fixture(&fixture) ||
        !GS_GLES31_fixture_compute_program(&fixture, compute_shader_source))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "%s",
                              "Could not prepare the compute test fixture");
        goto report;
    }

    GS_GLES31_clear_errors();
    glGetProgramiv(fixture.program, GL_COMPUTE_WORK_GROUP_SIZE, reported_local_size);
    if (!GS_GLES31_EXPECT_ERROR(&result, GL_NO_ERROR, "Query linked compute local size"))
        goto report;

    GS_GLES31_CHECK(&result,
                   reported_local_size[0] == (GLint)TP003_LOCAL_X &&
                   reported_local_size[1] == (GLint)TP003_LOCAL_Y &&
                   reported_local_size[2] == (GLint)TP003_LOCAL_Z,
                   "Expected linked local size (%u,%u,%u), got (%d,%d,%d)",
                   TP003_LOCAL_X, TP003_LOCAL_Y, TP003_LOCAL_Z,
                   reported_local_size[0], reported_local_size[1], reported_local_size[2]);

    GS_GLES31_fill_uint(data, TP003_BUFFER_VALUES, 0xFFFFFFFFu);
    output_buffer = GS_GLES31_fixture_ssbo(
        &fixture, 0, (GLsizeiptr)sizeof(data), data, GL_DYNAMIC_READ);
    if (output_buffer == 0)
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "%s", "Could not create the output SSBO");
        goto report;
    }

    GS_GLES31_clear_errors();
    glDispatchCompute(TP003_GROUPS_X, TP003_GROUPS_Y, TP003_GROUPS_Z);
    if (!GS_GLES31_EXPECT_ERROR(&result, GL_NO_ERROR, "glDispatchCompute(2,1,1)"))
        goto report;

    if (!GS_GLES31_read_ssbo(output_buffer, (GLsizeiptr)sizeof(data), data))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "%s", "Could not read compute output");
        goto report;
    }

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
                        GS_GLES31_CHECK(&result, data[base + index] == expected[index],
                                       "Invocation %u %s: expected %u, got %u",
                                       linear_index, fields[index], expected[index],
                                       data[base + index]);
                }
            }
        }
    }

report:
    GS_GLES31_report_results(&result, 1);
}

void GS_GLES31_CS_DC_TP_003_draw(void)
{
}

void GS_GLES31_CS_DC_TP_003_close(void)
{
    GS_GLES31_end_fixture(&fixture);
}
