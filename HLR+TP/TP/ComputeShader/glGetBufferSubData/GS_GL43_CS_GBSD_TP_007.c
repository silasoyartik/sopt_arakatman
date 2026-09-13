#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GS_BUFFER_TEST_GL43
#include "../../compute_helpers.h"

/*
GL43 - ComputeShader - glGetBufferSubData

Isolate each invalid range argument in separate calls on one valid unmapped buffer.

Covered requirements:
        - GS-GL43-CS-GBSD-008
        - GS-GL43-CS-GBSD-009
        - GS-GL43-CS-GBSD-010
*/

static const char* test_case8 = "GS_GL43_CS_GBSD_TC_008";
static const char* test_case9 = "GS_GL43_CS_GBSD_TC_009";
static const char* test_case10 = "GS_GL43_CS_GBSD_TC_010";
static const char* test_procedure = "GS_GL43_CS_GBSD_TP_007";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLboolean test_success8 = GL_FALSE;
static GLboolean test_executed8 = GL_FALSE;
static GLboolean test_success9 = GL_FALSE;
static GLboolean test_executed9 = GL_FALSE;
static GLboolean test_success10 = GL_FALSE;
static GLboolean test_executed10 = GL_FALSE;
static GLuint buffers[4] = {0};

/* Initialization */
void GS_GL43_CS_GBSD_TP_007_init(void) {
    test_success = GL_TRUE;

    GLenum err;
    const char* test_case = test_case8;
    int setup_complete = 0;
    test_success8 = GL_FALSE;
    test_executed8 = GL_FALSE;
    test_success9 = GL_FALSE;
    test_executed9 = GL_FALSE;
    test_success10 = GL_FALSE;
    test_executed10 = GL_FALSE;

    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }

    const unsigned char data[16] = {3,  17,  29,  41,  53,  67,  79,  83,
                                    97, 109, 127, 139, 151, 163, 179, 193};
    unsigned char out[32] = {0};
    GLint64 actual_size = 0;
    /* Prepare storage through a non-indexed target. */
    glGenBuffers(1, &buffers[0]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    if (buffers[0] == 0) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Buffer setup failed");
        goto finish;
    }
    glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[0]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    glBufferData(GL_COPY_WRITE_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    glBindBuffer(GL_COPY_READ_BUFFER, buffers[0]);
    glGetBufferParameteri64v(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &actual_size);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    if (!(actual_size == sizeof(data))) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Buffer-size precondition failed");
        goto finish;
    }
    setup_complete = 1;

    test_executed8 = GL_TRUE;
    test_success8 = GL_FALSE;
    glGetBufferSubData(GL_COPY_READ_BUFFER, (GLintptr)-1, 4, out);
    err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_INVALID_VALUE, got: 0x%x",
                      (unsigned)err);
        goto finish;
    }
    test_success8 = GL_TRUE;

    test_case = test_case9;
    test_executed9 = GL_TRUE;
    test_success9 = GL_FALSE;
    glGetBufferSubData(GL_COPY_READ_BUFFER, 0, (GLsizeiptr)-1, out);
    err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_INVALID_VALUE, got: 0x%x",
                      (unsigned)err);
        goto finish;
    }
    test_success9 = GL_TRUE;

    test_case = test_case10;
    test_executed10 = GL_TRUE;
    test_success10 = GL_FALSE;
    glGetBufferSubData(GL_COPY_READ_BUFFER, (GLintptr)actual_size - 1, 2, out);
    err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_INVALID_VALUE, got: 0x%x",
                      (unsigned)err);
        goto finish;
    }
    glGetBufferSubData(GL_COPY_READ_BUFFER, 0, (GLsizeiptr)actual_size + 1, out);
    err = glGetError();
    if (err != GL_INVALID_VALUE) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_INVALID_VALUE, got: 0x%x",
                      (unsigned)err);
        goto finish;
    }
    test_success10 = GL_TRUE;

    test_success = GL_TRUE;
    goto report;
finish:
    test_success = GL_FALSE;
    if (!setup_complete) {
        test_executed8 = GL_TRUE;
        test_success8 = GL_FALSE;
        test_executed9 = GL_TRUE;
        test_success9 = GL_FALSE;
        test_executed10 = GL_TRUE;
        test_success10 = GL_FALSE;
    }
report:
    if (test_executed8) {
        if (test_success8) {
            TEST_LOG_SUCCESS(test_case8, test_procedure);
        } else {
            TEST_LOG_FAIL(test_case8, test_procedure, "Requirement check or shared setup failed");
            test_success = GL_FALSE;
        }
    } else {
        TEST_LOG_INFO("[ %s ][ %s ] Not run: earlier step failed", test_case8, test_procedure);
        test_success = GL_FALSE;
    }
    if (test_executed9) {
        if (test_success9) {
            TEST_LOG_SUCCESS(test_case9, test_procedure);
        } else {
            TEST_LOG_FAIL(test_case9, test_procedure, "Requirement check or shared setup failed");
            test_success = GL_FALSE;
        }
    } else {
        TEST_LOG_INFO("[ %s ][ %s ] Not run: earlier step failed", test_case9, test_procedure);
        test_success = GL_FALSE;
    }
    if (test_executed10) {
        if (test_success10) {
            TEST_LOG_SUCCESS(test_case10, test_procedure);
        } else {
            TEST_LOG_FAIL(test_case10, test_procedure, "Requirement check or shared setup failed");
            test_success = GL_FALSE;
        }
    } else {
        TEST_LOG_INFO("[ %s ][ %s ] Not run: earlier step failed", test_case10, test_procedure);
        test_success = GL_FALSE;
    }
}

/* Draw */
void GS_GL43_CS_GBSD_TP_007_draw(void) {}

/* Cleanup */
void GS_GL43_CS_GBSD_TP_007_close(void) {
    CHECK_GL_ERROR(test_case8, test_procedure, test_success);
    glDeleteBuffers(4, buffers);
    memset(buffers, 0, sizeof(buffers));
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case8, test_procedure, "Cleanup error: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
    }
    CHECK_GL_ERROR(test_case8, test_procedure, test_success);
}
