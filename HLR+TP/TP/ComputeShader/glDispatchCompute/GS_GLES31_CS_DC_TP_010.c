#include "../../helpers_gles31.h"

/* 
 * GS_GLES31_CS_DC_TP_010
 *
 * HLR: GS-GLES31-CS-DC-012: the active compute shader reads binding 0 and
 * 
 * writes binding 1. Two distinct input/output pairs verify that dispatch
 * accesses the currently bound resources. The first output and both inputs
 * must remain unchanged when the second pair is used.
 */
static GS_GLES31_TestFixture fixture = GS_GLES31_FIXTURE_INITIALIZER;
static GS_GLES31_TestResult result = GS_GLES31_RESULT(
    "GS_GLES31_CS_DC_TC_012", "GS_GLES31_CS_DC_TP_010");

static const GLuint input_a[4] = { 7u, 11u, 19u, 23u };
static const GLuint input_b[4] = { 31u, 37u, 41u, 47u };
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

static int verify_buffer(const char *label, GLuint buffer,
    const GLuint expected[4])
{
    GLuint observed[4];
    size_t i;
    int matches = 1;

    if (!GS_GLES31_read_ssbo(buffer, sizeof(observed), observed))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "%s: SSBO readback failed", label);
        return 0;
    }
    for (i = 0; i < 4; ++i)
    {
        if (!GS_GLES31_CHECK(&result, observed[i] == expected[i],
                "%s: value %u expected %u, got %u", label,
                (unsigned int)i, expected[i], observed[i]))
            matches = 0;
    }
    return matches;
}

static int run_case(const char *label, GLuint input, GLuint output,
    const GLuint input_values[4], const GLuint expected[4])
{
    if (!GS_GLES31_reset_ssbo(input, 0, sizeof(input_a), input_values) ||
        !GS_GLES31_reset_ssbo(output, 1, sizeof(sentinels), sentinels))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "%s: SSBO binding/reset failed", label);
        return 0;
    }

    GS_GLES31_clear_errors();
    glDispatchCompute(4, 1, 1);
    if (!GS_GLES31_EXPECT_ERROR(&result, GL_NO_ERROR, label))
        return 0;

    return verify_buffer(label, output, expected);
}

void GS_GLES31_CS_DC_TP_010_init(void)
{
    GLuint inputs[2], outputs[2];
    GLuint expected_a[4], expected_b[4];
    size_t i;

    GS_GLES31_reset_results(&result, 1);
    if (!GS_GLES31_begin_fixture(&fixture) ||
        !GS_GLES31_fixture_compute_program(&fixture, compute_shader_source))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "Compute fixture setup failed");
        goto report;
    }

    inputs[0] = GS_GLES31_fixture_ssbo(&fixture, 0,
        sizeof(input_a), input_a, GL_STATIC_READ);
    inputs[1] = GS_GLES31_fixture_ssbo(&fixture, 0,
        sizeof(input_b), input_b, GL_STATIC_READ);
    outputs[0] = GS_GLES31_fixture_ssbo(&fixture, 1,
        sizeof(sentinels), sentinels, GL_DYNAMIC_READ);
    outputs[1] = GS_GLES31_fixture_ssbo(&fixture, 1,
        sizeof(sentinels), sentinels, GL_DYNAMIC_READ);
    if (!inputs[0] || !inputs[1] || !outputs[0] || !outputs[1])
    {
        GS_GLES31_FAIL_RESULTS(&result, 1, "Input/output SSBO creation failed");
        goto report;
    }

    /* Expected values are calculated from CPU inputs, never GPU output. */
    for (i = 0; i < 4; ++i)
    {
        expected_a[i] = input_a[i] * 3u + 7u;
        expected_b[i] = input_b[i] * 3u + 7u;
    }

    if (!run_case("input A / output A", inputs[0], outputs[0], input_a, expected_a))
        goto report;
    if (!verify_buffer("unused output B", outputs[1], sentinels))
        goto report;

    if (!run_case("input B / output B", inputs[1], outputs[1], input_b, expected_b))
        goto report;
    verify_buffer("previous output A preserved", outputs[0], expected_a);
    verify_buffer("input A preserved", inputs[0], input_a);
    verify_buffer("input B preserved", inputs[1], input_b);

report:
    GS_GLES31_report_results(&result, 1);
}

void GS_GLES31_CS_DC_TP_010_draw(void)
{
}

void GS_GLES31_CS_DC_TP_010_close(void)
{
    GS_GLES31_end_fixture(&fixture);
}