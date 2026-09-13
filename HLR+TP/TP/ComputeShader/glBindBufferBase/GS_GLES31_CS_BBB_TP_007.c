#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GS_BUFFER_TEST_GLES31
#include "../../compute_helpers.h"

/*
GLES31 - ComputeShader - glBindBufferBase

Use valid buffer/index with targets that are not indexed binding targets.

Covered requirements:
        - GS-GLES31-CS-BBB-009
*/

static const char* test_case = "GS_GLES31_CS_BBB_TC_009";
static const char* test_procedure = "GS_GLES31_CS_BBB_TP_007";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLuint buffers[4] = {0};

/* Initialization */
void GS_GLES31_CS_BBB_TP_007_init(void) {
    test_success = GL_TRUE;

    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }

    const GLuint data[4] = {1, 2, 3, 4};
    const GLenum invalid[] = {GL_ARRAY_BUFFER, GL_COPY_READ_BUFFER, (GLenum)0xffffffffu};
    /* Prepare storage through a non-indexed target. */
    glGenBuffers(1, &buffers[0]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    if (buffers[0] == 0) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Buffer setup failed");
        test_success = GL_FALSE;
        return;
    }
    glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[0]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    glBufferData(GL_COPY_WRITE_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    for (unsigned i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        glBindBufferBase(invalid[i], 0, buffers[0]);
        err = glGetError();
        if (err != GL_INVALID_ENUM) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_INVALID_ENUM, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* Draw */
void GS_GLES31_CS_BBB_TP_007_draw(void) {}

/* Cleanup */
void GS_GLES31_CS_BBB_TP_007_close(void) {
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
    glDeleteBuffers(4, buffers);
    memset(buffers, 0, sizeof(buffers));
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Cleanup error: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
    }
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
}
