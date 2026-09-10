#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BufferSubData

Function shall set GL_INVALID_ENUM if target is not GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.

Covered requirements:
	- GS-GL20SC-BO-BSD-003
*/

static const char* test_case = "GS_GL20SC_BO_BSD_TC_003";
static const char* test_procedure = "GS_GL20SC_BO_BSD_TP_002";

static GLboolean test_success = GL_TRUE;
static GLuint array_buffer;
static GLuint elem_buffer;

/* Initialization */
void GS_GL20SC_BO_BSD_TP_002_init(void) {
    CHECK_ERROR(test_procedure);

    /* Generate buffers */
    glGenBuffers(1, &array_buffer);
    glGenBuffers(1, &elem_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buffer);

    /* Allocate storage */
    glBufferData(GL_ARRAY_BUFFER, 128, NULL, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 128, NULL, GL_STATIC_DRAW);

    CHECK_ERROR(test_procedure);

    /* Data to use with glBufferSubData */
    GLubyte dummy[16] = {0};

    for (size_t i = 0; i < num_of_enums; ++i) {
        GLenum target = enums[i];

        CHECK_ERROR(test_procedure);

        /* Call with this target */
        glBufferSubData(target, 0, sizeof(dummy), dummy);

        GLenum err = glGetError();

        if (target == GL_ARRAY_BUFFER || target == GL_ELEMENT_ARRAY_BUFFER) {
            /* GL_NO_ERROR expected */
            if (err != GL_NO_ERROR) {
                TEST_LOG_FAIL(test_case, test_procedure,
                    "(Error for valid target %s, error: 0x%x)", enum_names[i], err);
                test_success = GL_FALSE;
            }
        } else {
            /* GL_INVALID_ENUM expected*/
            if (err != GL_INVALID_ENUM) {
                TEST_LOG_FAIL(test_case, test_procedure,
                    "(GL_INVALID_ENUM not generated for invalid target %s, error: 0x%x)", enum_names[i], err);
                test_success = GL_FALSE;
            }
        }
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

void GS_GL20SC_BO_BSD_TP_002_draw(void) {

}

void GS_GL20SC_BO_BSD_TP_002_close(void) {
	CHECK_ERROR(test_procedure);
#ifdef __ubuntu__
    glDeleteBuffers(1, &array_buffer);
    glDeleteBuffers(1, &elem_buffer);
#endif
	CHECK_ERROR(test_procedure);
}
