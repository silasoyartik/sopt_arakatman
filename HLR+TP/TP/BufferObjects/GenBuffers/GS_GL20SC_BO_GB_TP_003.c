#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - GenBuffers

GL_INVALID_VALUE should be set if n is negative

Covered requirements:
	- GS-GL20SC-BO-GB-004
*/

static const char* test_case = "GS_GL20SC_BO_GB_TC_004";
static const char* test_procedure = "GS_GL20SC_BO_GB_TP_003";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;

/* Initialization */
void GS_GL20SC_BO_GB_TP_003_init(void) {
	CHECK_ERROR(test_procedure);

	GLenum err;
	GLuint buf;

	// n = 1, generate test buffer
	glGenBuffers(1, &buf);
	err = glGetError();

	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case, test_procedure, "n = 0, no error expected, got: 0x%x", err);
		test_success = GL_FALSE;
	}

	GLuint test_buff = buf;

	// n = 0, no error expected, buffer id should not be given
	glGenBuffers(0, &buf);
	err = glGetError();

	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case, test_procedure,  "n = 0, no error expected, got: 0x%x", err);
		test_success = GL_FALSE;
	}

	if (buf != test_buff) {
		TEST_LOG_FAIL(test_case, test_procedure,  "buffer generated with n = 0, generated: %d", buf);
		test_success = GL_FALSE;
	}


	// n = -1, GL_INVALID_VALUE expected, buffer id shouldn't not be given
	glGenBuffers(-1, &buf);
	err = glGetError();

	if (err != GL_INVALID_VALUE) {
		TEST_LOG_FAIL(test_case, test_procedure,  "n = -1, GL_INVALID_VALUE expected, got: 0x%x", err);
		test_success = GL_FALSE;
	}

	if (buf != test_buff) {
		TEST_LOG_FAIL(test_case, test_procedure,  "buffer generated with n = 0, generated: %d", buf);
		test_success = GL_FALSE;
	}

	// n = -10, GL_INVALID_VALUE expected, buffer id shouldn't not be given
	glGenBuffers(-10, &buf);
	err = glGetError();

	if (err != GL_INVALID_VALUE) {
		TEST_LOG_FAIL(test_case, test_procedure,  "n = -10, GL_INVALID_VALUE expected, got: 0x%x", err);
		test_success = GL_FALSE;
	}

	if (buf != test_buff) {
		TEST_LOG_FAIL(test_case, test_procedure,  "buffer generated with n = 0, generated: %d", buf);
		test_success = GL_FALSE;
	}


	if (test_success) {
		TEST_LOG_SUCCESS(test_case, test_procedure);
	}
}

/* Draw */
void GS_GL20SC_BO_GB_TP_003_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_GB_TP_003_close(void) {
	CHECK_ERROR(test_procedure);
}
