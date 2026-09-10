#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BofferObjects - GetBufferParameteriv

glGetBufferParameteriv shall set the error state as GL_INVALID_ENUM
if target is not GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.

Covered requirements:
	- GS-GL20SC-BO-GBP-004
*/

static const char* test_case = "GS_GL20SC_BO_GBP_TC_004";
static const char* test_procedure = "GS_GL20SC_BO_GBP_TP_003";

static GLboolean test_success = GL_TRUE;

static GLuint array_buff = 0;
static GLuint element_buff = 0;
static const GLsizeiptr array_size   = 1024;
static const GLsizeiptr element_size = 2048;

/* Initialization */
void GS_GL20SC_BO_GBP_TP_003_init(void) {
    CHECK_ERROR(test_procedure);

    GLint param = 0;

    /* Generate buffers */
    glGenBuffers(1, &array_buff);
    glGenBuffers(1, &element_buff);

    /* Bind and allocate data for array buffer */
    glBindBuffer(GL_ARRAY_BUFFER, array_buff);
    glBufferData(GL_ARRAY_BUFFER, array_size, NULL, GL_STATIC_DRAW);

    /* Bind and allocate data for element array buffer */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buff);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, element_size, NULL, GL_STATIC_DRAW);

    for(size_t i = 0; i < num_of_enums; ++i) {
        GLenum target = enums[i];

        CHECK_ERROR(test_procedure);

        /* Call with this target */
        glGetBufferParameteriv(target, GL_BUFFER_SIZE, &param);

        GLenum err = glGetError();

        if(target == GL_ARRAY_BUFFER || target == GL_ELEMENT_ARRAY_BUFFER) {
            /* These should NOT produce GL_INVALID_ENUM */
            if(err != GL_NO_ERROR) {
                TEST_LOG_FAIL(test_case, test_procedure,
                    "No error expected for valid enum: %s, got: 0x%x", enum_names[i], err);
                test_success = GL_FALSE;
            }
        } else {
            /* All other targets should produce GL_INVALID_ENUM */
            if(err != GL_INVALID_ENUM) {
                TEST_LOG_FAIL(test_case, test_procedure,
                    "GL_INVALID_ENUM expected for invalid enum: %s, got: 0x%x", enum_names[i], err);
                test_success = GL_FALSE;
            }
        }
    }

    if(test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

void GS_GL20SC_BO_GBP_TP_003_draw(void) {

}

/* Cleanup */
void GS_GL20SC_BO_GBP_TP_003_close(void) {
    CHECK_ERROR(test_procedure);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#ifdef __ubuntu__
	glDeleteBuffers(1, &array_buff);
	glDeleteBuffers(1, &element_buff);
#endif
    CHECK_ERROR(test_procedure);
}
