#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BufferData

Function shall set GL_INVALID_ENUM if usage is not GL_STATIC_DRAW, GL_DYNAMIC_DRAW or GL_STREAM_DRAW.

Covered requirements:
	- GS-GL20SC-BO-BD-011
*/

static const char* test_case = "GS_GL20SC_BO_BD_TC_011";
static const char* test_procedure = "GS_GL20SC_BO_BD_TP_006";

static GLboolean test_success = GL_TRUE;
static GLuint array_buffer = 0;

/* Initialization */
void GS_GL20SC_BO_BD_TP_006_init(void) {
	CHECK_ERROR(test_procedure);

    /* Generate and bind buffer */
    glGenBuffers(1, &array_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);

	for(size_t i = 0; i < num_of_enums; ++i) {
		GLenum target = enums[i];

		CHECK_ERROR(test_procedure);

		/* Call with this target */
		glBufferData(GL_ARRAY_BUFFER, 128, NULL, target);

		GLenum err = glGetError();

		if(target == GL_STATIC_DRAW || target == GL_DYNAMIC_DRAW || target == GL_STREAM_DRAW) {
			/* These should produce GL_NO_ERROR */
			if(err != GL_NO_ERROR) {
				TEST_LOG_FAIL(test_case, test_procedure, "(GL_INVALID_ENUM for valid target %s)", enum_names[i]);
				test_success = GL_FALSE;
			}
		} else {
			/* All other targets should produce GL_INVALID_ENUM */
			if(err != GL_INVALID_ENUM) {
				TEST_LOG_FAIL(test_case, test_procedure, "(GL_INVALID_ENUM not generated for invalid target %s)\n", enum_names[i]);
				test_success = GL_FALSE;
			}
		}
	}

	if(test_success) {
		TEST_LOG_SUCCESS(test_case, test_procedure);
	}
}


void GS_GL20SC_BO_BD_TP_006_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_BD_TP_006_close(void) {
#ifdef __ubuntu__
	glDeleteBuffers(1, &array_buffer);
#endif
}
