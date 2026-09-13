#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GS_BUFFER_TEST_GLES31
#include "../../compute_helpers.h"

/*
GLES31 - ComputeShader - glBindBufferBase

Accept all four targets at first and last supported indexed binding points.

Covered requirements:
        - GS-GLES31-CS-BBB-008
*/

static const char* test_case = "GS_GLES31_CS_BBB_TC_008";
static const char* test_procedure = "GS_GLES31_CS_BBB_TP_006";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLuint buffers[4] = {0};

/* Initialization */
void GS_GLES31_CS_BBB_TP_006_init(void) {
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
            TEST_LOG_FAIL(test_case, test_procedure, "%s",
                          "Required indexed target has no binding points");
            test_success = GL_FALSE;
            return;
        }
        const GLuint indices[] = {0, (GLuint)limit - 1};
        for (unsigned i = 0; i < 2; ++i) {
            glBindBufferBase(targets[t], indices[i], buffers[0]);
            err = glGetError();
            if (err != GL_NO_ERROR) {
                TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                              (unsigned)err);
                test_success = GL_FALSE;
                return;
            }
            {
                GLint actual_general = -1, actual_indexed = -1;
                glGetIntegerv(bindings[t], &actual_general);
                glGetIntegeri_v(bindings[t], indices[i], &actual_indexed);
                err = glGetError();
                if (err != GL_NO_ERROR) {
                    TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                                  (unsigned)err);
                    test_success = GL_FALSE;
                    return;
                }
                if (!((GLuint)actual_general == buffers[0] &&
                      (GLuint)actual_indexed == buffers[0])) {
                    TEST_LOG_FAIL(test_case, test_procedure, "%s",
                                  "Valid target binding rejected/incorrect");
                    test_success = GL_FALSE;
                    return;
                }
            }
        }
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* Draw */
void GS_GLES31_CS_BBB_TP_006_draw(void) {}

/* Cleanup */
void GS_GLES31_CS_BBB_TP_006_close(void) {
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
