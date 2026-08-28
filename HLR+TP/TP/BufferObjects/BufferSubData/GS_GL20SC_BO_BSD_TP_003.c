#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BufferSubData

Function shall set GL_INVALID_VALUE if the offset is negative.

Covered requirements:
	- GS-GL20SC-BO-BSD-004
*/

static const char* test_case = "GS_GL20SC_BO_BSD_TC_004";
static const char* test_procedure = "GS_GL20SC_BO_BSD_TP_003";

static GLboolean test_success = GL_TRUE;
static GLuint array_buffer = 0;
static GLuint element_buffer = 0;

/* Initialization */
void GS_GL20SC_BO_BSD_TP_003_init(void) {
    CHECK_ERROR(test_procedure);

    /* Generate and bind ARRAY_BUFFER */
    glGenBuffers(1, &array_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    glBufferData(GL_ARRAY_BUFFER, 128, NULL, GL_STATIC_DRAW);

    /* Generate and bind ELEMENT_ARRAY_BUFFER */
    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 128, NULL, GL_STATIC_DRAW);

    GLubyte dummy[16] = {0};

    CHECK_ERROR(test_procedure);

    /* GL_ARRAY_BUFFER with negative offset */
    glBufferSubData(GL_ARRAY_BUFFER, -4, sizeof(dummy), dummy);
    GLenum err = glGetError();

    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "GL_INVALID_VALUE not generated for GL_ARRAY_BUFFER with negative offset, got: 0x%x", err);
        test_success = GL_FALSE;
    }

    /* GL_ELEMENT_ARRAY_BUFFER with negative offset */
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, -4, sizeof(dummy), dummy);
    err = glGetError();

    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "GL_INVALID_VALUE not generated for GL_ELEMENT_ARRAY_BUFFER with negative offset, got: 0x%x", err);
        test_success = GL_FALSE;
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* Draw */
void GS_GL20SC_BO_BSD_TP_003_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_BSD_TP_003_close(void) {
	CHECK_ERROR(test_procedure);
#ifdef __ubuntu__
   glDeleteBuffers(1, &array_buffer);
   glDeleteBuffers(1, &element_buffer);
#endif
	CHECK_ERROR(test_procedure);
}
