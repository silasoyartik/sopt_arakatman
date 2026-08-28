#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - GenBuffers

Function shall return previously unused buffer object names.

Covered requirements:
	- GS-GL20SC-BO-GB-003
*/

static const char* test_case = "GS_GL20SC_BO_GB_TC_003";
static const char* test_procedure = "GS_GL20SC_BO_GB_TP_002";

static GLboolean test_success = GL_TRUE;
static GLuint buffers_set1[3];
static GLuint buffers_set2[3];

/* Initialization */
void GS_GL20SC_BO_GB_TP_002_init(void) {
    CHECK_ERROR(test_procedure);

    /* Generate first set of 3 buffer names */
    glGenBuffers(3, buffers_set1);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Unexpected error after first glGenBuffers(3), got: 0x%x", err);
        test_success = GL_FALSE;
    }

    /* Generate second set of 3 buffer names */
    glGenBuffers(3, buffers_set2);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Unexpected error after second glGenBuffers(3), got: 0x%x", err);
        test_success = GL_FALSE;
    }

    /* Check uniqueness across both sets */
    for (int i = 0; i < 3; ++i) {
        if (buffers_set1[i] == 0) {
            TEST_LOG_FAIL(test_case, test_procedure,
                "Buffer name in set1[%d] is zero", i);
            test_success = GL_FALSE;
        }
        if (buffers_set2[i] == 0) {
            TEST_LOG_FAIL(test_case, test_procedure,
                "Buffer name in set2[%d] is zero", i);
            test_success = GL_FALSE;
        }
    }

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (buffers_set1[i] == buffers_set2[j]) {
                TEST_LOG_FAIL(test_case, test_procedure,
                    "Previously used buffer name given: index1:%d, index2:%d, id:%u",
                    i, j, buffers_set1[i]);
                test_success = GL_FALSE;
            }
        }
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

void GS_GL20SC_BO_GB_TP_002_draw(void) {

}

void GS_GL20SC_BO_GB_TP_002_close(void) {
	CHECK_ERROR(test_procedure);
#ifdef __ubuntu__
    glDeleteBuffers(3, buffers_set1);
    glDeleteBuffers(3, buffers_set2);
#endif
	CHECK_ERROR(test_procedure);
}
