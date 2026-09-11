#include "../../helpers_gles31.h"

/*
 * GS_GLES31_CS_DC_TP_004
 *
 * HLR: GS-GLES31-CS-DC-011.
 *
 * Dispatch (2,2,2), local size (2,3,2): verify all five compute built-ins
 * for 96 invocations. Non-unit Z sizes exercise local and global Z and
 * the Z contribution to gl_LocalInvocationIndex.
 */
#define TP004_GROUPS_X 2u
#define TP004_GROUPS_Y 2u
#define TP004_GROUPS_Z 2u
#define TP004_LOCAL_X 2u
#define TP004_LOCAL_Y 3u
#define TP004_LOCAL_Z 2u
#define TP004_GLOBAL_X (TP004_GROUPS_X * TP004_LOCAL_X)
#define TP004_GLOBAL_Y (TP004_GROUPS_Y * TP004_LOCAL_Y)
#define TP004_GLOBAL_Z (TP004_GROUPS_Z * TP004_LOCAL_Z)
#define TP004_TOTAL_INVOCATIONS (TP004_GLOBAL_X * TP004_GLOBAL_Y * TP004_GLOBAL_Z)
#define TP004_VALUES_PER_INVOCATION 14u
#define TP004_BUFFER_VALUES (TP004_TOTAL_INVOCATIONS * TP004_VALUES_PER_INVOCATION)
#define TP004_MARKER_BASE 0xB1000000u

static GS_GLES31_TestResult result =
    GS_GLES31_RESULT("GS_GLES31_CS_DC_TC_011", "GS_GLES31_CS_DC_TP_004");
static GS_GLES31_TestFixture fixture = GS_GLES31_FIXTURE_INITIALIZER;

/*
 * Records: NumWorkGroups.xyz, WorkGroupID.xyz, LocalInvocationID.xyz,
 * GlobalInvocationID.xyz, LocalInvocationIndex, marker.
 * Index using the fixed expected grid so incorrect NumWorkGroups values
 * cannot change record addresses. Bounds prevent out-of-range writes.
 */
static const char *compute_shader_source =
    "#version 310 es\n"
    "layout(local_size_x = 2, local_size_y = 3, local_size_z = 2) in;\n"
    "layout(std430, binding = 0) buffer OutputBuffer { uint data[]; };\n"
    "void main()\n"
    "{\n"
    "    uvec3 id = gl_GlobalInvocationID;\n"
    "    if (id.x >= 4u || id.y >= 6u || id.z >= 4u) return;\n"
    "    uint linear_index = id.x + 4u * (id.y + 6u * id.z);\n"
    "    uint base = linear_index * 14u;\n"
    "    data[base + 0u] = gl_NumWorkGroups.x;\n"
    "    data[base + 1u] = gl_NumWorkGroups.y;\n"
    "    data[base + 2u] = gl_NumWorkGroups.z;\n"
    "    data[base + 3u] = gl_WorkGroupID.x;\n"
    "    data[base + 4u] = gl_WorkGroupID.y;\n"
    "    data[base + 5u] = gl_WorkGroupID.z;\n"
    "    data[base + 6u] = gl_LocalInvocationID.x;\n"
    "    data[base + 7u] = gl_LocalInvocationID.y;\n"
    "    data[base + 8u] = gl_LocalInvocationID.z;\n"
    "    data[base + 9u] = id.x;\n"
    "    data[base + 10u] = id.y;\n"
    "    data[base + 11u] = id.z;\n"
    "    data[base + 12u] = gl_LocalInvocationIndex;\n"
    "    data[base + 13u] = 0xB1000000u + linear_index;\n"
    "}\n";

void GS_GLES31_CS_DC_TP_004_init(void)
{
    GLuint data[TP004_BUFFER_VALUES];
    GLuint output_buffer;
    GLuint global_x, global_y, global_z, index;
    static const char *fields[] = {
        "NumWorkGroups.x", "NumWorkGroups.y", "NumWorkGroups.z",
        "WorkGroupID.x", "WorkGroupID.y", "WorkGroupID.z",
        "LocalInvocationID.x", "LocalInvocationID.y", "LocalInvocationID.z",
        "GlobalInvocationID.x", "GlobalInvocationID.y", "GlobalInvocationID.z",
        "LocalInvocationIndex", "marker"
    };

    GS_GLES31_reset_results(&result, 1);
    if (!GS_GLES31_begin_fixture(&fixture) ||
        !GS_GLES31_fixture_compute_program(&fixture, compute_shader_source))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "%s",
                              "Could not prepare the compute test fixture");
        goto report;
    }

    GS_GLES31_fill_uint(data, TP004_BUFFER_VALUES, 0xFFFFFFFFu);
    output_buffer = GS_GLES31_fixture_ssbo(
        &fixture, 0, (GLsizeiptr)sizeof(data), data, GL_DYNAMIC_READ);
    if (output_buffer == 0)
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "%s", "Could not create the output SSBO");
        goto report;
    }

    GS_GLES31_clear_errors();
    glDispatchCompute(TP004_GROUPS_X, TP004_GROUPS_Y, TP004_GROUPS_Z);
    if (!GS_GLES31_EXPECT_ERROR(&result, GL_NO_ERROR, "glDispatchCompute(2,2,2)"))
        goto report;

    if (!GS_GLES31_read_ssbo(output_buffer, (GLsizeiptr)sizeof(data), data))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "%s", "Could not read compute output");
        goto report;
    }

    /* Derive every expected built-in from CPU coordinates and declared sizes. */
    for (global_z = 0; global_z < TP004_GLOBAL_Z; ++global_z)
    {
        for (global_y = 0; global_y < TP004_GLOBAL_Y; ++global_y)
        {
            for (global_x = 0; global_x < TP004_GLOBAL_X; ++global_x)
            {
                const GLuint group_x = global_x / TP004_LOCAL_X;
                const GLuint group_y = global_y / TP004_LOCAL_Y;
                const GLuint group_z = global_z / TP004_LOCAL_Z;
                const GLuint local_x = global_x % TP004_LOCAL_X;
                const GLuint local_y = global_y % TP004_LOCAL_Y;
                const GLuint local_z = global_z % TP004_LOCAL_Z;
                const GLuint local_index =
                    local_x + TP004_LOCAL_X * (local_y + TP004_LOCAL_Y * local_z);
                const GLuint linear_index =
                    global_x + TP004_GLOBAL_X * (global_y + TP004_GLOBAL_Y * global_z);
                const GLuint base = linear_index * TP004_VALUES_PER_INVOCATION;
                const GLuint expected[] = {
                    TP004_GROUPS_X, TP004_GROUPS_Y, TP004_GROUPS_Z,
                    group_x, group_y, group_z, local_x, local_y, local_z,
                    global_x, global_y, global_z, local_index,
                    TP004_MARKER_BASE + linear_index
                };

                for (index = 0; index < TP004_VALUES_PER_INVOCATION; ++index)
                    GS_GLES31_CHECK(&result, data[base + index] == expected[index],
                                   "Invocation (%u,%u,%u) %s: expected %u, got %u",
                                   global_x, global_y, global_z, fields[index],
                                   expected[index], data[base + index]);
            }
        }
    }

report:
    GS_GLES31_report_results(&result, 1);
}

void GS_GLES31_CS_DC_TP_004_draw(void)
{
}

void GS_GLES31_CS_DC_TP_004_close(void)
{
    GS_GLES31_end_fixture(&fixture);
}
