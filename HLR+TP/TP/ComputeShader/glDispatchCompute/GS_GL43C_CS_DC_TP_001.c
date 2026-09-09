#include <GL/gl.h>
#include "../../helpers.h"

/*
 * GL43C - ComputeShaders - glDispatchCompute
 *
 * Covered requirements:
 *   GS-GL43C-CS-DC-001
 *   GS-GL43C-CS-DC-003
 *   GS-GL43C-CS-DC-004
 *   GS-GL43C-CS-DC-005
 *   GS-GL43C-CS-DC-006
 *   GS-GL43C-CS-DC-007
 *   GS-GL43C-CS-DC-008
 *   GS-GL43C-CS-DC-009
 */
static const char *test_case1 = "GS_GL43C_CS_DC_TC_001";
static const char *test_case3 = "GS_GL43C_CS_DC_TC_003";
static const char *test_case4 = "GS_GL43C_CS_DC_TC_004";
static const char *test_case5 = "GS_GL43C_CS_DC_TC_005";
static const char *test_case6 = "GS_GL43C_CS_DC_TC_006";
static const char *test_case7 = "GS_GL43C_CS_DC_TC_007";
static const char *test_case8 = "GS_GL43C_CS_DC_TC_008";
static const char *test_case9 = "GS_GL43C_CS_DC_TC_009";
static const char *test_procedure = "GS_GL43C_CS_DC_TP_001";

static GLboolean test_success1 = GL_TRUE;
static GLboolean test_success3 = GL_TRUE;
static GLboolean test_success4 = GL_TRUE;
static GLboolean test_success5 = GL_TRUE;
static GLboolean test_success6 = GL_TRUE;
static GLboolean test_success7 = GL_TRUE;
static GLboolean test_success8 = GL_TRUE;
static GLboolean test_success9 = GL_TRUE;

static GS_GL_TestEnvironment environment = GS_GL_ENV_INITIALIZER;

void GS_GL43C_CS_DC_TP_001_init(void)
{
    void (*dispatch_compute)(GLuint, GLuint, GLuint) = glDispatchCompute;[cite: 4]
    GLint max_wg[3] = {0, 0, 0};
    GLuint dummy_vertex_program = 0;
    GLuint tfo_buffer = 0;
    GLenum err;

    if (!GS_GL_prepare_environment(&environment))
    {
        TEST_LOG_FAIL(test_case1, test_procedure,
            "Could not prepare an initialized GL environment");[cite: 4]
        test_success1 = GL_FALSE;
        test_success3 = GL_FALSE;
        test_success4 = GL_FALSE;
        test_success5 = GL_FALSE;
        test_success6 = GL_FALSE;
        test_success7 = GL_FALSE;
        test_success8 = GL_FALSE;
        test_success9 = GL_FALSE;
        return;
    }

    /* TC_001 & TC_003: Check function pointer and no active program */
    while(glGetError() != GL_NO_ERROR);
    glUseProgram(0);
    dispatch_compute(1, 1, 1);
    err = glGetError();
    if (err != GL_INVALID_OPERATION)
    {
        TEST_LOG_FAIL(test_case3, test_procedure,
            "Expected GL_INVALID_OPERATION without active program, got: 0x%x", err);
        test_success3 = GL_FALSE;
        test_success1 = GL_FALSE;
    }

    /* TC_004: Program without compute shader */
    dummy_vertex_program = GS_GL_compile_dummy_vertex_program();
    glUseProgram(dummy_vertex_program);
    dispatch_compute(1, 1, 1);
    err = glGetError();
    if (err != GL_INVALID_OPERATION)
    {
        TEST_LOG_FAIL(test_case4, test_procedure,
            "Expected GL_INVALID_OPERATION with non-compute program, got: 0x%x", err);
        test_success4 = GL_FALSE;
    }

    /* TC_005, TC_006, TC_007: Exceeding dimensions */
    GLuint compute_program = GS_GL_compile_dummy_compute_program();
    glUseProgram(compute_program);
    
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_wg[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &max_wg[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &max_wg[2]);

    dispatch_compute(max_wg[0] + 1, 1, 1);
    if (glGetError() != GL_INVALID_VALUE) test_success5 = GL_FALSE;

    dispatch_compute(1, max_wg[1] + 1, 1);
    if (glGetError() != GL_INVALID_VALUE) test_success6 = GL_FALSE;

    dispatch_compute(1, 1, max_wg[2] + 1);
    if (glGetError() != GL_INVALID_VALUE) test_success7 = GL_FALSE;

    if (!test_success5) TEST_LOG_FAIL(test_case5, test_procedure, "X limit bypass failed");
    if (!test_success6) TEST_LOG_FAIL(test_case6, test_procedure, "Y limit bypass failed");
    if (!test_success7) TEST_LOG_FAIL(test_case7, test_procedure, "Z limit bypass failed");

    /* TC_008: Zero dimension dispatch */
    while(glGetError() != GL_NO_ERROR);
    dispatch_compute(0, 10, 10);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case8, test_procedure,
            "Expected GL_NO_ERROR when dim is 0, got: 0x%x", err);
        test_success8 = GL_FALSE;
    }

    /* TC_009: Transform feedback active */
    glGenBuffers(1, &tfo_buffer);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfo_buffer);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 1024, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfo_buffer);
    
    glBeginTransformFeedback(GL_POINTS);
    dispatch_compute(1, 1, 1);
    err = glGetError();
    glEndTransformFeedback();
    
    if (err != GL_INVALID_OPERATION)
    {
        TEST_LOG_FAIL(test_case9, test_procedure,
            "Expected GL_INVALID_OPERATION during transform feedback, got: 0x%x", err);
        test_success9 = GL_FALSE;
    }

    if (test_success1) TEST_LOG_SUCCESS(test_case1, test_procedure);[cite: 4]
    if (test_success3) TEST_LOG_SUCCESS(test_case3, test_procedure);
    if (test_success4) TEST_LOG_SUCCESS(test_case4, test_procedure);
    if (test_success5) TEST_LOG_SUCCESS(test_case5, test_procedure);
    if (test_success6) TEST_LOG_SUCCESS(test_case6, test_procedure);
    if (test_success7) TEST_LOG_SUCCESS(test_case7, test_procedure);
    if (test_success8) TEST_LOG_SUCCESS(test_case8, test_procedure);
    if (test_success9) TEST_LOG_SUCCESS(test_case9, test_procedure);
}

void GS_GL43C_CS_DC_TP_001_draw(void) { }[cite: 4]

void GS_GL43C_CS_DC_TP_001_close(void)
{
    GS_GL_cleanup_environment(&environment);[cite: 4]
}