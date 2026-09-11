#include "../../helpers_gles31.h"

/*
 * GS_GLES31_CS_DC_TP_002
 *
 * HLR: GS-GLES31-CS-DC-009.
 *
 * Dispatch (2,3,2), local size (1,1,1): observe every group coordinate
 * and gl_NumWorkGroups. Atomic counters detect missing or repeated groups.
 */
#define TP002_GROUPS_X 2u
#define TP002_GROUPS_Y 3u
#define TP002_GROUPS_Z 2u
#define TP002_TOTAL_GROUPS (TP002_GROUPS_X * TP002_GROUPS_Y * TP002_GROUPS_Z)
#define TP002_VALUES_PER_GROUP 8u
#define TP002_BUFFER_VALUES (1u + TP002_TOTAL_GROUPS * TP002_VALUES_PER_GROUP)
#define TP002_MARKER_BASE 0xC0DE0000u

static GS_GLES31_TestResult result =
    GS_GLES31_RESULT("GS_GLES31_CS_DC_TC_009", "GS_GLES31_CS_DC_TP_002");
static GS_GLES31_TestFixture fixture = GS_GLES31_FIXTURE_INITIALIZER;

/*
 * data[0]: actual invocation count (one invocation per work group).
 * Each group record: visits, NumWorkGroups.xyz, WorkGroupID.xyz, marker.
 * Only the first visitor writes the record, including under duplicate IDs.
 * Fixed indexing keeps the output address independent of gl_NumWorkGroups.
 */
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
    GLuint data[TP002_BUFFER_VALUES];
    GLuint output_buffer;
    GLuint x, y, z, index;

    GS_GLES31_reset_results(&result, 1);
    if (!GS_GLES31_begin_fixture(&fixture) ||
        !GS_GLES31_fixture_compute_program(&fixture, compute_shader_source))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "%s",
                              "Could not prepare the compute test fixture");
        goto report;
    }

    GS_GLES31_fill_uint(data, TP002_BUFFER_VALUES, 0xFFFFFFFFu);
    data[0] = 0u;
    for (index = 0; index < TP002_TOTAL_GROUPS; ++index)
        data[1u + index * TP002_VALUES_PER_GROUP] = 0u;

    output_buffer = GS_GLES31_fixture_ssbo(
        &fixture, 0, (GLsizeiptr)sizeof(data), data, GL_DYNAMIC_READ);
    if (output_buffer == 0)
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "%s", "Could not create the output SSBO");
        goto report;
    }

    GS_GLES31_clear_errors();
    glDispatchCompute(TP002_GROUPS_X, TP002_GROUPS_Y, TP002_GROUPS_Z);
    if (!GS_GLES31_EXPECT_ERROR(&result, GL_NO_ERROR, "glDispatchCompute(2,3,2)"))
        goto report;

    if (!GS_GLES31_read_ssbo(output_buffer, (GLsizeiptr)sizeof(data), data))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "%s", "Could not read compute output");
        goto report;
    }

    GS_GLES31_CHECK(&result, data[0] == TP002_TOTAL_GROUPS,
                   "Expected %u executed groups, got %u", TP002_TOTAL_GROUPS, data[0]);
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
                    GS_GLES31_CHECK(&result, data[base + index] == expected[index],
                                   "Group (%u,%u,%u) %s: expected %u, got %u",
                                   x, y, z, fields[index], expected[index], data[base + index]);
            }
        }
    }

report:
    GS_GLES31_report_results(&result, 1);
}

void GS_GLES31_CS_DC_TP_002_draw(void)
{
}

void GS_GLES31_CS_DC_TP_002_close(void)
{
    GS_GLES31_end_fixture(&fixture);
}
