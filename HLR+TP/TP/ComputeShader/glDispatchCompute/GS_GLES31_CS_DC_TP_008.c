#include "../../helpers_gles31.h"

/*
 * GS_GLES31_CS_DC_TP_008
 *
 * HLR: GS-GLES31-CS-DC-006 / 007: exceeding the Y / Z work-group count limit
 * 
 * must generate GL_INVALID_VALUE. Each dispatch exceeds only one axis;
 * a failure in the Y scenario does not prevent the Z scenario from running.
 */
static GS_GLES31_TestResult results[] = {
    GS_GLES31_RESULT("GS_GLES31_CS_DC_TC_006", "GS_GLES31_CS_DC_TP_008"),
    GS_GLES31_RESULT("GS_GLES31_CS_DC_TC_007", "GS_GLES31_CS_DC_TP_008")
};
static GS_GLES31_TestFixture fixture = GS_GLES31_FIXTURE_INITIALIZER;

void GS_GLES31_CS_DC_TP_008_init(void)
{
    GLuint max_groups_y;
    GLuint max_groups_z;

    GS_GLES31_reset_results(results, 2);
    if (!GS_GLES31_begin_fixture(&fixture))
    {
        GS_GLES31_FAIL_RESULTS(results, 2,
            "Could not prepare a current OpenGL ES 3.1 test environment");
        goto report;
    }

    if (!GS_GLES31_fixture_compute_program(&fixture,
            GS_GLES31_noop_compute_source()))
    {
        GS_GLES31_FAIL_RESULTS(results, 2,
            "Could not create and activate the compute program");
        goto report;
    }

    /* GS-GLES31-CS-DC-006: only the Y dimension is invalid. */
    if (GS_GLES31_work_group_limit(1, &max_groups_y))
    {
        GS_GLES31_clear_errors();
        glDispatchCompute(1, max_groups_y + 1u, 1);
        GS_GLES31_EXPECT_ERROR(&results[0], GL_INVALID_VALUE,
            "glDispatchCompute(1, max_groups_y + 1, 1)");
    }
    else
    {
        GS_GLES31_FAIL_RESULTS(&results[0], 1,
            "Could not query a valid maximum Y work-group count");
    }

    /* GS-GLES31-CS-DC-007: independent query and dispatch for the Z limit. */
    if (GS_GLES31_work_group_limit(2, &max_groups_z))
    {
        GS_GLES31_clear_errors();
        glDispatchCompute(1, 1, max_groups_z + 1u);
        GS_GLES31_EXPECT_ERROR(&results[1], GL_INVALID_VALUE,
            "glDispatchCompute(1, 1, max_groups_z + 1)");
    }
    else
    {
        GS_GLES31_FAIL_RESULTS(&results[1], 1,
            "Could not query a valid maximum Z work-group count");
    }

report:
    GS_GLES31_report_results(results, 2);
}

void GS_GLES31_CS_DC_TP_008_draw(void)
{
}

void GS_GLES31_CS_DC_TP_008_close(void)
{
    GS_GLES31_end_fixture(&fixture);
}
