#include "../../helpers_gles31.h"

/*
 * GS_GLES31_CS_DC_TP_007
 *
 * HLR: GS-GLES31-CS-DC-005: num_groups_x greater than
 * 
 * GL_MAX_COMPUTE_WORK_GROUP_COUNT[0] must generate GL_INVALID_VALUE.
 * A valid compute executable and Y = Z = 1 isolate the X limit.
 */
static GS_GLES31_TestResult result = GS_GLES31_RESULT(
    "GS_GLES31_CS_DC_TC_005", "GS_GLES31_CS_DC_TP_007");
static GS_GLES31_TestFixture fixture = GS_GLES31_FIXTURE_INITIALIZER;

void GS_GLES31_CS_DC_TP_007_init(void)
{
    GLuint max_groups_x;

    GS_GLES31_reset_results(&result, 1);
    if (!GS_GLES31_begin_fixture(&fixture))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1,
            "Could not prepare a current OpenGL ES 3.1 test environment");
        goto report;
    }

    if (!GS_GLES31_fixture_compute_program(&fixture,
            GS_GLES31_noop_compute_source()))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1,
            "Could not create and activate the compute program");
        goto report;
    }

    if (!GS_GLES31_work_group_limit(0, &max_groups_x))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1,
            "Could not query a valid maximum X work-group count");
        goto report;
    }

    /* The helper converts the positive GLint limit before unsigned addition. */
    GS_GLES31_clear_errors();
    glDispatchCompute(max_groups_x + 1u, 1, 1);
    GS_GLES31_EXPECT_ERROR(&result, GL_INVALID_VALUE,
        "glDispatchCompute(max_groups_x + 1, 1, 1)");

report:
    GS_GLES31_report_results(&result, 1);
}

void GS_GLES31_CS_DC_TP_007_draw(void)
{
}

void GS_GLES31_CS_DC_TP_007_close(void)
{
    GS_GLES31_end_fixture(&fixture);
}
