#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BindBuffer

Function shall set GL_INVALID_ENUM if target is not GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.

Covered requirements:
	- GS-GL20SC-BO-BB-011
*/

static const char* test_case = "GS_GL20SC_BO_BB_TC_011";
static const char* test_procedure = "GS_GL20SC_BO_BB_TP_007";

static GLboolean test_success = GL_TRUE;
static GLuint buf;

/* Initialization */
void GS_GL20SC_BO_BB_TP_007_init(void) {
    CHECK_ERROR(test_procedure);

    glGenBuffers(1, &buf);
    CHECK_ERROR(test_procedure);

    for(size_t i = 0; i < num_of_enums; ++i) {
        GLenum target = enums[i];

        CHECK_ERROR(test_procedure);

        /* Call with this target */
        glBindBuffer(target, buf);

        GLenum err = glGetError();

        if(target == GL_ARRAY_BUFFER || target == GL_ELEMENT_ARRAY_BUFFER) {
            /* These should produce GL_NO_ERROR */
            if(err != GL_NO_ERROR) {
                TEST_LOG_FAIL(test_case, test_procedure,
                    "(GL_INVALID_ENUM for valid target %s)", enum_names[i]);

                test_success = GL_FALSE;
            }
        } else {
            /* All other targets should produce GL_INVALID_ENUM */
            if(err != GL_INVALID_ENUM) {
                TEST_LOG_FAIL(test_case, test_procedure,
                    "(GL_INVALID_ENUM not generated for invalid target %s)", enum_names[i]);

                test_success = GL_FALSE;
            }
        }
    }

    if(test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

void GS_GL20SC_BO_BB_TP_007_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_BB_TP_007_close(void) {
    CHECK_ERROR(test_procedure);
#ifdef __ubuntu__
    glDeleteBuffers(1, &buf);
#endif
}
