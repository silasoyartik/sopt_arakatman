#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BufferData

Function shall set GL_INVALID_OPERATION if target is bound to reserved buffer object name 0.

Covered requirements:
	- GS-GL20SC-BO-BD-012
*/

static const char* test_case = "GS_GL20SC_BO_BD_TC_012";
static const char* test_procedure = "GS_GL20SC_BO_BD_TP_007";

static GLboolean test_success = GL_TRUE;


/* Initialization */
void GS_GL20SC_BO_BD_TP_007_init(void) {
    CHECK_ERROR(test_procedure);

    // Initial values, explicit set just in case
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // --- Test ARRAY_BUFFER bound to 0 ---
    CHECK_ERROR(test_procedure);

    glBufferData(GL_ARRAY_BUFFER, 128, NULL, GL_STATIC_DRAW);
    GLenum err = glGetError();

    if(err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "(expected GL_INVALID_OPERATION when ARRAY_BUFFER bound to 0, got 0x%X)", err);
        test_success = GL_FALSE;
    }

    // --- Test ELEMENT_ARRAY_BUFFER bound to 0 ---
    CHECK_ERROR(test_procedure);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 128, NULL, GL_DYNAMIC_DRAW);
    err = glGetError();

    if(err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case, test_procedure,
            "(expected GL_INVALID_OPERATION when ELEMENT_ARRAY_BUFFER bound to 0, got 0x%X)", err);
        test_success = GL_FALSE;
    }

    if(test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

void GS_GL20SC_BO_BD_TP_007_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_BD_TP_007_close(void) {
    CHECK_ERROR(test_procedure);
}
