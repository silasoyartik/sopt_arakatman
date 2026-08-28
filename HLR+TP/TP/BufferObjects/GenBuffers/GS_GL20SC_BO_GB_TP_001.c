#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - GenBuffers

Graphics library shall provide glGenBuffers function.

Function shall return n unique buffer object names into the memory area pointed by the buffers.

Covered requirements:
	- GS-GL20SC-BO-GB-001
	- GS-GL20SC-BO-GB-002
*/

static const char* test_case1 = "GS_GL20SC_BO_GB_TC_001";
static const char* test_case2 = "GS_GL20SC_BO_GB_TC_002";
static const char* test_procedure = "GS_GL20SC_BO_GB_TP_001";

static GLboolean test_success1 = GL_TRUE;
static GLboolean test_success2 = GL_TRUE;
static GLuint buffers[5] = {0, 0, 0, 0, 0};

/* Initialization */
void GS_GL20SC_BO_GB_TP_001_init(void) {
    CHECK_ERROR(test_procedure);
    GLenum err;

    /* Generate 3 buffer names */

    // Test Case 001
    glGenBuffers(3, buffers);

    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case1, test_procedure,
            "Unexpected error after glGenBuffers(3), got: 0x%x", err);
        test_success1 = GL_FALSE;
    }

    if (test_success1) {
    	TEST_LOG_SUCCESS(test_case1, test_procedure);
    }

    // Test Case 002

    /* Check that only 3 buffers generated */
    if (buffers[0] == 0 || buffers[1] == 0 || buffers[2] == 0 ||
    	buffers[3] != 0 || buffers[4] != 0) {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "Expected 3 buffer names generated, got: 0:%d, 1:%d, 2:%d, 3:%d, 4:%d", buffers[0], buffers[1], buffers[2], buffers[3], buffers[4]);
        test_success2 = GL_FALSE;
    }

    /* Check that generated buffer names are unique */
    for (int i = 0; i < 3; ++i) {
        for (int j = i + 1; j < 3; ++j) {
            if (buffers[i] == buffers[j]) {
                TEST_LOG_FAIL(test_case2, test_procedure,
                    "Buffer names not unique: buffers[%d] == buffers[%d] == %u",
                    i, j, buffers[i]);
                test_success2 = GL_FALSE;
            }
        }
    }

    GLfloat dummyData [] = { 0.0f, 1.0f, 2.0f };

    // Attempt to use the generated buffer, no error should be generated
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dummyData), dummyData, GL_STATIC_DRAW);
    err = glGetError();

    if (err != GL_NO_ERROR) {
    	TEST_LOG_FAIL(test_case2, test_procedure, "error generated while using generated buffer: 0x%x", err);
    	test_success2 = GL_FALSE;
    }

    if (test_success2) {
        TEST_LOG_SUCCESS(test_case2, test_procedure);
    }
}

void GS_GL20SC_BO_GB_TP_001_draw(void) {

}

void GS_GL20SC_BO_GB_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
#ifdef __ubuntu__
    glDeleteBuffers(3, buffers);
#endif
	CHECK_ERROR(test_procedure);
}