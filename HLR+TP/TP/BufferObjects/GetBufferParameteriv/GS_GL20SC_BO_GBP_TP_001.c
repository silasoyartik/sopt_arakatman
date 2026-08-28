#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../../test_utility.h"

/*
GL20SC - BofferObjects - GetBufferParameteriv

Graphics library shall provide glGetBufferParameteriv function

This function tests the main functionality of glGetBufferParameteriv for GL_BUFFER_SIZE

Covered requirements:
	- GS-GL20SC-BO-GBP-001
	- GS-GL20SC-BO-GBP-002
*/

static const char* test_case1 = "GS_GL20SC_BO_GBP_TC_001";
static const char* test_case2 = "GS_GL20SC_BO_GBP_TC_002";
static const char* test_procedure = "GS_GL20SC_BO_GBP_TP_001";

/* ---- Static state ---- */
static GLboolean test_success1 = GL_TRUE;
static GLboolean test_success2 = GL_TRUE;
static GLuint array_buff = 0;
static GLuint element_buff = 0;
static const GLsizeiptr array_size   = 1024;  /* known size for array buffer */
static const GLsizeiptr element_size = 2048;  /* known size for element buffer */

/* Initialization */
void GS_GL20SC_BO_GBP_TP_001_init(void) {
    CHECK_ERROR(test_procedure);

    /* Generate buffers */
    glGenBuffers(1, &array_buff);
    glGenBuffers(1, &element_buff);

    /* Bind and allocate data for array buffer */
    glBindBuffer(GL_ARRAY_BUFFER, array_buff);
    glBufferData(GL_ARRAY_BUFFER, array_size, NULL, GL_STATIC_DRAW);

    /* Bind and allocate data for element array buffer */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buff);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, element_size, NULL, GL_STATIC_DRAW);

    GLint size = 0;
    GLenum err;

    // Check array buffer size
    CHECK_ERROR(test_procedure);
    glBindBuffer(GL_ARRAY_BUFFER, array_buff);

    // Test Case 001
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    err = glGetError();

    if (err != GL_NO_ERROR) {
    	TEST_LOG_FAIL(test_case1, test_procedure, "No error expected when using glGetBufferParameteriv function, got:0x%x", err);
    	test_success1 = GL_FALSE;
    }

    if (test_success1) {
    	TEST_LOG_SUCCESS(test_case1, test_procedure);
    }

    // Test Case 002
    if(size != array_size) {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "(Array buffer size mismatch: expected %d, got %d)", array_size, size);
        test_success2 = GL_FALSE;
    }

    // Check element array buffer size
    CHECK_ERROR(test_procedure);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buff);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    if(size != element_size) {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "(Element buffer size mismatch: expected %d, got %d)", element_size, size);
        test_success2 = GL_FALSE;
    }

    if(test_success2) {
        TEST_LOG_SUCCESS(test_case2, test_procedure);
    }
}

void GS_GL20SC_BO_GBP_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_GBP_TP_001_close(void) {
    CHECK_ERROR(test_procedure);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#ifdef __ubuntu__
	glDeleteBuffers(1, &array_buff);
	glDeleteBuffers(1, &element_buff);
#endif
    CHECK_ERROR(test_procedure);
}
