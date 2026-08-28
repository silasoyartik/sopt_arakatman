#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BufferData

Function shall set GL_INVALID_VALUE if size is negative.

Covered requirements:
	- GS-GL20SC-BO-BD-010
*/

static const char* test_case = "GS_GL20SC_BO_BD_TC_010";
static const char* test_procedure = "GS_GL20SC_BO_BD_TP_005";

static GLboolean test_success = GL_TRUE;
static GLuint array_buffer = 0;
static GLuint elem_buffer  = 0;

/* Initialization */
void GS_GL20SC_BO_BD_TP_005_init(void) {
	CHECK_ERROR(test_procedure);

    /* Generate buffers */
    glGenBuffers(1, &array_buffer);
    glGenBuffers(1, &elem_buffer);

	GLenum err;

	CHECK_ERROR(test_procedure);

	/* Attempt to allocate with negative size for ARRAY_BUFFER*/
	glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
	glBufferData(GL_ARRAY_BUFFER, -1, NULL, GL_STATIC_DRAW);

	err = glGetError();
	if(err != GL_INVALID_VALUE) {
		TEST_LOG_FAIL(test_case, test_procedure, " (ARRAY_BUFFER): Failed (expected GL_INVALID_VALUE, got 0x%X)", err);
		test_success = GL_FALSE;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buffer);

	CHECK_ERROR(test_procedure);

	/* Attempt to allocate with negative size for ELEMENT_ARRAY_BUFFER*/
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, -1, NULL, GL_DYNAMIC_DRAW);

	err = glGetError();
	if(err != GL_INVALID_VALUE) {
		TEST_LOG_FAIL(test_case, test_procedure, "(ELEMENT_ARRAY_BUFFER): Failed (expected GL_INVALID_VALUE, got 0x%X)", err);
		test_success = GL_FALSE;
	}

	if (test_success) {
		TEST_LOG_SUCCESS(test_case, test_procedure);
	}
}


void GS_GL20SC_BO_BD_TP_005_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_BD_TP_005_close(void) {
#ifdef __ubuntu__
    glDeleteBuffers(1, &array_buffer);
    glDeleteBuffers(1, &elem_buffer);
#endif
}
