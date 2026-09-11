#include "../../helpers_gles31.h"

/*
 * GS_GLES31_CS_DC_TP_005
 *
 * HLR: GS-GLES31-CS-DC-003: dispatch without an active compute executable must
 * 
 * generate GL_INVALID_OPERATION. Both the current program and the program
 * pipeline must be zero, since a pipeline can supply the executable when
 * no program is selected with glUseProgram().
 */
static GS_GLES31_TestResult result = GS_GLES31_RESULT(
    "GS_GLES31_CS_DC_TC_003", "GS_GLES31_CS_DC_TP_005");
static GS_GLES31_TestFixture fixture = GS_GLES31_FIXTURE_INITIALIZER;

void GS_GLES31_CS_DC_TP_005_init(void)
{
    GS_GLES31_reset_results(&result, 1);
    if (!GS_GLES31_begin_fixture(&fixture))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1,
            "Could not prepare a current OpenGL ES 3.1 test environment");
        goto report;
    }

    /* Establish and query GL_CURRENT_PROGRAM = GL_PROGRAM_PIPELINE_BINDING = 0. */
    if (!GS_GLES31_use_program(0))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1,
            "Could not establish the absence of a program and program pipeline");
        goto report;
    }

    /* All dimensions are valid; only the missing executable is invalid. */
    GS_GLES31_clear_errors();
    glDispatchCompute(1, 1, 1);
    GS_GLES31_EXPECT_ERROR(&result, GL_INVALID_OPERATION,
        "glDispatchCompute(1, 1, 1) without an active compute executable");

report:
    GS_GLES31_report_results(&result, 1);
}

void GS_GLES31_CS_DC_TP_005_draw(void)
{
}

void GS_GLES31_CS_DC_TP_005_close(void)
{
    GS_GLES31_end_fixture(&fixture);
}
