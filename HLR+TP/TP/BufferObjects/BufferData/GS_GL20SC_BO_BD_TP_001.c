#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BufferData

Library shall provide the Buffer Data function.
Function shall create data store with specified size.

Covered requirements:
	- GS-GL20SC-BO-BD-001
	- GS-GL20SC-BO-BD-002
*/

static const char* test_case1 = "GS_GL20SC_BO_BD_TC_001";
static const char* test_case2 = "GS_GL20SC_BO_BD_TC_002";

static const char* test_procedure = "GS_GL20SC_BO_BD_TP_001";

static GLboolean test_success = GL_TRUE;

static GLuint array_buffer = 0;
static GLuint elem_buffer  = 0;

/* Initialization */
void GS_GL20SC_BO_BD_TP_001_init(void) {
	CHECK_ERROR(test_procedure);

    /* Generate buffers */
    glGenBuffers(1, &array_buffer);
    glGenBuffers(1, &elem_buffer);

    /* Bind them to their targets */
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buffer);
	CHECK_ERROR(test_procedure);

    GLint size = -1;
    GLenum err;

	// Test Case 001
	// Library shall provide the buffer data function

	/* ---- ARRAY_BUFFER ---- */
	const GLsizeiptr array_size = 1024;
	glBufferData(GL_ARRAY_BUFFER, array_size, NULL, GL_STATIC_DRAW);

	err = glGetError();

	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case1, test_procedure, "No error expected when using the buffer data function, got: 0x%x", err);
		test_success = GL_FALSE;
	}

	if (test_success) {
		TEST_LOG_SUCCESS(test_case1, test_procedure);
	} else {
		test_success = GL_TRUE;
	}

	// Test Case 002
	// Creating data store with specified size

	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	if(size != array_size) {
		TEST_LOG_FAIL(test_case2, test_procedure, "ARRAY_BUFFER test failed, (expected %ld, got %d)", (long)array_size, size);
		test_success = GL_FALSE;
	}

	CHECK_ERROR(test_procedure);

	/* ---- ELEMENT_ARRAY_BUFFER ---- */
	const GLsizeiptr elem_size = 2048;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elem_size, NULL, GL_DYNAMIC_DRAW);

	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	if(size != elem_size) {
		TEST_LOG_FAIL(test_case2, test_procedure, "ELEMENT_ARRAY_BUFFER test failed, (expected %ld, got %d)", (long)elem_size, size);
		test_success = GL_FALSE;
	}

	if (test_success) {
		TEST_LOG_SUCCESS(test_case2, test_procedure);
	}
}

/* Draw */
void GS_GL20SC_BO_BD_TP_001_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_BD_TP_001_close(void) {
#ifdef __ubuntu__
	glDeleteBuffers(1, &array_buffer);
	glDeleteBuffers(1, &elem_buffer);
#endif
}
