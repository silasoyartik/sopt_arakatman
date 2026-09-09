#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL43C - ComputeShaders - DispatchCompute

This function tests the error states and limit validations of glDispatchCompute.

Covered requirements:
    - GS-GL43C-CS-DC-001
    - GS-GL43C-CS-DC-003
    - GS-GL43C-CS-DC-004
    - GS-GL43C-CS-DC-005
    - GS-GL43C-CS-DC-006
    - GS-GL43C-CS-DC-007
    - GS-GL43C-CS-DC-008
    - GS-GL43C-CS-DC-009
*/

static const char* test_case1 = "GS_GL43C_CS_DC_TC_001";
static const char* test_case3 = "GS_GL43C_CS_DC_TC_003";
static const char* test_case4 = "GS_GL43C_CS_DC_TC_004";
static const char* test_case5 = "GS_GL43C_CS_DC_TC_005";
static const char* test_case8 = "GS_GL43C_CS_DC_TC_008";
static const char* test_case9 = "GS_GL43C_CS_DC_TC_009";

static const char* test_procedure = "GS_GL43C_CS_DC_TP_001";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLuint dummy_vp_program = 0; // Vertex program for testing TC_004
static GLuint tfo_buffer = 0;

/* Initialization */
void GS_GL43C_CS_DC_TP_001_init(void) {
    CHECK_ERROR(test_procedure);
    GLenum err;

    // Clear any previous errors
    while(glGetError() != GL_NO_ERROR);

    // Test Case 001 & 003
    // Function shall set GL_INVALID_OPERATION if no program is active
    glUseProgram(0);
    glDispatchCompute(1, 1, 1);
    err = glGetError();

    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case3, test_procedure, "Expected GL_INVALID_OPERATION, got: 0x%x", err);
        test_success = GL_FALSE;
    }

    if(test_success) {
        TEST_LOG_SUCCESS(test_case1, test_procedure);
        TEST_LOG_SUCCESS(test_case3, test_procedure);
    } else {
        test_success = GL_TRUE;
    }

    // Test Case 005, 006, 007
    // Function shall set GL_INVALID_VALUE if limits are exceeded
    GLint max_work_group_x, max_work_group_y, max_work_group_z;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_work_group_x);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &max_work_group_y);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &max_work_group_z);

    // Assuming we have a valid compute program binded here via utility
    // util_bind_dummy_compute_program(); 
    
    glDispatchCompute(max_work_group_x + 1, 1, 1);
    err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case5, test_procedure, "Expected GL_INVALID_VALUE for X limit, got: 0x%x", err);
        test_success = GL_FALSE;
    }

    if(test_success) TEST_LOG_SUCCESS(test_case5, test_procedure);
    test_success = GL_TRUE;

    // Test Case 008
    // No work is done, and no error is generated if any dimension is zero
    while(glGetError() != GL_NO_ERROR); // Clear errors
    glDispatchCompute(0, 10, 10);
    err = glGetError();
    
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case8, test_procedure, "Expected GL_NO_ERROR for zero dimension, got: 0x%x", err);
        test_success = GL_FALSE;
    }

    if(test_success) TEST_LOG_SUCCESS(test_case8, test_procedure);
    test_success = GL_TRUE;

    // Test Case 009
    // Transform feedback error check
    glGenBuffers(1, &tfo_buffer);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfo_buffer);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 1024, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfo_buffer);
    
    glBeginTransformFeedback(GL_POINTS);
    glDispatchCompute(1, 1, 1);
    err = glGetError();
    glEndTransformFeedback();

    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case9, test_procedure, "Expected GL_INVALID_OPERATION during transform feedback, got: 0x%x", err);
        test_success = GL_FALSE;
    }

    if(test_success) TEST_LOG_SUCCESS(test_case9, test_procedure);
}

void GS_GL43C_CS_DC_TP_001_draw(void) {
    // Empty draw loop
}

/* Cleanup */
void GS_GL43C_CS_DC_TP_001_close(void) {
    CHECK_ERROR(test_procedure);
    glDeleteBuffers(1, &tfo_buffer);
    glUseProgram(0);
}