#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BindBuffer

Library shall perform operations on the bound buffer object,
and shall return states of the bound buffer object.

Covered requirements:
	- GS-GL20SC-BO-BB-006
	- GS-GL20SC-BO-BB-007
*/

static const char* test_case6 = "GS_GL20SC_BO_BB_TC_006";
static const char* test_case7 = "GS_GL20SC_BO_BB_TC_007";

static const char* test_procedure = "GS_GL20SC_BO_BB_TP_004";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLuint arrayA = 0, arrayB = 0;
static GLuint elemA  = 0, elemB  = 0;

/* Initialization */
void GS_GL20SC_BO_BB_TP_004_init(void) {
    CHECK_ERROR(test_procedure);

    glGenBuffers(1, &arrayA);
    glGenBuffers(1, &arrayB);
    glGenBuffers(1, &elemA);
    glGenBuffers(1, &elemB);
    CHECK_ERROR(test_procedure);

    GLint size  = -1;
    GLint usage = -1;
    GLboolean array_ok = GL_TRUE;
    GLboolean elem_ok  = GL_TRUE;

    // Test Case 006
    glBindBuffer(GL_ARRAY_BUFFER, arrayA);
    glBufferData(GL_ARRAY_BUFFER, 256, NULL, GL_STATIC_DRAW);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
    if(size != 256 || usage != GL_STATIC_DRAW) {
        TEST_LOG_FAIL(test_case6, test_procedure,
            "(ARRAY_BUFFER mismatch after binding arrayA: size=%d, usage=0x%X)", size, usage);

        test_success = GL_FALSE;
    }

    CHECK_ERROR(test_procedure);
    glBindBuffer(GL_ARRAY_BUFFER, arrayB);
    glBufferData(GL_ARRAY_BUFFER, 512, NULL, GL_DYNAMIC_DRAW);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
    if(size != 512 || usage != GL_DYNAMIC_DRAW) {
        TEST_LOG_FAIL(test_case6, test_procedure,
            "(ARRAY_BUFFER mismatch after binding arrayB: size=%d, usage=0x%X)", size, usage);

        test_success = GL_FALSE;
    }

    CHECK_ERROR(test_procedure);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemA);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 128, NULL, GL_STATIC_DRAW);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
    if(size != 128 || usage != GL_STATIC_DRAW) {
        TEST_LOG_FAIL(test_case6, test_procedure,
            "(ELEMENT_ARRAY_BUFFER mismatch after binding elemA: size=%d, usage=0x%X)", size, usage);

        test_success = GL_FALSE;
    }

    CHECK_ERROR(test_procedure);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
    if(size != 1024 || usage != GL_DYNAMIC_DRAW) {
        TEST_LOG_FAIL(test_case6, test_procedure,
            "(ELEMENT_ARRAY_BUFFER mismatch after binding elemB: size=%d, usage=0x%X)", size, usage);

        test_success = GL_FALSE;
    }

    if(test_success) {
        TEST_LOG_SUCCESS(test_case6, test_procedure);
    } else {
    	test_success = GL_TRUE;
    }

    // Test Case 007
    CHECK_ERROR(test_procedure);
    glBindBuffer(GL_ARRAY_BUFFER, arrayA);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
    if(size != 256 || usage != GL_STATIC_DRAW) {
        TEST_LOG_FAIL(test_case7, test_procedure,
            "(ARRAY_BUFFER mismatch after rebinding arrayA: size=%d, usage=0x%X)", size, usage);

        test_success = GL_FALSE;
    }

    CHECK_ERROR(test_procedure);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemA);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
    if(size != 128 || usage != GL_STATIC_DRAW) {
        TEST_LOG_FAIL(test_case7, test_procedure,
            "(ELEMENT_ARRAY_BUFFER mismatch after rebinding elemA: size=%d, usage=0x%X)", size, usage);

        test_success = GL_FALSE;
    }

    if(test_success) {
        TEST_LOG_SUCCESS(test_case7, test_procedure);
    }
}

void GS_GL20SC_BO_BB_TP_004_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_BB_TP_004_close(void) {
    CHECK_ERROR(test_procedure);
#ifdef __ubuntu__
	glDeleteBuffers(1, &arrayA);
	glDeleteBuffers(1, &arrayB);
	glDeleteBuffers(1, &elemA);
	glDeleteBuffers(1, &elemB);
#endif
}
