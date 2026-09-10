#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BindBuffer

This function tests the main functionality of glBindBuffer.
Function shall bind the buffer object name ID specified by buffer to target.

Covered requirements:
	- GS-GL20SC-BO-BB-001
	- GS-GL20SC-BO-BB-002
*/

static const char* test_case1 = "GS_GL20SC_BO_BB_TC_001";
static const char* test_case2 = "GS_GL20SC_BO_BB_TC_002";

static const char* test_procedure = "GS_GL20SC_BO_BB_TP_001";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLuint array_buff = 0;
static GLuint elem_buff  = 0;

/* Initialization */
void GS_GL20SC_BO_BB_TP_001_init(void) {
    CHECK_ERROR(test_procedure);

    /* Generate buffer names */
    glGenBuffers(1, &array_buff);
    glGenBuffers(1, &elem_buff);
    CHECK_ERROR(test_procedure);

    GLenum err;

    // Test Case 001
    // Check existence of the function
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    err = glGetError();

    if (err != GL_NO_ERROR) {
    	TEST_LOG_FAIL(test_case1, test_procedure, "error generated while using glBindBuffer function, err:0x%x", err);
    	test_success = GL_FALSE;
    }

    if(test_success) {
        TEST_LOG_SUCCESS(test_case1, test_procedure);
    } else {
    	test_success = GL_TRUE;
    }

    GLint size  = -1;
    GLint usage = -1;
    GLint binded_array, binded_elem;

    // Test Case 002
    // Function shall bind ID to target
    glBindBuffer(GL_ARRAY_BUFFER, array_buff);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buff);
    CHECK_ERROR(test_procedure);

    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &binded_array);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &binded_elem);
    CHECK_ERROR(test_procedure);

    if (binded_array != array_buff) {
    	TEST_LOG_FAIL(test_case2, test_procedure, "Binded array buffer should be: %u, got: %u", array_buff, binded_array);
    	test_success = GL_FALSE;
    }

    if (binded_elem != elem_buff) {
    	TEST_LOG_FAIL(test_case2, test_procedure, "Binded element array buffer should be: %u, got: %u", elem_buff, binded_elem);
    	test_success = GL_FALSE;
    }

    if(test_success) {
        TEST_LOG_SUCCESS(test_case2, test_procedure);
    }
}

void GS_GL20SC_BO_BB_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_BB_TP_001_close(void) {
    CHECK_ERROR(test_procedure);
#ifdef __ubuntu__
	glDeleteBuffers(1, &array_buff);
	glDeleteBuffers(1, &elem_buff);
#endif

}
