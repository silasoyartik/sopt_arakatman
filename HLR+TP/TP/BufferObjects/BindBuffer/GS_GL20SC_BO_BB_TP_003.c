#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BindBuffer

Library shall break all bindings to the target without making any changes to the newly bound buffer object's state,

Covered requirements:
	- GS-GL20SC-BO-BB-004
	- GS-GL20SC-BO-BB-005
*/

static const char* test_case4 = "GS_GL20SC_BO_BB_TC_004";
static const char* test_case5 = "GS_GL20SC_BO_BB_TC_005";

static const char* test_procedure = "GS_GL20SC_BO_BB_TP_003";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLuint buffA = 0;
static GLuint buffB = 0;
static const GLsizeiptr data_size = 512;

/* Initialization */
void GS_GL20SC_BO_BB_TP_003_init(void) {
    CHECK_ERROR(test_procedure);

    glGenBuffers(1, &buffA);
    glGenBuffers(1, &buffB);
    CHECK_ERROR(test_procedure);

    /* Bind buffer A and give it some data */
    glBindBuffer(GL_ARRAY_BUFFER, buffA);
    glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_DYNAMIC_DRAW);
    CHECK_ERROR(test_procedure);

    GLint size  = -1;
    GLint usage = -1;

    // Bind buffer B to the same target
    glBindBuffer(GL_ARRAY_BUFFER, buffB);

    // Test Case 004
    // Array Buffer should be binded to b

    GLint bind;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bind);

    if (bind != buffB) {
    	TEST_LOG_FAIL(test_case4, test_procedure, "Buffer should be binded to %u, actually binded to %u", buffB, bind);
    	test_success = GL_FALSE;
    }

    if(test_success) {
        TEST_LOG_SUCCESS(test_case4, test_procedure);
    } else {
    	test_success = GL_TRUE;
    }

    // Test Case 005
    // States should be correct
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
    if(size != 0 || usage != GL_STATIC_DRAW) {
        TEST_LOG_FAIL(test_case5, test_procedure,
            "(Buffer B state mismatch: size=%d, usage=0x%X)", size, usage);

        test_success = GL_FALSE;
    }

    // Rebind buffer A and check its state
    CHECK_ERROR(test_procedure);
    glBindBuffer(GL_ARRAY_BUFFER, buffA);

    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
    if(size != data_size || usage != GL_DYNAMIC_DRAW) {
        TEST_LOG_FAIL(test_case5, test_procedure,
            "(Buffer A state mismatch: size=%d, usage=0x%X)", size, usage);

        test_success = GL_FALSE;
    }

    if(test_success) {
        TEST_LOG_SUCCESS(test_case5, test_procedure);
    }
}

void GS_GL20SC_BO_BB_TP_003_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_BB_TP_003_close(void) {
    CHECK_ERROR(test_procedure);
#ifdef __ubuntu__
    glDeleteBuffers(1, &buffA);
    glDeleteBuffers(1, &buffB);
#endif
}
