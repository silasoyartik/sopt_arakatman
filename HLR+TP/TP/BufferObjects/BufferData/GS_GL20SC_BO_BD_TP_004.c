#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BufferData

Function shall set GL_INVALID_ENUM if target is not GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.

Covered requirements:
	- GS-GL20SC-BO-BD-009
*/

static const char* test_case = "GS_GL20SC_BO_BD_TC_009";
static const char* test_procedure = "GS_GL20SC_BO_BD_TP_004";

static GLboolean test_success = GL_TRUE;
static GLuint array_buffer = 0;
static GLuint elem_buffer  = 0;

/* Initialization */
void GS_GL20SC_BO_BD_TP_004_init(void) {
	CHECK_ERROR(test_procedure);

    /* Generate buffers */
    glGenBuffers(1, &array_buffer);
    glGenBuffers(1, &elem_buffer);

    /* Bind them to their targets */
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buffer);

	for(size_t i = 0; i < num_of_enums; ++i) {
		GLenum target = enums[i];

		CHECK_ERROR(test_procedure);

		/* Call with this target */
		glBufferData(target, 128, NULL, GL_STATIC_DRAW);

		GLenum err = glGetError();

		if(target == GL_ARRAY_BUFFER || target == GL_ELEMENT_ARRAY_BUFFER) {
			/* These should produce GL_NO_ERROR */
			if(err != GL_NO_ERROR) {
				TEST_LOG_FAIL(test_case, test_procedure, "(GL_INVALID_ENUM for valid target %s)", enum_names[i]);
				test_success = GL_FALSE;
			}
		} else {
			/* All other targets should produce GL_INVALID_ENUM */
			if(err != GL_INVALID_ENUM) {
				TEST_LOG_FAIL(test_case, test_procedure, "(GL_INVALID_ENUM not generated for invalid target %s)", enum_names[i]);
				test_success = GL_FALSE;
			}
		}
	}

	if(test_success) {
		TEST_LOG_SUCCESS(test_case, test_procedure);
	}
}

void GS_GL20SC_BO_BD_TP_004_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_BD_TP_004_close(void) {
#ifdef __ubuntu__
	glDeleteBuffers(1, &array_buffer);
	glDeleteBuffers(1, &elem_buffer);
#endif
}
