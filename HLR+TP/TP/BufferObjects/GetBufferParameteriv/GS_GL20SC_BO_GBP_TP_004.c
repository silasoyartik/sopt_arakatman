#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BofferObjects - GetBufferParameteriv

GL_INVALID_OPERATION shall be set when buffer name is 0

Covered requirements:
	- GS-GL20SC-BO-GBP-005
*/

static const char* test_case = "GS_GL20SC_BO_GBP_TC_005";
static const char* test_procedure = "GS_GL20SC_BO_GBP_TP_004";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;

/* Initialization */
void GS_GL20SC_BO_GBP_TP_004_init(void) {
    CHECK_ERROR(test_procedure);

    GLint param = 0;

    /* Bind reserved buffer object name 0 to ARRAY_BUFFER */
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &param);

    GLenum err = glGetError();

    if(err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "(Expected GL_INVALID_OPERATION when buffer name is 0 for GL_ARRAY_BUFFER, got 0x%X)", err);
        test_success = GL_FALSE;
    }

    /* Bind reserved buffer object name 0 to ELEMENT_ARRAY_BUFFER */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &param);

    err = glGetError();

    if(err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "(Expected GL_INVALID_OPERATION when buffer name is 0 for GL_ELEMENT_ARRAY_BUFFER, got 0x%X)", err);
        test_success = GL_FALSE;
    }

    if(test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

void GS_GL20SC_BO_GBP_TP_004_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_GBP_TP_004_close(void) {
    CHECK_ERROR(test_procedure);
}
