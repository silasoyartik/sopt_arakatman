#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BindBuffer

Initial generation shall have size of 0 and usage of GL_STATIC_DRAW.

Covered requirements:
	- GS-GL20SC-BO-BB-003
*/

static const char* test_case = "GS_GL20SC_BO_BB_TC_003";

static const char* test_procedure = "GS_GL20SC_BO_BB_TP_002";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLuint array_buff = 0;
static GLuint elem_buff  = 0;

/* Initialization */
void GS_GL20SC_BO_BB_TP_002_init(void) {
    CHECK_ERROR(test_procedure);

    /* Generate buffer names */
    glGenBuffers(1, &array_buff);
    glGenBuffers(1, &elem_buff);
    CHECK_ERROR(test_procedure);

    GLint size  = -1;
    GLint usage = -1;

    // Test Case 003
    // Initial generation shall have size 0 and GL_STATIC_DRAW
    glBindBuffer(GL_ARRAY_BUFFER, array_buff);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buff);
    CHECK_ERROR(test_procedure);


    // Check ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, array_buff);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
    CHECK_ERROR(test_procedure);

    if(size != 0 || usage != GL_STATIC_DRAW) {
        TEST_LOG_FAIL(test_case, test_procedure, "(ARRAY_BUFFER mismatch: size=%d, usage=0x%X)", size, usage);
    }

    // Check ELEMENT_ARRAY_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buff);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
    CHECK_ERROR(test_procedure);

    if(size != 0 || usage != GL_STATIC_DRAW) {
        TEST_LOG_FAIL(test_case, test_procedure, "(ELEMENT_ARRAY_BUFFER mismatch: size=%d, usage=0x%X)", size, usage);
    }

    if(test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

void GS_GL20SC_BO_BB_TP_002_draw(void) {
}

/* Cleanup */
void GS_GL20SC_BO_BB_TP_002_close(void) {
    CHECK_ERROR(test_procedure);
#ifdef __ubuntu__
	glDeleteBuffers(1, &array_buff);
	glDeleteBuffers(1, &elem_buff);
#endif

}
