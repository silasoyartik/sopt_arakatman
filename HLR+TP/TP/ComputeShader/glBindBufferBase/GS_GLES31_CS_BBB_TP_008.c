#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GS_BUFFER_TEST_GLES31
#include "../../compute_helpers.h"

/*
GLES31 - ComputeShader - glBindBufferBase

Check index==limit and index>limit for each target with otherwise valid inputs.

Covered requirements:
        - GS-GLES31-CS-BBB-010
*/

static const char* test_case = "GS_GLES31_CS_BBB_TC_010";
static const char* test_procedure = "GS_GLES31_CS_BBB_TP_008";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLuint buffers[4] = {0};

/* Initialization */
void GS_GLES31_CS_BBB_TP_008_init(void) {
    test_success = GL_TRUE;

    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }

    const GLenum targets[] = {GL_ATOMIC_COUNTER_BUFFER, GL_SHADER_STORAGE_BUFFER,
                              GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER};
    const GLenum bindings[] = {GL_ATOMIC_COUNTER_BUFFER_BINDING, GL_SHADER_STORAGE_BUFFER_BINDING,
                               GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, GL_UNIFORM_BUFFER_BINDING};
    const GLenum limits[] = {
        GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS,
        GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, GL_MAX_UNIFORM_BUFFER_BINDINGS};
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

    for (unsigned t = 0; t < 4; ++t) {
        GLint limit = 0;
        glGetIntegerv(limits[t], &limit);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (!(limit > 0)) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Invalid runtime binding limit");
            test_success = GL_FALSE;
            return;
        }
        {
            GLint actual_general = -1, actual_indexed = -1;
            glGetIntegerv(bindings[t], &actual_general);
            glGetIntegeri_v(bindings[t], 0, &actual_indexed);
            err = glGetError();
            if (err != GL_NO_ERROR) {
                TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                              (unsigned)err);
                test_success = GL_FALSE;
                return;
            }
            if (!((GLuint)actual_general == 0 && (GLuint)actual_indexed == 0)) {
                TEST_LOG_FAIL(test_case, test_procedure, "%s", "Initial binding state not clean");
                test_success = GL_FALSE;
                return;
            }
        }
        for (GLuint extra = 0; extra < 2; ++extra) {
            glBindBufferBase(targets[t], (GLuint)limit + extra, buffers[0]);
            err = glGetError();
            if (err != GL_INVALID_VALUE) {
                TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_INVALID_VALUE, got: 0x%x",
                              (unsigned)err);
                test_success = GL_FALSE;
                return;
            }
        }
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* Draw */
void GS_GLES31_CS_BBB_TP_008_draw(void) {}

/* Cleanup */
void GS_GLES31_CS_BBB_TP_008_close(void) {
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
