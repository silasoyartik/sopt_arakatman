#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../../test_utility.h"

/*
GL20SC - BofferObjects - GetBufferParameteriv

This function tests the main functionality of glGetBufferParameteriv for GL_BUFFER_USAGE

Covered requirements:
	- GS-GL20SC-BO-GBP-003
*/

static const char* test_case = "GS_GL20SC_BO_GBP_TC_003";
static const char* test_procedure = "GS_GL20SC_BO_GBP_TP_002";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLuint array_buff = 0;
static GLuint element_buff = 0;
static const GLsizeiptr array_size   = 1024;
static const GLsizeiptr element_size = 2048;

/* Initialization */
void GS_GL20SC_BO_GBP_TP_002_init(void) {
    CHECK_ERROR(test_procedure);

    /* Generate buffers */
    glGenBuffers(1, &array_buff);
    glGenBuffers(1, &element_buff);

    /* Bind and allocate data for array buffer with STATIC_DRAW usage */
    glBindBuffer(GL_ARRAY_BUFFER, array_buff);
    glBufferData(GL_ARRAY_BUFFER, array_size, NULL, GL_STATIC_DRAW);

    /* Bind and allocate data for element array buffer with DYNAMIC_DRAW usage */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buff);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, element_size, NULL, GL_DYNAMIC_DRAW);

    GLint usage = 0;

    // Check array buffer usage
    CHECK_ERROR(test_procedure);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
    if(usage != GL_STATIC_DRAW) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "(Array buffer usage mismatch: expected GL_STATIC_DRAW, got 0x%X)", usage);
        test_success = GL_FALSE;
    }

    // Check element array buffer usage
    CHECK_ERROR(test_procedure);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
    if(usage != GL_DYNAMIC_DRAW) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "(Element buffer usage mismatch: expected GL_DYNAMIC_DRAW, got 0x%X)", usage);
        test_success = GL_FALSE;
    }

    if(test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

void GS_GL20SC_BO_GBP_TP_002_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_GBP_TP_002_close(void) {
    CHECK_ERROR(test_procedure);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#ifdef __ubuntu__
	glDeleteBuffers(1, &array_buff);
	glDeleteBuffers(1, &element_buff);
#endif
    CHECK_ERROR(test_procedure);
}
