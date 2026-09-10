#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BufferData

Function shall set GL_OUT_OF_MEMORY if data store cannot be created.

Covered requirements:
	- GS-GL20SC-BO-BD-008
*/

static const char* test_case = "GS_GL20SC_BO_BD_TC_008";
static const char* test_procedure = "GS_GL20SC_BO_BD_TP_003";

static GLboolean test_success = GL_TRUE;

#define CHUNK_SIZE (1024 * 1024 * 1024)

/* ~1 GB chunk size for cumulative allocation */
// static const GLsizeiptr CHUNK_SIZE = 1024 * 1024 * 1024; /* 1 GB */

/* Initialization */
void GS_GL20SC_BO_BD_TP_003_init(void) {
	CHECK_ERROR(test_procedure);

	GLenum err;
	GLuint buffer_id = 0;

	/* ---- ARRAY_BUFFER ---- */
	/* Cumulative allocation loop: generate new buffer each iteration, add ~1GB chunks until OOM */
	while (1) {
		glGenBuffers(1, &buffer_id);
		glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

		CHECK_ERROR(test_procedure);

		glBufferData(GL_ARRAY_BUFFER, CHUNK_SIZE, NULL, GL_STATIC_DRAW);
		err = glGetError();

		if (err != GL_NO_ERROR) {
			break;
		}

	}

	if (err != GL_OUT_OF_MEMORY) {
		TEST_LOG_FAIL(test_case, test_procedure, "GL_OUT_OF_MEMORY expected on GL_ARRAY_BUFFER loop, instead got: 0x%x", err);
		test_success = GL_FALSE;
	}

	if (test_success) {
		/* ---- ELEMENT_ARRAY_BUFFER ---- */
		/* Cumulative allocation loop: generate new buffer each iteration, add ~1GB chunks until OOM */
		while (1) {
			glGenBuffers(1, &buffer_id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id);

			CHECK_ERROR(test_procedure);

			glBufferData(GL_ELEMENT_ARRAY_BUFFER, CHUNK_SIZE, NULL, GL_DYNAMIC_DRAW);
			err = glGetError();

			if (err != GL_NO_ERROR) {
				break;
			}
		}
	}

	if (err != GL_OUT_OF_MEMORY) {
		TEST_LOG_FAIL(test_case, test_procedure, "GL_OUT_OF_MEMORY expected on GL_ELEMENT_ARRAY_BUFFER loop, instead got: 0x%x", err);
		test_success = GL_FALSE;
	}

	if (test_success) {
		TEST_LOG_SUCCESS(test_case, test_procedure);
	}
}

void GS_GL20SC_BO_BD_TP_003_draw(void) {

}

void GS_GL20SC_BO_BD_TP_003_close(void) {
}
