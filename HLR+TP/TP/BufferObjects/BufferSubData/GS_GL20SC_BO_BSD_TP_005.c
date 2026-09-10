#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BufferSubData

Function shall set GL_INVALID_VALUE when offset + size exceeds the buffer size.

Covered requirements:
	- GS-GL20SC-BO-BSD-006
*/

static const char* test_case = "GS_GL20SC_BO_BSD_TC_006";
static const char* test_procedure = "GS_GL20SC_BO_BSD_TP_005";

static GLboolean test_success = GL_TRUE;
static GLuint array_buffer = 0;
static GLuint element_buffer = 0;

/* Initialization */
void GS_GL20SC_BO_BSD_TP_005_init(void) {
    CHECK_ERROR(test_procedure);

    /* Generate and bind ARRAY_BUFFER */
    glGenBuffers(1, &array_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    glBufferData(GL_ARRAY_BUFFER, 128, NULL, GL_DYNAMIC_DRAW);

    /* Generate and bind ELEMENT_ARRAY_BUFFER */
    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 128, NULL, GL_DYNAMIC_DRAW);

    GLubyte dummy[64] = {0};

    CHECK_ERROR(test_procedure);

    /* TESTS FOR GL_ARRAY_BUFFER */

    /* Test 1: offset + size == buffer size NO ERROR */
    glBufferSubData(GL_ARRAY_BUFFER, 64, 64, dummy);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Unexpected error when offset+size == buffer size (ARRAY_BUFFER), got: 0x%x", err);
        test_success = GL_FALSE;
    }

    /* Test 2: offset + size > buffer size (by 1) INVALID_VALUE */
    glBufferSubData(GL_ARRAY_BUFFER, 65, 64, dummy);
    err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "GL_INVALID_VALUE not generated when offset+size exceeded buffer size (ARRAY_BUFFER), got: 0x%x", err);
        test_success = GL_FALSE;
    }

    /* Test 3: offset + size > buffer size INVALID_VALUE */
    glBufferSubData(GL_ARRAY_BUFFER, 80, 64, dummy);
    err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "GL_INVALID_VALUE not generated when offset+size exceeded buffer size (ARRAY_BUFFER), got: 0x%x", err);
        test_success = GL_FALSE;
    }

    /* TESTS FOR GL_ELEMENT_ARRAY_BUFFER */

    /* Test 4: offset + size == buffer size NO ERROR */
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 64, 64, dummy);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Unexpected error when offset+size == buffer size (ELEMENT_ARRAY_BUFFER), got: 0x%x", err);
        test_success = GL_FALSE;
    }

    /* Test 5: offset + size > buffer size (by 1) INVALID_VALUE */
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 65, 64, dummy);
    err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "GL_INVALID_VALUE not generated when offset+size exceeded buffer size (ELEMENT_ARRAY_BUFFER), got: 0x%x", err);
        test_success = GL_FALSE;
    }

    /* Test 6: offset + size > buffer size INVALID_VALUE */
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 80, 64, dummy);
    err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "GL_INVALID_VALUE not generated when offset+size exceeded buffer size (ELEMENT_ARRAY_BUFFER), got: 0x%x", err);
        test_success = GL_FALSE;
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* Draw */
void GS_GL20SC_BO_BSD_TP_005_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_BSD_TP_005_close(void) {
	CHECK_ERROR(test_procedure);
#ifdef __ubuntu__
    glDeleteBuffers(1, &array_buffer);
    glDeleteBuffers(1, &element_buffer);
#endif
	CHECK_ERROR(test_procedure);
}
