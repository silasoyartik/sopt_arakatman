#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GS_BUFFER_TEST_GLES31
#include "../../compute_helpers.h"

/*
GLES31 - ComputeShader - glBindBufferBase

Unbind both general and indexed bindings from a nonzero baseline.

Covered requirements:
        - GS-GLES31-CS-BBB-006
*/

static const char* test_case = "GS_GLES31_CS_BBB_TC_006";
static const char* test_procedure = "GS_GLES31_CS_BBB_TP_004";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLuint buffers[4] = {0};

/* Initialization */
void GS_GLES31_CS_BBB_TP_004_init(void) {
    test_success = GL_TRUE;

    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }

    const GLuint data[4] = {1, 2, 3, 4};
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
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, buffers[0], 0, sizeof(data));
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    {
        GLint actual_general = -1, actual_indexed = -1;
        glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, &actual_general);
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, 0, &actual_indexed);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (!((GLuint)actual_general == buffers[0] && (GLuint)actual_indexed == buffers[0])) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Nonzero baseline missing");
            test_success = GL_FALSE;
            return;
        }
    }
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    {
        GLint actual_general = -1, actual_indexed = -1;
        glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, &actual_general);
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, 0, &actual_indexed);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (!((GLuint)actual_general == 0 && (GLuint)actual_indexed == 0)) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Zero did not unbind both points");
            test_success = GL_FALSE;
            return;
        }
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* Draw */
void GS_GLES31_CS_BBB_TP_004_draw(void) {}

/* Cleanup */
void GS_GLES31_CS_BBB_TP_004_close(void) {
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
