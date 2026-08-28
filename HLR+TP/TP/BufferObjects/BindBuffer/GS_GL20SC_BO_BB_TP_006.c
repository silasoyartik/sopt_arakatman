#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BindBuffer

Error state should be GL_INVALID_OPERATION if bindBuffer is called with invalid buffer parameter.

Covered requirements:
	- GS-GL20SC-BO-BB-010
*/

static const char* test_case = "GS_GL20SC_BO_BB_TC_010";
static const char* test_procedure = "GS_GL20SC_BO_BB_TP_006";

static GLboolean test_success = GL_TRUE;

/* Initialization */
void GS_GL20SC_BO_BB_TP_006_init(void) {
    CHECK_ERROR(test_procedure);

    GLenum err;
    GLuint fake_id = 12345;  /* not obtained via glGenBuffers */

    // --- Test ARRAY_BUFFER with invalid buffer id ---
    glBindBuffer(GL_ARRAY_BUFFER, fake_id);
    err = glGetError();

    if(err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "(GL_INVALID_OPERATION not generated when binding non-generated buffer id %u to ARRAY_BUFFER, got 0x%X)",
            fake_id, err);

        test_success = GL_FALSE;
    }

    // --- Test ELEMENT_ARRAY_BUFFER with invalid buffer id ---
    CHECK_ERROR(test_procedure);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fake_id);
    err = glGetError();

    if(err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "(GL_INVALID_OPERATION not generated when binding non-generated buffer id %u to ELEMENT_ARRAY_BUFFER, got 0x%X)",
            fake_id, err);

        test_success = GL_FALSE;
    }

    if(test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

void GS_GL20SC_BO_BB_TP_006_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_BB_TP_006_close(void) {
    CHECK_ERROR(test_procedure);
}
