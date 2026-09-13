#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GS_BUFFER_TEST_GL43
#include "../../compute_helpers.h"

/*
GL43 - ComputeShader - glGetBufferSubData

General binding zero with otherwise valid arguments.

Covered requirements:
        - GS-GL43-CS-GBSD-007
*/

static const char* test_case = "GS_GL43_CS_GBSD_TC_007";
static const char* test_procedure = "GS_GL43_CS_GBSD_TP_006";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;

/* Initialization */
void GS_GL43_CS_GBSD_TP_006_init(void) {
    test_success = GL_TRUE;

    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }

    unsigned char out[4] = {0};
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    GLint binding = -1;
    glGetIntegerv(GL_COPY_READ_BUFFER_BINDING, &binding);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    if (binding != 0) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Zero binding precondition failed");
        test_success = GL_FALSE;
        return;
    }
    glGetBufferSubData(GL_COPY_READ_BUFFER, 0, sizeof(out), out);
    err = glGetError();
    if (err != GL_INVALID_OPERATION) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_INVALID_OPERATION, got: 0x%x",
                      (unsigned)err);
        test_success = GL_FALSE;
        return;
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* Draw */
void GS_GL43_CS_GBSD_TP_006_draw(void) {}

/* Cleanup */
void GS_GL43_CS_GBSD_TP_006_close(void) {
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Cleanup error: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
    }
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
}
