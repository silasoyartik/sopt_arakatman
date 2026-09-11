#include "../../helpers_gles31.h"

/*
 * GS_GLES31_CS_DC_TP_001
 *
 * HLR: GS-GLES31-CS-DC-001, GS-GLES31-CS-DC-002, GS-GLES31-CS-DC-013.
 *
 * Dispatch (4,1,1), local size (1,1,1): the active executable must
 * write {1,2,3,4}. Dispatch acceptance and execution are reported separately.
 */
static GS_GLES31_TestResult results[] = {
    GS_GLES31_RESULT("GS_GLES31_CS_DC_TC_001", "GS_GLES31_CS_DC_TP_001"),
    GS_GLES31_RESULT("GS_GLES31_CS_DC_TC_002", "GS_GLES31_CS_DC_TP_001"),
    GS_GLES31_RESULT("GS_GLES31_CS_DC_TC_013", "GS_GLES31_CS_DC_TP_001")
};
static GS_GLES31_TestFixture fixture = GS_GLES31_FIXTURE_INITIALIZER;

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
    const GLuint expected_data[] = {1u, 2u, 3u, 4u};
    GLuint data[4];
    GLuint output_buffer;
    GLuint index;

    GS_GLES31_reset_results(results, 3);
    /* The fixture loads and validates entry points before the first API call. */
    if (!GS_GLES31_begin_fixture(&fixture) ||
        !GS_GLES31_fixture_compute_program(&fixture, compute_shader_source))
    {
        GS_GLES31_FAIL_RESULTS(results, 3, "%s",
                              "Could not prepare the compute test fixture");
        goto report;
    }

    GS_GLES31_fill_uint(data, 4, 0xFFFFFFFFu);
    output_buffer = GS_GLES31_fixture_ssbo(
        &fixture, 0, (GLsizeiptr)sizeof(data), data, GL_DYNAMIC_READ);
    if (output_buffer == 0)
    {
        GS_GLES31_FAIL_RESULTS(results, 3, "%s", "Could not create the output SSBO");
        goto report;
    }

    GS_GLES31_clear_errors();
    glDispatchCompute(4, 1, 1);
    if (!GS_GLES31_EXPECT_ERROR(&results[2], GL_NO_ERROR, "glDispatchCompute(4,1,1)"))
    {
        GS_GLES31_FAIL_RESULTS(results, 2, "%s",
                              "Compute execution could not be verified: dispatch failed");
        goto report;
    }

    /* Readback failure must not change the independently verified HLR-013. */
    if (!GS_GLES31_read_ssbo(output_buffer, (GLsizeiptr)sizeof(data), data))
    {
        GS_GLES31_FAIL_RESULTS(results, 2, "%s", "Could not read compute output");
        goto report;
    }

    for (index = 0; index < 4; ++index)
    {
        GS_GLES31_CHECK(&results[0], data[index] == expected_data[index],
                       "Invocation %u: expected output %u, got %u",
                       index, expected_data[index], data[index]);
        GS_GLES31_CHECK(&results[1], data[index] == expected_data[index],
                       "Active executable output at %u: expected %u, got %u",
                       index, expected_data[index], data[index]);
    }

report:
    GS_GLES31_report_results(results, 3);
}

void GS_GLES31_CS_DC_TP_001_draw(void)
{
}

void GS_GLES31_CS_DC_TP_001_close(void)
{
    GS_GLES31_end_fixture(&fixture);
}
