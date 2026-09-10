#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BufferSubData

Function shall set GL_INVALID_OPERATION if the target is bound to reserved buffer object name 0.

Covered requirements:
	- GS-GL20SC-BO-BSD-007
*/

static const char* test_case = "GS_GL20SC_BO_BSD_TC_007";
static const char* test_procedure = "GS_GL20SC_BO_BSD_TP_006";

static GLboolean test_success = GL_TRUE;

/* Initialization */
void GS_GL20SC_BO_BSD_TP_006_init(void) {
    CHECK_ERROR(test_procedure);

    GLubyte dummy[16] = {0};

    /* GL_ARRAY_BUFFER with buffer name 0 */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(dummy), dummy);
    GLenum err = glGetError();

    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "GL_INVALID_OPERATION not generated for GL_ARRAY_BUFFER with buffer name 0, got: 0x%x", err);
        test_success = GL_FALSE;
    }

    /* GL_ELEMENT_ARRAY_BUFFER with buffer name 0 */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(dummy), dummy);
    err = glGetError();

    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "GL_INVALID_OPERATION not generated for GL_ELEMENT_ARRAY_BUFFER with buffer name 0, got: 0x%x", err);
        test_success = GL_FALSE;
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* Draw */
void GS_GL20SC_BO_BSD_TP_006_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_BSD_TP_006_close(void) {
	CHECK_ERROR(test_procedure);
}
