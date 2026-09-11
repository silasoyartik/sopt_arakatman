#include "../../helpers_gles31.h"

/* 
 * GS_GLES31_CS_DC_TP_009
 *
 * HLR: GS-GLES31-CS-DC-008: any zero dispatch dimension accepts the command
 * 
 * without executing work groups. A positive control first proves that this
 * shader and SSBO observation path can detect execution.
 */
static GS_GLES31_TestFixture fixture = GS_GLES31_FIXTURE_INITIALIZER;
static GS_GLES31_TestResult result = GS_GLES31_RESULT(
    "GS_GLES31_CS_DC_TC_008", "GS_GLES31_CS_DC_TP_009");
static GLuint output_buffer;

static const GLuint sentinels[4] = {
    0xAAAAAAAAu, 0xBBBBBBBBu, 0xCCCCCCCCu, 0xDDDDDDDDu
};
static const GLuint executed_values[4] = { 0x12345678u, 0u, 0u, 0u };

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

static int run_case(const char *label, GLuint x, GLuint y, GLuint z,
    const GLuint expected[4])
{
    GLuint observed[4];
    size_t i;
    int matches = 1;

    if (!GS_GLES31_reset_ssbo(output_buffer, 0, sizeof(sentinels), sentinels))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "%s: SSBO reset failed", label);
        return 0;
    }

    GS_GLES31_clear_errors();
    glDispatchCompute(x, y, z);
    if (!GS_GLES31_EXPECT_ERROR(&result, GL_NO_ERROR, label))
        return 0;

    if (!GS_GLES31_read_ssbo(output_buffer, sizeof(observed), observed))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "%s: SSBO readback failed", label);
        return 0;
    }

    for (i = 0; i < 4; ++i)
    {
        if (!GS_GLES31_CHECK(&result, observed[i] == expected[i],
                "%s: value %u expected 0x%x, got 0x%x", label,
                (unsigned int)i, expected[i], observed[i]))
            matches = 0;
    }
    return matches;
}

void GS_GLES31_CS_DC_TP_009_init(void)
{
    GS_GLES31_reset_results(&result, 1);
    if (!GS_GLES31_begin_fixture(&fixture) ||
        !GS_GLES31_fixture_compute_program(&fixture, compute_shader_source))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "Compute fixture setup failed");
        goto report;
    }

    output_buffer = GS_GLES31_fixture_ssbo(&fixture, 0,
        sizeof(sentinels), sentinels, GL_DYNAMIC_READ);
    if (output_buffer == 0)
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "Output SSBO creation failed");
        goto report;
    }

    /* A nonworking writer must not make the no-execution checks pass. */
    if (!run_case("positive control (1,1,1)", 1, 1, 1, executed_values))
        goto report;

    /* Keep each axis independent, including after an assertion failure. */
    run_case("zero X (0,1,1)", 0, 1, 1, sentinels);
    run_case("zero Y (1,0,1)", 1, 0, 1, sentinels);
    run_case("zero Z (1,1,0)", 1, 1, 0, sentinels);

report:
    GS_GLES31_report_results(&result, 1);
}

void GS_GLES31_CS_DC_TP_009_draw(void)
{
}

void GS_GLES31_CS_DC_TP_009_close(void)
{
    GS_GLES31_end_fixture(&fixture);
    output_buffer = 0;
}