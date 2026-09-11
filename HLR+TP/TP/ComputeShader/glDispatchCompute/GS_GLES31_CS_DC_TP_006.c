#include "../../helpers_gles31.h"

/*
 * GS_GLES31_CS_DC_TP_006
 *
 * HLR: GS-GLES31-CS-DC-004: dispatch with a successfully linked graphics-only
 * 
 * program must generate GL_INVALID_OPERATION. A nonzero current program
 * takes precedence over the program pipeline; the fixture also clears the
 * pipeline binding for a known initial state.
 */
static GS_GLES31_TestResult result = GS_GLES31_RESULT(
    "GS_GLES31_CS_DC_TC_004", "GS_GLES31_CS_DC_TP_006");
static GS_GLES31_TestFixture fixture = GS_GLES31_FIXTURE_INITIALIZER;

/* These stages construct a valid program containing no compute stage. */
static const char *vertex_shader_source =
    "#version 310 es\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "}\n";

static const char *fragment_shader_source =
    "#version 310 es\n"
    "precision mediump float;\n"
    "layout(location = 0) out vec4 fragmentColor;\n"
    "void main()\n"
    "{\n"
    "    fragmentColor = vec4(1.0);\n"
    "}\n";

void GS_GLES31_CS_DC_TP_006_init(void)
{
    GS_GLES31_reset_results(&result, 1);
    if (!GS_GLES31_begin_fixture(&fixture))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1,
            "Could not prepare a current OpenGL ES 3.1 test environment");
        goto report;
    }

    if (!GS_GLES31_fixture_graphics_program(&fixture,
            vertex_shader_source, fragment_shader_source))
    {
        GS_GLES31_FAIL_RESULTS(&result, 1,
            "Could not create, link and activate the graphics-only program");
        goto report;
    }

    /* Valid dimensions isolate the absence of a compute stage. */
    GS_GLES31_clear_errors();
    glDispatchCompute(1, 1, 1);
    GS_GLES31_EXPECT_ERROR(&result, GL_INVALID_OPERATION,
        "glDispatchCompute(1, 1, 1) with a graphics-only program");

report:
    GS_GLES31_report_results(&result, 1);
}

void GS_GLES31_CS_DC_TP_006_draw(void)
{
}

void GS_GLES31_CS_DC_TP_006_close(void)
{
    GS_GLES31_end_fixture(&fixture);
}
